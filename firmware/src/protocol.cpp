/*
 * File:	protocol.cpp
 * Author:	Tristan Emma
 * Purpose:	library for custom protocol:
 * 		intilializing function 
 * 		protocol parsing state machine
 * 		protocol sending function
 * 		
 * 		Protocol Specification:
 * 		SOF	LEN	SRC	CMD	PAYLOAD	CRC
 * 		1B	1B	1B	1B	N bytes	1B
 * 
 * 		SOF = 0xAA
 * 		LEN = 2 + |PAYLOAD|
 * 		SRC = node_id
 * 		CMD = MACRO for command
 * 		PAYLOAD = array of bytes
 * 		CRC = XOR of all bytes from LEN through PAYLOAD
 */ 
#include "protocol.h"
#include "config.h"
#include <Arduino.h>

/* states of parser */
typedef enum {
	WAIT_SOF,
	READ_LEN,
	READ_BODY,
	READ_CRC
} ParserState;

/* write_fn set on init for where data being sent */
static protocol_write_fn write_fn;

/* variable associated with Parser State Machine */
static ParserState state = WAIT_SOF;
static Frame rx;
static uint8_t rx_index = 0;
static uint8_t crc = 0;

/*
 * protocol_init(protocol_write_fn cb){
 * 	initializes state machine 
 * 	sets write_fn
 */
void protocol_init(protocol_write_fn cb) {
	write_fn = cb;
	state = WAIT_SOF;
}

void protocol_set_esp(protocol_write_fn cb){
	write_fn = cb;

}

/*
 * protocol_process_byte(uint8_t b)
 * 	main Parser state Machine:
 * 	-determines current action based on state
 * 	-stores bytes according to protocol in static Frame rx
 * 	-confirms checksum then dispatches the frame
 * 	-sends error via write_fn if checksum does not match
 */
void protocol_process_byte(uint8_t b) {
	switch (state) {

		case WAIT_SOF: {
			if (b == SOF) {
				state = READ_LEN;
				crc = 0;
			}
			break;
		}
		case READ_LEN: {
			rx.len = b;
			crc ^= b;
			rx_index = 0;
			state = READ_BODY;
			break;
		}
		case READ_BODY:{
			if (rx_index == 0)
				rx.src = b;
			else if (rx_index == 1)
				rx.cmd = b;
			else
				rx.payload[rx_index - 2] = b;

			crc ^= b;
			rx_index++;

			if (rx_index >= rx.len)
				state = READ_CRC;
			break;
		}
		case READ_CRC: {
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
}

/*
 * protocol_send(uint8_t cmd, uing8_t *payload, uint8_t len)
 * 	takes cmd id, array of bytes as payload, and the length of
 * 	that array
 * 	uses write_fn to send bytes as specified by protocol
 * 	builds checksum and sends as final byte
 */
void protocol_send(uint8_t cmd, uint8_t *payload, uint8_t len) {
	uint8_t frame_bytes[len + 5];
	uint8_t crc = 0;

	frame_bytes[0] = SOF;

	frame_bytes[1] = len + 2; // SRC + CMD + payload
	crc ^= (len + 2);

	frame_bytes[2] = g_config.node_id;
	crc ^= g_config.node_id;

	frame_bytes[3] = cmd;
	//Serial.print(temp_cmd);
	//Serial.print(" ");
	crc ^= cmd;

	uint8_t j = 4;
	for (uint8_t i = 0; i < len; i++) {
		frame_bytes[j] = payload[i];
		crc ^= payload[i];
		j++;
	}

	frame_bytes[j] = crc;
	write_fn(frame_bytes, len + 5);
}

void protocol_send_report(Frame *f){
	uint8_t frame_bytes[f->len + 3];
	uint8_t crc = 0;

	frame_bytes[0] = SOF;

	frame_bytes[1] = f->len; // SRC + CMD + payload
	crc ^= f->len;

	frame_bytes[2] = f->src;
	crc ^= f->src;

	frame_bytes[3] = f->cmd;
	//Serial.print(temp_cmd);
	//Serial.print(" ");
	crc ^= f->cmd;

	uint8_t j = 4;
	for (uint8_t i = 0; i < f->len - 2; i++) {
		frame_bytes[j] = (f->payload)[i];
		crc ^= (f->payload)[i];
		j++;
	}

	frame_bytes[j] = crc;
	write_fn(frame_bytes, f->len + 5);
}
