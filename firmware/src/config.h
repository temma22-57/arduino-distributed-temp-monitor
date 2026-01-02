/*
 * File:	config.h
 * Author:	Tristan Emma
 * Purpose:	Header file for config.cpp
 * 		Used as library for EEPROM interaction and
 * 		device config actions
 *
 * 		Contains:
 * 		-typedef for DeviceConfig struct 
 * 		 (EEPROM config format)
 * 		-extern declaration for global variable g_config
 * 		 (declared in config.cpp)
 * 		-function prototypes for config.cpp
 */ 

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define CONFIG_VERSION 2

typedef struct {
    uint8_t version;
    uint8_t node_id;
    uint16_t sample_interval_s;
    uint16_t wdt_target;
    uint8_t crc;
} config_t;

extern config_t g_config;

void config_load(void);
void config_save(void);

uint8_t config_crc(config_t *cfg);
uint16_t config_wdt_target(config_t *cfg);
#endif
