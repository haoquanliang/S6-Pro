#ifndef __MAP_H
#define __MAP_H

int nibble_for_char(char c);
void bt_map_start(void);
void bt_get_time(char *ptr);
void bt_map_kick(void);
bool bt_map_time_get(u8 *buf, u8 len);
void bt_map_data_callback(uint8_t *packet);
#endif // __MAP_H
