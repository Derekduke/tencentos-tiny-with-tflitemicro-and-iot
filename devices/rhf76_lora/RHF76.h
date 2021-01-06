/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef __RHF76_H__
#define __RHF76_H__

#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "tos_at.h"
#include "lora_module_wrapper.h"

#define UART_RXBUF_MAXSIZE           256

typedef enum lora_class {
    LORA_CLASS_A,
    LORA_CLASS_B,
    LORA_CLASS_C,
} lora_class_t;

typedef enum lora_mode {
    LORA_MODE_LWOTAA,
    LORA_MODE_LWABP,
} lora_mode_t;

typedef enum lora_id_type {
    LORA_ID_TYPE_DEVADDR,
    LORA_ID_TYPE_DEVEUI,
    LORA_ID_TYPE_APPEUI,
} lora_id_type_t;

typedef enum lora_key_type {
    LORA_KEY_TYPE_APPKEY,
    LORA_KEY_TYPE_APPSKEY,
    LORA_KEY_TYPE_NWKSKEY,
} lora_key_type_t;

#define RHF76_ATCMD_SET_CLASS_A                 "AT+CLASS=A\r\n"
#define RHF76_ATCMD_SET_CLASS_B                 "AT+CLASS=B\r\n"
#define RHF76_ATCMD_SET_CLASS_C                 "AT+CLASS=C\r\n"
#define RHF76_ATCMD_REPLY_CLASS_A               "+CLASS: A"
#define RHF76_ATCMD_REPLY_CLASS_B               "+CLASS: B"
#define RHF76_ATCMD_REPLY_CLASS_C               "+CLASS: C"

#define RHF76_ATCMD_RESET                       "AT+RESET\r\n"

#define RHF76_ATCMD_FMT_SET_ID_DEVADDR          "AT+ID=\"devaddr\",\"%s\"\r\n"
#define RHF76_ATCMD_FMT_SET_ID_DEVEUI           "AT+ID=\"deveui\",\"%s\"\r\n"
#define RHF76_ATCMD_FMT_SET_ID_DEVAPPEUI        "AT+ID=\"appeui\",\"%s\"\r\n"

#define RHF76_ATCMD_FMT_SET_KEY_TYPE_APPKEY     "AT+KEY=\"appkey\",%s\r\n"
#define RHF76_ATCMD_FMT_SET_KEY_TYPE_APPSKEY    "AT+KEY=\"appskey\",%s\r\n"
#define RHF76_ATCMD_FMT_SET_KEY_TYPE_NWKSKEY    "AT+KEY=\"nwkskey\",%s\r\n"
#define RHF76_ATCMD_FMT_SEND_CMSGHEX            "AT+CMSGHEX=\"%s\"\r\n"
#define RHF76_ATCMD_FMT_SEND_MSGHEX             "AT+MSGHEX=\"%s\"\r\n"

#define RHF76_ATCMD_SET_REPT                    "AT+REPT=%d\r\n"
#define RHF76_ATCMD_SET_DELAY                   "AT+DELAY=%s\r\n"

#define RHF76_ATCMD_SET_BAND_CN470              "AT+DR=CN470\r\n"
#define RHF76_ATCMD_REPLY_BAND_CN470            "+DR: CN470"
#define RHF76_ATCMD_SET_DATA_RATE               "AT+DR=%d\r\n"

#define RHF76_ATCMD_SET_CHANNEL                 "at+ch=num,80-87\r\n"
#define RHF76_ATCMD_SET_ADR_OFF                 "at+adr=off\r\n"
#define RHF76_ATCMD_JOIN                        "AT+join\r\n"

#define RHF76_ATCMD_SET_MODE_LWOTAA             "AT+MODE=LWOTAA\r\n"
#define RHF76_ATCMD_SET_MODE_LWABP              "AT+MODE=LWABP\r\n"
#define RHF76_ATCMD_REPLY_MODE_LWOTAA           "+MODE: LWOTAA"
#define RHF76_ATCMD_REPLY_MODE_LWABP            "+MODE: LWABP"

int rhf76_lora_init(hal_uart_port_t uart_port);

/**
  * @brief  set the delay for RX1,RX2,JRX1,JRX2 OR query current delay config
  * @note   Examples:
  *         to set the delay(ms):
  *           rhf76_set_delay("RX1,2000"); 
  *
  *         to query current delay config:     
  *           rhf76_set_delay("?");
  *
  * @param  params     operation string
  * 
  * @retval int Status
  */
int rhf76_set_delay(char *param);

/**
  * @brief  set repeat times when sending unconfirmed message 
  * @note   it is equal to sending
  *         AT+REPT=2 
  *         +REPT: 2
  *             
  * @param  num       repeat times
  * 
  * @retval int Status
  */
int rhf76_set_repeat(uint8_t num);


/**
  * @brief  set date rate, which would affect the maximum payload size 
  * @note   this function is for DEBUG purpose only, it allows user to execute
  *         AT+ commands by passing the AT+ command to the function as an arg-
  *         ument. 
  *         For example,users can query the RF configuration by following code:
  *
  *         rhf76_at_cmd_exe("AT+MODE=TEST\r\n");
  *         rhf76_at_cmd_exe("AT+TEST=?\r\n");
  *
  * @param  num     data rate 
  * 
  * @retval int Status
  */
int rhf76_set_data_rate(uint8_t num);


/**
  * @brief  execute AT+ commands 
  * @note   this function is for DEBUG purpose only, it allows user to execute
  *         AT+ commands by passing the AT+ command to the function as an arg-
  *         ument. 
  *         For example,users can query the RF configuration by following code:
  *
  *         rhf76_at_cmd_exe("AT+MODE=TEST\r\n");
  *         rhf76_at_cmd_exe("AT+TEST=?\r\n");
  *
  * @param  cmd       AT+ commands
  * 
  * @retval int Status
  */
int rhf76_at_cmd_exe(char *cmd);

#endif /* __RHF76_H__ */

