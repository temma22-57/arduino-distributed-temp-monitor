/*
 * File:	protocol.h
 * Author:	Tristan Emma
 * Purpose:	header file for protocol.c and commands.c
 * 		macro definitions for command byte codes, 
 * 		and error byte codes
 * 		typedef for Frame struct (for bytes in protocol)
 *
 * 		function prototypes for protocol.c
 * 		function prototype for commands.c
 *
 *		Protocol Specification:
 * 		SOF	LEN	SRC	CMD	PAYLOAD	CRC
 * 		1B	1B	1B	1B	N bytes	1B
 * 
 * 		SOF = 0xAA
 * 		LEN = 2 + |PAYLOAD|
 * 		SRC = node_id
 * 		CMD = MACRO for command
 * 		PAYLOAD = array of bytes being sent  MAX 32 BYTES
 * 		CRC = XOR of all bytes from LEN through PAYLOAD
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#define SOF 0xAA	// byte code for start-of-frame
#define MAX_PAYLOAD 32	// max 32 bytes payload in frame

/* Command Byte Codes */
#define CMD_PING	 0x01
#define CMD_GET_INFO	 0x02
#define CMD_GET_TEMP	 0x10
#define CMD_GET_MAC    0x15

#define CMD_SET_INTERVAL 0x20
#define CMD_SET_NODE_ID	 0x22

#define CMD_SETUP_ESP_AS_NODE  0x30
#define CMD_SETUP_ESP_AS_HOST  0x31
#define CMD_ADD_NODE_TO_HOST 0x33
#define CMD_SEND_TO_NODE  0x35

#define CMD_TEMP_REPORT	 0x80
#define CMD_INFO_REPORT	 0x81
#define CMD_MAC_REPORT   0x85

#define CMD_SUCCESS 0xF1
#define CMD_ERROR	 0xFF

/* Success Byte Codes*/
#define SUCCESS_PING 0xBB
#define SUCCESS_ESP_SETUP 0xC0
#define SUCCESS_ESP_ADD_HOST 0xC2
#define SUCCESS_ESP_ADD_NODE 0xC3
#define SUCCESS_ESP_SEND 0xC4

/* Error Byte Codes */
#define ERR_UNKNOWN_CMD	 0x01
#define ERR_BAD_CRC	 0x02
#define ERR_BAD_LENGTH	 0x03

#define ERR_ESP_INIT 0x10
#define ERR_ESP_ADD_HOST 0x12
#define ERR_ESP_ADD_NODE 0x13
#define ERR_ESP_NOT_SENT 0x14
#define ERR_ESP_BAD_SENDER 0x15


typedef struct {
	uint8_t len;
	uint8_t src;
	uint8_t cmd;
	uint8_t payload[MAX_PAYLOAD];
} Frame;

/* function typedef for future ability to write via WiFi/BLE */
typedef void (*protocol_write_fn)(const uint8_t *data, size_t len);

void protocol_init(protocol_write_fn write_cb);
void protocol_set_esp(protocol_write_fn write_cb);
void protocol_process_byte(uint8_t);
void protocol_send(uint8_t cmd, uint8_t *payload, uint8_t len);
void protocol_send_report(Frame *f);

void dispatch_frame(Frame *f);

#endif

