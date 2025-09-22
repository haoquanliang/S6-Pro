#ifndef __DUEROS_DMA_PROFILE_H
#define __DUEROS_DMA_PROFILE_H

bool ble_send_packet(uint8_t *buf, uint8_t len);
void ble_update_scan_data(uint8_t *scan_buf, uint32_t buf_size);
void ble_update_adv_data(uint8_t *adv_buf, uint32_t buf_size);
#endif  //__DUEROS_DMA_PROFILE_H