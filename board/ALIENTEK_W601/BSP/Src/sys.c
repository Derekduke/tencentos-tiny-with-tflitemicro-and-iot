#include "sys.h"
#include "tos_config.h"
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


/**
 * @brief	ϵͳ���ú���
 *
 * @remark	�ú�����startup.s�е���,W601ϵͳʱ�ӳ�ʼ��Ϊ80MHz
 *
 * @param   void
 *
 * @return  void
 */
void SystemInit(void)
{
    tls_sys_clk sysclk;
    /*ϵͳʱ������Ϊ80MHz*/
    tls_sys_clk_set(CPU_CLK_80M);

    /*�ر�����IO�ĸ��ù��ܣ�ֻ����SWD���ù���*/
    wm_gpio_af_disable();
    tls_sys_clk_get(&sysclk);
    SysTick_Config(sysclk.cpuclk * UNIT_MHZ / TOS_CFG_CPU_TICK_PER_SECOND);
    /*�ж����ȼ�������*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /*�������˯�ߵ͹���ģʽ*/
    NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP, ENABLE);

    /*ѡ��PMUʱ��*/
    tls_pmu_clk_select(0);
}


/**
 * @brief	THUMBָ�֧�ֻ��������
 *			�������·���ʵ��ִ�л��ָ��WFI
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void WFI_SET(void)
{
    WFI;
}
/**
 * @brief	�ر������ж�(���ǲ�����fault��NMI�ж�)
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR
}
/**
 * @brief	���������ж�
 *
 * @param   void
 *
 * @return  __asm
 */
__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR
}
/**
 * @brief	����ջ����ַ
 *
 * @param   addr	ջ����ַ
 *
 * @return  __asm
 */
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
















