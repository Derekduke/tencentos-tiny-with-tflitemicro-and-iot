#ifndef __LCD_H
#define __LCD_H
#include "stm32l4xx_hal.h"

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

extern uint16_t POINT_COLOR; //Ĭ�ϻ�����ɫ
extern uint16_t BACK_COLOR;  //Ĭ�ϱ�����ɫ

//LCD�Ŀ�͸߶���
#define LCD_Width 240
#define LCD_Height 240

//������ɫ
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40 //��ɫ
#define BRRED 0XFC07 //�غ�ɫ
#define GRAY 0X8430  //��ɫ
//GUI��ɫ

#define DARKBLUE 0X01CF  //����ɫ
#define LIGHTBLUE 0X7D7C //ǳ��ɫ
#define GRAYBLUE 0X5458  //����ɫ
//������ɫΪPANEL����ɫ

#define LIGHTGREEN 0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE 0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE 0X2B12	//ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

/*
	LCD_PWR:	LCD_PWR_Pin, 	PB7
	LCD_RST:	LCD_RESET_Pin,  PB6
	LCD_DC:		LCD_WR_Pin, 	PB4
	LCD_CS:		LCD_CS_Pin, 	PD7
*/
// #define LCD_PWR(n) (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET))
// #define LCD_RST(n) (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET))
// #define LCD_DC(n)  (n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET))
// #define LCD_CS(n)  (n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET))

#define LCD_PWR(n)	HAL_GPIO_WritePin(LCD_PWR_GPIO_Port, 	LCD_PWR_Pin, 	(n?GPIO_PIN_SET:GPIO_PIN_RESET))
#define LCD_RST(n)	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, (n?GPIO_PIN_SET:GPIO_PIN_RESET))
#define LCD_DC(n) 	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, 	LCD_WR_Pin, 	(n?GPIO_PIN_SET:GPIO_PIN_RESET))
#define LCD_CS(n) 	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, 	LCD_CS_Pin, 	(n?GPIO_PIN_SET:GPIO_PIN_RESET))

void LCD_Init(void);																				 //��ʼ��
void LCD_DisplayOn(void);																			 //����ʾ
void LCD_DisplayOff(void);																			 //����ʾ
void LCD_Write_HalfWord(const uint16_t da);															 //д����ֽ����ݵ�LCD
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);							 //����������ʾ����
void LCD_Clear(uint16_t color);																		 //����
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);   //��䵥ɫ
void LCD_Draw_Point(uint16_t x, uint16_t y);														 //����
void LCD_Draw_ColorPoint(uint16_t x, uint16_t y, uint16_t color);									 //������ɫ��
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);								 //����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);							 //������
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);											 //��Բ
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size);									 //��ʾһ���ַ�
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);					 //��ʾһ������
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);	//��ʾ����
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p); //��ʾ�ַ���
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);		 //��ʾͼƬ
void Display_ALIENTEK_LOGO(uint16_t x, uint16_t y);													 //��ʾALIENTEK LOGO

#endif
