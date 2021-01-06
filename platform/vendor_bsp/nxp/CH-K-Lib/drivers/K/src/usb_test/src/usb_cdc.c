#include "usb_cdc.h"
#include "usb.h"
#include "sys.h"
CDC_Line_Coding LineCoding;  //ʵ����LINE_CODEING �ṹ��

//CDC�ӿ� �����ж�
uint8_t USB_CDC_InterfaceReq_Handler(void)
{
	switch(Setup_Pkt->bRequest)
	{        
		case GET_LINE_CODING:
			USBD_EPWrite(EP0,(uint8_t*)&LineCoding,7);
		break;
		case SET_LINE_CODING:
			if(BIT_CHK(EP0,guint8_tUSB_Toogle_flags))//
			{
				BIT_CLR(EP0,guint8_tUSB_Toogle_flags);
				EP_OUT_Transfer(EP0,(uint8_t*)&LineCoding);
				USBD_EPWrite(EP0, 0, 0);       
			}
		break;
		case SET_CONTROL_LINE_STATE:
			USBD_EPWrite(EP0,0,0);//
			break;
		case LOADER_MODE:
          //  Buffer_Init(CDC_OUT_Data,CDC_BUFFER_SIZE);
         //   FLAG_SET(LOADER,guint8_tUSB_Flags);
         //   CDC_OUT_Data[0]=0xFF;
		break;
       default:break;
    }
}

//USB CDC��������
void USB_CDCComTx(uint8_t *tx_buf,uint8_t len)
{
   USBD_EPWrite(EP2,tx_buf,len);
}

/*
*  LPLD_USB_VirtualCom_Rx
*    ��USB CDC��ģʽ����ɴ���ģʽ���ú����Ǵ��ڽ��պ���
*    ����
*      |__*rx_buf ָ���û����ݴ洢�������ڴ�����յ�������
*    ����ֵ���������ݵĳ��ȣ����ֽ�Ϊ��λ
*/
uint8_t USB_CDCComRx(uint8_t *rx_buf)
{
  uint8_t len;
  uint8_t temp=0;
  uint8_t *puint8_tEPBuffer;
  if(BIT_CHK(EP_OUT,guint8_tUSB_Flags))// ��������ݵ���
  {
		len = USB_EP_OUT_SizeCheck(EP_OUT); 
		temp = len;
		puint8_tEPBuffer = CDC_OUTPointer; //ָ���Ӧ�� ������
		//��ʼ����
		while(len--) *rx_buf++=*puint8_tEPBuffer++;  
	
    usbEP_Reset(EP_OUT);
    usbSIE_CONTROL(EP_OUT);
    BIT_CLR(EP_OUT,guint8_tUSB_Flags);
		}
  return temp;
}


static uint32_t LWordSwap(uint32_t uint32_tDataSwap)
{
    uint32_t uint32_tTemp;
    uint32_tTemp= (uint32_tDataSwap & 0xFF000000) >> 24;
    uint32_tTemp+=(uint32_tDataSwap & 0xFF0000)   >> 8;
    uint32_tTemp+=(uint32_tDataSwap & 0xFF00)     << 8;
    uint32_tTemp+=(uint32_tDataSwap & 0xFF)       << 24;
    return(uint32_tTemp);    
}
void USB_CDC_Init(void)
{
  /*��ʼ��CDC��ͨѶ�Ĳ���*/
  LineCoding.DTERate=LWordSwap(115200);
  LineCoding.CharFormat=0;
  LineCoding.ParityType=0;
  LineCoding.Databits=0x08;
  /*��ʼ��CDC�����ݻ�����*/
 // Buffer_Init(CDC_OUT_Data,CDC_BUFFER_SIZE);
}
