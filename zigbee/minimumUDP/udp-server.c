
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define MAX_PAYLOAD_LEN 200
static struct uip_udp_conn *server_conn;

uint32_t localUTCtime;

static struct ctimer UTCtick_ctimer;
static struct ctimer sending_ctimer;

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&resolv_process,&udp_server_process);
/*---------------------------------------------------------------------------*/
static void UTCtick(){
	ctimer_reset(&UTCtick_ctimer);
	localUTCtime++;
}

static void sending(){
	ctimer_reset(&sending_ctimer);

	char buf[MAX_PAYLOAD_LEN];
	PRINTF("----sending: 'hello %u'\r\n", (unsigned int)localUTCtime);

	sprintf(buf, "hello %u", (unsigned int)localUTCtime);
  	uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
	uip_udp_packet_sendto(server_conn, buf, strlen(buf),&server_conn->ripaddr, UIP_HTONS(3001));
	memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
}

static void tcpip_handler(void){
  if(uip_newdata()) {

	// char buf[MAX_PAYLOAD_LEN];
    ((char *)uip_appdata)[uip_datalen()] = 0;
    uint32_t *universalUTCtime = (uint32_t *)uip_appdata;
    localUTCtime = *universalUTCtime;

    // PRINTF("Received: %d (RSSI: %d)\r\n", (unsigned int)*universalUTCtime, (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI));
    PRINTF("Received: %d\r\n", (unsigned int)*universalUTCtime);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data){
	#if UIP_CONF_ROUTER
	  uip_ipaddr_t ipaddr;
	#endif /* UIP_CONF_ROUTER */

	  PROCESS_BEGIN();
	  PRINTF("UDP server started\r\n");

	#if RESOLV_CONF_SUPPORTS_MDNS
	  resolv_set_hostname("contiki-udp-server");
	#endif
	#if UIP_CONF_ROUTER
	  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
	#endif /* UIP_CONF_ROUTER */

	server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
	udp_bind(server_conn, UIP_HTONS(3000));


	ctimer_set(&UTCtick_ctimer, CLOCK_SECOND, UTCtick, NULL);
	ctimer_set(&sending_ctimer, CLOCK_SECOND * 2, sending, NULL);


	while(1) {
	PROCESS_YIELD();
	//Wait for tcipip event to occur
		if(ev == tcpip_event){
			tcpip_handler();
		}
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
