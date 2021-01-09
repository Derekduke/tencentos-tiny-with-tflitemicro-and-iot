#include "cmsis_os.h"
#include "mcu_init.h"
#include "esp8266.h"
#include "sal_module_wrapper.h"
#define USE_ESP8266

extern uint16_t camera_buffer[];
extern uint8_t frame_flag;
static uint8_t model_buffer[96*96];

#define TASK1_STK_SIZE          2048
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

#define TASK2_STK_SIZE          1024
void task2(void *arg);
osThreadDef(task2, osPriorityNormal, 1, TASK2_STK_SIZE);

uint8_t rgb565_to_gray(uint16_t bg_color)
{
    uint8_t bg_r = 0;
    uint8_t bg_g = 0;
    uint8_t bg_b = 0;
    bg_r = ((bg_color>>11)&0xff)<<3;
    bg_g = ((bg_color>>5)&0x3f)<<2;
    bg_b = (bg_color&0x1f)<<2;
    uint8_t gray = (bg_r*299 + bg_g*587 + bg_b*114 + 500) / 1000;
    return gray;
}

void input_convert(uint16_t* camera_buffer , uint8_t* model_buffer)
{
	for(int i=0 ; i<OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT ; i++)
	{
		model_buffer[i] = rgb565_to_gray(camera_buffer[i]);
	}
}

void task1(void *arg)
{
	 printf("task1 start\n");
	 uint8_t res_p = 0 , person_flag = 0;
	 uint32_t count = 0 , frame = 0;
//	 int socket_id_0 =0 ;
//	 extern int esp8266_sal_init(hal_uart_port_t uart_port);
//   extern int esp8266_join_ap(const char *ssid, const char *pwd);
//	 esp8266_sal_init(HAL_UART_PORT_2);
//   esp8266_join_ap("OnePlus7", "147258369..");
//	 socket_id_0 = tos_sal_module_connect("106.55.124.154", "8883", TOS_SAL_PROTO_TCP); 
//   if (socket_id_0 == -1) {
//        printf("TCP0 connect failed\r\n");
//   } else {
//        printf("TCP0 connect success! fd: %d\n", socket_id_0);
//   }
//	 
//	 data_template_light_thread();
	
	 //person_detect_init();
    while (1) {
      if(frame_flag == 1){
				
				if(HAL_DCMI_Stop(&hdcmi))
						Error_Handler(); //stop DCMI
				frame++;
				input_convert(camera_buffer,model_buffer);
				res_p = person_detect(model_buffer);
				LCD_2IN4_Display(camera_buffer,OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT);
				if(res_p == 1)
				{
					HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin|LED_Pin, GPIO_PIN_SET);
					if(count == 0)
						frame = 0;
					if(frame-count > 5)
						count = 0;
					else
					{
						count ++;
					}
					if(count == 5)
					{
						person_flag = 1;	
						count = 0;
						printf("detect a dangerous person!!\n");
					}
				}
				else
				{
					HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin|LED_Pin, GPIO_PIN_RESET);
				}
				frame_flag = 0;
				person_flag = 0;
				if(HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_CONTINUOUS,(uint32_t)camera_buffer ,\
					 (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))Error_Handler(); //restart DCMI
			}
			osDelay(50);
    }
}

void task2(void *arg)
{
    while (1) {
        printf("***person detection task\r\n");
        osDelay(50);
    }
}


void application_entry(void *arg)
{
	 //data_template_light_thread();
   printf("***Start person detection task! \r\n");
   osThreadCreate(osThread(task1), NULL); // Create task1
   //osThreadCreate(osThread(task2), NULL); // Create task2
}

