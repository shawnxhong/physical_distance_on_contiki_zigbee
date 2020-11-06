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

static unsigned char packet[256] = {0};

PROCESS(zigphy_rx_process, "zigphy_rx_process");
AUTOSTART_PROCESSES(&zigphy_rx_process);
PROCESS_THREAD(zigphy_rx_process, ev, data)
{
	PROCESS_BEGIN();
	watchdog_stop();
	NETSTACK_RADIO.set_value (RADIO_PARAM_POWER_MODE, 1);
	NETSTACK_RADIO.set_value (RADIO_PARAM_RX_MODE, 0); //disable Address Filtering
	while(1) {
		if (NETSTACK_RADIO.read((void*)packet, 256) > 0) {
			printf("RSSI: %d\n", (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI));
		}
	}
	PROCESS_END();
}
