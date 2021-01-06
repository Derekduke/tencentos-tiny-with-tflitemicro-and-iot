#include "bsp_usart.h"

UART_HandleTypeDef debug_usart;
UART_HandleTypeDef wifi_usart;

 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */  
void debug_usart_init(void)
{ 
  
    debug_usart.Instance          = DEBUG_USART;

    debug_usart.Init.BaudRate     = DEBUG_USART_BAUDRATE;
    debug_usart.Init.WordLength   = UART_WORDLENGTH_8B;
    debug_usart.Init.StopBits     = UART_STOPBITS_1;
    debug_usart.Init.Parity       = UART_PARITY_NONE;
    debug_usart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    debug_usart.Init.Mode         = UART_MODE_TX_RX;

    HAL_UART_Init(&debug_usart);

    /*ʹ�ܴ��ڽ��ն� */
    __HAL_UART_ENABLE_IT(&debug_usart, UART_IT_RXNE);  
}

void wifi_usart_init(void)
{ 
  
    wifi_usart.Instance          = WIFI_USART;

    wifi_usart.Init.BaudRate     = WIFI_USART_BAUDRATE;
    wifi_usart.Init.WordLength   = UART_WORDLENGTH_8B;
    wifi_usart.Init.StopBits     = UART_STOPBITS_1;
    wifi_usart.Init.Parity       = UART_PARITY_NONE;
    wifi_usart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    wifi_usart.Init.Mode         = UART_MODE_TX_RX;

    HAL_UART_Init(&wifi_usart);

    /*ʹ�ܴ��ڽ��ն� */
    __HAL_UART_ENABLE_IT(&wifi_usart, UART_IT_RXNE);  
}

/**
  * @brief UART MSP ��ʼ�� 
  * @param huart: UART handle
  * @retval ��
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
    GPIO_InitTypeDef  GPIO_InitStruct;
    
    if (huart->Instance == DEBUG_USART)
    {
        DEBUG_USART_CLK_ENABLE();

        DEBUG_USART_RX_GPIO_CLK_ENABLE();
        DEBUG_USART_TX_GPIO_CLK_ENABLE();

        /**USART1 GPIO Configuration    
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX 
        */
        /* ����Tx����Ϊ���ù���  */
        GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed =  GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);

        /* ����Rx����Ϊ���ù��� */
        GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
        GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
        HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
        //��ռ���ȼ�0�������ȼ�1
        HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,6,0);	
         //ʹ��USART1�ж�ͨ��  
        HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );	
    } else if (huart->Instance == WIFI_USART)
    {
        WIFI_USART_CLK_ENABLE();

        WIFI_USART_RX_GPIO_CLK_ENABLE();
        WIFI_USART_TX_GPIO_CLK_ENABLE();

        /**USART1 GPIO Configuration    
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX 
        */
        /* ����Tx����Ϊ���ù���  */
        GPIO_InitStruct.Pin = WIFI_USART_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed =  GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(WIFI_USART_TX_GPIO_PORT, &GPIO_InitStruct);

        /* ����Rx����Ϊ���ù��� */
        GPIO_InitStruct.Pin = WIFI_USART_RX_PIN;
        GPIO_InitStruct.Mode=GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
        HAL_GPIO_Init(WIFI_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
        //��ռ���ȼ�0�������ȼ�1
        HAL_NVIC_SetPriority(WIFI_USART_IRQ ,2,0);	
         //ʹ��USART1�ж�ͨ��  
        HAL_NVIC_EnableIRQ(WIFI_USART_IRQ );		   
    }
}

/*****************  �����ַ��� **********************/
void usart_send_string(UART_HandleTypeDef *huart, uint8_t *str)
{
    unsigned int k=0;
    do {
      HAL_UART_Transmit(huart,(uint8_t *)(str + k) ,1,1000);
      k++;
    } while(*(str + k)!='\0');
  
}
//�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&debug_usart, (uint8_t *)&ch, 1, 1);	
	
	return (ch);
}

//�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{		
	int ch;
	HAL_UART_Receive(&debug_usart, (uint8_t *)&ch, 1, 1);	
	return (ch);
}

int send_buff(char *buf, int len)
{
    usart_send_string(&debug_usart, (uint8_t *)buf);
    return len;
}

/*********************************************END OF FILE**********************/
