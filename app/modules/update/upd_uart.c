#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              my_printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define UPD_CMD_LEN   12

#if UPD_UART_EN

enum {
    UPD_IDEL,
    UPD_START,
    UPD_END,
};

enum {
    UPD_CHECK_UART  = 0x01,
    UPD_READ_DATA   = 0x02,
    UPD_SEND_STATUS = 0x03,
};

//总共16byte
typedef struct __attribute__((packed)) {
    u16 sign;
    u8 cmd;
    u8 status;
    u32 addr;
    u32 len;
    u16 crc;
    u8 rsv[2];
} upd_txcmd_t;

//总共12byte
typedef struct __attribute__((packed)) {
    u16 sign;
    u8 cmd;
    u8 status;
    u32 addr;
    u32 data_crc;
    u16 crc;
    u8 rsv[2];
} upd_rxcmd_t;

typedef struct {
    upd_txcmd_t txcmd;
    upd_rxcmd_t rxcmd;
    u32 rx_cnt;
    u8 upd_step;
    gpio_t tx;
    gpio_t rx;
} upd_uart_cb_t;

typedef struct {
    u32 baud;
    u32 port_map;
    u16 rx_port;
    u16 tx_port;
    u8 uart_sel;
} upd_uart_cfg_t;

static upd_uart_cb_t upd_uart_cb;

static const u8 str_upd_start[] = {'S','T','A','R','T','_','U','P','D','^','_','^'};
static const u8 str_upd_start_success[] = {'R','E','C','E','I','V','E','S','T','A','R','T'};

u8 *upd_uart_get_upd_buf(void);

void upd_uart_putchar(u8 ch)
{
#if UART1_EN
    UART1DATA = ch;
    while (!(UART1CON & BIT(8)));
#elif UART2_EN
    UART2DATA = ch;
    while (!(UART2CON & BIT(8)));
#endif
}

static u8 upd_uart_get_data(u8 *ch)
{
#if UART1_EN
    return bsp_uart1_get(ch);
#elif UART2_EN
    return bsp_uart2_get(ch);
#else
    return 0;
#endif
}

AT(.text.update)
void upd_uart_get_info(u8 *buf, u8 len)
{
    upd_uart_cfg_t cfg;
    cfg.baud            = UPD_UART_BAUD,
    cfg.port_map        = UPD_UART_IO_MAP,
    cfg.rx_port         = UPD_UART_RX_IO,
    cfg.tx_port         = UPD_UART_TX_IO,
    cfg.uart_sel        = UPD_UART_SEL,
    memcpy(buf, &cfg, len);
}

static u32 upd_cal_sum(const void *buf, u16 len)
{
    u8 *ptr = (u8*)buf;
    u32 sum = 0;
    for (u32 i = 0; i < len; i++){
        sum += ptr[i];
    }
    return sum;
}

u8 upd_uart_get_step(void)
{
    return upd_uart_cb.upd_step;
}

void upd_uart_set_step(u8 step)
{
    upd_uart_cb.upd_step = step;
}

static void upd_send_msg(const void *buf, u16 len)
{
    TRACE("upd tx:");
#if TRACE_EN
    print_r(buf,len);
#endif
    u8 *ptr = (u8 *)buf;
    for (u32 i = 0; i < len; i++) {
        upd_uart_putchar(*(ptr+i));
    }
    TRACE("\n");
}

static bool upd_uart_wait_get_data(void *buf, u32 len)
{
    TRACE("upd rx len %d\n",len);
    u8 *ptr = buf;
    u32 timeout = tick_get();
    u8 ch = 0;
    upd_uart_cb.rx_cnt = 0;
    while (upd_uart_cb.rx_cnt < len) {
        if (upd_uart_get_data(&ch)) {
            ptr[upd_uart_cb.rx_cnt++] = ch;
        }

        if (tick_check_expire(timeout, 1000)) {
            TRACE("upd rx timeout:%d %d\n", upd_uart_cb.rx_cnt, len);
            return false;
        }
        WDT_CLR();
    }

    if (upd_uart_cb.rxcmd.data_crc != upd_cal_sum(ptr,len)) {
        TRACE("crc error:%x %x\n", upd_uart_cb.rxcmd.data_crc, upd_cal_sum(buf,len));
#if TRACE_EN
        my_print_r(ptr,len);
#endif
        return false;
    }
    TRACE("recive_sucess\n");
//    print_r(buf, len);
    return true;

}

static bool upd_uart_wait_get_cmd(void *r_buf, u32 len)
{
    TRACE("upd rx:");
    u8 *buf = r_buf;
    u32 timeout = tick_get();
    u8 ch = 0;
    upd_uart_cb.rx_cnt = 0;
    while (upd_uart_cb.rx_cnt < len) {
        if (upd_uart_get_data(&ch)) {
            if ((upd_uart_cb.rx_cnt == 0)&&(ch == 0xAA)) {
                buf[upd_uart_cb.rx_cnt++] = ch;
            } else if((upd_uart_cb.rx_cnt == 1)&&(ch == 0x55)) {
                buf[upd_uart_cb.rx_cnt++] = ch;
            } else if(upd_uart_cb.rx_cnt > 1) {
                buf[upd_uart_cb.rx_cnt++] = ch;
            }
        }
        if (tick_check_expire(timeout, 1000)) {
            return false;
        }
        WDT_CLR();
    }

#if TRACE_EN
    print_r(buf,len);
#endif
    if (upd_uart_cb.rxcmd.crc != upd_cal_sum(buf,len - 4)) {
        my_printf("crc error:%x %x\n", upd_uart_cb.rxcmd.crc, upd_cal_sum(buf,len - 4));
        return false;
    }
    return true;

}

static bool upd_uart_get_start_msg(void)
{
    u8 ch = 0;
    while (upd_uart_get_data(&ch)) {
        if (ch == str_upd_start[upd_uart_cb.rx_cnt]) {
            upd_uart_cb.rx_cnt++;
        } else {
            upd_uart_cb.rx_cnt=0;
        }
        if (upd_uart_cb.rx_cnt == sizeof(str_upd_start)) {
            upd_uart_cb.rx_cnt = 0;
            return true;
        }
    }
    return false;
}

void upd_uart_process(void)
{
    if (upd_uart_get_step() == UPD_IDEL) {
        if (upd_uart_get_start_msg()) {
            upd_send_msg(str_upd_start_success, sizeof(str_upd_start_success));
            upd_uart_set_step(UPD_START);
            func_cb.sta = FUNC_NULL;
        }
    }
}

void upd_uart_check_process(u8 ch)
{
    if (upd_uart_get_step() == UPD_IDEL) {
        if (ch == str_upd_start[upd_uart_cb.rx_cnt]) {
            upd_uart_cb.rx_cnt++;
        } else {
            upd_uart_cb.rx_cnt=0;
        }
        if (upd_uart_cb.rx_cnt == sizeof(str_upd_start)) {
            upd_uart_cb.rx_cnt = 0;
            upd_send_msg(str_upd_start_success, sizeof(str_upd_start_success));
            upd_uart_set_step(UPD_START);
            func_cb.sta = FUNC_NULL;
        }
    }
}

u8 upd_uart_read(u8 *buf,u32 addr)
{
    u8 err_cnt = 10;
    delay_ms(2);
    u8 *upd_buf = upd_uart_get_upd_buf();
    upd_txcmd_t *p = &upd_uart_cb.txcmd;
    memset(p, 0, sizeof(upd_txcmd_t));
    p->sign     = 0x55AA;
    p->cmd      = UPD_READ_DATA;
    p->status   = 0;
    p->addr     = addr;
    p->len      = 512;
    p->crc = upd_cal_sum(p, sizeof(upd_txcmd_t) - 2);
    while (err_cnt--) {
        upd_send_msg(p, sizeof(upd_txcmd_t));
        if (upd_uart_wait_get_cmd(&upd_uart_cb.rxcmd, sizeof(upd_uart_cb.rxcmd))) {
            if (upd_uart_wait_get_data(upd_buf, p->len)) {
                return 0;
            }
        }
    }
    return 1;
}

bool upd_uart_send_status(u8 percent)
{
    upd_txcmd_t *p = &upd_uart_cb.txcmd;
    memset(p, 0, sizeof(upd_txcmd_t));
    p->sign     = 0x55AA;
    p->cmd      = UPD_SEND_STATUS;
    p->status   = percent;
    p->crc      = upd_cal_sum(p, sizeof(upd_txcmd_t) - 2);
    upd_send_msg(p, sizeof(upd_txcmd_t));
    delay_5ms(4);
    return true;
}

void upd_uart_check_update(void)
{
    if (upd_uart_get_step()) {
         delay_ms(20);								//发送完start响应之后，延时一段时间等待主机进入升级模式。防止主机接收不到cmd
         func_update();                          	//尝试升级
         upd_uart_set_step(0);
    }
}

void upd_uart_init(void)
{
    gpio_cfg_init(&upd_uart_cb.tx, UPD_UART_TX_IO);
    gpio_cfg_init(&upd_uart_cb.rx, UPD_UART_RX_IO);
}
#endif
