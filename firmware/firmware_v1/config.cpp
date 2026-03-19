/*
 * File:	config.cpp
 * Author:	Tristan Emma
 * Purpose:	Library for EEPROM interactions. EEPROM is used
 *		to store configurations settings, in this case
 *		1 byte for config version, 1 byte for node_id,
 *		2 bytes for interval between samples, 2 bytes
 *		for tick count target and a final byte for the 
 *		check sum (crc)
 *
 *		Uses EEPROM.put() for safe writing. Only writes
 *		if data is different. Important for limited 
 *		read/write lifespan of EEPROM of about 100,000 rw's
 */

#include "config.h"
#include <Arduino.h>
#include <EEPROM.h>

// default device configuration
#define DEFAULT_NODE_ID		1
#define DEFAULT_INTERVAL_S	8

// watchdog setting used (8s timeout)
#define WDT_INTERVAL_S		8

config_t g_config;

/*
 * uint8_t config_crc(config_t *cfg)
 * 	Calculates checksum bit for config
 * 	- Running XOR of every byte in the config
 *
 * 	returns: uint8_t checksum byte
 */
uint8_t config_crc(config_t *cfg) {
	uint8_t crc = 0;
	crc ^= cfg->version;
	crc ^= cfg->node_id;
	crc ^= (cfg->sample_interval_s >> 8);
	crc ^= (cfg->sample_interval_s & 0xFF);
	crc ^= (cfg->wdt_target >> 8);
	crc ^= (cfg->wdt_target & 0xFF);
	return crc;
}

/*
 * uint16_t confi_wdt_target(config_t *cfg)
 * 	Calculates target tick count between temperature reports
 * 	based on the watchdog interval, and the interval in the
 * 	config
 *
 * 	returns uint16_t target tick count
 */
uint16_t config_wdt_target(config_t *cfg){
	return g_config.sample_interval_s / WDT_INTERVAL_S;
}


/*
 * set_defaults()
 * 	resets config to default values
 * 	- Macro defined config version, node_id, and interval
 * 	- Calculates wdt_target and checksum
 * 	
 * 	stores all in global config
 */
static void set_defaults(void) {
	g_config.version = CONFIG_VERSION;
	g_config.node_id = DEFAULT_NODE_ID;
	g_config.sample_interval_s = DEFAULT_INTERVAL_S;
	g_config.wdt_target = config_wdt_target(&g_config);
	g_config.crc = config_crc(&g_config);
}

/*
 * config_load()
 * 	Initializer function to be called in setup()
 * 	Loads config from EEPROM and checks version and
 * 	checksum, resetting to defaults if either don't match
 */
void config_load(void) {
	EEPROM.get(0, g_config);

	if (g_config.version != CONFIG_VERSION ||
		g_config.crc != config_crc(&g_config)) 
	{	
		set_defaults();
		EEPROM.put(0, g_config);
	}
}

/*
 * config_save()
 * 	stores config to EEPROM
 * 	Used after external updates to interval and node_id
 */
void config_save(void) {
	g_config.crc = config_crc(&g_config);
	EEPROM.put(0, g_config);
}
