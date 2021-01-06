/**
  ******************************************************************************
  * @file    lptmr.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "lptmr.h"
#include "common.h"

/* �ص����� ָ�� */

/* �ж�������� */
static const IRQn_Type LPTMR_IRQnTable[] = 
{
    (IRQn_Type)28,
};

/**
 * @brief  ��ʼ������LPTMģ�鴦�ڼ�ʱ��ģʽ
 * @code
 *      //����LPTM�����ڼ�ʱ��ģʽ��ʱ������500����
 *      LPTMR_TC_InitTypeDef LPTMR_TC_InitStruct1; //����һ���ṹ����
 *      LPTMR_TC_InitStruct1.timeInMs = 500;       //���ü�ʱʱ����500ms
 *      LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
 * @endcode
 * @param  LPTMR_TC_InitStruct: �������ýṹ��
 *         @arg timeInMs :��ʱʱ�� ��λΪms
 * @retval None
 */
void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct)
{
	/* open clock gate */
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; 
    LPTMR0->CSR = 0x00; 
    LPTMR0->PSR = 0x00;
    LPTMR0->CMR = 0x00;
    
    /* disable module first */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* free counter will reset whenever compare register is writtened. */
    LPTMR0->CSR &= ~LPTMR_CSR_TFC_MASK;  
    
    /* timer counter mode */
    LPTMR0->CSR &= ~LPTMR_CSR_TMS_MASK; 
    
	/* bypass the prescaler, which mean we use 1KHZ LPO directly */
    LPTMR0->PSR = LPTMR_PSR_PCS(1)| LPTMR_PSR_PBYP_MASK; 
    
    /* set CMR(compare register) */
    LPTMR0->CMR = LPTMR_CMR_COMPARE(LPTMR_TC_InitStruct->timeInMs);
    
    /* enable moudle */
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;  
}

/**
 * @brief  ��ʼ������LPTMģ�鴦���������ģʽ
 * @code
 *      //����LPTM�������������ģʽ������������0xFFFE
 *      LPTMR_PC_InitTypeDef LPTMR_PC_InitStruct1; //����һ���ṹ����
 *      LPTMR_PC_InitStruct1.timeInMs = 500;       //���ü�ʱʱ����500ms
 *      LPTMR_TC_Init(&LPTMR_TC_InitStruct1);
 * @endcode
 * @param  LPTMR_PC_InitTypeDef: �������ýṹ��
 *         @arg counterOverflowValue       :�������������ޣ�����Ϊ0xFFFF
 *         @arg inputSource :����Դѡ�� kLPTMR_PC_InputSource_CMP0-CMP0��Ϊ�������ʱ��Դ kLPTMR_PC_InputSource_ALT1-�ⲿ����LPTMR_ALT1��Ϊ�ⲿ����ʱ��Դ kLPTMR_PC_InputSource_ALT2-�ⲿ����LPTMR_ALT2��Ϊ�ⲿ����ʱ��Դ
 *         @arg pinPolarity :�����������ѡ�� kLPTMR_PC_PinPolarity_RigsingEdge �����ؼ��� kLPTMR_PC_PinPolarity_FallingEdge �½��ؼ���
 * @retval None
 */
void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct)
{
	/* open clock gate */
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; 
    LPTMR0->CSR = 0x00; 
    LPTMR0->PSR = 0x00;
    LPTMR0->CMR = 0x00;
    
    /* disable module first */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    
    /* free counter will reset whenever compare register is writtened. */
    LPTMR0->CSR &= ~LPTMR_CSR_TFC_MASK;  
    
    /* timer counter mode */
    LPTMR0->CSR |= LPTMR_CSR_TMS_MASK; 
    
	/* bypass the glitch filter, which mean we use 1KHZ LPO directly */
    LPTMR0->PSR = LPTMR_PSR_PCS(1)| LPTMR_PSR_PBYP_MASK;
    
    /* set CMR(compare register) */
    LPTMR0->CMR = LPTMR_CMR_COMPARE(LPTMR_PC_InitStruct->counterOverflowValue);
    
    /* input source */
    switch(LPTMR_PC_InitStruct->inputSource)
    {
        case kLPTMR_PC_InputSource_CMP0:
            LPTMR0->CSR |= LPTMR_CSR_TPS(0);
            break;
        case kLPTMR_PC_InputSource_ALT1:
            LPTMR0->CSR |= LPTMR_CSR_TPS(1);
            break;
        case kLPTMR_PC_InputSource_ALT2:
            LPTMR0->CSR |= LPTMR_CSR_TPS(2);
            break; 
        default:
            break;
    }
    
    /* pin polarity */
    switch(LPTMR_PC_InitStruct->pinPolarity)
    {
        case kLPTMR_PC_PinPolarity_RigsingEdge:
            LPTMR0->CSR &= ~LPTMR_CSR_TPP_MASK;
            break;
        case kLPTMR_PC_PinPolarity_FallingEdge:
            LPTMR0->CSR |= LPTMR_CSR_TPP_MASK;
            break;
        default:
            break;
    }
    
    /* enable moudle */
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; 
}

/**
 * @brief  LPTMģ���жϺ�DMA��������
 * @code
 *     //����LPTMģ���������ж�
 *     LPTMR_ITDMAConfig(kLPTMR_IT_TOF, true);
 * @endcode
 * @param  LPTMR_ITDMAConfig_Type: LPTM�ж�����
 *         @arg kLPTMR_IT_Disable  :�ر��ж�
 *         @arg kLPTMR_IT_TOF      :������������ж�
 * @retval None
 */
void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config, bool status)
{
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; 
    
    if(status)
    {
        NVIC_EnableIRQ(LPTMR_IRQnTable[0]);
    }

    switch (config)
    {
        case kLPTMR_IT_TOF:
            (status)?
            (LPTMR0->CSR |= LPTMR_CSR_TIE_MASK):
            (LPTMR0->CSR &= ~LPTMR_CSR_TIE_MASK);
            break;
        default:
            break;
    }
}

/**
 * @brief  ע���жϻص�����
 * @param  AppCBFun: �ص�����ָ�����
 * @retval None
 * @note ���ڴ˺����ľ���Ӧ�������Ӧ��ʵ��
 */


/**
 * @brief  ��ȡ�����������������
 * @code
 *     //��ȡ��������ĸ���
 *     uint32_t counter;
 *     counter = LPTMR_PC_ReadCounter();
 * @endcode
 * @param  None
 * @retval �����������
 */
uint32_t LPTMR_PC_ReadCounter(void)
{
    /* must first read */
    uint32_t *p = (uint32_t *)&LPTMR0->CNR;
    *p = 0x1234;
	return (uint32_t)((LPTMR0->CNR & LPTMR_CNR_COUNTER_MASK) >> LPTMR_CNR_COUNTER_SHIFT); 
}

void LPTMR_SetTime(uint32_t ms)
{
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK; 
    LPTMR0->CMR = LPTMR_CMR_COMPARE(ms);
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK; 
}

uint32_t LPTMR_PC_QuickInit(uint32_t MAP)
{
    uint32_t i;
    map_t * pq = (map_t*)&(MAP);
    LPTMR_PC_InitTypeDef LPTMR_PC_InitStruct1;
    LPTMR_PC_InitStruct1.counterOverflowValue = 0xFFFF;
    switch(pq->chl)
    {
        case 1:
            LPTMR_PC_InitStruct1.inputSource = kLPTMR_PC_InputSource_ALT1;
            break;
        case 2:
            LPTMR_PC_InitStruct1.inputSource = kLPTMR_PC_InputSource_ALT2;
            break;
        default:
            break;
    }
    LPTMR_PC_InitStruct1.pinPolarity = kLPTMR_PC_PinPolarity_RigsingEdge;
    /* init pinmux */
    for(i = 0; i < pq->pin_count; i++)
    {
        SetPinMux(pq->io, pq->pin_start + i, pq->mux); 
    }
    
    /* init moudle */
    LPTMR_PC_Init(&LPTMR_PC_InitStruct1);
    return pq->ip;
}

/**
 * @brief  ��������������������
 * @code
 *     //�����������ĸ���
 *     LPTMR_ClearCounter();
 * @endcode
 * @param  None
 * @retval None
 */
void LPTMR_ClearCounter(void)
{
    /* disable and reenable moudle to clear counter */
    LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}

/**
 * @brief  �жϴ��������
 * @param  LPTimer_IRQHandler :LPTM�жϴ�����
 * @note �����ڲ������ж��¼�����
 */
//void LPTimer_IRQHandler(void)
//{
//    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

//}


#if 0
static const QuickInit_Type LPTMR_QuickInitTable[] = 
{
    { 0, 0, 6,19, 1, 1}, //LPTMR_ALT1_PA19 6
    { 0, 2, 4, 5, 1, 2}, //LPTMR_ALT2_PC05 4
};
#endif

