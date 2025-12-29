/*
 * File:	protocol.c
 * Author:	Tristan Emma
 * Purpose:	library for custom protocol
 */ 
#include "protocol.h"
#include <Arduino.h>

typedef enum {
    WAIT_SOF,
    READ_LEN,
    READ_BODY,
    READ_CRC
} ParserState;

static ParserState state = WAIT_SOF;
static Frame rx;
static uint8_t index = 0;
static uint8_t crc = 0;

void protocol_init(void) {
    state = WAIT_SOF;
}

void protocol_process_byte(uint8_t b) {
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
            }
            state = WAIT_SOF;
            break;
    }
}

void protocol_send(uint8_t cmd, uint8_t *payload, uint8_t len) {
    uint8_t crc = 0;

    Serial.write(SOF);

    Serial.write(len + 2); // SRC + CMD + payload
    crc ^= (len + 2);

    Serial.write(g_config.node_id);
	crc ^= g_config.node_id;


    Serial.write(cmd);
    crc ^= cmd;

    for (uint8_t i = 0; i < len; i++) {
        Serial.write(payload[i]);
        crc ^= payload[i];
    }

    Serial.write(crc);
}
