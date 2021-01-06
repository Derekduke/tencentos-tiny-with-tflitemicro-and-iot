#include "key.h"
#include "dwt.h"
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
 *	KEY��������
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2019/7/10
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2019-2029
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/

/**
 * @brief	������ʼ������
 *
 * @param   void
 *
 * @return  void
 */
void KEY_Init(void)
{
    /*
    	KEY0 	- PA7
    	KEY1 	- PA6
    	KEY_UP 	- PA8	�ߵ�ƽ��Ч
    */

    tls_gpio_cfg(WM_IO_PA_06, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_cfg(WM_IO_PA_07, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_FLOATING);
    tls_gpio_cfg(WM_IO_PA_08, WM_GPIO_DIR_INPUT, WM_GPIO_ATTR_PULLLOW);
}

/**
 * @brief	����������
 *
 * @remark	ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY_UP!!
 *
 * @param   mode	0:��֧��������,1:֧��������
 *
 * @return  u8		���ذ���ֵ
*					0:û���κΰ�������,1:KEY0����,2:KEY1����,3:KEY2����,4:KEY_UP����
 */
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;   //�����ɿ���־

    if(mode == 1)key_up = 1; //֧������

    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY_UP == 1))
    {
        delay_ms(10);
        key_up = 0;

        if(KEY0 == 0)       return KEY0_PRES;

        else if(KEY1 == 0)  return KEY1_PRES;

        else if(KEY_UP == 1) return WKUP_PRES;
    }

    else if(KEY0 == 1 && KEY1 == 1 && KEY_UP == 0)key_up = 1;

    return 0;   //�ް�������
}
