/*
 * File:	isr_lib.cpp
 * Author:	Tristan Emma
 * Purpose:	Library for interrupt based design implemented
 * 		for power-aware system.
 *
 * 		Contains:
 * 		-Watchdog initializer function 
 * 		-ISR and volatile variable declarations timer
 * 		 interrupt
 * 		-safe_to_sleep() to verify nothing is still being
 * 		 cleared from serial writes (or other output), and
 * 		 nothing is available to be read
 *		-enter_sleep() storefront function to enter sleep
 */

#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "protocol.h"
#include "config.h"
#include "isr_lib.h"

// tick counter for longer than 8s sleep
volatile int g_wdt_ticks = 0;

// used to verify safe to sleep after receiving Serial input
int last_rx_ms = 0;

// watchdog timer Interrupt Service Routine, increment ticks
ISR(WDT_vect) {
	g_wdt_ticks++;
}

/*
 * watchdog_init()
 * 	Setup for watchdog timer interrupts
 * 	-clears reset flag
 * 	-enters configuration mode
 * 	-and configures interrupt mode to be 8s
 */
void watchdog_init() {
	MCUSR &= ~(1 << WDRF);	//clear reset flag
	wdt_disable();

	//enter configuration mode
	WDTCSR |= (1 << WDCE) | (1 << WDE);

	// set interrupt mode w 8s interval
	WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
}

/*
 * bool safe_to_sleep()
 * 	safety wrapper called as condition to sleep to ensure
 * 	nothing gets lost over serial from sleeping before data
 * 	arrives and can be parsed
 *
 * 	returns bool (false if not safe, true if safe)
 */
bool safe_to_sleep() {
	if (Serial.available() > 0) return false;

	if (millis() - last_rx_ms < RX_IDLE_TIMEOUT_MS) return false;

	return true;
}

/*
 * enter_sleep()
 * 	wrapper for sleeping 
 */
void enter_sleep() {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}
