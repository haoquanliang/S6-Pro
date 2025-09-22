#ifndef _OFFLINE_LOG_H
#define _OFFLINE_LOG_H

void offline_log_put(u8 *buf, u8 len);
void offline_log_init(uint32_t start_addr, uint32_t size, uint32_t save_size);
void offline_log_erase_proccess(void);
void offline_log_write_proccess(void);
void offline_log_dump(void (*dump_cb)(uint8_t *buf, uint16_t len));

#endif
