#include "include.h"
#include "debug.h"

#if OFFLINE_LOG_EN

struct exception_log_t {
    u32 crc16;
    u32 wptpnd;
    u32 wdtcon;
    u32 epc;
    u32 pcerr;
    u32 exceptpnd;
    u32 cpu_gprs[32];
    u8 heap[96];
} exception_log AT(.epc.cb);

#define SPRINTF_LEN         256
static char sprintf_buf[SPRINTF_LEN];
static u32 last_log_utc;
static u8 dump_err_type = 0;
static u8 dump_err_code = 0;

AT(.com_text.utc)
void utc_to_datetime(time_info_t *time, u32 utc)
{
    time->second = utc % 60;
    utc /= 60;
    time->minute = utc % 60;
    utc = utc / 60;
    time->hour = utc % 24;
}

void debug_set_utc_time(u32 utc)
{
    time_info_t local_time;

    rtc_time_to_tm(RTCCNT, &local_time);
    sys_log_info("old utc time:%d %d/%d/%d %d:%d:%d\n", RTCCNT, local_time.year + 1970, local_time.montch, local_time.day, local_time.hour, local_time.minute, local_time.second);

    RTCCNT = utc + 28800; //加8小时，北京时间
    rtc_time_to_tm(RTCCNT, &local_time);
    sys_log_info("set utc time:%d %d/%d/%d %d:%d:%d\n", utc, local_time.year + 1970, local_time.montch, local_time.day, local_time.hour, local_time.minute, local_time.second);
}

AT(.com_text.utc)
u32 get_cur_utc(void)
{
    u32 utc = RTCCNT;
    return  utc > 28800 ? (utc - 28800) : utc; //转换成标准时区
}

AT(.com_text.log)
void sys_log_error(const char *format, ...)
{
    va_list param;
    printf("[ERR]");
    va_start(param, format);
    a_vprintf(format, param);
    printf("\n");

    if (1) {
        u16 ms = RTCCON2 >> 21;
        time_info_t local_time;
        utc_to_datetime(&local_time, RTCCNT);

        int len = snprintf(sprintf_buf, SPRINTF_LEN, "[%02d:%02d:%02d.%03d]", local_time.hour, local_time.minute, local_time.second, ms);
        len += vsnprintf(sprintf_buf + len, SPRINTF_LEN - len, format, param);
        offline_log_put((u8 *)sprintf_buf, len);
    }
    va_end(param);
}

AT(.com_text.log)
void sys_log_info(const char *format, ...)
{
    va_list param;
    printf("[INF]");
    va_start(param, format);
    a_vprintf(format, param);

    if (1) {
		u32 ticks = RTCCNT;
 		time_info_t local_time;

        utc_to_datetime(&local_time, ticks);

        int len = 0;
        if (last_log_utc != ticks) {
            len = snprintf(sprintf_buf, SPRINTF_LEN, "[%02d:%02d:%02d]", local_time.hour, local_time.minute, local_time.second);
            last_log_utc = ticks;
        }
        len += vsnprintf(sprintf_buf + len, SPRINTF_LEN - len, format, param);
        offline_log_put((u8 *)sprintf_buf, len);
    }
    va_end(param);
}

AT(.com_text.log)
void sys_log_dump8(u8 *buf, int size)
{
    print_r(buf, size);

    if (1) {
        int len = 0;
        for (int i = 0; i < size; i++) {
            len += snprintf(sprintf_buf + len, SPRINTF_LEN - len, "%x ", (unsigned int)buf[i]);
        }
        offline_log_put((u8 *)sprintf_buf, len);
    }
}

AT(.com_text.log)
void sys_log_dump32(u32 *buf, int size)
{
    print_r32(buf, size);
    if (1) {
        int len = 0;
        for (int i = 0; i < size; i++) {
            len += snprintf(sprintf_buf + len, SPRINTF_LEN - len, "%x ", (unsigned int)buf[i]);
        }
        offline_log_put((u8 *)sprintf_buf, len);
    }
}

static void offline_log_dump_printf_cb(uint8_t *buf, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        uart_putchar(buf[i]);
    }
    WDT_CLR();
}

static void offline_log_dump_huart_cb(uint8_t *buf, uint16_t len)
{
    huart_tx(buf, len/2);
    huart_wait_txdone();
    huart_tx(buf + len/2, len/2);
    huart_wait_txdone();
    WDT_CLR();
}

//AT(.com_rodata.err.str)
//static const char str_dump_err[] = "err%d: %x\n";
//
//AT(.com_text.dump.err)
//void dump_err(uint err_type, uint err_code)
//{
//    printk(str_dump_err, err_type, err_code);
//    dump_err_type = err_type;
//    dump_err_code = err_code;
//}

AT(.text.func.process.log)
void offline_log_process(void)
{
    if (dump_err_type) {
        sys_log_info("dump err%d:%d\n", dump_err_type, dump_err_code);
        dump_err_type = 0;
    }
    offline_log_write_proccess();
    if (f_bt.disp_status < BT_STA_PLAYING) {
        offline_log_erase_proccess();
    }
}

AT(.com_text.offline)
void offline_log_exception_isr(uint32_t *cpu_gprs)
{
    struct exception_log_t *p = &exception_log;
    p->wptpnd    = 0;
    p->wdtcon    = 0;
    p->epc       = 0;
    p->pcerr     = 0;
    p->exceptpnd = 0;
    memcpy(p->cpu_gprs, cpu_gprs, 32 * 4);
    memcpy(p->heap, (u8 *)(cpu_gprs[2] - 32), 96);
    p->crc16 = calc_crc(&p->wptpnd, sizeof(struct exception_log_t) - 2, 0xffff);
}

void offline_log_exception_check(void)
{
    struct exception_log_t *p = &exception_log;
    u16 crc16 = calc_crc(&p->wptpnd, sizeof(struct exception_log_t) - 2, 0xffff);
    if (crc16 == p->crc16) {
        sys_log_error("EPC=%x, %x, %x, %x, %x\n", p->epc, p->pcerr, p->exceptpnd, p->wptpnd, p->wdtcon);
        sys_log_dump32(p->cpu_gprs, 32);
        offline_log_process();
        sys_log_dump8(p->heap, 96);
        offline_log_process();
    }
    memset(p, 0, sizeof(struct exception_log_t));
}

void offline_log_dump_print(void)
{
    offline_log_dump(offline_log_dump_printf_cb);
}

void offline_log_dump_huart(void)
{
    u8 txbuf[7];
    u32 log_len = offline_log_get_size();
    printf("huart tx log_len:%x\n", log_len);
//    *((u32 *)txbuf) = 0x03FEAA55;
    txbuf[0] = 0x55;
    txbuf[1] = 0xAA;
    txbuf[2] = 0xFE;
    txbuf[3] = 0x03;
    txbuf[4] = log_len;
    txbuf[5] = log_len >> 8;
    txbuf[6] = log_len >> 16;
    huart_tx(txbuf, 7);
    huart_wait_txdone();

    offline_log_dump(offline_log_dump_huart_cb);
}

void offline_log_end(void)
{
    offline_log_write_proccess();
    offline_log_write_cache_page();
}
#endif
