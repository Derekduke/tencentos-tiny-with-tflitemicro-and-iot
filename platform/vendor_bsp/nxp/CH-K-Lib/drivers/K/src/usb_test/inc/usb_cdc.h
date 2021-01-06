#ifndef __USB_CDC_H__
#define __USB_CDC_H__
#include <stdint.h>

//USB_CDC�豸״̬
#define WAITING_FOR_ENUMERATION  0x00
//USB setup�ӿ��������
#define SET_LINE_CODING          0x20
#define GET_LINE_CODING          0x21
#define SET_CONTROL_LINE_STATE   0x22
#define LOADER_MODE              0xAA
#define GET_INTERFACE            0x0A
#define SERIAL_STATE             0x20

/* Defines */
#define EP_OUT          3
#define EP_IN           2   


#define CDC_INPointer   BufferPointer[bEP02IN_ODD]
#define CDC_OUTPointer  BufferPointer[bEP3OUT_ODD]
//����CDCֱ��ͨ�Žṹ��
typedef struct
{
    uint32_t  DTERate;
    uint8_t   CharFormat;
    uint8_t   ParityType;
    uint8_t   Databits;
}CDC_Line_Coding;




//������ʵ�ֵĽӿں���
void USB_CDC_Init(void);
#endif

