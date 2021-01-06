#include "usb_desc.h"
#include "usb.h"

#define VENDOR_ID               (0x1234)
#define PRODUCT_ID              (0x5678)
#define PRODUCT_RELEASE         (0x1234)
    
static uint8_t DeviceSesc[]= 
{
        DEVICE_DESCRIPTOR_LENGTH,       /* bLength */
        DEVICE_DESCRIPTOR,              /* bDescriptorType */
        LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        (uint8_t)(LSB(VENDOR_ID)),                 /* idVendor (LSB) */
        (uint8_t)(MSB(VENDOR_ID)),                 /* idVendor (MSB) */
        (uint8_t)(LSB(PRODUCT_ID)),                /* idProduct (LSB) */
        (uint8_t)(MSB(PRODUCT_ID)),                /* idProduct (MSB) */
        (uint8_t)(LSB(PRODUCT_RELEASE)),           /* bcdDevice (LSB) */
        (uint8_t)(MSB(PRODUCT_RELEASE)),           /* bcdDevice (MSB) */
        STRING_OFFSET_IMANUFACTURER,    /* iManufacturer */
        STRING_OFFSET_IPRODUCT,         /* iProduct */
        STRING_OFFSET_ISERIAL,          /* iSerialNumber */
        0x01                            /* bNumConfigurations */
};

static uint8_t Report_Descriptor[]=
{
 //ÿ�п�ʼ�ĵ�һ�ֽ�Ϊ����Ŀ��ǰ׺��ǰ׺�ĸ�ʽΪ��
 //D7~D4��bTag��D3~D2��bType��D1~D0��bSize�����·ֱ��ÿ����Ŀע�͡�
 //����һ��ȫ�֣�bTypeΪ1����Ŀ��ѡ����;ҳΪ��ͨ����Generic Desktop Page(0x01)
 //�����һ�ֽ����ݣ�bSizeΪ1����������ֽ����Ͳ�ע���ˣ�
 //�Լ�����bSize���жϡ�
 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 //����һ���ֲ���bTypeΪ2����Ŀ��˵����������Ӧ�ü�����;�������
 0x09, 0x02, // USAGE (Mouse)
 //����һ������Ŀ��bTypeΪ0����Ŀ�������ϣ������������0x01��ʾ
 //�ü�����һ��Ӧ�ü��ϡ�����������ǰ������;ҳ����;����Ϊ
 //��ͨ�����õ���ꡣ
 0xa1, 0x01, // COLLECTION (Application)
 //����һ���ֲ���Ŀ��˵����;Ϊָ�뼯��
 0x09, 0x01, //   USAGE (Pointer)
 //����һ������Ŀ�������ϣ������������0x00��ʾ�ü�����һ��
 //�����ϣ���;��ǰ��ľֲ���Ŀ����Ϊָ�뼯�ϡ�
 0xa1, 0x00, //   COLLECTION (Physical)
 //����һ��ȫ����Ŀ��ѡ����;ҳΪ������Button Page(0x09)��
 0x05, 0x09, //     USAGE_PAGE (Button)
 //����һ���ֲ���Ŀ��˵����;����СֵΪ1��ʵ��������������
 0x19, 0x01, //     USAGE_MINIMUM (Button 1)
 //����һ���ֲ���Ŀ��˵����;�����ֵΪ3��ʵ����������м���
 0x29, 0x03, //     USAGE_MAXIMUM (Button 3)
 //����һ��ȫ����Ŀ��˵�����ص����ݵ��߼�ֵ���������Ƿ��ص��������ֵ����
 //��СΪ0����Ϊ����������Bit����ʾһ�������������СΪ0�����Ϊ1��
 0x15, 0x00, //     LOGICAL_MINIMUM (0)
 //����һ��ȫ����Ŀ��˵���߼�ֵ���Ϊ1��
 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
 //����һ��ȫ����Ŀ��˵�������������Ϊ������
 0x95, 0x03, //     REPORT_COUNT (3)
 //����һ��ȫ����Ŀ��˵��ÿ��������ĳ���Ϊ1��bit��
 0x75, 0x01, //     REPORT_SIZE (1)
 //����һ������Ŀ��˵����3������Ϊ1bit�������������ͳ���
 //��ǰ�������ȫ����Ŀ�����壩������Ϊ���룬
 //����Ϊ��Data,Var,Abs��Data��ʾ��Щ���ݿ��Ա䶯��Var��ʾ
 //��Щ�������Ƕ����ģ�ÿ�����ʾһ����˼��Abs��ʾ����ֵ��
 //��������Ľ�����ǣ���һ��������bit0��ʾ����1��������Ƿ��£�
 //�ڶ���������bit1��ʾ����2���Ҽ����Ƿ��£�������������bit2��ʾ
 //����3���м����Ƿ��¡�
 0x81, 0x02, //     INPUT (Data,Var,Abs)
 //����һ��ȫ����Ŀ��˵������������Ϊ1��
 0x95, 0x01, //     REPORT_COUNT (1)
 //����һ��ȫ����Ŀ��˵��ÿ��������ĳ���Ϊ5bit��
 0x75, 0x05, //     REPORT_SIZE (5)
 //����һ������Ŀ�������ã���ǰ������ȫ����Ŀ��֪������Ϊ5bit��
 //����Ϊ1������������Ϊ�����������ص�����һֱ��0����
 //���ֻ��Ϊ�˴���һ���ֽڣ�ǰ������3��bit��������һЩ����
 //���ѣ���������û��ʵ����;�ġ�
 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
 //����һ��ȫ����Ŀ��ѡ����;ҳΪ��ͨ����Generic Desktop Page(0x01)
 0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
 //����һ���ֲ���Ŀ��˵����;ΪX��
 0x09, 0x30, //     USAGE (X)
 //����һ���ֲ���Ŀ��˵����;ΪY��
 0x09, 0x31, //     USAGE (Y)
 //����һ���ֲ���Ŀ��˵����;Ϊ����
 0x09, 0x38, //     USAGE (Wheel)
 //��������Ϊȫ����Ŀ��˵�����ص��߼���С�����ֵ��
 //��Ϊ���ָ���ƶ�ʱ��ͨ���������ֵ����ʾ�ģ�
 //���ֵ����˼���ǣ���ָ���ƶ�ʱ��ֻ�����ƶ�����
 //�����ƶ�ʱ��XֵΪ���������ƶ�ʱ��YֵΪ����
 //���ڹ��֣����������Ϲ�ʱ��ֵΪ����
 0x15, 0x81, //     LOGICAL_MINIMUM (-127)
 0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
 //����һ��ȫ����Ŀ��˵��������ĳ���Ϊ8bit��
 0x75, 0x08, //     REPORT_SIZE (8)
 //����һ��ȫ����Ŀ��˵��������ĸ���Ϊ3����
 0x95, 0x03, //     REPORT_COUNT (3)
 //����һ������Ŀ����˵��������8bit���������������õģ�
 //����Ϊ��Data,Var,Rel��Data˵�������ǿ��Ա�ģ�Var˵��
 //��Щ�������Ƕ����ģ�����һ��8bit��ʾX�ᣬ�ڶ���8bit��ʾ
 //Y�ᣬ������8bit��ʾ���֡�Rel��ʾ��Щֵ�����ֵ��
 0x81, 0x06, //     INPUT (Data,Var,Rel)
 //��������������Ŀ�����ر�ǰ��ļ����á�
 //���ǿ����������ϣ�����Ҫ�����Ρ�bSizeΪ0�����Ժ���û���ݡ�
 0xc0,       //   END_COLLECTION
 0xc0        // END_COLLECTION
};
//ͨ������ı����������Ķ��壬����֪�����ص����뱨�����4�ֽڡ�
//��һ�ֽڵĵ�3λ������ʾ�����Ƿ��µģ���5λΪ����0�����á�
//�ڶ��ֽڱ�ʾX��ĵı����������ֽڱ�ʾY��ĸı����������ֽڱ�ʾ
//���ֵĸı������������ж϶˵�1��Ӧ��Ҫ��������ĸ�ʽ����ʵ�ʵ�
//������ݡ�
///////////////////////////�������������////////////////////////////
//�ַ���������0

static uint8_t LangIDDesc[] = 
{
        0x04,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        0x09,0x04,          /*bString Lang ID - 0x0409 - English*/
};

static uint8_t stringImanufacturerDesc[]=
{
        0x12,                                            /*bLength*/
        STRING_DESCRIPTOR,                               /*bDescriptorType 0x03*/
        'm',0,'b',0,'e',0,'d',0,'.',0,'o',0,'r',0,'g',0, /*bString iManufacturer - mbed.org*/
};

static uint8_t stringIproductDesc[]=
{
        0x16,                                                       /*bLength*/
        STRING_DESCRIPTOR,                                          /*bDescriptorType 0x03*/
        'U',0,'S',0,'B',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 /*bString iProduct - USB DEVICE*/
};


static uint8_t qualiferDesc[]=
{
        0x0A,
        0x06,
        LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        0x01,                            /* bNumConfigurations */    
        0x00
};
    
//�ַ���������3
static uint8_t stringIserialDesc[] = 
{
        0x16,                                                           /*bLength*/
        STRING_DESCRIPTOR,                                              /*bDescriptorType 0x03*/
        '0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,    /*bString iSerial - 0123456789*/
};	 


//����������
static uint8_t Configuration_Descriptor[9+9+9+7+7]= 
{
	//����������
	0x09,		    //bLength�������������ĳ��ȣ�9�ֽ�
	0x02,		    //bDescriptorType��0x02��ʾ��������Ϊ����������
	sizeof(Configuration_Descriptor)&0xFF,(sizeof(Configuration_Descriptor)>>8)&0xFF,	    //wTotalLength��������Ϣ���ܳ��ȣ��������á��ӿںͶ˵㣩��32�ֽ�
	0x01,		    //bNumInterfaces�򣬸�������֧�ֵĽӿ���������һ������1
	0x01,		    //bConfigurationValue������ֵ��1 
	0x00,		    //iConfiguration�������ַ���������������0
	0x80,		    //bmAttibutes�����õ�����(�������߹��硢�Թ��缰���̻��ѵ�����) 
	                //λ7��1-����Ϊ1��λ6��0-�Թ��磻λ5��0-��֧��Զ�̻���
	0x32,		    //MaxPower���豸��������ȡ����������2mAΪ��λ��50*2mA��100mA

	//�ӿ�������
	0x09,           //bLength�򣬽ӿ����������ȣ�9�ֽ�
	0x04,           //bDescriptorType��0x04��ʾ��������Ϊ�ӿ�������
	0x00,           //bInterfaceNumber�򣬽ӿں�
	0x00,           //bAlternateSetting�򣬽ӿڵĿ��滻����ֵ
	0x02,           //bNumEndpoints�򣬽ӿ�ʹ�õĶ˵��������˵�0����2
	0x03,           //bInterfaceClass�򣬽ӿ�������USB�豸�ࣺ0xFF��ʾ��Ӧ���Զ���
	0x02,           //bInterfaceSubClass�򣬽ӿ�������USB�豸���ࣺ0xFF��ʾ��Ӧ���Զ���
	0x01,           //bInterfaceProtocol�򣬽ӿڲ��õ�USB�豸��Э�飺0xFF��ʾ��Ӧ���Զ���
	0x00,           //iInterface�򣬽ӿ��ַ�����������������0

 /******************HID������************************/
 //bLength�ֶΡ���HID��������ֻ��һ���¼������������Գ���Ϊ9�ֽڡ�
 0x09,
 //bDescriptorType�ֶΡ�HID�������ı��Ϊ0x21��
 0x21,
 //bcdHID�ֶΡ���Э��ʹ�õ�HID2.0Э�顣ע����ֽ����ȡ�
    LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
    MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
 //bCountyCode�ֶΡ��豸���õĹ��Ҵ��룬����ѡ��Ϊ����������0x21��
 0x21,
 //bNumDescriptors�ֶΡ��¼�����������Ŀ������ֻ��һ��������������
 0x01,
 //bDescriptorType�ֶΡ��¼������������ͣ�Ϊ���������������Ϊ0x22��
 0x22,
 //bDescriptorLength�ֶΡ��¼��������ĳ��ȡ��¼�������Ϊ������������
 sizeof(Report_Descriptor)&0xFF,
 (sizeof(Report_Descriptor)>>8)&0xFF,  
  
	 //�˵�OUT������
	0x07,           //bLength�򣬶˵����������ȣ�7�ֽ�
	0x05,           //bDescriptorType��0x05��ʾ��������Ϊ�˵�������
	0x82,           //bEndpointAddress�򣬶˵�źʹ��䷽�򣺶˵�2��IN
	0x03,           //bmAttributes�򣬶˵����ԣ����ݶ˵㡢�鴫��
	0x40,0x00,      //wMaxPacketSize�򣬶˵�֧��������ݰ����ȣ�32�ֽ�          
	0x64,           //bInterval����ѯ�������msΪ��λ��

	 //�˵�IN������
	0x07,           //bLength�򣬶˵����������ȣ�7�ֽ�
	0x05,           //bDescriptorType��0x05��ʾ��������Ϊ�˵�������
	0x03,           //bEndpointAddress�򣬶˵�źʹ��䷽�򣺶˵�3��OUT   
	0x03,           //bmAttributes�򣬶˵����ԣ����ݶ˵㡢�鴫��
	0x40,0x00,      //wMaxPacketSize�򣬶˵�֧��������ݰ����ȣ�32�ֽ�        
	0x64,           //bInterval����ѯ�������msΪ��λ��
};



uint8_t* USBD_GetStdDesc(SETUP_PACKET *packet, uint32_t *len)
{
    uint8_t *p;
    
    *len = 0;
    p = 0;
    
    switch(DESCRIPTOR_TYPE(packet->wValue))
	{
		case DEVICE_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("device desc\r\n"));
            p = (uint8_t*)DeviceSesc;
            *len = sizeof(DeviceSesc);
			break;
        
		case CONFIGURATION_DESCRIPTOR:
			USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("config desc\r\n"));
            p = (uint8_t*)Configuration_Descriptor;
            *len = sizeof(Configuration_Descriptor);
			break;
        
		case STRING_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc:"));
            switch (DESCRIPTOR_INDEX(packet->wValue))
			{
				case STRING_OFFSET_LANGID:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("language\r\n"));
                    p = (uint8_t*)LangIDDesc;
                    *len = sizeof(LangIDDesc);
					break;
				case STRING_OFFSET_IMANUFACTURER:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("vender\r\n"));
                    p = (uint8_t*)stringImanufacturerDesc;
                    *len = sizeof(stringImanufacturerDesc);
					break;
				case STRING_OFFSET_IPRODUCT:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("vender string\r\n"));
                    p = (uint8_t*)stringIproductDesc;
                    *len = sizeof(stringIproductDesc);
					break;
				case STRING_OFFSET_ISERIAL:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("vender serail number\r\n"));
                    p = (uint8_t*)stringIserialDesc;
                    *len = sizeof(stringIserialDesc);
					break;
				case STRING_OFFSET_ICONFIGURATION:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("congiuration\r\n"));
					break;
				case STRING_OFFSET_IINTERFACE:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("interface\r\n"));
					break;
				default: 
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("unknow:%d\r\n",DESCRIPTOR_INDEX(packet->wValue)));
					break; 
			}
			break;
        case REPORT_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report desc\r\n"));
            p = (uint8_t*)Report_Descriptor;
            *len = sizeof(Report_Descriptor);
            break;
        case QUALIFIER_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report qualifer\r\n"));
            p = (uint8_t*)qualiferDesc;
            *len = sizeof(qualiferDesc);
            break;
        
		default:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("desc:unknow%d\r\n",  DESCRIPTOR_TYPE(packet->wValue)));
			break;
    }
    return p;
}



