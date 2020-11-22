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
#include <math.h>

//prepare for connection to the server
#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define MAX_PAYLOAD_LEN 200

int self_ID = 666;
int other_ID;

char buf[24];
char payload[24] = "out";
char* token;
// payload format:
// "ID:888" : broadcasting self ID to the world
// "HIT:888:666": report collision of self id and other ID to backend

int chanl;
int value;
int i = 0;

// Whether buzzer is on or off
bool buzzerOn = false;
int buzzFreq = 500;

signed short rssi = -1200;

int alarmTimer = 0;

//static struct ctimer alarmTimer;

// Stop alarm after 5 seconds
void alarmCallback() {
    printf("Stopping alarm...\n\r");
    buzzer_stop();
    buzzerOn = false;
}

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
	// payload format:
	// "ID:888" : broadcasting self ID to the world
	// "HIT:888:666": report collision of self id and other ID to backend
		
        // Broadcast payload to the world
		sprintf(payload, "ID:%d", self_ID);
		
		if (i % 1000 == 0){
			printf("Sending: [%s] on channel %d\n\r", payload, chanl);
		}
		
		//send and clean send box
		NETSTACK_RADIO.send(payload, sizeof(payload)+1);
		memset(&payload, 0, sizeof(payload)+1);
		
        // Only receive signal if not buzzing already
        if (buzzerOn == false) {			
			
            // Receive incoming signal
		    if (NETSTACK_RADIO.read((void*)buf, 48) > 0) {
				
				token = strtok(buf, ":"); // token == ID or HIT
	
				if (strcmp(token, "ID") == 0){
					token = strtok(NULL, ":");
					
					other_ID =  atoi(token);
					
					rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
					//printf("Received: [%s] -> RSSI: [%d] on channel %d\n\r", buf, rssi, chanl);
					
		            float alpha = -16.022;
		            float constant = 20.958;    
		            float distance = exp((rssi - constant)/alpha);
		            printf("Distance from node %d = %d, rssi: %d\n\r", other_ID, (int)distance, rssi);

					// Turn on buzzing if too close
					// distance smaller than 200 cm
					if (rssi >= -49){
		                if (buzzerOn == false) {
						    buzzer_start(buzzFreq);
		                    buzzerOn = true;
		                    buzzFreq += 100;
		                    alarmTimer = 1000;
		                    printf("CLOSE CONTACT! Alarm buzzing...\r\n");
							
							// send collision message. 
		                    sprintf(payload, "HIT:%d:%d", self_ID, other_ID);
		                    NETSTACK_RADIO.send(payload, sizeof(payload)+1);
							memset(&payload, 0, sizeof(payload)+1);
		                }
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
