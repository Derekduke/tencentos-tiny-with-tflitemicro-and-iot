#include "main.h"
#include "cmsis_os.h"
#include "ethernetif.h"
#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "bsp_eth.h"
/* Variables Initialization */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];

void TCPIP_Init(void)
{
    tcpip_init(NULL, NULL);

    /* IP addresses initialization */
    /* USER CODE BEGIN 0 */
    #if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
    #else
    IP4_ADDR(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
    IP4_ADDR(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
    IP4_ADDR(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
    #endif /* USE_DHCP */
    /* USER CODE END 0 */
    /* Initilialize the LwIP stack without RTOS */
    /* add the network interface (IPv4/IPv6) without RTOS */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* Registers the default network interface */
    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif))
    {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
    }
    else
    {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
    }

    #if LWIP_DHCP	   			//��ʹ����DHCP
    int err;
    /*  Creates a new DHCP client for this interface on the first call.
    Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
    the predefined regular intervals after starting the client.
    You can peek in the netif->dhcp struct for the actual DHCP status.*/

    printf("�����̽�ʹ��DHCP��̬����IP��ַ,�������Ҫ����lwipopts.h�н�LWIP_DHCP����Ϊ0\n\n");

    err = dhcp_start(&gnetif);      //����dhcp
    if(err == ERR_OK)
    printf("lwip dhcp init success...\n\n");
    else
    printf("lwip dhcp init fail...\n\n");
    while(ip_addr_cmp(&(gnetif.ip_addr),&ipaddr))   //�ȴ�dhcp�����ip��Ч
    {
    tos_task_delay(1);
    } 
    #endif
    printf("����IP��ַ��:%d.%d.%d.%d\n\n",  \
        ((gnetif.ip_addr.addr)&0x000000ff),       \
        (((gnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((gnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((gnetif.ip_addr.addr)&0xff000000)>>24);
}
