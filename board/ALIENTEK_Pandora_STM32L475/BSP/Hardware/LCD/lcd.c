#include "mcu_init.h"
#include "font.h"
#include "cmsis_os.h"
/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	LCD TFT��������
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/

//LCD�����С���ã��޸Ĵ�ֵʱ��ע�⣡�������޸�������ֵʱ���ܻ�Ӱ�����º���	LCD_Clear/LCD_Fill/LCD_DrawLine
#define LCD_TOTAL_BUF_SIZE (240 * 240 * 2)
#define LCD_Buf_Size 1152
static uint8_t lcd_buf[LCD_Buf_Size];

uint16_t POINT_COLOR = BLACK; //������ɫ	Ĭ��Ϊ��ɫ
uint16_t BACK_COLOR = WHITE;  //������ɫ	Ĭ��Ϊ��ɫ

/**
 * @brief	LCD���ƽӿڳ�ʼ��
 *
 * @param   void
 *
 * @return  void
 */
static void LCD_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*
    	LCD_PWR:	PB7
    	LCD_RST:	PB6
    	LCD_DC:		PB4
    	LCD_CS:		PD7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

    LCD_CS(0);
    LCD_PWR(0);

    LCD_RST(0);
    HAL_Delay(120);
    LCD_RST(1);

}

/**
 * @brief	LCD�ײ�SPI�������ݺ���
 *
 * @param   data	���ݵ���ʼ��ַ
 * @param   size	�������ݴ�С
 *
 * @return  void
 */
static void LCD_SPI_Send(uint8_t *data, uint16_t size)
{
    HAL_SPI_Transmit(&hspi3,data,size,1000);
}

/**
 * @brief	д���LCD
 *
 * @param   cmd		��Ҫ���͵�����
 *
 * @return  void
 */
static void LCD_Write_Cmd(uint8_t cmd)
{
    LCD_DC(0);
    LCD_SPI_Send(&cmd, 1);
}

/**
 * @brief	д���ݵ�LCD
 *
 * @param   cmd		��Ҫ���͵�����
 *
 * @return  void
 */
static void LCD_Write_Data(uint8_t data)
{
    LCD_DC(1);
    LCD_SPI_Send(&data, 1);
}

/**
 * @brief	д����ֵ����ݵ�LCD
 *
 * @param   cmd		��Ҫ���͵�����
 *
 * @return  void
 */
void LCD_Write_HalfWord(const uint16_t da)
{
    uint8_t data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;

    LCD_DC(1);
    LCD_SPI_Send(data, 2);
}

/**
 * ��������д��LCD��������
 *
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
 *
 * @return  void
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_Write_Cmd(0x2a);
    LCD_Write_Data(x1 >> 8);
    LCD_Write_Data(x1);
    LCD_Write_Data(x2 >> 8);
    LCD_Write_Data(x2);

    LCD_Write_Cmd(0x2b);
    LCD_Write_Data(y1 >> 8);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2 >> 8);
    LCD_Write_Data(y2);

    LCD_Write_Cmd(0x2C);
}

/**
 * ��LCD��ʾ
 *
 * @param   void
 *
 * @return  void
 */
void LCD_DisplayOn(void)
{
    LCD_PWR(1);
}
/**
 * �ر�LCD��ʾ
 *
 * @param   void
 *
 * @return  void
 */
void LCD_DisplayOff(void)
{
    LCD_PWR(0);
}

/**
 * ��һ����ɫ���LCD��
 *
 * @param   color	������ɫ
 *
 * @return  void
 */
void LCD_Clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    for (j = 0; j < LCD_Buf_Size / 2; j++)
    {
        lcd_buf[j * 2] = data[0];
        lcd_buf[j * 2 + 1] = data[1];
    }

    LCD_DC(1);

    for (i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_Buf_Size); i++)
    {
        LCD_SPI_Send(lcd_buf, LCD_Buf_Size);
    }
}

/**
 * ��һ����ɫ�����������
 *
 * @param   x_start,y_start     �������
 * @param   x_end,y_end			�յ�����
 * @param   color       		�����ɫ
 *
 * @return  void
 */
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if (size > LCD_Buf_Size)
    {
        size_remain = size - LCD_Buf_Size;
        size = LCD_Buf_Size;
    }

    LCD_Address_Set(x_start, y_start, x_end, y_end);

    while (1)
    {
        for (i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_DC(1);
        LCD_SPI_Send(lcd_buf, size);

        if (size_remain == 0)
            break;

        if (size_remain > LCD_Buf_Size)
        {
            size_remain = size_remain - LCD_Buf_Size;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}

/**
 * ���㺯��
 *
 * @param   x,y		��������
 *
 * @return  void
 */
void LCD_Draw_Point(uint16_t x, uint16_t y)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_HalfWord(POINT_COLOR);
}

/**
 * �������ɫ����
 *
 * @param   x,y		��������
 *
 * @return  void
 */
void LCD_Draw_ColorPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_HalfWord(color);
}

/**
 * @brief	���ߺ���(ֱ�ߡ�б��)
 *
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
 *
 * @return  void
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    uint32_t i = 0;

    if (y1 == y2)
    {
        /*���ٻ�ˮƽ��*/
        LCD_Address_Set(x1, y1, x2, y2);

        for (i = 0; i < x2 - x1; i++)
        {
            lcd_buf[2 * i] = POINT_COLOR >> 8;
            lcd_buf[2 * i + 1] = POINT_COLOR;
        }

        LCD_DC(1);
        LCD_SPI_Send(lcd_buf, (x2 - x1) * 2);
        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
        incx = 1;

    else if (delta_x == 0)
        incx = 0;

    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
        incy = 1;

    else if (delta_y == 0)
        incy = 0;

    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x;

    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++)
    {
        LCD_Draw_Point(row, col);
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief	��һ������
 *
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
 *
 * @return  void
 */
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/**
 * @brief	��һ��Բ
 *
 * @param   x0,y0	Բ������
 * @param   r       Բ�뾶
 *
 * @return  void
 */
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while (a <= b)
    {
        LCD_Draw_Point(x0 - b, y0 - a);
        LCD_Draw_Point(x0 + b, y0 - a);
        LCD_Draw_Point(x0 - a, y0 + b);
        LCD_Draw_Point(x0 - b, y0 - a);
        LCD_Draw_Point(x0 - a, y0 - b);
        LCD_Draw_Point(x0 + b, y0 + a);
        LCD_Draw_Point(x0 + a, y0 - b);
        LCD_Draw_Point(x0 + a, y0 + b);
        LCD_Draw_Point(x0 - b, y0 + a);
        a++;

        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }

        LCD_Draw_Point(x0 + a, y0 + b);
    }
}

/**
 * @brief	��ʾһ��ASCII���ַ�
 *
 * @param   x,y		��ʾ��ʼ����
 * @param   chr		��Ҫ��ʾ���ַ�
 * @param   size	�����С(֧��16/24/32������)
 *
 * @return  void
 */
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size)
{
    uint8_t temp, t1, t;
    uint8_t csize; //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    uint16_t colortemp;
    uint8_t sta;

    chr = chr - ' '; //�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩

    if ((x > (LCD_Width - size / 2)) || (y > (LCD_Height - size)))
        return;

    LCD_Address_Set(x, y, x + size / 2 - 1, y + size - 1); //(x,y,x+8-1,y+16-1)

    if ((size == 16) || (size == 32)) //16��32������
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for (t = 0; t < csize; t++)
        {
            if (size == 16)
                temp = asc2_1608[chr][t]; //����1608����
            else if (size == 32)
                temp = asc2_3216[chr][t]; //����3216����
            else
                return; //û�е��ֿ�

            for (t1 = 0; t1 < 8; t1++)
            {
                if (temp & 0x80)
                    colortemp = POINT_COLOR;
                else
                    colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

    else if (size == 12) //12������
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for (t = 0; t < csize; t++)
        {
            temp = asc2_1206[chr][t];

            for (t1 = 0; t1 < 6; t1++)
            {
                if (temp & 0x80)
                    colortemp = POINT_COLOR;
                else
                    colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

    else if (size == 24) //24������
    {
        csize = (size * 16) / 8;

        for (t = 0; t < csize; t++)
        {
            temp = asc2_2412[chr][t];

            if (t % 2 == 0)
                sta = 8;
            else
                sta = 4;

            for (t1 = 0; t1 < sta; t1++)
            {
                if (temp & 0x80)
                    colortemp = POINT_COLOR;
                else
                    colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
}

/**
 * @brief	m^n����
 *
 * @param   m,n		�������
 *
 * @return  m^n�η�
 */
static uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
        result *= m;

    return result;
}

/**
 * @brief	��ʾ����,��λΪ0����ʾ
 *
 * @param   x,y		�������
 * @param   num		��Ҫ��ʾ������,���ַ�Χ(0~4294967295)
 * @param   len		��Ҫ��ʾ��λ��
 * @param   size	�����С
 *
 * @return  void
 */
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size);
                continue;
            }

            else
                enshow = 1;
        }

        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size);
    }
}

/**
 * @brief	��ʾ����,��λΪ0,���Կ�����ʾΪ0���ǲ���ʾ
 *
 * @param   x,y		�������
 * @param   num		��Ҫ��ʾ������,���ַ�Χ(0~999999999)
 * @param   len		��Ҫ��ʾ��λ��
 * @param   size	�����С
 * @param   mode	1:��λ��ʾ0		0:��λ����ʾ
 *
 * @return  void
 */
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size);

                continue;
            }

            else
                enshow = 1;
        }

        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size);
    }
}

/**
 * @brief	��ʾ�ַ���
 *
 * @param   x,y		�������
 * @param   width	�ַ���ʾ������
 * @param   height	�ַ���ʾ����߶�
 * @param   size	�����С
 * @param   p		�ַ�����ʼ��ַ
 *
 * @return  void
 */
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) //�ж��ǲ��ǷǷ��ַ�!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
            break; //�˳�

        LCD_ShowChar(x, y, *p, size);
        x += size / 2;
        p++;
    }
}

/**
 * @brief	��ʾͼƬ
 *
 * @remark	Image2Lcdȡģ��ʽ��	C��������/ˮƽɨ��/16λ���ɫ(RGB565)/��λ��ǰ		�����Ĳ�Ҫѡ
 *
 * @param   x,y		�������
 * @param   width	ͼƬ���
 * @param   height	ͼƬ�߶�
 * @param   p		ͼƬ����������ʼ��ַ
 *
 * @return  void
 */
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p)
{
    if (x + width > LCD_Width || y + height > LCD_Height)
    {
        return;
    }

    LCD_Address_Set(x, y, x + width - 1, y + height - 1);

    LCD_DC(1);

    LCD_SPI_Send((uint8_t *)p, width * height * 2);
}

/**
 * @brief	LCD��ʼ��
 *
 * @param   void
 *
 * @return  void
 */
void LCD_Init(void)
{
    
    LCD_Gpio_Init();
    
    LCD_CS(0);
    LCD_PWR(0);

    LCD_RST(0);
    HAL_Delay(120);
    LCD_RST(1);
    
    HAL_Delay(120);
    /* Sleep Out */
    LCD_Write_Cmd(0x11);
    /* wait for power stability */
    HAL_Delay(120);

    /* Memory Data Access Control */
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x00);

    /* RGB 5-6-5-bit  */
    LCD_Write_Cmd(0x3A);
    LCD_Write_Data(0x65);

    /* Porch Setting */
    LCD_Write_Cmd(0xB2);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x33);
    LCD_Write_Data(0x33);

    /*  Gate Control */
    LCD_Write_Cmd(0xB7);
    LCD_Write_Data(0x72);

    /* VCOM Setting */
    LCD_Write_Cmd(0xBB);
    LCD_Write_Data(0x3D); //Vcom=1.625V

    /* LCM Control */
    LCD_Write_Cmd(0xC0);
    LCD_Write_Data(0x2C);

    /* VDV and VRH Command Enable */
    LCD_Write_Cmd(0xC2);
    LCD_Write_Data(0x01);

    /* VRH Set */
    LCD_Write_Cmd(0xC3);
    LCD_Write_Data(0x19);

    /* VDV Set */
    LCD_Write_Cmd(0xC4);
    LCD_Write_Data(0x20);

    /* Frame Rate Control in Normal Mode */
    LCD_Write_Cmd(0xC6);
    LCD_Write_Data(0x0F); //60MHZ

    /* Power Control 1 */
    LCD_Write_Cmd(0xD0);
    LCD_Write_Data(0xA4);
    LCD_Write_Data(0xA1);

    /* Positive Voltage Gamma Control */
    LCD_Write_Cmd(0xE0);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2B);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x54);
    LCD_Write_Data(0x4C);
    LCD_Write_Data(0x18);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x0B);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x23);

    /* Negative Voltage Gamma Control */
    LCD_Write_Cmd(0xE1);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2C);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x44);
    LCD_Write_Data(0x51);
    LCD_Write_Data(0x2F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x20);
    LCD_Write_Data(0x23);

    /* Display Inversion On */
    LCD_Write_Cmd(0x21);

    LCD_Write_Cmd(0x29);

    LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    LCD_Clear(WHITE);

    /*����ʾ*/
    LCD_DisplayOn();
}
