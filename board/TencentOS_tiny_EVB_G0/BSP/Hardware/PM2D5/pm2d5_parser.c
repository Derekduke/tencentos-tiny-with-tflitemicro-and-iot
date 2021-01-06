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
#include "pm2d5_parser.h"

static pm2d5_parser_ctrl_t pm2d5_parser_ctrl;

static k_stack_t pm2d5_parser_task_stack[PM2D5_PARSER_TASK_STACK_SIZE];

static uint8_t pm2d5_parser_buffer[PM2D5_PARSER_BUFFER_SIZE];

/**
 * @brief   PM2D5������ԭʼ����
 * @note    ������ÿ���ϱ�32�ֽڣ�ͷ��0x42��0x4d�̶������ڽ�����ͷ��֡���������ж��Ƿ����������ݰ�������ֻ�洢�����28�ֽ�
 */
typedef struct pm2d5_raw_data_st {

    uint8_t     data1_h;
    uint8_t     data1_l;

    uint8_t     data2_h;
    uint8_t     data2_l;

    uint8_t     data3_h;
    uint8_t     data3_l;

    uint8_t     data4_h;
    uint8_t     data4_l;

    uint8_t     data5_h;
    uint8_t     data5_l;

    uint8_t     data6_h;
    uint8_t     data6_l;

    uint8_t     data7_h;
    uint8_t     data7_l;

    uint8_t     data8_h;
    uint8_t     data8_l;

    uint8_t     data9_h;
    uint8_t     data9_l;

    uint8_t     data10_h;
    uint8_t     data10_l;

    uint8_t     data11_h;
    uint8_t     data11_l;

    uint8_t     data12_h;
    uint8_t     data12_l;

    uint8_t     data13_h;   //�汾��
    uint8_t     data13_l;   //�������

    uint8_t     chk_sum_h;  //��У���8λ
    uint8_t     chk_sum_l;  //��У���8λ
} pm2d5_raw_data_t;

typedef union pm2d5_raw_data_un {
    uint8_t data[30];
    pm2d5_raw_data_t pm2d5_raw_data;
}pm2d5_raw_data_u;

/**
 * @brief   ��PM2D5������������һ���ֽ�����
 * @param   data  ���������
 * @retval  none
 * @note    ��Ҫ�û��ڴ����жϺ������ֶ�����
*/
void pm2d5_parser_input_byte(uint8_t data)
{
    if (tos_chr_fifo_push(&pm2d5_parser_ctrl.parser_rx_fifo, data) == K_ERR_NONE) {
        /* �������ݳɹ����ͷ��ź��������� */
        tos_sem_post(&pm2d5_parser_ctrl.parser_rx_sem);
    }
}

/**
 * @brief   PM2D5��������chr fifo��ȡ��һ���ֽ�����
 * @param   none
 * @retval  �������ض�ȡ���ݣ����󷵻�-1
*/
static int pm2d5_parser_getchar(void)
{
    uint8_t chr;
    k_err_t err;
    
    /* ���õȴ��ź������ź���Ϊ�ձ�ʾchr fifo�������� */
    if (tos_sem_pend(&pm2d5_parser_ctrl.parser_rx_sem, TOS_TIME_FOREVER) != K_ERR_NONE) {
        return -1;
    }
    
    /* ��chr fifo��ȡ������ */
    err = tos_chr_fifo_pop(&pm2d5_parser_ctrl.parser_rx_fifo, &chr);

    return err == K_ERR_NONE ? chr : -1;
}

/**
 * @brief   PM2D5��ȡ������ԭʼ���ݲ�����
 * @param   void
 * @retval  �����ɹ�����0������ʧ�ܷ���-1
*/
static int pm2d5_parser_read_raw_data(pm2d5_raw_data_u *pm2d5_raw_data, pm2d5_data_u *pm2d5_data)
{
    int i;
    uint8_t  len_h,len_l;
    uint16_t len;
    uint16_t check_sum;
    uint16_t check_sum_cal = 0x42 + 0x4d;
   
    /* ��ȡ������֡���� */
    len_h = pm2d5_parser_getchar();
    len_l = pm2d5_parser_getchar();
    len = (len_h << 8) | len_l;
    
    if ( len != 0x001C) {
        //�Ǵ�����ֵ���ݣ���ջ���
        for (i = 0; i < len; i++) {
            pm2d5_parser_getchar();
        }
        return -1;
    }
    
    /* ��ȡ������ԭʼ���� */
    for (i = 0; i < len; i++) {
        pm2d5_raw_data->data[i] = pm2d5_parser_getchar();
    }
    
    /* ��У�� */
    //ͨ�����ݼ����У��
    check_sum_cal = check_sum_cal + len_h + len_l;
    for (i = 0; i < len -2; i++) {
        check_sum_cal += pm2d5_raw_data->data[i];
    }
    //Э���и����ĺ�У��ֵ
    check_sum = (pm2d5_raw_data->pm2d5_raw_data.chk_sum_h << 8) + pm2d5_raw_data->pm2d5_raw_data.chk_sum_l;
    if (check_sum_cal != check_sum) {
        return -1;
    }
    
    /* �洢������ֵ */
    for (i = 0; i < sizeof(pm2d5_data_t); i++) {
        pm2d5_data->data[i] = pm2d5_raw_data->data[i];
    }
    
    return 0;
}

extern k_mail_q_t mail_q;
pm2d5_raw_data_u pm2d5_raw_data;
pm2d5_data_u     pm2d5_data;

/**
 * @brief   PM2D5����������
*/
static void pm2d5_parser_task_entry(void *arg)
{
    int chr, last_chr = 0;
    
    while (1) {
       
        chr = pm2d5_parser_getchar();
        if (chr < 0) {
            printf("parser task get char fail!\r\n");
            continue;
        }
        
        if (chr == 0x4d && last_chr == 0x42) {
            /* ��������ͷ */
            if (0 ==  pm2d5_parser_read_raw_data(&pm2d5_raw_data, &pm2d5_data)) {
                /* ��������֮��ͨ�����䷢�� */
                tos_mail_q_post(&mail_q, &pm2d5_data, sizeof(pm2d5_data_t));
            }
        }
        
        last_chr = chr;
    }
}

/**
 * @brief   ��ʼ��PM2D5������
 * @param   none
 * @retval  ȫ�������ɹ�����0���κ�һ������ʧ���򷵻�-1
*/
int pm2d5_parser_init(void)
{
    k_err_t ret;
    
    memset((pm2d5_parser_ctrl_t*)&pm2d5_parser_ctrl, 0, sizeof(pm2d5_parser_ctrl));
    
    /* ���� chr fifo */
    ret = tos_chr_fifo_create(&pm2d5_parser_ctrl.parser_rx_fifo, pm2d5_parser_buffer, sizeof(pm2d5_parser_buffer));
    if (ret != K_ERR_NONE) {
        printf("pm2d5 parser chr fifo create fail, ret = %d\r\n", ret);
        return -1;
    }
    
    /* �����ź��� */
    ret = tos_sem_create(&pm2d5_parser_ctrl.parser_rx_sem, 0);
    if (ret != K_ERR_NONE) {
        printf("pm2d5 parser_rx_sem create fail, ret = %d\r\n", ret);
        return -1;
    }
    
    /* �����߳� */
    ret = tos_task_create(&pm2d5_parser_ctrl.parser_task, "pm2d5_parser_task", 
                          pm2d5_parser_task_entry, NULL, PM2D5_PARSER_TASK_PRIO,
                          pm2d5_parser_task_stack,PM2D5_PARSER_TASK_STACK_SIZE,0);
    if (ret != K_ERR_NONE) {
        printf("pm2d5 parser task create fail, ret = %d\r\n", ret);
        return -1;
    }

    return 0;
}
