/**
 * @Copyright 			(c) 2019,mculover666 All rights reserved	
 * @filename  			lcd_spi2_drv.c
 * @breif						Drive LCD based on spi2 commucation interface
 * @version
 *            			v1.0    ��ɻ����������򣬿���ˢ��              mculover666    2019/7/10
 *                  v1.1    ��Ӵ�㡢���ߡ������Ρ���Բ�㷨ʵ��     mculover666    2019/7/12
 *                  v1.2    �����ʾӢ��ASCII�ַ����ַ���           mculover666    2019/7/12
 *                  v1.3    ��ӻ�����â�Ǻ��������ڻ��ߺ�����       mculover666    2019/7/12
 *                  v1.4    �����ʾͼƬ����                       mculover666    2019/7/13   
 *                  v2.0    ʹ�ú꿪�ؿ����ַ���ʾ��ͼƬ��ʾ         mculover666    2019/7/13  
 * @note            ��ֲ˵�����ǳ���Ҫ����
 *                      1. LCD_SPI_Send��LCD�ĵײ㷢�ͺ���������ǲ�ͬ��оƬ����SPI�ӿڣ�ʹ��CubeMX���ɳ�ʼ�����룬
 *                         ���޸Ĵ�"lcd_spi2_drv.h"��LCD�������ź궨�壬
 *                         Ȼ���޸�LCD_SPI_Send�е����ݼ��ɣ�
 *                      2. ���ʹ�õ���ST7789V2Һ�������������ǲ�ͬ�ֱ��ʵ���Ļ���޸�"lcd_spi2_drv.h"�е�LCD_Width��LCD_Height�궨�弴�ɣ�
 *                      3. LCD_Buf_Size���������޸ģ���Ӱ�켸�����еĺ�������������ȷ���˽�����
 *                      4. ������������Ҫspi.h��spi.c��֧�֣�
 *                      5. ����������������������;
 *                      6. ʹ��ASCII�ַ���ʾ���ܺ�ͼƬ��ʾ����������"lcd_spi2_drv.c"�򿪶�Ӧ�ĺ꿪�أ�
 *                      7. ���������͸���������϶࣬���������򲻰�������ʹ��sprintf���Ƹ�ʽ��Ȼ������ַ�����ʾ�����������½⡣
 */

#include "lcd_spi2_drv.h"
#include "spi.h"
#if USE_ASCII_FONT_LIB
#include "font.h"
#endif /* USE_ASCII_FONT_LIB */

#define LCD_TOTAL_BUF_SIZE	(240*240*2)
#define LCD_Buf_Size 1152

static uint8_t lcd_buf[LCD_Buf_Size];

/**
 * @brief    SPI �����ֽں���
 * @param    TxData	Ҫ���͵�����
 * @param    size	�������ݵ��ֽڴ�С
 * @return  0:д��ɹ�,����:д��ʧ��
 */
static uint8_t SPI_WriteByte(uint8_t *TxData,uint16_t size)
{
	return HAL_SPI_Transmit(&hspi2,TxData,size,1000);
}


/**
 *@brief		LCD�������ų�ʼ��
 *@param		none
 *@retval		none
*/

static void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LCD_PWR_GPIO_Port, LCD_PWR_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, LCD_WR_RS_Pin|LCD_RST_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = LCD_PWR_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_PWR_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : PCPin PCPin */
    GPIO_InitStruct.Pin = LCD_WR_RS_Pin|LCD_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* ��λLCD */
    LCD_PWR(0);
    LCD_RST(0);
    HAL_Delay(100);
    LCD_RST(1);

		/* ��ʼ��SPI2�ӿ� */
    MX_SPI2_Init();
}

/**
 * @brief	LCD�ײ�SPI�������ݺ���
 * @param   data ���� ���ݵ���ʼ��ַ
 * @param   size ���� ���������ֽ���
 * @return  none
 */
static void LCD_SPI_Send(uint8_t *data, uint16_t size)
{
    SPI_WriteByte(data, size);
}

/**
 * @brief	д���LCD
 * @param   cmd ���� ��Ҫ���͵�����
 * @return  none
 */
static void LCD_Write_Cmd(uint8_t cmd)
{
    LCD_WR_RS(0);
    LCD_SPI_Send(&cmd, 1);
}

/**
 * @brief	д���ݵ�LCD
 * @param 	dat ���� ��Ҫ���͵�����
 * @return  none
 */
static void LCD_Write_Data(uint8_t dat)
{
    LCD_WR_RS(1);
    LCD_SPI_Send(&dat, 1);
}
/**
 * @brief		д16λ�����ݣ������ֽڣ���LCD
 * @param   dat ���� ��Ҫ���͵�16bit����
 * @return  none
 */
static void LCD_Write_2Byte(const uint16_t dat)
{
    uint8_t data[2] = {0};

    data[0] = dat >> 8;
    data[1] = dat;

    LCD_WR_RS(1);
    LCD_SPI_Send(data, 2);
}
/**
 * @brief	��������д��LCD��������
 * @param   x1,y1	���� �������
 * @param   x2,y2	���� �յ�����
 * @return  none
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
 * @breif	��LCD��ʾ����
 * @param   none
 * @return  none
 */
void LCD_DisplayOn(void)
{
    LCD_PWR(1);
}
/**
 * @brief	�ر�LCD��ʾ����
 * @param   none
 * @return  none
 */
void LCD_DisplayOff(void)
{
    LCD_PWR(0);
}
/**
 * @brief	��һ����ɫ���LCD��
 * @param   color ���� ������ɫ(16bit)
 * @return  none
 */
void LCD_Clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};  //LCD��Ļɫ�����16bit��data[0]����ɫ���ݵĸ�λ��data[1]����ɫ���ݵĵ�λ

    data[0] = color >> 8;
    data[1] = color;

    LCD_Address_Set(0, 0, LCD_Width - 1, LCD_Height - 1);

    for(j = 0; j < LCD_Buf_Size / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    LCD_WR_RS(1);

    for(i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_Buf_Size); i++)
    {
        LCD_SPI_Send(lcd_buf, LCD_Buf_Size);
    }
}
/**
 * @brief	LCD��ʼ��
 * @param   none
 * @return  none
 */
void LCD_Init(void)
{
    LCD_GPIO_Init();

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
    LCD_Write_Data(0x3D);   //Vcom=1.625V

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
    LCD_Write_Data(0x0F);	//60MHZ

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
    LCD_PWR(1);
}
/**
 * @brief		����ɫ���㺯��
 * @param   x,y	���� ��������
 * @return  none
 */
void LCD_Draw_ColorPoint(uint16_t x, uint16_t y,uint16_t color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_2Byte(color);
}
/**
 * @brief		����ɫ���ߺ���(ֱ�ߡ�б��)
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
 * @return  none
 */
void LCD_Draw_ColorLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t	i = 0;
    int16_t		delta_x = 0, delta_y = 0;
    int8_t		incx = 0, incy = 0;
    uint16_t	distance = 0;
    uint16_t    t = 0;
    uint16_t	x = 0, y = 0;
    uint16_t 	x_temp = 0, y_temp = 0;
	

    if(y1 == y2)
    {
        /* ���ٻ�ˮƽ�� */
        LCD_Address_Set(x1, y1, x2, y2);

        for(i = 0; i < x2 - x1; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_WR_RS(1);
        LCD_SPI_Send(lcd_buf, (x2 - x1) * 2);
        return;
    }
    else
    {
        /* ��б�ߣ�Bresenham�㷨�� */
        /* ��������֮����x��y����ļ�࣬�õ�������x��y����Ĳ���ֵ */
        delta_x = x2 - x1;
        delta_y = y2 - y1;
        if(delta_x > 0)
        {
            //б��(������)
            incx = 1;
        }
        else if(delta_x == 0)
        {
            //��ֱб��(����)
            incx = 0;
        }
        else
        {
            //б��(���ҵ���)
            incx = -1;
            delta_x = -delta_x;
        }
        if(delta_y > 0)
        {
            //б��(������)
            incy = 1;
        }
        else if(delta_y == 0)
        {
            //ˮƽб��(ˮƽ��)
            incy = 0;
        }
        else
        {
            //б��(���ҵ���)
            incy = -1;
            delta_y = -delta_y;
        }			
        
        /* ���㻭�ʴ�����(ȡ��������е����ֵ) */
        if(delta_x > delta_y)
        {
            distance = delta_x;
        }
        else
        {
            distance = delta_y;
        }
        
        /* ��ʼ��� */
        x = x1;
        y = y1;
        //��һ������Ч������t�Ĵ�����һ
        for(t = 0; t <= distance + 1;t++)
        {
            LCD_Draw_ColorPoint(x, y, color);
        
            /* �ж���ʵ��ֵ��������ص� */
            x_temp += delta_x;	
            if(x_temp > distance)
            {
                //x����Խ�磬��ȥ����ֵ��Ϊ��һ�μ����׼��
                x_temp -= distance;		
                //��x����������
                x += incx;
                    
            }
            y_temp += delta_y;
            if(y_temp > distance)
            {
                //y����Խ�磬��ȥ����ֵ��Ϊ��һ�μ����׼��
                y_temp -= distance;
                //��y����������
                y += incy;
            }
        }
    }
}
/**
 * @breif	����ɫ�����κ���
 * @param   x1,y1 ���� ������ʼ��
 * @param	x2,y2 ���� �����յ�
 * @param	color	���� ��ɫ
 * @retval	none
 */
void LCD_Draw_ColorRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    LCD_Draw_ColorLine(x1,y1,x2,y1,color);
    LCD_Draw_ColorLine(x1,y1,x1,y2,color);
    LCD_Draw_ColorLine(x1,y2,x2,y2,color);
    LCD_Draw_ColorLine(x2,y1,x2,y2,color);
}
/**
 * @breif	����ɫ��Բ����
 * @param   x1,x2 ���� Բ������
 * @param	r ���� �뾶
 * @param	color	���� ��ɫ
 * @retval	none
 */
void LCD_Draw_ColorCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
	/* Bresenham��Բ�㷨 */
	int16_t a = 0, b = r;
    int16_t d = 3 - (r << 1);		//�㷨���߲���
		
	/* ���Բ����Ļ�ɼ������⣬ֱ���˳� */
    if (x - r < 0 || x + r > LCD_Width || y - r < 0 || y + r > LCD_Height) 
    {
		return;
    }
		
	/* ��ʼ��Բ */
    while(a <= b)
    {
        LCD_Draw_ColorPoint(x - b, y - a, color);
        LCD_Draw_ColorPoint(x + b, y - a, color);
        LCD_Draw_ColorPoint(x - a, y + b, color);
        LCD_Draw_ColorPoint(x - b, y - a, color);
        LCD_Draw_ColorPoint(x - a, y - b, color);
        LCD_Draw_ColorPoint(x + b, y + a, color);
        LCD_Draw_ColorPoint(x + a, y - b, color);
        LCD_Draw_ColorPoint(x + a, y + b, color);
        LCD_Draw_ColorPoint(x - b, y + a, color);
        a++;

        if(d < 0)
        {
			d += 4 * a + 6;
        }
        else
        {
            d += 10 + 4 * (a - b);
            b--;
        }

        LCD_Draw_ColorPoint(x + a, y + b, color);
    }
}
/**
 * @brief	��һ����ɫ���/���ĳ����������
 * @param   color ���� ������ɫ(16bit)
 * @return  none
 */
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t i = 0;
    uint32_t size = 0, size_remain = 0;

    size = (x2 - x1 + 1) * (y2 - y1 + 1) * 2;

    if(size > LCD_Buf_Size)
    {
        size_remain = size - LCD_Buf_Size;
        size = LCD_Buf_Size;
    }

    LCD_Address_Set(x1, y1, x2, y2);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_WR_RS(1);
        LCD_SPI_Send(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_Buf_Size)
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
#if USE_ASCII_FONT_LIB
/**
 * @brief	��ʾһ��ASCII���ַ�
 * @param   x,y		��ʾ��ʼ����
 * @param   ch		��Ҫ��ʾ���ַ�
 * @param   size	�����С(֧��16/24/32������)
 * @return  none
 * @note	��Ҫfont.h�ֿ��ļ���֧��
 */
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint16_t back_color, uint16_t font_color, uint8_t font_size)
{
	int i = 0, j = 0;
	uint8_t temp = 0;
	uint8_t size = 0;
	uint8_t t = 0;
	
	/* �����ʾ�Ƿ��Խ�� */
	 if((x > (LCD_Width - font_size / 2)) || (y > (LCD_Height - font_size)))	
		 return;
	
	/* �����ַ���С�����Դ�������� */
	LCD_Address_Set(x, y, x + font_size/2 - 1, y + font_size - 1);
	 
	 /* �����ַ����ֿ��е�ƫ��ֵ*/
	 ch = ch - ' ';
	 
	 /* ��ʾ16��/32������ */
	 if((font_size == 16) || (font_size == 32) )
	 {
		  /* ��������һ���ַ���Ӧ������ռ���ֽ��� */
 			size = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size / 2);

			for(i = 0; i < size; i++)
			{
					if(font_size == 16)
							temp = asc2_1608[ch][i];	//����1608����
					else if(font_size == 32)
							temp = asc2_3216[ch][i];	//����3216����
					else 
							return;			//û�е��ֿ�

					for(j = 0; j < 8; j++)
					{
							if(temp & 0x80)
								LCD_Write_2Byte(font_color);
							else 
								LCD_Write_2Byte(back_color);

							temp <<= 1;
					}
			}
	 }
	  /* ��ʾ12������ */
	 else if(font_size == 12)
	 {
		  /* ��������һ���ַ���Ӧ������ռ���ֽ��� */
 			size = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size / 2);

			for(i = 0; i < size; i++)
			{
				  temp = asc2_1206[ch][i];

					for(j = 0; j < 6; j++)
					{
							if(temp & 0x80)
								LCD_Write_2Byte(font_color);
							else 
								LCD_Write_2Byte(back_color);

							temp <<= 1;
					}
			}
	 }
	 /* ��ʾ24������ */
	 else if(font_size == 24)
	 {
		  /* ��������һ���ַ���Ӧ������ռ���ֽ��� */
 			size = (font_size * 16) / 8;

			for(i = 0; i < size; i++)
			{
				  temp = asc2_2412[ch][i];
					if(i % 2 == 0)
							t = 8;
					else
							t = 4;
					for(j = 0; j < t; j++)
					{
							if(temp & 0x80)
								LCD_Write_2Byte(font_color);
							else 
								LCD_Write_2Byte(back_color);

							temp <<= 1;
					}
			}
	 }	 
	 /* �������� */
	 else
		 return;
}
/**
 * @brief		��ʾһ��ASCII���ַ���
 * @param   x,y		��ʾ��ʼ����
 * @param   str		��Ҫ��ʾ���ַ���
 * @param   size	�����С(֧��16/24/32������)
 * @return  none
 * @note		1. ��Ҫfont.h�ֿ��ļ���֧��
 * 					2. ����ָ��width����ʾ�������ַ�
 */
void LCD_ShowCharStr(uint16_t x, uint16_t y, char* str, uint16_t back_color, uint16_t font_color, uint8_t font_size)
{
	while((*str <= '~') && (*str >= ' '))	//�ж��Ƿ�Ƿ��ַ�
	{
			LCD_ShowChar(x,y,*str,back_color, font_color,font_size);
			x += font_size / 2;
			str++;
	}
}
#endif /* USE_ASCII_FONT_LIB */
/**
 * @breif		����ɫ����â�Ǻ���
 * @param   x,y ���� ��â�����ĵ�
 * @param		r ���� ��â�ǰ뾶
 * @param		color	���� ��ɫ
 * @retval	none
 */
void LCD_Draw_ColorSixPointStar(uint16_t x, uint16_t y, uint8_t r, uint16_t color)
{
		uint16_t a = r / 2;
		uint16_t b = 1.432*r;
	
		/* ���Ƶ����� */
		LCD_Draw_ColorLine(x-b,y-a,x+b,y-a,color);
		LCD_Draw_ColorLine(x+b,y-a,x,y+r,color);
		LCD_Draw_ColorLine(x,y+r,x-b,y-a,color);
	
		/* ���������� */
		LCD_Draw_ColorLine(x-b,y+a,x+b,y+a,color);
		LCD_Draw_ColorLine(x+b,y+a,x,y-r,color);
		LCD_Draw_ColorLine(x,y-r,x-b,y+a,color);

}

#if USE_PICTURE_DISPLAY
/**
 * @brief	��ʾͼƬ����
 * @param   x,y	    ���� �������
 * @param   width	���� ͼƬ���
 * @param   height	���� ͼƬ�߶�
 * @param   p       ���� ͼƬ����������ʼ��ַ
 * @return  none
 * @note	Image2Lcdȡģ��ʽ��C��������/ˮƽɨ��/16λ���ɫ(RGB565)/��λ��ǰ�������Ĳ�ѡ
 */
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p)
{
	uint32_t img_size = width * height * 2;		//ͼƬ��ռ�ֽ���
	uint32_t remain_size = img_size;		    //ͼƬÿ�η��ͺ�ʣ����ֽ���
	uint8_t i = 0;
	
	/* ������ */
    if(x + width > LCD_Width || y + height > LCD_Height)
    {
        return;
    }
				
    LCD_Address_Set(x, y, x + width - 1, y + height - 1);

    LCD_WR_RS(1);

    /* SPIÿ�������2^16 = 65536������,ͼƬ����СΪ240*240*2 = 115200���ᳬ���˴�С���������ѭ�������㷨 */
    for(i = 0;i <= img_size / 65536; i++)
    {
        if(remain_size / 65536 >= 1)
        {
            LCD_SPI_Send((uint8_t *)p, 65535);
            p += 65535;
            remain_size -= 65535;
        }
        else
        {
            LCD_SPI_Send((uint8_t *)p, remain_size % 65535);
        }
            
    }  
}
#endif /*  USE_PICTURE_DISPLAY */
