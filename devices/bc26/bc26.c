#include "bc26.h"
#include "tos_hal.h"
#include "sal_module_wrapper.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

typedef struct ip_addr_st {
    uint8_t seg1;
    uint8_t seg2;
    uint8_t seg3;
    uint8_t seg4;
}ip_addr_t;

static ip_addr_t domain_parser_addr={0};

static int bc26_reset(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 3000, "AT+QRST=1\r\n");
    return 0;
}

static int bc26_psm_lock(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, "AT+SM=LOCK\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int bc26_open_cfun(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, "AT+CFUN=1\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int bc26_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

static int bc26_open_err_code(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    tos_at_cmd_exec(&echo, 1000, "AT+CMEE=1\r\n");
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

static int bc26_check_sim(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    tos_at_cmd_exec(&echo, 1000, "AT+CIMI\r\n");
    if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
        return 0;
    }
    return -1;
}

static int bc26_get_net(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 2000, "AT+CGATT=1\r\n");
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int bc26_signal_quality_check(void)
{
    int rssi, ber;
    at_echo_t echo;
    char echo_buffer[32], *str;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+CSQ\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    str = strstr(echo.buffer, "+CSQ:");
    if (!str) 
    {
        return -1;
    }
    sscanf(str, "+CSQ:%d,%d", &rssi, &ber);
    if (rssi == 99) {
        return -1;
    }

    return 0;
}

static int bc26_set_band(void)
{
    int try = 0;
    at_echo_t echo;
    char echo_buffer[32];

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "OK");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+QBAND=1,8\r\n");
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }

    return -1;
}

static int bc26_band_check(void)
{
    int band;
    at_echo_t echo;
    char echo_buffer[32], *str;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 1000, "AT+QBAND?\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    str = strstr(echo.buffer, "+QBAND:");
    if (!str) 
    {
        return -1;
    }
    sscanf(str, "+QBAND:%d", &band);
    if (band != 8) {
        return -1;
    }

    return 0;
}

static int bc26_init(void)
{
    printf("Init BC26...\n" );

    if (bc26_reset() != 0) {
        printf("bc26 reset FAILED\n");
        return -1;
    }
    tos_stopwatch_delay_ms(3000);

    if (bc26_psm_lock() != 0) {
        printf("bc26 psm lock FAILED\n");
        return -1;
    }

    if (bc26_open_cfun() != 0) {
        printf("bc26 open cfun FAILED\n");
        return -1;
    }

    if (bc26_echo_close() != 0) {
        printf("echo close FAILED\n");
        return -1;
    }

    if (bc26_open_err_code() != 0) {
        printf("bc26 open err code FAILED\n");
        return -1;
    }

    if (bc26_check_sim() != 0) {
        printf("bc26 check sim FAILED\n");
        return -1;
    }

    if (bc26_get_net() != 0) {
        printf("bc26 get net FAILED\n");
        return -1;
    }

    if (bc26_set_band() != 0) {
        printf("set band FAILED\n");
        return -1;
    }

    if (bc26_band_check() != 0) {
        printf("check band FAILED\n");
        return -1;
    }

    if (bc26_signal_quality_check() != 0) {
        printf("check csq FAILED\n");
        return -1;
    }
    printf("Init BC26 done\n");
    
    return 0;
}

static int bc26_connect(const char *ip, const char *port, sal_proto_t proto)
{
    int id, send_port = 0,try = 0, is_connected = 0;
    at_echo_t echo;
    char echo_buffer[32],*str;

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);

    tos_at_cmd_exec(&echo, 2000, "AT+QSOC=1,1,1\r\n");
    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }
    str = strstr(echo.buffer, "+QSOC=");
    if (!str) 
    {
        return -1;
    }
    sscanf(str, "+QSOC=%d", &id);

    printf("get socket id is %d\r\n",id);
    id = tos_at_channel_alloc_id(id, ip, port);
    if (id == -1) {
        return -1;
    }

    sscanf(port, "%d", &send_port);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 8000, "AT+QSOCON=%d,%d,\"%s\"\r\n", id, send_port,ip);
        if (echo.status == AT_ECHO_STATUS_OK) {
            is_connected = 1;
            break;
        }

        continue;
    }

    if (!is_connected) {
        tos_at_channel_free(id);
        return -1;
    }

    return id;
}

static char __num2hex(uint8_t num)
{
    if (num <= 0x9) {
        return num + '0';
    }

    if ((0xA <= num) && (num <= 0xF)) {
        return num - 0xA + 'A';
    }

    return (char)-1;
}

static void __hex2str(uint8_t *in, char *out, int len)
{
    int i = 0;

    for (i = 0; i < len; ++i) {
        out[i * 2] = __num2hex(in[i] >> 4);
        out[i * 2 + 1] = __num2hex(in[i] & 0x0F);
    }
    out[2 * len] = '\0';
}


static int bc26_send(int id, const void *buf, size_t len)
{
    char *str_buf = NULL;
    at_echo_t echo;

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    str_buf = tos_mmheap_calloc(2 * len + 1, sizeof(char));
    if (!str_buf) {
        return -1;
    }

    __hex2str((uint8_t *)buf, str_buf, len);

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000,
                            "AT+QSOSEND=%d,%d,%s\r\n",
                            id, len, str_buf);

    tos_mmheap_free(str_buf);

    if (echo.status != AT_ECHO_STATUS_OK) {
        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();

    return len;
}


static int bc26_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
     return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int bc26_recv(int id, void *buf, size_t len)
{
    return bc26_recv_timeout(id, buf, len, (uint32_t)4000);
}

k_sem_t domain_parser_sem;

static int bc26_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    at_echo_t echo;
    char echo_buffer[128];
    
    tos_sem_create_max(&domain_parser_sem, 0, 1);

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+QIDNSGIP=1,\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK)
	{
        return -1;
    }
    
    tos_sem_pend(&domain_parser_sem, TOS_TIME_FOREVER);

    /*
        +QIURC: "dnsgip",0,1,0

		+QIURC: "dnsgip","xxx.xxx.xxx"
    */
    snprintf(host_ip, host_ip_len, "%d.%d.%d.%d", domain_parser_addr.seg1, domain_parser_addr.seg2, domain_parser_addr.seg3, domain_parser_addr.seg4);
    host_ip[host_ip_len - 1] = '\0';

    printf("GOT IP: %s\n", host_ip);

    return 0;
}

static int bc26_close(int id)
{
    tos_at_cmd_exec(NULL, 1000, "AT+QSODIS=%d",id);

    tos_at_cmd_exec(NULL, 1000, "AT+QSOCL=%d",id);

    tos_at_channel_free(id);

    return 0;
}
__STATIC__ uint8_t __ascii2hex(char in) {
    if (('0' <= in) && (in <= '9')) {
        return in - '0';
    }
    if (('A' <= in) && (in <= 'F')) {
        return in - 'A' + 10;
    }
    if (('a' <= in) && (in <= 'f')) {
        return in - 'a' + 10;
    }
    return (uint8_t)-1;
}

__STATIC__ void __asciistr2hex(char *in, uint8_t *out, int len) {
    int i = 0;

    for (i = 0; i < len; i += 2) {
        out[i / 2] = (__ascii2hex(in[i]) << 4) + __ascii2hex(in[i + 1]);
    }
}

__STATIC__ uint8_t incoming_data_buffer[1024];
__STATIC__ char ascii_stream[1024];
__STATIC__ uint8_t hex_stream[512];

__STATIC__ void bc26_incoming_data_process(void)
{
    uint8_t data;
    int channel_id = 0, data_len = 0, read_len, i = 0, ds_i = 0;
    int socket = 0, ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0, port = 0, length = 0, remaining_length = 0;

    /*
    +QSONMI=0,4\r\n
    0: socket
    4: length
    */

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == ',') {
            break;
        }
        channel_id = channel_id * 10 + (data - '0');
    }

    while (1) {
        if (tos_at_uart_read(&data, 1) != 1) {
            return;
        }

        if (data == '\r') {
            continue;
        }
        if (data == '\n') {
            break;
        }
        data_len = data_len * 10 + (data - '0');
    }

    if (data_len > sizeof(incoming_data_buffer)) {
        data_len = sizeof(incoming_data_buffer);
    }

    // tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(NULL, 0, "AT+QSORF=%d,%d\r\n", channel_id, data_len);

    // wait for uart buffer to be filled
    /*
        ATTENTION:
        we cannot use tos_at_cmd_exec(NULL, timeout) to delay, because we are in at framework's parser
        task now(current function is a callback called by parser task), delay in tos_at_cmd_exec is
        tos_task_delay, this may cause a task switch, data receiving may be interrupted.
        so we must tos_at_cmd_exec(NULL, 0), and do the delay by tos_stopwatch_delay_ms.
     */
    tos_stopwatch_delay_ms(1000);

    /*
    1,xxx.yyy.zzz.www,8000,3,010203,0\r\n
    1: socket
    xxx.yyy.zzz.www: remote ip
    8000: remote port
    3: length
    010203: data
    0: remaining length
     */

    // skip the leading \r\n
    if (tos_at_uart_read(&data, 1) != 1) {
        if (data != '\r') {
            return;
        }
    }

    if (tos_at_uart_read(&data, 1) != 1) {
        if (data != '\n') {
            return;
        }
    }

    read_len = tos_at_uart_readline(incoming_data_buffer, sizeof(incoming_data_buffer));

    for (i = 0; i < read_len; ++i) {
        if (incoming_data_buffer[i] == ',') {
            ++i;
            break;
        }
        socket = socket * 10 + (incoming_data_buffer[i] - '0');
    }

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == '.') {
            ++i;
            break;
        }
        ip1 = ip1 * 10 + (incoming_data_buffer[i] - '0');
    }

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == '.') {
            ++i;
            break;
        }
        ip2 = ip2 * 10 + (incoming_data_buffer[i] - '0');
    }

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == '.') {
            ++i;
            break;
        }
        ip3 = ip3 * 10 + (incoming_data_buffer[i] - '0');
    }

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == ',') {
            ++i;
            break;
        }
        ip4 = ip4 * 10 + (incoming_data_buffer[i] - '0');
    }

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == ',') {
            ++i;
            break;
        }
        port = port * 10 + (incoming_data_buffer[i] - '0');
    }

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == ',') {
            ++i;
            break;
        }
        length = length * 10 + (incoming_data_buffer[i] - '0');
    }

    if (length > sizeof(ascii_stream)) {
        return;
    }

    for (ds_i = 0; i < read_len; ++i) {
        if (incoming_data_buffer[i] == ',') {
            ++i;
            break;
        }
        ascii_stream[ds_i++] = incoming_data_buffer[i];
    }
    ascii_stream[ds_i] = '\0';

    for (; i < read_len; ++i) {
        if (incoming_data_buffer[i] == '\r') {
            break;
        }
        remaining_length = remaining_length * 10 + (incoming_data_buffer[i] - '0');
    }

    __asciistr2hex(ascii_stream, hex_stream, length * 2);
    tos_at_channel_write(channel_id, hex_stream, length);
}

__STATIC__ void bc26_demoin_parser_data_process(void)
{
    uint8_t data;
    if (tos_at_uart_read(&data, 1) != 1) {
        return;
    }

    if (data == '0') {
        return;
    }
    
    if (data == '\"') {
        /* start parser domain */
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '.') {
                break;
            }
            domain_parser_addr.seg1 = domain_parser_addr.seg1 *10 + (data-'0');
        }
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '.') {
                break;
            }
            domain_parser_addr.seg2 = domain_parser_addr.seg2 *10 + (data-'0');
        }
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '.') {
                break;
            }
            domain_parser_addr.seg3 = domain_parser_addr.seg3 *10 + (data-'0');
        }
        while (1) {
            if (tos_at_uart_read(&data, 1) != 1) {
                return;
            }
            if (data == '\"') {
                break;
            }
            domain_parser_addr.seg4 = domain_parser_addr.seg4 *10 + (data-'0');
        }
        tos_sem_post(&domain_parser_sem);
    }
    return;
    
}

at_event_t bc26_at_event[] = {
    { "+QSONMI=", bc26_incoming_data_process },
    { "+QIURC: \"dnsgip\",", bc26_demoin_parser_data_process}
};

sal_module_t nb_iot_module_bc26 = {
    .init           = bc26_init,
    .connect        = bc26_connect,
    .send           = bc26_send,
    .recv_timeout   = bc26_recv_timeout,
    .recv           = bc26_recv,
    .close          = bc26_close,
    .parse_domain   = bc26_parse_domain,
};


int bc26_sal_init(hal_uart_port_t uart_port)
{
    if (tos_at_init(uart_port, bc26_at_event,
                        sizeof(bc26_at_event) / sizeof(bc26_at_event[0])) != 0) {
        return -1;
    }

    tos_stopwatch_delay_ms(1000);

    if (tos_sal_module_register(&nb_iot_module_bc26) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}

