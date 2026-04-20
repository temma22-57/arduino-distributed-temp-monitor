// new serial commands:
//  setup esp_now
//  add new receiver node
//   - new error not in host mode
//  get mac address
//  new command series for through hub "set remote x"

#include "protocol.h"
#include <esp_now.h>
#include <WiFi.h>

static esp_now_peer_info_t host;
static esp_npw_peer_info_t nodes[MAX_PEERS];
static uint8_t peers;


static uint8_t broadcastAddress[6];

void esp_now_write(const uint8_t *data, size_t len){
	esp_now_send(broadcastAddress, data, len);
}

void esp_now_set_broadcastAddress(uint8_t *mac){
  for(uint8_t i = 0; i < 6; i++){
    broadcastAddress[i] = mac[i];
  }
}

void onDataRecvHost(const uint8_t *mac, const uint8_t *incomingData, int len){
  // might need to parse out first 6 from *mac, technically not accurate desciription here
  // see docs.expressif.com
  if(esp_now_is_peer_exist(mac)){
    for(uint8_t i = 0; i < len; i++){
      protocol_process_byte(incomingData[i]);
    }
  } else {
    uint8_t err = ERR_ESP_BAD_SENDER;
		protocol_send(CMD_ERROR, &err, 1);//error bad sender
  }
}

void onDataRecvNode(const uint8_t *mac, const uint8_t *incomingData, int len){
  bool bad_sender = false;
  for(uint8_t i = 0; i < 6; i++){
    if(mac[i] != broadcastAddress[i]){
      uint8_t err = ERR_ESP_BAD_SENDER;
		  protocol_send(CMD_ERROR, &err, 1);//error handling here
      bad_sender = true;
    }
  }
  if(!bad_sender){
    for(uint8_t i = 0; i < len; i++){
      protocol_process_byte(incomingData[i]);
    }
  }
}

void onDataSentHost(const uint8_t *mac, esp_now_send_status_t status){
  if(status != ESP_NOW_SEND_SUCCESS){
    uint8_t err = ERR_ESP_NOT_SENT;
		protocol_send(CMD_ERROR, &err, 1);//send serial error
  } else {
    uint8_t code = SUCCESS_ESP_SEND;
    protocol_send(CMD_SUCCESS, &code, 1);
  }
}

void onDataSentNode(const uint8_t *mac, esp_now_send_status_t status){
  if(status != ESP_NOW_SEND_SUCCESS){
    ;//send error
  }
}

void esp_now_setup_node(uint8_t *host_mac){
  if(esp_now_init() != ESP_OK){
    uint8_t err = ERR_ESP_INIT;
		protocol_send(CMD_ERROR, &err, 1);//send serial error
  }

  esp_now_register_recv_cb(onDataRecvNode);
  esp_now_register_send_cb(onDataSentNode);
  
  uint8_t code = SUCCESS_ESP_SETUP;
  protocol_send(CMD_SUCCESS, &code, 1);

  esp_now_set_broadcastAddress(host_mac);

  peers = MAX_PEERS;
  
  memcpy(host.peer_addr, host_mac, 6);
  host.channel = 0;
  host.encrypt = false;

  esp_err_t add_host_err = esp_now_add_peer(&host);
  if(add_host_err == ESP_OK){
    //send positive feedback
    uint8_t code = SUCCESS_ESP_ADD_HOST;
    protocol_send(CMD_SUCCESS, &code, 1);

    protocol_set_esp(esp_now_write);
  } else {
    uint8_t err = ERR_ESP_ADD_HOST;
		protocol_send(CMD_ERROR, &err, 1);//serial error handling here
  }
}

void esp_now_setup_host(){
  if(esp_now_init() != ESP_OK){
    uint8_t err = ERR_ESP_INIT;
		protocol_send(CMD_ERROR, &err, 1);//send serial error
  }

  esp_now_register_recv_cb(onDataRecvHost);
  esp_now_register_send_cb(onDataSentHost);

  peers = 0;

  uint8_t code = SUCCESS_ESP_SETUP;
  protocol_send(CMD_SUCCESS, &code, 1);
}

void add_node_to_host(uint8_t *mac){
	if(peers >= MAX_PEERS){
		return;
	}

  esp_now_peer_info_t *new_peer = (nodes + peers++);
  memcpy(new_peer->peer_addr, mac, 6);
  new_peer->channel = 0;
  new_peer->encrypt = false;

  esp_err_t add_host_err = esp_now_add_peer(new_peer);
  if(add_host_err == ESP_OK){
    uint8_t code = SUCCESS_ESP_ADD_NODE;
    protocol_send(CMD_SUCCESS, &code, 1);//send positive feedback
  } else {
    uint8_t err = ERR_ESP_ADD_NODE;
		protocol_send(CMD_ERROR, &err, 1);//serial error handling here
  }
}
