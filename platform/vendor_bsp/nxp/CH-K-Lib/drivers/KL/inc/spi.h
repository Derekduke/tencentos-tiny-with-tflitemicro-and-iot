#ifndef __CH_LIB_KL_SPI_H__
#define __CH_LIB_KL_SPI_H__

#include <stdint.h>
#include <stdbool.h>
  
//!< SPIģ���豸
#define HW_SPI0     (0x00)
#define HW_SPI1     (0x01)
#define HW_SPI2     (0x02)

//!< SPI֡��ʽѡ��
typedef enum
{
    kSPI_CPOL0_CPHA0,
    kSPI_CPOL0_CPHA1,
    kSPI_CPOL1_CPHA0,
    kSPI_CPOL1_CPHA1
}SPI_FrameFormat_Type;

typedef enum
{
    kSPI_Master,
    kSPI_Slave,
} SPI_Mode_Type;


//!< interrupt and DMA select
typedef enum
{
    kSPI_IT_Rx,            //!< SPI���յ�һ֡
    kSPI_IT_Tx,            //!< SPI������һ֡
}SPI_ITDMAConfig_Type;


#define kSPI_MSBFirst             (0x00)      //!< �ȷ������λ
#define kSPI_LSBFirst             (0x01)      //!< �ȷ������λ

//!< ��ʼ���ṹ
typedef struct
{
    uint32_t                instance;               //!< ģ���
	SPI_Mode_Type           mode;                   //!< ����ģʽ
    uint8_t                 dataSize;               //!< ÿ֡�����ж���λ ͨ��Ϊ8��16
    uint8_t                 bitOrder;               //!< �ȷ���λ�����ȷ���λ
    SPI_FrameFormat_Type    frameFormat;            //!< ����֡��ʽѡ��
    uint32_t                baudrate;               //!< ����
}SPI_InitTypeDef;

//!< ���ٳ�ʼ���ṹ
#define SPI1_SCK_PE02_MOSI_PE01_MISO_PE00  (0X0000C0A1U)
#define SPI0_SCK_PE17_MOSI_PE18_MISO_PE19  (0X0000E2A0U)
#define SPI0_SCK_PA15_MOSI_PA16_MISO_PA17  (0X0000DE80U)
#define SPI0_SCK_PC05_MOSI_PC06_MISO_PC07  (0X0000CA90U)
#define SPI0_SCK_PD01_MOSI_PD02_MISO_PD03  (0X0000C298U)
#define SPI1_SCK_PD05_MOSI_PD06_MISO_PD07  (0X0000CA99U)


//!< Callback Type
typedef void (*SPI_CallBackType)(void);

//!< API functions
void SPI_Init(SPI_InitTypeDef * SPI_InitStruct);
uint32_t SPI_QuickInit(uint32_t MAP, SPI_FrameFormat_Type frameFormat, uint32_t baudrate);
uint16_t SPI_ReadWriteByte(uint32_t instance, uint16_t data);
void SPI_ITDMAConfig(uint32_t instance, SPI_ITDMAConfig_Type config, bool status);
void SPI_CallbackInstall(uint32_t instance, SPI_CallBackType AppCBFun);



#endif

