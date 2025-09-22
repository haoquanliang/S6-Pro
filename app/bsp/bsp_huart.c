#include "include.h"
#include "api.h"

#if HUART_EN

void sco_huart_tx_done(void);

AT(.com_huart.text)
void huart_tx_done_cb(void)
{
#if ANC_ALG_DUMP_EN || BT_SCO_DUMP_TX_EN
    sco_huart_tx_done();
#endif
}

AT(.com_huart.text)
void huart_rx_done_cb(void)
{
    if(eq_rx_buf[0] == 0xF5 && eq_rx_buf[1] == 0xA0 && eq_rx_buf[2] == 0xA5 && eq_rx_buf[3] == 0x96 && eq_rx_buf[4] == 0x87 && eq_rx_buf[5] == 0x5A){
		WDT_RST();
		while(1);
	}

#if EQ_DBG_IN_UART
    if(bsp_eq_rx_done(eq_rx_buf)){
        return;
    }
#endif

#if ANC_ALG_DBG_EN
    if (bsp_anc_alg_dbg_rx_done(eq_rx_buf, 0)) {
        return;
    }
#endif // ANC_ALG_DBG_EN

#if (CHARGE_BOX_INTF_SEL == INTF_HUART)
    charge_box_packet_huart_recv(eq_rx_buf);
#endif

#if QTEST_EN /*&& (TEST_INTF_SEL == INTF_HUART)*/
    if(QTEST_IS_ENABLE()) {
        qtest_packet_huart_recv(eq_rx_buf);
    }
#endif

#if IODM_TEST_EN && (TEST_INTF_SEL == INTF_HUART)
    iodm_packet_huart_recv(eq_rx_buf);
#endif
}

void bsp_huart_init(void)
{
    huart_t huart0;
    u32 baud_rate = 1500000;
#if (BT_DUMP_6M_EN && BT_SCO_DUMP_TX_EN)
    baud_rate = 6000000;
#endif
    if (xcfg_cb.huart_sel == HUART_TR_PA7) {
        if (UART0_PRINTF_SEL == PRINTF_PA7) {
            FUNCMCON0 = 0x0f << 8;
        }
    } else if (xcfg_cb.huart_sel == HUART_TR_PB2) {
        if (UART0_PRINTF_SEL == PRINTF_PB2) {
            FUNCMCON0 = 0x0f << 8;
        }
    } else if (xcfg_cb.huart_sel == HUART_TR_PB3) {
        if (UART0_PRINTF_SEL == PRINTF_PB3) {
            FUNCMCON0 = 0x0f << 8;
        }
    }
    if((xcfg_cb.huart_sel == HUART_TR_VUSB)){
         if(!sys_cb.vusb_uart_flag){
            PWRCON0 |= BIT(30);                             //Enable VUSB GPIO
#if (CHARGE_BOX_INTF_SEL == INTF_HUART) || (QTEST_EN /*&& (TEST_INTF_SEL == INTF_HUART)*/)
            baud_rate = 9600;
#endif
            xcfg_cb.chg_inbox_pwrdwn_en=0;                  //入仓耳机关机
            sys_cb.vusb_uart_flag = 1;
         }else{
             return;
         }
    }
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);

    memset(&huart0, 0x00, sizeof(huart0));
    huart0.rx_port = xcfg_cb.huart_sel;
    huart0.tx_port = xcfg_cb.huart_sel;
    huart0.rxisr_en = 1;
    huart0.txisr_en = 1;
    huart0.rxbuf   = eq_rx_buf;
    huart0.rxbuf_size = EQ_BUFFER_LEN;

    huart_init(&huart0, baud_rate);
}
#else
void bsp_huart_init(void) {}
#endif

u8* huart_get_rxbuf(u16 *len)
{
    *len  = EQ_BUFFER_LEN;
    return eq_rx_buf;
}

#if BT_FCC_TEST_EN || LE_BQB_RF_EN
ALIGNED(4)
u8 huart_buffer[128];

void bt_uart_init(void)
{
    huart_t huart0;

    memset(&huart0, 0x00, sizeof(huart0));
#if LE_BQB_RF_EN
    huart0.rx_port = HUART_TR_PB3;
    huart0.tx_port = HUART_TR_PB4;
    huart0.rxbuf_loop = 1;
    huart0.rxbuf      = huart_buffer;
    huart0.rxbuf_size = 128;

    huart_init(&huart0, 9600);
#else
    huart0.rx_port = xcfg_cb.huart_sel;
    huart0.tx_port = xcfg_cb.huart_sel;
    huart0.txisr_en = 0;
    huart0.rxbuf_loop = 1;
    huart0.rxbuf      = huart_buffer;
    huart0.rxbuf_size = 128;

    huart_init(&huart0, 1500000);
#endif
}

void bt_uart_exit(void)
{
}
#endif
