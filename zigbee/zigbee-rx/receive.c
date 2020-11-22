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

//static unsigned char packet[256] = {0};
char buf[24];
int chanl;
int tag_A;
int tag_B;
char* token;

PROCESS(zigphy_rx_process, "zigphy_rx_process");
AUTOSTART_PROCESSES(&zigphy_rx_process);

PROCESS_THREAD(zigphy_rx_process, ev, data)
{
	PROCESS_BEGIN();
	
	watchdog_stop();
	
	NETSTACK_RADIO.set_value (RADIO_PARAM_POWER_MODE, 1);
	NETSTACK_RADIO.set_value (RADIO_PARAM_RX_MODE, 0); //disable Address Filtering
	
	NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, RADIO_CONST_CHANNEL_MAX);
	NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &chanl);
	
	while(1) {
		if (NETSTACK_RADIO.read((void*)buf, 48) > 0) {
		// incoming format:
		// "ID:888" : broadcasting self ID to the world
		// "HIT:888:666": report collision of self id and other ID to backend
		
			token = strtok(buf, ":"); // token == ID or HIT
			if ( strcmp(token, "HIT") == 0){
				
				token = strtok(NULL, ":");
				tag_A = atoi(token);
				token = strtok(NULL, ":");
				tag_B = atoi(token);
				printf("%d-%d\r\n", tag_A, tag_B);
			}
		}
	}
	PROCESS_END();
}
