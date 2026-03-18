// new serial commands:
//  setup esp_now
//  add new receiver node
//   - new error not in host mode
//  get mac address
//  new command series for through hub "set remote x"

#ifndef ESP_LIB_H
#define ESP_LIB_H


void esp_now_write(const uint8_t *, size_t);
void esp_now_set_broadcastAddress(uint8_t *);

void esp_now_setup_node(uint8_t *);

void esp_now_setup_host();

void add_node(uint8_t*); 

#endif
