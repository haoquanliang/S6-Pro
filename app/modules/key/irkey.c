#include "include.h"

#if IRKEY_EN

typedef struct {
    uint8_t key_val;
    bool state;
} irkey_t;

static irkey_t irkey AT(.bss.irkey.cb);

//AT(.com_rodata.bb)
//const char str_irkey_cb[] = "irkey: %04x,%04x,  %02x\n";

AT(.com_text.irkey)
void ir_rx_press_cb(uint16_t addr, uint16_t cmd)
{
    u8 key_val = NO_KEY;

#if IR_ADDR_FF00_EN
    if (addr == IR_ADDR_FF00) {
        key_val = ir_tbl_FF00[cmd & 0xff];
    }
#endif // IR_ADDR_FF00_EN

#if IR_ADDR_BF00_EN
    if (addr == IR_ADDR_BF00) {
        key_val = ir_tbl_BF00[cmd & 0xff];
    }
#endif // IR_ADDR_BF00_EN

#if IR_ADDR_FD02_EN
    if (addr == IR_ADDR_FD02) {
        key_val = ir_tbl_FD02[cmd & 0xff];
    }
#endif // IR_ADDR_FD02_EN

#if IR_ADDR_FE01_EN
    if (addr == IR_ADDR_FE01) {
        key_val = ir_tbl_FE01[cmd & 0xff];
    }
#endif // IR_ADDR_FE01_EN

#if IR_ADDR_7F80_EN
    if (addr == IR_ADDR_7F80) {
        key_val = ir_tbl_7F80[cmd & 0xff];
    }
#endif

    irkey.state = true;
    irkey.key_val = key_val;
//    printf(str_irkey_cb, addr, cmd, key_val);
}

AT(.com_text.irkey)
void ir_rx_release_cb(void)
{
    irkey.state = false;
}

AT(.com_text.irkey)
uint8_t irkey_get_val(void)
{
    if (irkey.state) {
        return irkey.key_val;
    }
    return NO_KEY;
}

void irkey_clr_state(void)
{
    irkey.state = false;
}

void irkey_init(void)
{
    irkey.state = false;

    bsp_irrx_init();
}
#endif // IRKEY_EN

