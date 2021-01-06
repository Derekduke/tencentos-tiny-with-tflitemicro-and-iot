/********************************************************
 *
 * @file    ds18b20.c
 * @brief   ds18b20����
 * @author  Mculover666
 * @date    2020/08/29
 * @note    ��������us����ʱʹ��DWT
 *
********************************************************/

#include "ds18b20.h"
#include <stdio.h>

/**
  * @brief    ���͸�λ�ź�
  * @param    none
  * @retval   none
*/
static void DS18B20_Send_Reset_Single(void)
{
	DS18B20_OutPut_Mode();
    
    /* �������� 480 - 960 us*/
	DS18B20_Out(0);
	DS18B20_Delay_us(750);
    
    /* �ͷ����� 15 - 60 us */
	DS18B20_Out(1);
	DS18B20_Delay_us(15);
}

/**
  * @brief    ���DS18B20��������
  * @param    none
  * @retval   0 DS18B20�豸����
  * @retval   1  DS18B20�豸��Ӧ��λ�ź�ʧ��
  * @retval   2  DS18B20�豸�ͷ�����ʧ��
*/
static uint8_t DS18B20_Check_Ready_Single(void)
{
	uint8_t cnt = 0;
    
	/* 1.���������� */
	DS18B20_InPut_Mode();
    
    //�ȴ�DS18B20 �������� ��60~240 us ��Ӧ��λ�źţ�
	while (DS18B20_In() && cnt < 240) {
		DS18B20_Delay_us(1);
		cnt++;
	}
    
	if (cnt > 240) {
        return 1;
    }
    
	/* 2.���DS18B20�Ƿ��ͷ����� */	
	cnt = 0;
	DS18B20_InPut_Mode();
    
    //�ж�DS18B20�Ƿ��ͷ����ߣ�60~240 us ��Ӧ��λ�ź�֮����ͷ����ߣ�
	while ((!DS18B20_In()) && cnt<240) {
		DS18B20_Delay_us(1);
		cnt++;
	}
    
	if (cnt > 240) {
        return 2;
    } else {
        return 0;
    }
}

/**
  * @brief    ���DS18B20�豸�Ƿ�����
  * @param    none
  * @retval   0 DS18B20�豸����
  * @retval   1  DS18B20�豸��Ӧ��λ�ź�ʧ��
  * @retval   2  DS18B20�豸�ͷ�����ʧ��
*/
static uint8_t DS18B20_Check_Device(void)
{
    /*1.�������͸�λ�ź�*/
	DS18B20_Send_Reset_Single();
    
    /*2.����������*/
	return DS18B20_Check_Ready_Single();
}
/**
  * @brief    DS18B20��ʼ��
  * @param    none
  * @retval   none
*/
void DS18B20_Init(void)
{
	/* 1.DS18B20�������ų�ʼ�� */
    //��main�������Ѿ���ʼ��������Ҫ�ٴ��ظ���

	/*2.���DS18B20�豸�Ƿ�����*/
	switch (DS18B20_Check_Device()) {
		case 0:	
            printf("DS18B20_Init OK!\n");
            break;
		case 1:
            printf("DS18B20�豸��Ӧ��λ�ź�ʧ�ܣ�\n");
            break;
		case 2:
            printf("DS18B20�豸�ͷ�����ʧ�ܣ�\n");
            break;
	}
}

/**
  * @brief    ��DS18B20дһ���ֽ�
  * @param    cmd Ҫд����ֽ�
  * @retval   none
*/
static uint8_t DS18B20_Write_Byte(uint8_t cmd)
{
	uint8_t i = 0;
    
    /* 1. ��������Ϊ���ģʽ */
	DS18B20_OutPut_Mode();
    
    /* 2. �������ݣ���λ��ǰ */
	for (i = 0; i < 8; i++) {
		DS18B20_Out(0);
		DS18B20_Delay_us(2);  
		DS18B20_Out(cmd & 0x01);
		DS18B20_Delay_us(60);
		DS18B20_Out(1);
		cmd >>= 1;
		DS18B20_Delay_us(2);
	}
    
	return 0;
}
 
/**
  * @brief    ��DS18B20��һ���ֽ�
  * @param    none
  * @retval   ��ȡ����һ���ֽ�����
*/
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i = 0;
    uint8_t data = 0;
    
    /* ��ȡ���� */
	for (i  =0; i < 8; i++)	{
		DS18B20_OutPut_Mode();
		DS18B20_Out(0);  
		DS18B20_Delay_us(2);
		DS18B20_Out(1);
        
		DS18B20_InPut_Mode();
		DS18B20_Delay_us(10);
		data >>= 1 ;
		if (DS18B20_In()) {
            data |= 0x80;
        }
        
		DS18B20_Delay_us(60);
		DS18B20_Out(1);
	}
    
	return data;
}

/**
  * @brief    ��DS18B20��ȡһ������
  * @param    none
  * @retval   ��ȡ�����¶�����
  * @note     ������������ֻ��һ��DS18B20�����
*/
uint16_t DS18B20_Read_Temperature(void)
{
    uint16_t temp = 0;
    uint8_t  temp_H, temp_L;

    DS18B20_Check_Device();
    
    DS18B20_Write_Byte(0xCC); //����ROM���м��
    DS18B20_Write_Byte(0x44); //����һ���¶�ת��
    
    //�ȴ��¶�ת�����
    while (DS18B20_Read_Byte() != 0xFF);
    
    DS18B20_Check_Device();   //���룬����ʡ��
    
    DS18B20_Write_Byte(0xCC); //����ROM���м��
    DS18B20_Write_Byte(0xBE); //��ȡ�¶�
    
    temp_L = DS18B20_Read_Byte(); //��ȡ���¶ȵ�λ����
    temp_H = DS18B20_Read_Byte(); //��ȡ���¶ȸ�λ����
    temp   = temp_L | (temp_H << 8);   //�ϳ��¶�

    return temp;
    
}
