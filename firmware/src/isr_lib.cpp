/*
 * File:	isr_lib.cpp
 * Author:	Tristan Emma
 * Purpose:	
 */

#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "protocol.h"
#include "config.h"
#include "isr_lib.h"

/* System interrupts for low power mode */
volatile int g_wdt_ticks = 0;

int last_rx_ms = 0;

ISR(WDT_vect) {
	g_wdt_ticks++;
}

void watchdog_init() {
	MCUSR &= ~(1 << WDRF);	//clear reset flag
	wdt_disable();

	//enter configuration mode
	WDTCSR |= (1 << WDCE) | (1 << WDE);

	// set interrupt mode w 8s interval
	WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
}

bool safe_to_sleep() {
	if (Serial.available() > 0) return false;

	if (millis() - last_rx_ms < RX_IDLE_TIMEOUT_MS) return false;

	return true;
}

void enter_sleep() {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}