/*
 * File:	main.ino
 * Author:	Tristan Emma
 * Purpose:	main Arduino controller. Contains setup() and
 * 		loop()
 */

#include <Arduino.h>
#include "protocol.h"
#include "config.h"

static unsigned long last_sample_ms = 0;

const int sensor_pin = A0;

/* SparkFun Invernters Kit temp sensor code */
int16_t read_temperature_x100(void) {
	float voltage, degreesC;
	int16_t degreesC_x100;
	
	voltage = analogRead(sensor_pin) * 0.004882814;
	
	degreesC = (voltage - 0.5) * 100;
	degreesC_x100 = (int16_t)(degreesC * 100)
	
	return degreesC_x100;
}

void serial_write(const uint8_t *data, size_t len) {
	Serial.write(data, len);

void setup() {
    Serial.begin(115200);
    config_load();
    protocol_init();
}

void loop() {
    /* Handle incoming serial bytes */
    while (Serial.available()) {
        protocol_process_byte(Serial.read());
    }

    /* Periodic temperature reporting */
    unsigned long now = millis();
    if (now - last_sample_ms >= g_config.sample_interval_s * 1000UL) {
        last_sample_ms = now;

        int16_t temp = read_temperature_x100();
        uint8_t payload[2] = {
            (uint8_t)(temp >> 8),
            (uint8_t)(temp & 0xFF)
        };
        protocol_send(CMD_TEMP_REPORT, payload, 2);
    }

    /* Power management hook (sleep comes next) */
}
