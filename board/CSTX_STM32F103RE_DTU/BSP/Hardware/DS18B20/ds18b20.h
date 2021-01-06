/********************************************************
 *
 * @file    ds18b20.h
 * @brief   ds18b20����
 * @author  Mculover666
 * @date    2020/08/29
 * @note    ��������us����ʱʹ��DWT
 *
********************************************************/

#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "stm32f1xx.h"
#include "core_delay/core_delay.h"

/* DS18B20�������� */
#define DS18B20_GPIO_PORT     GPIOA
#define DS18B20_GPIO_PIN      GPIO_PIN_1

/* DS18B20����IOģʽ���� */
#define DS18B20_OutPut_Mode() {DS18B20_GPIO_PORT->CRL &= 0x0FFFFFFF;DS18B20_GPIO_PORT->CRL |= 0x30000000;}
#define DS18B20_InPut_Mode()  {DS18B20_GPIO_PORT->CRL &= 0x0FFFFFFF;DS18B20_GPIO_PORT->CRL |= 0x80000000;}

/* DS18B20����IO�������� */
#define DS18B20_Out(n)        (n?HAL_GPIO_WritePin(DS18B20_GPIO_PORT,DS18B20_GPIO_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(DS18B20_GPIO_PORT,DS18B20_GPIO_PIN,GPIO_PIN_RESET))
#define DS18B20_In()           HAL_GPIO_ReadPin(DS18B20_GPIO_PORT,DS18B20_GPIO_PIN)

/* DS18B20 us ����ʱ���� */
#define DS18B20_Delay_us(n)   CPU_TS_Tmr_Delay_US(n)

void DS18B20_Init(void);
uint16_t DS18B20_Read_Temperature(void);

 
#endif /* _DS18B20_H_ */
