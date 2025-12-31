/*
 * File:	protocol.c
 * Author:	Tristan Emma
 * Purpose:	library for custom protocol
 */ 
#include "protocol.h"
#include "config.h"
#include <Arduino.h>

typedef enum {
	WAIT_SOF,
	READ_LEN,
	READ_BODY,
	READ_CRC
} ParserState;

static protocol_write_fn write_fn;

static ParserState state = WAIT_SOF;
static Frame rx;
static uint8_t index = 0;
static uint8_t crc = 0;

void protocol_init(protocol_write_fn cb) {
	write_fn = cb;
	state = WAIT_SOF;
}

void protocol_process_byte(uint8_t b) {
	Serial.print(b);
	switch (state) {

		case WAIT_SOF:
			if (b == SOF) {
				state = READ_LEN;
				crc = 0;
			}
			break;

		case READ_LEN:
			rx.len = b;
			crc ^= b;
			index = 0;
			state = READ_BODY;
			break;

		case READ_BODY:
			if (index == 0)
				rx.src = b;
			else if (index == 1)
				rx.cmd = b;
			else
				rx.payload[index - 2] = b;

			crc ^= b;
			index++;

			if (index >= rx.len)
				state = READ_CRC;
			break;

		case READ_CRC:
			if (crc == b) {
				dispatch_frame(&rx);
			} else {
				uint8_t err = ERR_BAD_CRC;
				protocol_send(CMD_ERROR, &err, 1);
			}
			state = WAIT_SOF;
			break;
	}
}

void protocol_send(uint8_t cmd, uint8_t *payload, uint8_t len) {
	uint8_t crc = 0;

	uint8_t temp_sof = SOF;
	write_fn(&temp_sof, 1);

	uint8_t temp_len = len + 2;
	write_fn(&temp_len, 1); // SRC + CMD + payload
	crc ^= (len + 2);

	write_fn(&(g_config.node_id), 1);
	crc ^= g_config.node_id;

	uint8_t temp_cmd = cmd;
	//Serial.print(temp_cmd);
	//Serial.print(" ");
	write_fn(&temp_cmd, 1);
	crc ^= cmd;

	for (uint8_t i = 0; i < len; i++) {
		write_fn(payload + i, 1);
		crc ^= payload[i];
	}

	write_fn(&crc, 1);
}
