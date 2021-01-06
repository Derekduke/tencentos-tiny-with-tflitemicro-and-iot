/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-02-15 20:51:38
 * @LastEditTime : 2020-02-16 00:06:40
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "bsp_dwt.h"
#include "stm32f1xx_hal.h"
/*
**********************************************************************
*         ʱ�����ؼĴ�������
**********************************************************************
*/
/*
 ��Cortex-M������һ�������DWT(Data Watchpoint and Trace)��
 ��������һ��32λ�ļĴ�����CYCCNT������һ�����ϵļ�������
 ��¼�����ں�ʱ�����еĸ�������ܼ�¼��ʱ��Ϊ��
 60s=2��32�η�/72000000
 (�����ں�Ƶ��Ϊ72M���ں���һ�ε�ʱ����Ϊ1/72M=13.8ns)
 ��CYCCNT���֮�󣬻���0���¿�ʼ���ϼ�����
 ʹ��CYCCNT�����Ĳ������裺
 1����ʹ��DWT���裬����������ں˵��ԼĴ���DEMCR��λ24���ƣ�д1ʹ��
 2��ʹ��CYCCNT�Ĵ���֮ǰ������0
 3��ʹ��CYCCNT�Ĵ����������DWT_CTRL(�����Ϻ궨��ΪDWT_CR)��λ0���ƣ�д1ʹ��
 */
 
#if USE_DWT_DELAY


#define  DWT_CR      *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/**
  * @brief  ��ʼ��ʱ���
  * @param  ��
  * @retval ��
  * @note   ʹ����ʱ����ǰ��������ñ�����
  */
void dwt_init(void)
{
    /* ʹ��DWT���� */
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

    /* DWT CYCCNT�Ĵ���������0 */
    DWT_CYCCNT = (uint32_t)0u;

    /* ʹ��Cortex-M DWT CYCCNT�Ĵ��� */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

/**
  * @brief  ��ȡ��ǰʱ���
  * @param  ��
  * @retval ��ǰʱ�������DWT_CYCCNT�Ĵ�����ֵ
  */
uint32_t dwt_read(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}

///**
//  * @brief  ��ȡ��ǰʱ���
//  * @param  ��
//  * @retval ��ǰʱ�������DWT_CYCCNT�Ĵ�����ֵ
//	* 				�˴���HAL���滻HAL_GetTick����������os
//  */
//uint32_t HAL_GetTick(void)
//{        
//    //�ȳ���ˣ���ֹ���
//  return ((uint32_t)DWT_CYCCNT / system_clk_freq * 1000);
//}

/**
  * @brief  ����CPU���ڲ�����ʵ�־�ȷ��ʱ��32λ������
  * @param  us : �ӳٳ��ȣ���λ1 us
  * @retval ��
  * @note   ʹ�ñ�����ǰ�����ȵ���dwt_init����ʹ�ܼ�������
            ��ʹ�ܺ�CPU_TS_INIT_IN_DELAY_FUNCTION
            �����ʱֵΪ8�룬��8*1000*1000
  */
void dwt_delay_us(__IO uint32_t us)
{
  uint32_t ticks;
  uint32_t told,tnow,tcnt=0;

  /* �ں����ڲ���ʼ��ʱ����Ĵ����� */  
#if (CPU_TS_INIT_IN_DELAY_FUNCTION)  
  /* ��ʼ��ʱ��������� */
  dwt_init();
#endif
  
  ticks = us * (get_cpu_clk_freq() / 1000000);  /* ��Ҫ�Ľ����� */      
  tcnt = 0;
  told = (uint32_t)dwt_read();         /* �ս���ʱ�ļ�����ֵ */

  while(1)
  {
    tnow = (uint32_t)dwt_read();  
    if(tnow != told)
    { 
        /* 32λ�������ǵ��������� */    
      if(tnow > told)
      {
        tcnt += tnow - told;  
      }
      /* ����װ�� */
      else 
      {
        tcnt += UINT32_MAX - told + tnow; 
      } 
      
      told = tnow;

      /*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
      if(tcnt >= ticks)break;
    }  
  }
}

#endif

