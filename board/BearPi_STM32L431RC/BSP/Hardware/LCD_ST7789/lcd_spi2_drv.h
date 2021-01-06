/**
 * @Copyright 			(c) 2019,mculover666 All rights reserved	
 * @filename  			lcd_spi2_drv.h
 * @breif				Drive LCD based on spi2 commucation interface
 * @version
 *            			v1.0    ��ɻ����������򣬿���ˢ��              mculover666    2019/7/10
 *                  v1.1    ��Ӵ�㡢���ߡ������Ρ���Բ�㷨ʵ��     mculover666    2019/7/12
 *                  v1.2    �����ʾӢ��ASCII�ַ����ַ���           mculover666    2019/7/12
 *                  v1.3    ��ӻ�����â�Ǻ��������ڻ��ߺ�����       mculover666    2019/7/12
 *                  v1.4    �����ʾͼƬ����                       mculover666     2019/7/13   
 *                  v2.0    ʹ�ú꿪�ؿ����ַ���ʾ��ͼƬ��ʾ         mculover666    2019/7/13  
 * @note                ��ֲ˵�����ǳ���Ҫ����
 *                      1. LCD_SPI_Send��LCD�ĵײ㷢�ͺ���������ǲ�ͬ��оƬ����SPI�ӿڣ�ʹ��CubeMX���ɳ�ʼ�����룬
 *                         ���޸Ĵ�"lcd_spi2_drv.h"��LCD�������ź궨�壬
 *                         Ȼ���޸�LCD_SPI_Send�е����ݼ��ɣ�
 *                      2. ���ʹ�õ���ST7789V2Һ�������������ǲ�ͬ�ֱ��ʵ���Ļ���޸�"lcd_spi2_drv.h"�е�LCD_Width��LCD_Height�궨�弴�ɣ�
 *                      3. LCD_Buf_Size���������޸ģ���Ӱ�켸�����еĺ�������������ȷ���˽�����
 *                      4. ������������Ҫspi.h��spi.c��֧�֣�
 *                      5. ���������������������룻
 *                      6. ʹ��ASCII�ַ���ʾ���ܺ�ͼƬ��ʾ����������"lcd_spi2_drv.c"�򿪶�Ӧ�ĺ꿪�أ�
 *                      7. ���������͸���������϶࣬���������򲻰�������ʹ��sprintf���Ƹ�ʽ��Ȼ������ַ�����ʾ�����������½⡣
 */

#ifndef _LCD_SPI2_DRV_H_
#define _LCD_SPI2_DRV_H_

#include "stm32l4xx_hal.h"

/* �궨�忪�� */
#define	USE_ASCII_FONT_LIB			1       //ʹ��ASCII�ַ���ʾ��򿪸ú�(ֵ��Ϊ1)
#define USE_PICTURE_DISPLAY			1       //ʹ��ͼƬ��ʾ��򿪸ú�(ֵ��Ϊ1)

#define	LCD_PWR(n)		(n?HAL_GPIO_WritePin(LCD_PWR_GPIO_Port,LCD_PWR_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_PWR_GPIO_Port,LCD_PWR_Pin,GPIO_PIN_RESET))
#define	LCD_WR_RS(n)	(n?HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port,LCD_WR_RS_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port,LCD_WR_RS_Pin,GPIO_PIN_RESET))
#define	LCD_RST(n)		(n?HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_RST_GPIO_Port,LCD_RST_Pin,GPIO_PIN_RESET))
#define LCD_Width		240
#define LCD_Height		240

//������ɫ
#define WHITE         0xFFFF	//��ɫ
#define YELLOW        0xFFE0	//��ɫ
#define BRRED 		  	0XFC07  //�غ�ɫ
#define PINK          0XF81F	//��ɫ
#define RED           0xF800	//��ɫ
#define BROWN 		  	0XBC40  //��ɫ
#define GRAY  		  	0X8430  //��ɫ
#define GBLUE		  		0X07FF	//��ɫ
#define GREEN         0x07E0	//��ɫ
#define BLUE          0x001F  //��ɫ
#define BLACK         0x0000	//��ɫ



void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t color);
void LCD_Init(void);			//LCD��ʼ������
void LCD_Draw_ColorPoint(uint16_t x, uint16_t y,uint16_t color);
void LCD_Draw_ColorLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Draw_ColorRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Draw_ColorCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
#if USE_ASCII_FONT_LIB
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint16_t back_color, uint16_t font_color, uint8_t font_size);
void LCD_ShowCharStr(uint16_t x, uint16_t y, char* str, uint16_t back_color, uint16_t font_color, uint8_t font_size);
#endif /* USE_ASCII_FONT_LIB */
void LCD_Draw_ColorSixPointStar(uint16_t x, uint16_t y, uint8_t r, uint16_t color);
#if USE_PICTURE_DISPLAY
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);
#endif /*USE_PICTURE_DISPLAY */

#endif /* _LCD_SPI2_DRV_H_ */
