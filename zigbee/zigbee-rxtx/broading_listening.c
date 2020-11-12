#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/netstack.h"
#include "dev/radio.h"
#include "dev/watchdog.h"
#include "sys/ctimer.h"
#include "sys/clock.h"
#include <stdio.h>
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#include "board-peripherals.h"

//prepare for connection to the server
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define MAX_PAYLOAD_LEN 200
static struct uip_udp_conn *server_conn;


char buf[24];
char payload[24] = "out";

int chanl;
int value;
int i = 0;

// Whether buzzer is on or off
bool buzzerOn = false;
int buzzFreq = 500;

signed short rssi = -100;


int alarmTimer = 0;

//static struct ctimer alarmTimer;

// Stop alarm after 5 seconds
void alarmCallback() {
    printf("Stopping alarm...\n\r");
    buzzer_stop();
    buzzerOn = false;
}

void send_record(){
	char msg[MAX_PAYLOAD_LEN];
	sprintf(msg, "close contact detacted");
	uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
	uip_udp_packet_sendto(server_conn, msg, strlen(msg),&server_conn->ripaddr, UIP_HTONS(3001));
	memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
}

PROCESS(zigphy_rx_process, "zigphy_rx_process");
//PROCESS(zigphy_tx_process, "zigphy_tx_process");
AUTOSTART_PROCESSES(&zigphy_rx_process);


PROCESS_THREAD(zigphy_rx_process, ev, data)
{
	//prepare for connection to the server
	#if UIP_CONF_ROUTER
	  uip_ipaddr_t ipaddr;
	#endif /* UIP_CONF_ROUTER */

	PROCESS_BEGIN();
	
	watchdog_stop();
	
	NETSTACK_RADIO.set_value (RADIO_PARAM_POWER_MODE, 1);
	NETSTACK_RADIO.set_value (RADIO_PARAM_RX_MODE, 0); //disable Address Filtering
	
	//NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &chanl);
	NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, RADIO_CONST_CHANNEL_MAX);
	NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &chanl);

	//prepare for connection to the server
	#if UIP_CONF_ROUTER
	  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
	#endif /* UIP_CONF_ROUTER */

	// print_local_addresses();
	server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
	udp_bind(server_conn, UIP_HTONS(3000));
	

	while(1) {
	
        // Broadcast payload to the world
		sprintf(payload, "I am gay: %d", i);
		if (i % 1000 == 0) printf("Sending: [%s] on channel %d\n\r", payload, chanl);
		NETSTACK_RADIO.send(payload, sizeof(payload)+1);
		memset(&payload, 0, sizeof(payload)+1);
		
        // Only receive signal if not buzzing already
        if (buzzerOn == false) {

            // Receive incoming signal
		    if (NETSTACK_RADIO.read((void*)buf, 48) > 0) {
			    rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
			    printf("Received: [%s] -> RSSI: [%d] on channel %d\n\r", buf, rssi, chanl);
			
			    // Turn on buzzing if too close
			    if (rssi >= -50){
                    if (buzzerOn == false) {
				        buzzer_start(buzzFreq);
                        buzzerOn = true;
                        buzzFreq += 100;
                        alarmTimer = 1000;
                        printf("CLOSE CONTACT! Alarm buzzing...\r\n");

                        send_record();
                    }	
			    }
		    }
        
        // Tag is already buzzing... so count down alarm timer
        } else {
            if (alarmTimer > 0) {
                alarmTimer--;
                if (alarmTimer == 0) {
                    alarmCallback();
                }
            }

            // Keep receiving signal to check the RSSI
		    if (NETSTACK_RADIO.read((void*)buf, 48) > 0) {
			    rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
			    if (i % 20 == 0) printf("Still received: [%s] -> RSSI: [%d] on channel %d\n\r", buf, rssi, chanl);
			}
        }   

        i++;
        memset(&buf, 0, sizeof(buf)+1);
	}


	PROCESS_END();
}
