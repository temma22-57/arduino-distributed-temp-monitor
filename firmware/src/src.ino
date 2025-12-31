/*
 * File:	main.ino
 * Author:	Tristan Emma
 * Purpose:	main Arduino controller. Contains setup() and
 * 		loop()
 */

#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "protocol.h"
#include "config.h"
#include "isr_lib.h"


const int sensor_pin = A0;

/* SparkFun Invernters Kit temp sensor code */
int16_t read_temperature_x100(void) {
	float voltage, degreesC;
	int16_t degreesC_x100;
	
	voltage = analogRead(sensor_pin) * 0.004882814;
	
	degreesC = (voltage - 0.5) * 100;
	degreesC_x100 = (int16_t)(degreesC * 100);
	
	return degreesC_x100;
}

void serial_write(const uint8_t *data, size_t len) {
	//Serial.write(data, len);

	Serial.print(*data, HEX);
	Serial.println();
}

void setup() {
	Serial.begin(9600);
	config_load();		     // load config from EEPROM
	protocol_init(serial_write); 

	watchdog_init();	     // setup interrupts
	sei();			     // enable global interrupt
}

void loop() {
	if (g_wdt_ticks >= g_config.wdt_target) {
		g_wdt_ticks = 0;

		int16_t temp = read_temperature_x100();
		uint8_t payload[2] = {
			(uint8_t)(temp >> 8),
			(uint8_t)(temp & 0xFF)
		};
		protocol_send(CMD_TEMP_REPORT, payload, 2);
		Serial.flush();
		
	}
	
	if (Serial.available()) {
		uint8_t b = Serial.read();
		last_rx_ms = millis();
		protocol_process_byte(b);
	}

	if(safe_to_sleep()) 
		enter_sleep();
}
