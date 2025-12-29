/*
 * File:	config.h
 * Author:	Tristan Emma
 * Purpose:	Header file for config.c
 * 		Used as library for EEPROM interaction with
 * 		device configuration
 * 		Contains typedef for DeviceConfig Struct, same
 * 		struct stored in EEPROM
 *
 * 		extern declaration for global variable g_config
 * 		in config.c
 * 		Funtion prototypes for config.c
 */ 

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define CONFIG_VERSION 1

typedef struct {
    uint8_t version;
    uint8_t node_id;
    uint16_t sample_interval_s;
    uint8_t crc;
} DeviceConfig;

extern DeviceConfig g_config;

void config_load(void);
void config_save(void);

uint8_t config_crc(DeviceConfig *cfg);

#endif
