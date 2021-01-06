/**
  ******************************************************************************
  * @file    lptmr.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
	
#ifndef __CH_LIB_KL_LPTMR_H__
#define __CH_LIB_KL_LPTMR_H__

#include <stdint.h>
#include <stdbool.h>

/* LPTMR �ص��������� */
typedef void (*LPTMR_CallBackType)(void);

/* LPTMR �ж�DMA���� */
typedef enum
{
    kLPTMR_IT_TOF,            /* ��������������ж� */
}LPTMR_ITDMAConfig_Type;

/* LPTMR ��ʱ������ ��ʼ�� */
typedef struct
{
    uint16_t        timeInMs;  //��ʱʱ��
}LPTMR_TC_InitTypeDef;

/* LPTMR �������Դѡ�� */
typedef enum
{
    kLPTMR_PC_InputSource_CMP0, /* CMP0 ��Ϊ�������ʱ��Դ */
    kLPTMR_PC_InputSource_ALT1, /* �ⲿ����LPTMR_ALT1��Ϊ�ⲿ����ʱ��Դ */
    kLPTMR_PC_InputSource_ALT2, /* �ⲿ����LPTMR_ALT2��Ϊ�ⲿ����ʱ��Դ */
}LPTMR_PC_IntputSource_Type;

/* LPTMR �ⲿ������Ϊ����ʱ ��������ѡ�� */
typedef enum
{
    kLPTMR_PC_PinPolarity_RigsingEdge,  /* �����ش������� */
    kLPTMR_PC_PinPolarity_FallingEdge,  /* �½��ش������� */
}LPTMR_PC_PinPolarity_Type;

/* LPTMR �����ⲿ��·���弼�� ��ʼ�� */
typedef struct
{
    uint16_t                        counterOverflowValue;   /* �����ۼ��������ֵ ���0xFFFF */
    LPTMR_PC_IntputSource_Type      inputSource;            /* ����Դѡ�� */
    LPTMR_PC_PinPolarity_Type       pinPolarity;            /* ���벶׽����ѡ�� */
}LPTMR_PC_InitTypeDef;

//!< ���ٳ�ʼ���ṹ ���ڵ�·�������
#define LPTMR_ALT1_PA19   (0x86780U)   /* ʹ��PTA19������Ϊ�������Դ */
#define LPTMR_ALT2_PC05   (0x104b10U)  /* ʹ��PTC05������Ϊ�������Դ */

//!< API functions
void LPTMR_TC_Init(LPTMR_TC_InitTypeDef* LPTMR_TC_InitStruct);
void LPTMR_PC_Init(LPTMR_PC_InitTypeDef* LPTMR_PC_InitStruct);
void LPTMR_ITDMAConfig(LPTMR_ITDMAConfig_Type config, bool status);
void LPTMR_CallbackInstall(LPTMR_CallBackType AppCBFun);
uint32_t LPTMR_PC_ReadCounter(void);
uint32_t LPTMR_PC_QuickInit(uint32_t MAP);
void LPTMR_ClearCounter(void);
void LPTMR_SetTime(uint32_t ms);

#endif

