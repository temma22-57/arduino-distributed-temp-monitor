/*
 * File:	commands.c
 * Author:	Tristan Emma
 * Purpose:	houses function for switch statement of command
 * 		and executes associated actions
 *
 * TODO:	Error Handling and sending
 */

#include "protocol.h"
#include "config.h"
#include <Arduino.h>

#define FIRMWARE_VERSION 1

extern int16_t read_temperature_x100(void);

static void send_info(void) {
    uint8_t payload[4] = {
        FIRMWARE_VERSION,
        CONFIG_VERSION,
        g_config.node_id,
        (uint8_t)g_config.sample_interval_s
    };
    protocol_send(CMD_INFO_REPORT, payload, 4);
}

void dispatch_frame(Frame *f) {
    switch (f->cmd) {

        case CMD_PING:
        case CMD_GET_INFO:
            send_info();
            break;

        case CMD_GET_TEMP: {
            int16_t temp = read_temperature_x100();
            uint8_t payload[2] = {
                (uint8_t)(temp >> 8),
                (uint8_t)(temp & 0xFF)
            };
            protocol_send(CMD_TEMP_REPORT, payload, 2);
            break;
        }

        case CMD_SET_INTERVAL:
            g_config.sample_interval_s =
                (f->payload[0] << 8) | f->payload[1];
            g_config.wdt_target = config_wdt_target(&g_config); 
	        config_save();
            send_info();
            break;

        case CMD_SET_NODE_ID:
            g_config.node_id = f->payload[0];
            config_save();
            send_info();
            break;

        default: {
            uint8_t err = ERR_UNKNOWN_CMD;
            protocol_send(CMD_ERROR, &err, 1);
            break;
        }
    }
}
