#include "sys.h"
#include "sccb.h"
#include "stdio.h"
#include "delay.h"

void SCCB_SCL(uint8_t sccb_scl)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (GPIO_PinState)sccb_scl);
}

void SCCB_SDA(uint8_t sccb_sda)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (GPIO_PinState)sccb_sda);
}

uint8_t SCCB_READ_SDA()
{
	uint8_t sccb_sda = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
	return sccb_sda;
}

//��ʼ��SCCB�ӿ� 
void SCCB_Init(void)
{				
  GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //ʹ��GPIOBʱ��
  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.Pin = GPIO_PIN_4|GPIO_PIN_5;//PB4,5 �������
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //PB4,5 �������
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;//100MHz
  GPIO_InitStructure.Pull = GPIO_PULLUP;//����
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	SCCB_SDA_OUT();	   
}			 

//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void SCCB_Start(void)
{
    SCCB_SDA(1);     //�����߸ߵ�ƽ	   
    SCCB_SCL(1);	    //��ʱ���߸ߵ�ʱ���������ɸ�����
    delay_us(50);  
    SCCB_SDA(0);
    delay_us(50);	 
    SCCB_SCL(0);	    //�����߻ָ��͵�ƽ��������������Ҫ	  
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void SCCB_Stop(void)
{
    SCCB_SDA(0);
    delay_us(50);	 
    SCCB_SCL(1);	
    delay_us(50); 
    SCCB_SDA(1);	
    delay_us(50);
}  
//����NA�ź�
void SCCB_No_Ack(void)
{
	delay_us(50);
	SCCB_SDA(1);	
	SCCB_SCL(1);	
	delay_us(50);
	SCCB_SCL(0);	
	delay_us(50);
	SCCB_SDA(0);	
	delay_us(50);
}
//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
uint8_t SCCB_WR_Byte(uint8_t dat)
{
	uint8_t j,res;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if(dat&0x80)SCCB_SDA(1);	
		else SCCB_SDA(0);
		dat<<=1;
		delay_us(50);
		SCCB_SCL(1);	
		delay_us(50);
		SCCB_SCL(0);		   
	}			 
	SCCB_SDA_IN();		//����SDAΪ���� 
	delay_us(50);
	SCCB_SCL(1);			//���յھ�λ,���ж��Ƿ��ͳɹ�
	delay_us(50);
	if(SCCB_READ_SDA())res=1;  //SDA=1����ʧ�ܣ�����1
	else res=0;         //SDA=0���ͳɹ�������0
	SCCB_SCL(0);		 
	SCCB_SDA_OUT();		//����SDAΪ���    
	return res;  
}	 
//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
uint8_t SCCB_RD_Byte(void)
{
	uint8_t temp=0,j;    
	SCCB_SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		delay_us(50);
		SCCB_SCL(1);
		temp=temp<<1;
		if(SCCB_READ_SDA())temp++;   
		delay_us(50);
		SCCB_SCL(0);
	}	
	SCCB_SDA_OUT();		//����SDAΪ���    
	return temp;
} 							    
//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start(); 					//����SCCB����
	if(SCCB_WR_Byte(SCCB_ID)){
		res=1;	//д����ID
	}
	delay_us(100);
  if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����ַ	  
	delay_us(100);
  if(SCCB_WR_Byte(data))res=1; 	//д����	 
  SCCB_Stop();	  
  return	res;
}		  					    
//���Ĵ���
//����ֵ:�����ļĴ���ֵ
uint8_t SCCB_RD_Reg(uint8_t reg)
{
	uint8_t val=0;
	SCCB_Start(); 				//����SCCB����
	SCCB_WR_Byte(SCCB_ID);		//д����ID	  
	delay_us(100);	 
  SCCB_WR_Byte(reg);			//д�Ĵ�����ַ	  
	delay_us(100);	  
	SCCB_Stop();   
	delay_us(100);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0X01);	//���Ͷ�����	  
	delay_us(100);
  val=SCCB_RD_Byte();		 	//��ȡ����
  SCCB_No_Ack();
  SCCB_Stop();
  return val;
}














