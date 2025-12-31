/*
 * File:	protocol.h
 * Author:	Tristan Emma
 * Purpose:	header file for protocol.c and commands.c
 * 		macro definitions for command byte codes, 
 * 		error byte codes
 * 		typedef for Frame struct of bytes in protocol
 *
 * 		function prototypes for protocol.c
 * 		function prototype for commands.c
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
#define CMD_SET_INTERVAL 0x20
#define CMD_SET_NODE_ID	 0x22

#define CMD_TEMP_REPORT	 0x80
#define CMD_INFO_REPORT	 0x81
#define CMD_ERROR	 0xFF

/* Error Byte Codes */
#define ERR_UNKNOWN_CMD	 0x01
#define ERR_BAD_CRC	 0x02
#define ERR_BAD_LENGTH	 0x03

typedef struct {
	uint8_t len;
	uint8_t src;
	uint8_t cmd;
	uint8_t payload[MAX_PAYLOAD];
} Frame;

typedef void (*protocol_write_fn)(const uint8_t *data, size_t len);

void protocol_init(protocol_write_fn write_cb);
void protocol_process_byte(uint8_t);
void protocol_send(uint8_t cmd, uint8_t *payload, uint8_t len);

void dispatch_frame(Frame *f);

#endif

