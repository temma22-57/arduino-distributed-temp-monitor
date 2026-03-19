/*
 * File:	commands.cpp
 * Author:	Tristan Emma
 * Purpose:	switch statement for commands in dispatch_frame()
 * 		- executes associated actions for the frame
 *		
 *		helper functions send_info() and send_temp() build
 *		payload and call protocol_send().
 */

#include <Arduino.h>
#include <WiFi.h>

#include "protocol.h"
#include "config.h"
#include "esp_lib.h"

#define FIRMWARE_VERSION 2

extern int16_t read_temperature_x100(void);

/*
 * send_info()
 * 	Builds payload for CMD_GET_INFO return 
 *	- Firmware version, Config version, Node_id, and interval
 *
 *	Calls protocol_send() to deliver payload
 */
static void send_info(void) {
	uint8_t payload[4] = {
		FIRMWARE_VERSION,
		CONFIG_VERSION,
		g_config.node_id,
		(uint8_t)g_config.sample_interval_s
	};
	protocol_send(CMD_INFO_REPORT, payload, 4);
}

/*
 * send_temp()
 * 	Builds payload for CMD_GET_TEMP return
 * 	-Upper byte, Lower byte of 16 bit temp
 *
 * 	Calls protocol_send() to deliver payload
 */
static void send_temp(void){
	int16_t temp = read_temperature_x100();
	uint8_t payload[2] = {
		(uint8_t)(temp >> 8),
		(uint8_t)(temp & 0xFF)
	};
	protocol_send(CMD_TEMP_REPORT, payload, 2);
}

static void send_mac(){
	uint8_t mac[6];
	WiFi.macAddress(mac);

	protocol_send(CMD_MAC_REPORT, mac, 6);
}

static void send_cmd_to_node(Frame *f){
	esp_now_set_broadcastAddress(f->payload);
	esp_now_write((f->payload + 6), (f->len - 2 - 6));
}

/*
 * dispatch_frame(Frame *f)
 * 	Dispatches the frame built out by the protocol parser
 * 	after input is received.
 * 	Performs repective actions for the various commands the
 * 	frame could hold
 */
void dispatch_frame(Frame *f) {
	switch (f->cmd) {

		case CMD_PING: {
			uint8_t code = SUCCESS_PING;
			protocol_send(CMD_SUCCESS, &code, 1);
			break;
		}
		case CMD_GET_INFO: {
			send_info();
			break;
		}
		case CMD_GET_TEMP: {
			send_temp();
			break;
		}
		case CMD_GET_MAC: {
			send_mac();
			break;
		}
		case CMD_SET_INTERVAL: {
			// combine payload to single int16_t 
			g_config.sample_interval_s =
				(f->payload[0] << 8) | f->payload[1]; 
			
			// save new config to EEPROM and send_info
			// back to PC
			config_save();
			send_info();
			break;
		}
		case CMD_SET_NODE_ID: {
			g_config.node_id = f->payload[0];
			config_save();
			send_info();
			break;
		}
		case CMD_SETUP_ESP_AS_NODE: {
			esp_now_setup_node(f->payload);
			break;
		}
		case CMD_SETUP_ESP_AS_HOST: {
			esp_now_setup_host();
			break;
		}
		case CMD_ADD_NODE_TO_HOST: {
			esp_now_setup_node(f->payload);
			break;
		}
		case CMD_SEND_TO_NODE: {
			send_cmd_to_node(f);
			break;
		}
		case CMD_TEMP_REPORT:
		case CMD_INFO_REPORT:
		case CMD_MAC_REPORT:
		case CMD_SUCCESS:
		case CMD_ERROR: {
			protocol_send_report(f);
			break;
		}
		default: {
			uint8_t err = ERR_UNKNOWN_CMD;
			protocol_send(CMD_ERROR, &err, 1);
			break;
		}
	}
}
