#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/netstack.h"
#include "dev/radio.h"
#include "dev/watchdog.h"
#include "sys/clock.h"
#include <stdio.h>
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#include "board-peripherals.h"


char buf[24];
char payload[24] = "out";

int chanl;
int value;
int i = 0;

//int buzFreq = 1000;

signed short rssi = -100;

PROCESS(zigphy_rx_process, "zigphy_rx_process");
//PROCESS(zigphy_tx_process, "zigphy_tx_process");
AUTOSTART_PROCESSES(&zigphy_rx_process);


PROCESS_THREAD(zigphy_rx_process, ev, data)
{
	PROCESS_BEGIN();
	
	watchdog_stop();
	
	NETSTACK_RADIO.set_value (RADIO_PARAM_POWER_MODE, 1);
	NETSTACK_RADIO.set_value (RADIO_PARAM_RX_MODE, 0); //disable Address Filtering
	
	//NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &chanl);
	NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, RADIO_CONST_CHANNEL_MAX);
	NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &chanl);
	
	
	
	while(1) {
	
		sprintf(payload, "I am gay: %d", i);
		
		if (++i % 1000 == 0) printf("sending: %s on %d\n", payload, chanl);
		
		NETSTACK_RADIO.send(payload, sizeof(payload)+1);
		
		memset(&payload, 0, sizeof(payload)+1);
		
		if (NETSTACK_RADIO.read((void*)buf, 48) > 0) {
			rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
			printf("received: %s, RSSI: %d on %d\n", buf, rssi, chanl);
			
			// turn on buzzing
			if ( rssi > -50){
				buzzer_start(1000);		
			}
			else
			{
				buzzer_stop();
			}
			memset(&buf, 0, sizeof(buf)+1);
		}		
	}
	PROCESS_END();
}
