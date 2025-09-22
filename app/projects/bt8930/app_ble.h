#ifndef _APP_BLE_H_
#define _APP_BLE_H_

#include "include.h"

uint16_t  CRC16(const uint8_t *buffer,uint32_t size);
void app_priv_ble_handle_received(uint8_t *ptr, uint16_t len);
static void handle_cmd_get_dev_info(uint8_t *payload, uint16_t payload_len, uint8_t *fsn);


#endif 

