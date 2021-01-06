#include "lora_demo.h"
#include "stm32l0xx_hal_flash_ex2.h"
#include "RHF76.h"
#include "bsp.h"
#include <Math.h>

/*
 ==================================================================================
 data template:

 Type       Name            Token       DataType    RW          Attribute
 property   temperature     temperature integer     readonly    range: [-100, 155]
                                                                initial: 0
                                                                step: 1
                                                                unit: centigrade

 property   humidity        humidity    integer     readonly    range: [-0, 100]
                                                                initial: 0
                                                                step: 1
                                                                unit: %

 property   pressure        pressure      integer     read-write  range: [259, 1260]
                                                                initial: 260
                                                                step: 1
                                                                unit: hPa

 property   magnFullscale   magnFullscale integer     read-write  range: [4, 16]
                                                                  initial: 4
                                                                  step: 1
                                                                  unit: guass

 property   magn_x          magn_x      float     read-write  range: [-16, 16]
                                                                initial: 0
                                                                step: 1
                                                                unit: guass

 property   magn_y          magn_y      float     read-write  range: [-16, 16]
                                                                initial: 0
                                                                step: 1
                                                                unit: guass

 property   magn_z          magn_z      float     read-write  range: [-16, 16]
                                                                initial: 0
                                                                step: 1
                                                                unit: guass

 property   altitude        altitude      float     read-write  range: [-1000, 10000]
                                                                initial: 0
                                                                step: 1
                                                                unit: m

 property   isconfirmed     isconfirmed   bool     read-write  0: confirmed message
                                                               1: unconfirmed message

 property   report_period   period      integer     read-write  range: [0, 3600]
                                                                initial: 0
                                                                step: 1
                                                                unit: second
 ==================================================================================
 up-link parser javascript:

 function RawToProtocol(fPort, bytes) {
	var data ={
		"method": "report",
		"clientToken": new Date(),
		"params": {}
	}
	data.params.magnFullscale = GetMagnFullscale(bytes[0]);
	
	var tempSensitivity = bytes[1];
	var humiSensitivity = bytes[2] | (bytes[3]<<8);
	var presSensitivity = bytes[4] | (bytes[5]<<8);
	var magnSensitivity = bytes[6] | (bytes[7]<<8);

	data.params.temperature   = (ConvertToInt16(bytes[8] | (bytes[9]<<8))*1.0/10).toFixed(2);
	data.params.humidity      = ((bytes[10] | (bytes[11]<<8))*1.0/10).toFixed(2);
	data.params.magnX         = (ConvertToInt16(bytes[12] | (bytes[13]<<8))*1.0/magnSensitivity).toFixed(2);
	data.params.magnY         = (ConvertToInt16(bytes[14] | (bytes[15]<<8))*1.0/magnSensitivity).toFixed(2);
	data.params.magnZ         = (ConvertToInt16(bytes[16] | (bytes[17]<<8))*1.0/magnSensitivity).toFixed(2);
	data.params.period        = bytes[18] | (bytes[19]<<8);
	data.params.pressure      = ((bytes[20] | (bytes[21]<<8) | (bytes[22]<<16) | (bytes[23]<<24))*1.0/presSensitivity).toFixed(2);
	data.params.altitude      = ((Math.pow(1017.92/data.params.pressure,1.0/5.257) - 1)*(data.params.temperature/10.0+273.15)/0.0065).toFixed(2);
	return data;
  }

  function ConvertToInt16(num)
  {
        var intNum = num;
        if ((num & 0x8000) > 0) {
                intNum = num - 0x10000;
        }
        return intNum;
  }

  function GetMagnFullscale(fullscale){
        var MagnFullscale = { 0: 4, 1: 8, 2: 12, 3: 16 };
        return MagnFullscale[fullscale] === undefined ? 4 : MagnFullscale[fullscale];
  }

 ==================================================================================
 down-link parser javascript:

 function ProtocolToRaw(obj) {
	var data = new Array();
	data[0] = 5; // fport = 5
	data[1] = 0; // unconfirmed mode
	data[2] = obj.params.period & 0x00FF;
	data[3] = (obj.params.period >> 8) & 0x00FF;
	data[4] = GetMagnFullscale(obj.params.magnFullscale);
	data[5] = obj.params.isconfirmed;
	return data;
  }

  function GetMagnFullscale(fullscale){
        var MagnFullscale = { 4: 0, 8: 1, 12: 2, 16: 3 };
        return MagnFullscale[fullscale]===undefined?0:MagnFullscale[fullscale];
  }

 */

typedef struct device_data_st {
    // -- data set 1
    uint8_t     magn_fullscale;                // fullscale of magnetometer (RW)
    uint8_t    temp_sensitivity;              // temperature sensitivity   (R)
    uint16_t    humi_sensitivity;              // humidity sensitivity      (R)
    uint16_t    press_sensitivity;             // pressure sensitivity      (R)
    uint16_t    magn_sensitivity;              // magnetic sensitivity      (R)
    int16_t     temperature;                   // temperature               (R)
    int16_t     humidity;                      // humidity                  (R)
    int16_t     magn_x;                        // X-magnetic value in LSB   (R)
    int16_t     magn_y;                        // Y-magnetic value in LSB   (R)
    int16_t     magn_z;                        // Z-magnetic value in LSB   (R)
    uint16_t    period;                        // report period             (R)
    uint32_t    pressure;                      // pressure                  (R)
    
    // --- data set 2
    uint16_t    accel_fullscale;               // fullscale of accelerometer(RW)
    uint16_t    gyro_fullscale;                // fullscale of magnetometer (RW)
    int16_t     accel_x;                       // X-accel value in LSB      (R)
    int16_t     accel_y;                       // Y-accel value in LSB      (R)
    int16_t     accel_z;                       // Z-accel value in LSB      (R)
    int16_t     gyro_x;                        // X-gyro value in LSB       (R)
    int16_t     gyro_y;                        // Y-gyro value in LSB       (R)
    int16_t     gyro_z;                        // Z-gyro value in LSB       (R)
    uint32_t    accel_sensitivity;             // accel sensitivity         (R)
    uint32_t    gyro_sensitivity;              // gyro sensitivity          (R)
} __PACKED__ dev_data_t;

typedef struct device_data_wrapper_st {
    union {
        dev_data_t  dev_data;
        uint8_t     serialize[sizeof(dev_data_t)];
    } u;
} dev_data_wrapper_t;

dev_data_wrapper_t dev_data_wrapper;

DeviceConfig_TypeDef device_config;

void set_config_to_default(DeviceConfig_TypeDef* config)
{
  config->config_address  = 0x08080000U;
  config->is_confirmed    = true;
  config->report_period   = 10;
  config->magn_fullscale  = MAGN_FULLSCALE_4;
  config->accel_fullscale = ACCEL_FULLSCALE_4;
  config->gyro_fullscale  = GYRO_FULLSCALE_250;
}

/**
  * @brief  Write the configuration to the internal EEPROM bank 1 
  * @note   a single config frame is of 32-bit(a word, 4bytes), and the config
  *         block starts with a frame whose value is 0x464E4F43U ('CONF' from 
  *         low to high) and ends with a frame with a value of 0xFFFFFFFFU; a
  *         single data frame has a following structure��
  *         ----------------------------------------------------------------
  *         | byte |           0          |     1    |    2     |    3     |
  *         ----------------------------------------------------------------
  *         | value|  Device Config Type  | value-L  | value-H  | reserve  |
  *         ----------------------------------------------------------------
  *         the reserve byte could be used as an extra byte for the config
  *         value, i.e. a 24-bit value.
  *
  * @param  config       system configurations
  * 
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef write_config_to_Flash(DeviceConfig_TypeDef config)
{
  uint32_t frame[5] = {0};
  frame[0] = 0x464E4F43U; // <'C'><'O'><'N'><'F'> from low to high
  frame[1] = (uint32_t)config.is_confirmed<<8 | (uint32_t)DCT_IS_CONFIRM;
  frame[2] = (uint32_t)config.report_period<<8 | (uint32_t)DCT_REPORT_PERIOD;
  frame[3] = (uint32_t)config.repeat_time<<8 | (uint32_t)DCT_REPEAT_TIME;
  frame[3] = (uint32_t)config.magn_fullscale<<8 | (uint32_t)DCT_MAGN_FULLSCALE;
  frame[3] = (uint32_t)config.accel_fullscale<<8 | (uint32_t)DCT_ACCEL_FULLSCALE;
  frame[3] = (uint32_t)config.gyro_fullscale<<8 | (uint32_t)DCT_GYRO_FULLSCALE;
  frame[4] = 0xFFFFFFFFU;
  
  HAL_FLASH_Unlock();
  uint8_t retry = 10;
  
  HAL_StatusTypeDef status = HAL_OK;
  for(int i=0; i<5; i++)
  {
    status = HAL_OK;
    do{
      status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, config.config_address+4*i, frame[i]);
    }while(retry--!=0 && status != HAL_OK);
  }
  HAL_FLASH_Lock();
  
  return status;
}

HAL_StatusTypeDef read_config_from_Flash(DeviceConfig_TypeDef* config)
{
  uint32_t data = 0;
  HAL_StatusTypeDef status = HAL_FLASH_ReadWord(config->config_address, &data);
  if(status == HAL_OK)
  {
    // a valid config starts with <'C'><'O'><'N'><'F'> and ended with a word of 0xFFFFFFFF
    if((char)(data&0xFF) == 'C' 
       &&(char)(data>>8&0xFF) == 'O'
       &&(char)(data>>16&0xFF) == 'N'
       &&(char)(data>>24&0xFF) == 'F')
    {
      int i = 0;
      int retry = 10;
      DeviceConfigType_TypeDef config_type = DCT_DEFAULT;
      while(data!=0xFFFFFFFF)
      {
        i+=4;
        status = HAL_FLASH_ReadWord(config->config_address+i, &data);
        if(status != HAL_OK){
          retry--;
          i-=4;
          if(retry == 0) break;
        }else{
          config_type = (DeviceConfigType_TypeDef)(data&0xFF);
          switch(config_type)
          {
          case DCT_IS_CONFIRM:
            {
              config->is_confirmed = (bool)(data>>8&0xFF);
              break;
            }
          case DCT_REPORT_PERIOD:
            {
              config->report_period = (uint16_t)(data>>8&0xFFFF);
              break;
            }
          case DCT_REPEAT_TIME:
            {
              config->repeat_time = (uint8_t)(data>>8&0xFF);
              break;
            }
          case DCT_MAGN_FULLSCALE:
            {
              config->magn_fullscale = (LIS3MDL_FullScaleTypeDef)(data>>8&0xFF);
              break;
            }
          case DCT_ACCEL_FULLSCALE:
            {
              config->accel_fullscale = (LSM6DS3_AccelFullscaleTypeDef)(data>>8&0xFF);
              break;
            }
          case DCT_GYRO_FULLSCALE:
            {
              config->gyro_fullscale = (LSM6DS3_GyroFullscaleTypeDef)(data>>8&0xFF);
              break;
            }
          default:
            {
              break;
            }
          }
        }
      }
    }
  }
  return status;
}

void recv_callback(uint8_t *data, uint8_t len)
{
    printf("len: %d\n", len);
    int i = 0;
    for (i = 0; i < len; ++i) {
        printf("data[%d]: %d\n", i, data[i]);
    }

    if (len == 1) {
        device_config.report_period = data[0];
    } else if (len >= 2) {
        device_config.is_confirmed = (bool)data[3];
        device_config.report_period = data[0] | (data[1] << 8);
        device_config.magn_fullscale = (LIS3MDL_FullScaleTypeDef)data[2];
        LIS3MDL_Set_FullScale(device_config.magn_fullscale);
        write_config_to_Flash(device_config);
    }
}

void print_to_screen(sensor_data_t sensor_data)
{
  float pressure = sensor_data.sensor_press.pressure*1.0/sensor_data.sensor_press.sensitivity;
  float altitude = (pow(1013.25/pressure,1.0/5.257) - 1)*((int16_t)sensor_data.sensor_tempnhumi.temperature/10.0+273.15)/0.0065;
  printf("temperature: %2.2f\n", (int16_t)sensor_data.sensor_tempnhumi.temperature / 10.0);
  printf("humidity   : %2.2f\n", sensor_data.sensor_tempnhumi.humidity / 10.0);
  printf("pressure   : %2.2f,\t altitude: %2.2f\n", pressure, altitude);
  printf("magn       : %2.3f, %2.3f, %2.3f\n", 
               (int16_t)sensor_data.sensor_magn.magn_x*1.0/sensor_data.sensor_magn.sensitivity, 
               (int16_t)sensor_data.sensor_magn.magn_y*1.0/sensor_data.sensor_magn.sensitivity, 
               (int16_t)sensor_data.sensor_magn.magn_z*1.0/sensor_data.sensor_magn.sensitivity);
}

/**
 * @brief     application entry
 * @modified  by jieranzhi 2020/03/31
 */
void application_entry(void *arg)
{
    // retrieve configuration from the EEPROM (if any)
    set_config_to_default(&device_config);
    HAL_StatusTypeDef status = read_config_from_Flash(&device_config);
    if(status != HAL_OK)
    {
      printf("retrieve configuration FAILED!\r\n");
    }
    
    // initialization 
    sensor_data_t sensor_data;
    BSP_Sensor_Init(device_config);
    rhf76_lora_init(HAL_UART_PORT_1);
    tos_lora_module_recvcb_register(recv_callback);
    tos_lora_module_join_otaa("8cf957200000f52c", "8cf957200000f52c6d09aaaaad204a72");

    // do the job
    while (1) {
        BSP_Sensor_Read(&sensor_data);
        print_to_screen(sensor_data);
        // generate data frame for data set 1
        dev_data_wrapper.u.dev_data.magn_fullscale     = (uint8_t)(sensor_data.sensor_magn.fullscale);
        dev_data_wrapper.u.dev_data.temp_sensitivity   = (uint8_t)(sensor_data.sensor_tempnhumi.temp_sensitivity);
        dev_data_wrapper.u.dev_data.humi_sensitivity   = (uint16_t)(sensor_data.sensor_tempnhumi.humi_sensitivity);
        dev_data_wrapper.u.dev_data.press_sensitivity  = (uint16_t)(sensor_data.sensor_press.sensitivity);
        dev_data_wrapper.u.dev_data.magn_sensitivity   = (uint16_t)(sensor_data.sensor_magn.sensitivity);
        dev_data_wrapper.u.dev_data.temperature        = (int16_t)(sensor_data.sensor_tempnhumi.temperature);
        dev_data_wrapper.u.dev_data.humidity           = (int16_t)(sensor_data.sensor_tempnhumi.humidity);
        dev_data_wrapper.u.dev_data.magn_x             = (int16_t)(sensor_data.sensor_magn.magn_x);
        dev_data_wrapper.u.dev_data.magn_y             = (int16_t)(sensor_data.sensor_magn.magn_y);
        dev_data_wrapper.u.dev_data.magn_z             = (int16_t)(sensor_data.sensor_magn.magn_z);
        dev_data_wrapper.u.dev_data.period             = device_config.report_period;
        dev_data_wrapper.u.dev_data.pressure           = (uint32_t)(sensor_data.sensor_press.pressure);
        
        // generate data frame for data set 2
        dev_data_wrapper.u.dev_data.accel_fullscale    = (uint16_t)(sensor_data.sensor_motion.accelFullscale);
        dev_data_wrapper.u.dev_data.gyro_fullscale     = (uint16_t)(sensor_data.sensor_motion.gyroFullscale);
        dev_data_wrapper.u.dev_data.accel_sensitivity  = (uint32_t)(sensor_data.sensor_motion.accelSensitivity);
        dev_data_wrapper.u.dev_data.gyro_sensitivity   = (uint32_t)(sensor_data.sensor_motion.gyroSensitivity);
        dev_data_wrapper.u.dev_data.accel_x            = (int16_t)(sensor_data.sensor_motion.accelX);
        dev_data_wrapper.u.dev_data.accel_y            = (int16_t)(sensor_data.sensor_motion.accelY);
        dev_data_wrapper.u.dev_data.accel_z            = (int16_t)(sensor_data.sensor_motion.accelZ);
        dev_data_wrapper.u.dev_data.gyro_x             = (int16_t)(sensor_data.sensor_motion.gyroX);
        dev_data_wrapper.u.dev_data.gyro_y             = (int16_t)(sensor_data.sensor_motion.gyroY);
        dev_data_wrapper.u.dev_data.gyro_z             = (int16_t)(sensor_data.sensor_motion.gyroZ);
        
        // package segmentation
        uint8_t data_frame1[25]={0};  // idx = 0
        uint8_t data_frame2[25]={1};  // idx = 1
        memcpy(data_frame1+1, dev_data_wrapper.u.serialize, sizeof(uint8_t)*24);
        memcpy(data_frame2+1, dev_data_wrapper.u.serialize+24, sizeof(uint8_t)*24);
        
        // send data to the server (via gateway)
        if(device_config.is_confirmed){
          tos_lora_module_send(data_frame1, sizeof(data_frame1));
          tos_lora_module_send(data_frame2, sizeof(data_frame2));
        }else{
          tos_lora_module_send_unconfirmed(data_frame1, sizeof(data_frame1));
          tos_lora_module_send_unconfirmed(data_frame2, sizeof(data_frame2));
        }
        tos_task_delay(device_config.report_period * 1000);
    }
}

