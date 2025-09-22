#include "include.h"




#if UART1_EN

typedef struct {
    volatile u8 w_cnt;
    volatile u8 r_cnt;
    u8 resv0;
    u8 resv1;
    u8 *buf;
    u32 ticks;
} uart1_cb_t;

static uart1_cb_t uart1_cb;
static u8 uart1_rx_buf[64];
vh_packet_t vh_packet;


#if (CHARGE_BOX_INTF_SEL == INTF_UART1) || (TEST_INTF_SEL == INTF_UART1)
AT(.com_text.bsp.uart)
u8 bsp_uart_packet_parse(vh_packet_t *p, u8 data)
{
    u8 recv_valid  = 0, parse_done = 0;
    u8 cnt = p->cnt;
    if ((cnt == 0) && (data == 0x55)) {
        recv_valid = 1;
    } else if ((cnt == 1) && (data == 0xAA)) {
        p->header = 0xAA55;
        recv_valid = 1;
    } else if ((cnt == 2) && (data == VHOUSE_DISTINGUISH)) {
        p->distinguish = data;
        recv_valid = 1;
    } else if (cnt == 3) {
        p->cmd = data;
        recv_valid = 1;
    } else if (cnt == 4) {
        if (data <= VH_DATA_LEN) {
            p->length = data;
            recv_valid = 1;
        }
    } else if ((cnt > 4) && (cnt <= (p->length + 1 + 4))) {
        recv_valid = 1;
        if (cnt > p->length + 4) {
            p->checksum = data;
            parse_done = 1;                                     //匹配完整充电仓命令包
            recv_valid = 0;
        } else {
            p->buf[cnt - 5] = data;
        }
    }

    if (recv_valid) {
        p->crc = crc8_tbl[p->crc ^ data];
        p->cnt++;
    } else {
        if ((parse_done) && (p->crc != p->checksum)) {
            parse_done = 0;
        }
        p->cnt = 0;
        p->crc = 0;
    }
    return parse_done;
}

#endif


AT(.com_text.uart1.isr)
static void uart1_rx_done_cb(u8 data)
{
#if ((CHARGE_BOX_INTF_SEL == INTF_UART1) || ((TEST_INTF_SEL == INTF_UART1) && IODM_TEST_EN))
    vh_packet_t *p = &vh_packet;
    u8 parse_done = bsp_uart_packet_parse(p,data);
    if (parse_done) {
#if (CHARGE_BOX_INTF_SEL == INTF_UART1)
        charge_box_packet_recv();
#endif
#if IODM_TEST_EN && (TEST_INTF_SEL == INTF_UART1)
        iodm_packet_recv();
#endif
    }
#endif

#if QTEST_EN && (TEST_INTF_SEL == INTF_UART1)
    if(QTEST_IS_ENABLE()) {
        qtest_packet_uart_recv(data);
    }
#endif
}

AT(.com_text.uart1.isr)
static void uart1_isr(void)
{
    u8 data;
    if(UART1CON & BIT(9)) {
        data = UART1DATA;
        UART1CPND = BIT(9);
        if(tick_check_expire(uart1_cb.ticks,50)){
            uart1_cb.w_cnt = uart1_cb.r_cnt = 0;
        }
        uart1_cb.ticks = tick_get();
        uart1_cb.buf[uart1_cb.w_cnt & 0x3f] = data;
        uart1_cb.w_cnt++;
        uart1_rx_done_cb(data);
    }
}

AT(.com_text.uart)
u8 bsp_uart1_get(u8 *ch)
{
    if (uart1_cb.r_cnt != uart1_cb.w_cnt) {
        *ch = uart1_cb.buf[uart1_cb.r_cnt & 0x3f];
        uart1_cb.r_cnt++;
        return 1;
    }
    return 0;
}

void bsp_uart1_init(u32 baudrate)
{
    memset(&uart1_cb, 0, sizeof(uart1_cb));
    memset(&vh_packet, 0, sizeof(vh_packet));
    uart1_cb.buf = uart1_rx_buf;
    u32 baud = 0;
    if (xcfg_cb.uart1_sel == UART1_TR_PA7) {
        if (UART0_PRINTF_SEL == PRINTF_PA7) {
            FUNCMCON0 = 0x0f << 8;
        }
        GPIOADE |= BIT(7);                                 //数字io
        GPIOAFEN |= BIT(7);                                //function io
        GPIOADIR |= BIT(7);                                //input
    }else if(xcfg_cb.uart1_sel == UART1_TR_VUSB){
         if(!sys_cb.vusb_uart_flag){
            if (UART0_PRINTF_SEL == PRINTF_VUSB) {
                FUNCMCON0 = 0x0f << 8;
            }
            PWRCON0 |= BIT(30);                             //Enable VUSB GPIO
#if (CHARGE_BOX_INTF_SEL == INTF_UART1) || (QTEST_EN && (TEST_INTF_SEL == INTF_UART1))
            baudrate = 9600;
#endif
            xcfg_cb.chg_inbox_pwrdwn_en = 0;                  //入仓耳机关机
            sys_cb.vusb_uart_flag = 1;
         }else{
             return;
         }
    } else if (xcfg_cb.uart1_sel == UART1_TR_PF0){
        GPIOFDE |= BIT(0);                                 //数字io
        GPIOFFEN |= BIT(0);                                //function io
        GPIOFDIR |= BIT(0);
    }
#if UPD_UART_EN
    baudrate = UPD_UART_BAUD;
#endif
    baud = ((get_sysclk_nhz() + (baudrate / 2)) / baudrate) - 1;
    CLKGAT0 |= BIT(7);                              //enable uart1 clk
    UART1CON = 0;
    FUNCMCON0 = (0x7 << 28) | ((xcfg_cb.uart1_sel + 1) << 24);          //when RX = 0X7, TX RX共用
    UART1BAUD = (baud << 16) | baud;
    UART1CON = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(0);   //RX EN, One line, fix baud, 2 Stop bits, Uart EN
    UART1CON |= 0xaaa << 16;                                 //KEY RESET MODE
    UART1CPND = BIT(8) | BIT(9);
    UART1CPND |= BIT(10) | BIT(15) ;
    uart1_register_isr(uart1_isr);
}

#endif

#if UART2_EN

typedef struct {
    volatile u8 w_cnt;
    volatile u8 r_cnt;
    u8 resv0;
    u8 resv1;
    u8 *buf;
    u32 ticks;
} uart2_cb_t;

static uart2_cb_t uart2_cb;
static u8 uart2_rx_buf[64];
vh_packet_t vh_packet;


#if (CHARGE_BOX_INTF_SEL == INTF_UART2) || (TEST_INTF_SEL == INTF_UART2)
AT(.com_text.bsp.uart)
u8 bsp_uart_packet_parse(vh_packet_t *p, u8 data)
{
    u8 recv_valid  = 0, parse_done = 0;
    u8 cnt = p->cnt;
    if ((cnt == 0) && (data == 0x55)) {
        recv_valid = 1;
    } else if ((cnt == 1) && (data == 0xAA)) {
        p->header = 0xAA55;
        recv_valid = 1;
    } else if ((cnt == 2) && (data == VHOUSE_DISTINGUISH)) {
        p->distinguish = data;
        recv_valid = 1;
    } else if (cnt == 3) {
        p->cmd = data;
        recv_valid = 1;
    } else if (cnt == 4) {
        if (data <= VH_DATA_LEN) {
            p->length = data;
            recv_valid = 1;
        }
    } else if ((cnt > 4) && (cnt <= (p->length + 1 + 4))) {
        recv_valid = 1;
        if (cnt > p->length + 4) {
            p->checksum = data;
            parse_done = 1;                                     //匹配完整充电仓命令包
            recv_valid = 0;
        } else {
            p->buf[cnt - 5] = data;
        }
    }

    if (recv_valid) {
        p->crc = crc8_tbl[p->crc ^ data];
        p->cnt++;
    } else {
        if ((parse_done) && (p->crc != p->checksum)) {
            parse_done = 0;
        }
        p->cnt = 0;
        p->crc = 0;
    }
    return parse_done;
}

#endif


AT(.com_text.uart2.isr)
static void uart2_rx_done_cb(u8 data)
{
#if ((CHARGE_BOX_INTF_SEL == INTF_UART2) || ((TEST_INTF_SEL == INTF_UART2) && IODM_TEST_EN))
    vh_packet_t *p = &vh_packet;
    u8 parse_done = bsp_uart_packet_parse(p,data);
    if (parse_done) {
#if (CHARGE_BOX_INTF_SEL == INTF_UART2)
        charge_box_packet_recv();
#endif
#if IODM_TEST_EN && (TEST_INTF_SEL == INTF_UART2)
        iodm_packet_recv();
#endif
    }
#endif

#if QTEST_EN && (TEST_INTF_SEL == INTF_UART2)
    if(QTEST_IS_ENABLE()) {
        qtest_packet_uart_recv(data);
    }
#endif
}

AT(.com_text.uart2.isr)
static void uart2_isr(void)
{
    u8 data;
    if(UART2CON & BIT(9)) {
        data = UART2DATA;
        UART2CPND = BIT(9);
        if(tick_check_expire(uart2_cb.ticks,50)){
            uart2_cb.w_cnt = uart2_cb.r_cnt = 0;
        }
        uart2_cb.ticks = tick_get();
        uart2_cb.buf[uart2_cb.w_cnt & 0x3f] = data;
        uart2_cb.w_cnt++;
        uart2_rx_done_cb(data);
    }
}

AT(.com_text.uart)
u8 bsp_uart2_get(u8 *ch)
{
    if (uart2_cb.r_cnt != uart2_cb.w_cnt) {
        *ch = uart2_cb.buf[uart2_cb.r_cnt & 0x3f];
        uart2_cb.r_cnt++;
        return 1;
    }
    return 0;
}

void bsp_uart2_init(u32 baudrate)
{
    memset(&uart2_cb, 0, sizeof(uart2_cb));
    memset(&vh_packet, 0, sizeof(vh_packet));
    uart2_cb.buf = uart2_rx_buf;
    u32 baud = 0;
    if (xcfg_cb.uart2_sel == UART2_TR_PB2) {
        if (UART0_PRINTF_SEL == PRINTF_PB2) {
            FUNCMCON0 = 0x0f << 8;
        }
        GPIOBDE |= BIT(2);
        GPIOBFEN |= BIT(2);
        GPIOBDIR |= BIT(2);
    }else if(xcfg_cb.uart2_sel == UART2_TR_VUSB){
         if(!sys_cb.vusb_uart_flag){
            if (UART0_PRINTF_SEL == PRINTF_VUSB) {
                FUNCMCON0 = 0x0f << 8;
            }
            PWRCON0 |= BIT(30);                             //Enable VUSB GPIO
#if (CHARGE_BOX_INTF_SEL == INTF_UART2) || (QTEST_EN && (TEST_INTF_SEL == INTF_UART2))
            baudrate = 9600;
#endif
            xcfg_cb.chg_inbox_pwrdwn_en = 0;                  //入仓耳机关机
            sys_cb.vusb_uart_flag = 1;
         }else{
             return;
         }
    } else if (xcfg_cb.uart2_sel == UART2_TR_PE7){
        GPIOEDE |= BIT(7);
        GPIOEFEN |= BIT(7);
        GPIOEDIR |= BIT(7);
    }
#if UPD_UART_EN
    baudrate = UPD_UART_BAUD;
#endif
    baud = ((get_sysclk_nhz() + (baudrate / 2)) / baudrate) - 1;
    CLKGAT0 |= BIT(8);                              //enable uart2 clk
    UART2CON = 0;
    FUNCMCON2 = (0x03 << 12) | ((xcfg_cb.uart2_sel + 1) << 8);          //when RX = 0X3, TX RX共用
    UART2BAUD = (baud << 16) | baud;
    UART2CON = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(0);   //RX EN, One line, fix baud, 2 Stop bits, Uart EN
    UART2CON |= 0xaaa << 16;                                 //KEY RESET MODE
    UART2CPND = BIT(8) | BIT(9);
    UART2CPND |= BIT(10) | BIT(15) ;
    uart2_register_isr(uart2_isr);
}
#endif


void set_vusb_uart_flag(u8 flag)
{
   sys_cb.vusb_uart_flag = flag;
}

void bsp_vusb_uart_dis(void)
{
    PWRCON0 &= ~BIT(30);                            //disable VUSB GPIO
//    UART1CON = 0;
    CLKGAT0 &= ~BIT(7);                             //disable uart1 clk
    sys_cb.vusb_uart_flag = 0;
}
