#include "bh1750.h"
#include "stm32l4xx.h"
#include "i2c.h"


float result_lx;
uint8_t BUF[2];
int result;


/***************************************************************
* ��������: Init_BH1750
* ˵    ��: д�����ʼ��BH1750
* ��    ��: ��
* �� �� ֵ: ��
***************************************************************/
void BH1750_Init(void)
{
    uint8_t t_Data = 0x01;
    HAL_I2C_Master_Transmit(&hi2c1,BH1750_Addr,&t_Data,1,0xff);
}

/***************************************************************
* ��������: Start_BH1750
* ˵    ��: ����BH1750
* ��    ��: ��
* �� �� ֵ: ��
***************************************************************/
void BH1750_Start(void)
{
    uint8_t t_Data = 0x10;
    HAL_I2C_Master_Transmit(&hi2c1,BH1750_Addr,&t_Data,1,0xff); 
}



/***************************************************************
* ��������: Convert_BH1750
* ˵    ��: ��ֵת��
* ��    ��: ��
* �� �� ֵ: ��ǿֵ
***************************************************************/
float BH1750_Convert(void)
{
	BH1750_Start();
	HAL_Delay(180);
	HAL_I2C_Master_Receive(&hi2c1, BH1750_Addr+1,BUF,2,0xff); 
	result=BUF[0];
	result=(result<<8)+BUF[1];  //�ϳ����ݣ�����������	
	result_lx=(float)(result/1.2);
  return result_lx;
}

