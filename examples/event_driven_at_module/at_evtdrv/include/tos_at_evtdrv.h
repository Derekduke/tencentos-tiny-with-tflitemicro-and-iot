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

#ifndef _TOS_AT_EVTDRV_H_
#define _TOS_AT_EVTDRV_H_

#include "tos_evtdrv.h"
#include "tos_at_utils_evtdrv.h"
#include "tos_hal.h"

#define AT_AGENT_ECHO_OK            "OK"
#define AT_AGENT_ECHO_FAIL          "FAIL"
#define AT_AGENT_ECHO_ERROR         "ERROR"

#define AT_DATA_CHANNEL_NUM                     6
#define AT_DATA_CHANNEL_FIFO_BUFFER_SIZE        (2048 + 1024)

#define AT_UART_RX_FIFO_BUFFER_SIZE             (2048 + 1024)
#define AT_RECV_CACHE_SIZE                      2048

#define AT_CMD_BUFFER_SIZE                      512

#define AT_PARSER_TASK_STACK_SIZE               2048
#define AT_PARSER_TASK_PRIO                     2

#define EVENT_AT_UART_INCOMING     		(1u << 0)
#define EVENT_AT_PARSE_STATUS_OVERFLOW	(1u << 1)
#define EVENT_AT_PARSE_STATUS_EVENT		(1u << 2)
#define EVENT_AT_PARSE_STATUS_EXPECT		(1U << 3)
#define EVENT_AT_PARSE_STATUS_NEWLINE	(1U << 4)

typedef enum at_status_en {
    AT_STATUS_OK,
    AT_STATUS_ERROR,
    AT_STATUS_INVALID_ARGS,
} at_status_t;

typedef struct at_cache_st {
    uint8_t    *buffer;
    size_t      buffer_size;
    size_t      recv_len;
    size_t      curr_len;
    size_t      last_data;
} at_cache_t;

typedef enum at_parse_status_en {
    AT_PARSE_STATUS_NONE,
    AT_PARSE_STATUS_NEWLINE,
    AT_PARSE_STATUS_EVENT,
    AT_PARSE_STATUS_EXPECT,
    AT_PARSE_STATUS_OVERFLOW,
} at_parse_status_t;

typedef enum at_echo_status_en {
    AT_ECHO_STATUS_NONE,
    AT_ECHO_STATUS_OK,
    AT_ECHO_STATUS_FAIL,
    AT_ECHO_STATUS_ERROR,
    AT_ECHO_STATUS_EXPECT,
} at_echo_status_t;

typedef enum at_channel_status_en {
    AT_CHANNEL_STATUS_NONE,         /*< usually means we are try to get a channel status with invalid id */
    AT_CHANNEL_STATUS_HANGING,      /*< channel is not used */
    AT_CHANNEL_STATUS_WORKING,      /*< channel is being using */
    AT_CHANNEL_STATUS_BROKEN,       /*< channel is broken(module link to remote server is broken) */
} at_channel_status_t;

typedef struct at_data_channel_st {
    uint8_t             is_free;
    k_chr_fifo_t        rx_fifo;
    uint8_t            *rx_fifo_buffer;

    at_channel_status_t status;

    const char         *remote_ip;
    const char         *remote_port;
} at_data_channel_t;

typedef struct at_echo_st {
    char                   *buffer;
    size_t                  buffer_size;
    char                   *echo_expect;
    evtdrv_task_id_t        module_task_id;
    evtdrv_event_flag_t     event_flags;
    int                     line_num;
    at_echo_status_t        status;
    size_t                  __w_idx;
    evtdrv_event_flag_t   __expect_event;
} at_echo_t;

typedef void (*at_event_callback_t)(void);

typedef struct at_event_st {
    const char         *event_header;
    at_event_callback_t event_callback;
} at_event_t;

typedef struct at_agent_st {
    evtdrv_task_id_t  at_task_id;

    at_data_channel_t   data_channel[AT_DATA_CHANNEL_NUM];

    at_event_t     *event_table;
    size_t          event_table_size;

    at_echo_t      *echo;

    at_cache_t      recv_cache;

    at_timer_t      timer;

    char           *cmd_buf;

    hal_uart_t      uart;

    k_chr_fifo_t    uart_rx_fifo;
    uint8_t        *uart_rx_fifo_buffer;
} at_agent_t;

#define AT_AGENT        ((at_agent_t *)(&at_agent))

/**
 * @brief Write data to a channel.
 * Write data to a channel with certain id.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 * @param[in]   buffer      data buffer to write.
 * @param[in]   buffer_len  length of the buffer.
 *
 * @return  errcode
 * @retval  -1              write failed(error).
 * @retval  none -1         the number of bytes written.
 */
int tos_at_channel_write(int channel_id, uint8_t *buffer, size_t buffer_len);

/**
 * @brief Read data from a channel.
 * Read data from a channel with a timeout.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 * @param[out]  buffer      buffer to hold the data read.
 * @param[in]   buffer_len  length of the buffer.
 * @param[in]   timeout     timeout.
 *
 * @return  errcode
 * @retval  -1              read failed(error).
 * @retval  none -1         the number of bytes read.
 */
int tos_at_channel_read_timed(int channel_id, uint8_t *buffer, size_t buffer_len, uint32_t timeout);

/**
 * @brief Read data from a channel.
 * Read data from a channel.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 * @param[out]  buffer      buffer to hold the data read.
 * @param[in]   buffer_len  length of the buffer.
 *
 * @return  errcode
 * @retval  -1              read failed(error).
 * @retval  none -1         the number of bytes read.
 */
int tos_at_channel_read(int channel_id, uint8_t *buffer, size_t buffer_len);

/**
 * @brief Allocate a channel.
 * Allocate a channel with certain id.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 * @param[in]   ip          remote ip of the channel.
 * @param[in]   port        remote port of the channel.
 *
 * @return  errcode
 * @retval  -1              allocate failed(error).
 * @retval  none -1         the id of the channel.
 */
int tos_at_channel_alloc_id(int channel_id, const char *ip, const char *port);

/**
 * @brief Allocate a channel.
 * Allocate a channel.
 *
 * @attention None
 *
 * @param[in]   ip          remote ip of the channel.
 * @param[in]   port        remote port of the channel.
 *
 * @return  errcode
 * @retval  -1              allocate failed(error).
 * @retval  none -1         the id of the channel.
 */
int tos_at_channel_alloc(const char *ip, const char *port);

/**
 * @brief Free a channel.
 * Free a channel with certain id.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 *
 * @return  errcode
 * @retval  -1              free failed(error).
 * @retval  0               free successfully.
 */
int tos_at_channel_free(int channel_id);

/**
 * @brief Set channel broken.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 *
 * @return  errcode
 * @retval  -1              set failed(error).
 * @retval  0               set successfully.
 */
__API__ int tos_at_channel_set_broken(int channel_id);

/**
 * @brief Judge whether channel is working.
 *
 * @attention None
 *
 * @param[in]   channel_id  id of the channel.
 *
 * @return  at channel status(type of at_channel_status_t)
 */
__API__ int tos_at_channel_is_working(int channel_id);

/**
 * @brief Initialize the at framework.
 *
 * @attention None
 *
 * @param[in]   uart_port           port number of the uart thougth which the module connect to the MCU.
 * @param[in]   event_table         the listened event table.
 * @param[in]   event_table_size    the size of the listened event table.
 *
 * @return  errcode
 * @retval  -1              initialize failed(error).
 * @retval  0               initialize successfully.
 */
__API__ int tos_at_init(hal_uart_port_t uart_port, evtdrv_task_id_t at_task_id, at_event_t *event_table, size_t event_table_size);

/**
 * @brief De-initialize the at framework.
 *
 * @attention None
 *
 * @return
None
 */
void tos_at_deinit(void);

/**
 * @brief Create a echo struct.
 *
 * @attention None
 *
 * @param[in]   echo           pointer to the echo struct.
 * @param[out]  buffer         buffer to hold the received message from the module.
 * @param[in]   buffer_size    size of the buffer.
 * @param[in]   echo_expect    the expected echo message.
 *
 * @return  errcode
 * @retval  -1              create failed(error).
 * @retval  0               create successfully.
 */
int tos_at_echo_create(at_echo_t *echo, char *buffer, size_t buffer_size, char *echo_expect, evtdrv_task_id_t module_task_id, evtdrv_event_flag_t event_flags);

/**
 * @brief Execute an at command.
 *
 * @attention None
 *
 * @param[in]   echo            pointer to the echo struct.
 * @param[in]   timeout         command wait timeout .
 * @param[in]   cmd             at command.
 *
 * @return  errcode
 * @retval  -1              execute failed(error).
 * @retval  0               execute successfully.
 */
int tos_at_cmd_exec(at_echo_t *echo, uint32_t timeout, const char *cmd, ...);

/**
 * @brief Send raw data througth uart.
 *
 * @attention None
 *
 * @param[in]   echo            pointer to the echo struct.
 * @param[in]   timeout         command wait timeout .
 * @param[in]   buf             data to send.
 * @param[in]   size            size of the buf.
 *
 * @return  errcode
 * @retval  -1              execute failed(error).
 * @retval  0               execute successfully.
 */
int tos_at_raw_data_send(at_echo_t *echo, uint32_t timeout, const uint8_t *buf, size_t size);

/**
 * @brief Write byte to the at uart.
 * The function called by the uart interrupt, to put the data from the uart to the at framework.
 *
 * @attention None
 *
 * @param[in]   data            uart received data.
 *
 * @return  None
 */
void tos_at_uart_input_byte(uint8_t data);

/**
 * @brief Read data from the uart.
 * Read data from the uart, usually called in listened event callback.
 *
 * @attention None
 *
 * @param[out]  buffer          buffer to hold the data read from the uart.
 * @param[in]   buffer_len      length of the buffer.
 *
 * @return  length of the data read from the uart.
 */
int tos_at_uart_read(uint8_t *buffer, size_t buffer_len);

/**
 * @brief Read data from the uart.
 * Read data from the uart until meet a '\n', usually called in listened event callback.
 *
 * @attention None
 *
 * @param[out]  buffer          buffer to hold the data read from the uart.
 * @param[in]   buffer_len      length of the buffer.
 *
 * @return  length of the data read from the uart.
 */
int tos_at_uart_readline(uint8_t *buffer, size_t buffer_len);

/**
 * @brief Read data from the uart.
 * Read data from the uart until no more incoming data, usually called in listened event callback.
 *
 * @attention None
 *
 * @param[out]  buffer          buffer to hold the data read from the uart.
 * @param[in]   buffer_len      length of the buffer.
 *
 * @return  length of the data read from the uart.
 */
int tos_at_uart_drain(uint8_t *buffer, size_t buffer_len);

/**
 * @brief Get the remote ip of a channel.
 * Get the remote ip of a channel with certain id.
 *
 * @attention None
 *
 * @param[in]   channel_id      id of the channel.
 *
 * @return  remote ip of the channel.
 */
const char *tos_at_agent_channel_ip_get(int channel_id);

/**
 * @brief Get the remote port of a channel.
 * Get the remote port of a channel with certain id.
 *
 * @attention None
 *
 * @param[in]   channel_id      id of the channel.
 *
 * @return  remote port of the channel.
 */
const char *tos_at_agent_channel_port_get(int channel_id);

evtdrv_event_flag_t tos_at_evtdrv_task(evtdrv_event_flag_t event);

#endif /* _TOS_AT_EVTDRV_H_ */

