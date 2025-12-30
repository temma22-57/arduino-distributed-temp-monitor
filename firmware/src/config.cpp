/*
 * File:	config.cpp
 * Author:	Tristan Emma
 * Purpose:	Library for EEPROM interactions. EEPROM is used
 *		to store configurations settings, in this case
 *		1 byte for config version, 1 byte for node_id,
 *		2 bytes for interval between samples, and a
 *		final byte for the check sum (crc)
 *
 *		Uses EEPROM.put() for safe writing. Only writes
 *		if data is different. Important for limited 
 *		red/write lifespan of about 100,000
 */

#include "config.h"
#include <Arduino.h>
#include <EEPROM.h>

#define DEFAULT_NODE_ID    1
#define DEFAULT_INTERVAL  10

DeviceConfig g_config;

uint8_t config_crc(DeviceConfig *cfg) {
    uint8_t crc = 0;
    crc ^= cfg->version;
    crc ^= cfg->node_id;
    crc ^= (cfg->sample_interval_s >> 8);
    crc ^= (cfg->sample_interval_s & 0xFF);
    return crc;
}

static void set_defaults(void) {
    g_config.version = CONFIG_VERSION;
    g_config.node_id = DEFAULT_NODE_ID;
    g_config.sample_interval_s = DEFAULT_INTERVAL;
    g_config.crc = config_crc(&g_config);
}

void config_load(void) {
    EEPROM.get(0, g_config);

    if (g_config.version != CONFIG_VERSION ||
        g_config.crc != config_crc(&g_config)) {
        set_defaults();
        EEPROM.put(0, g_config);
    }
}

void config_save(void) {
    g_config.crc = config_crc(&g_config);
    EEPROM.put(0, g_config);
}
