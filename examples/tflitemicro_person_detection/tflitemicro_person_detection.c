#include "cmsis_os.h"
#include "mcu_init.h"
#include "esp8266.h"
#include "sal_module_wrapper.h"

#define USE_ESP8266

extern uint16_t camera_buffer[];
extern uint8_t frame_flag;
static uint8_t model_buffer[96*96];

int  person_flag = 0;
int person_count = 0;
int dagerous_flag = 0;


#define TASK1_STK_SIZE          2048
void task1(void *arg);
osThreadDef(task1, osPriorityHigh, 1, TASK1_STK_SIZE);

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
	 extern int esp8266_sal_init(hal_uart_port_t uart_port);
   extern int esp8266_join_ap(const char *ssid, const char *pwd);
	
	 esp8266_sal_init(HAL_UART_PORT_2);
   esp8266_join_ap("OnePlus7", "147258369..");
	
	 data_template_light_thread();	

    while (1) {
			osDelay(50);
    }
}

void task2(void *arg)
{
		uint8_t res_p = 0;
	  uint32_t count = 0 , count_dagerous = 0;
		osDelay(20000);  //延迟等待联网
	
    while (1) {
			  if(frame_flag == 1){
					
				if(HAL_DCMI_Stop(&hdcmi))
						Error_Handler(); //stop DCMI
				
				input_convert(camera_buffer,model_buffer);
				res_p = person_detect(model_buffer);
				LCD_2IN4_Display(camera_buffer,OV2640_PIXEL_WIDTH,OV2640_PIXEL_HEIGHT);
				
				if(res_p == 1){
					
					HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin, GPIO_PIN_SET);
					
				  if(person_flag == 0){
							person_count ++;  //人流量计数统计
					}	
						person_flag = 1;
            count_dagerous++;					
						count = 0;                           
			  }else{
					count ++;
					if(count >= 2){ //延迟无人判断
              HAL_GPIO_WritePin(GPIOB, LCD_DC_Pin, GPIO_PIN_RESET);
						  count_dagerous = 0;
              person_flag = 0;
					}
				}
				
				if(count_dagerous > 30)dagerous_flag = 1;
				else dagerous_flag = 0;
				
				if(HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_CONTINUOUS,(uint32_t)camera_buffer ,\
					 (OV2640_PIXEL_WIDTH*OV2640_PIXEL_HEIGHT)/2))Error_Handler(); //restart DCMI
				}
				
        osDelay(100);
    }
}


void application_entry(void *arg)
{
	 //data_template_light_thread();
   printf("***Start person detection task! \r\n");
   osThreadCreate(osThread(task1), NULL); // Create task1
   osThreadCreate(osThread(task2), NULL); // Create task2
}

