#ifndef _DEBUG_H
#define _DEBUG_H

typedef struct {
    u8 second;
    u8 minute;
    u8 hour;
    u8 day;
    u8 montch;
    u8 year;
    u8 wday;
}time_info_t;

void offline_log_put(u8 *buf, u8 len);
void offline_log_init(uint32_t start_addr, uint32_t size, uint32_t save_size);
void offline_log_erase_proccess(void);
void offline_log_write_proccess(void);
void offline_log_dump(void (*dump_cb)(uint8_t *buf, uint16_t len));
void huart_wait_txdone(void);
void debug_set_utc_time(u32 utc);
int offline_log_get_size(void);
int offline_log_get(uint32_t addr, uint8_t *buf, uint32_t size);
void sys_log_error(const char *format, ...);
void sys_log_info(const char *format, ...);
void offline_log_write_cache_page(void);
void offline_log_dump_print(void);
void offline_log_end(void);
u32 get_cur_utc(void);
void offline_log_dump_huart(void);
void offline_log_process(void);
void debug_dump_info_cache_miss_cnt(void);
void sys_log_dump8(u8 *buf, int size);
void sys_log_dump32(u32 *buf, int size);
void offline_log_exception_check(void);
#endif
