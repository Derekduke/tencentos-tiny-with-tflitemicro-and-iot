#ifndef __SYS_H
#define __SYS_H

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK W601������
 *	ϵͳʱ�ӳ�ʼ��
 *	����ʱ������/�жϹ����
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2019/7/10
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2019-2029
 *	All rights reserved
 *	******************************************************************************
 *	�汾�޸�˵��
 *	��
 *	******************************************************************************/


#include "misc.h"
#include "wm_type_def.h"
#include "wm_cpu.h"
#include "wm_ram_config.h"
#include "wm_gpio.h"
#include "wm_regs.h"
#include "wm_gpio_afsel.h"
#include "wm_uart_new.h"
#include "wm_uart.h"
#include "wm_pmu.h"




//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).M4ͬM3����,ֻ�ǼĴ�����ַ����.
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))


//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��32!
#define PAout(n)   BIT_ADDR(HR_GPIOA_DATA,n)  //��� 
#define PAin(n)    BIT_ADDR(HR_GPIOA_DATA,n)  //���� 

#define PBout(n)   BIT_ADDR(HR_GPIOB_DATA,n)  //��� 
#define PBin(n)    BIT_ADDR(HR_GPIOB_DATA,n)  //���� 


//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ

#endif











