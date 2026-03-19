/*
 * File:	isr_lib.h
 * Author:	Tristan Emma
 * Purpose:	houses global g_wdt_ticks for use in 
 * 		src.ino and function prototypes for 
 * 		isr_lib.cpp
 */

#ifndef ISR_LIB_H
#define ISR_LIB_H

#define RX_IDLE_TIMEOUT_MS 20 // about 2 chars at 9600 baud

extern volatile int g_wdt_ticks;
extern int last_rx_ms;

void watchdog_init();

bool safe_to_sleep();
void enter_sleep();

#endif

