#include "include.h"
#include "led.h"

#if LED_DISP_EN
led_func_t bled_func AT(.buf.led);
led_func_t rled_func AT(.buf.led);
gpio_t bled_gpio AT(.buf.led);
gpio_t rled_gpio AT(.buf.led);

AT(.rodata.led.cfg)
const led_cfg_t led_cfg_poweron     = {0x00, 0xff, 1, 0};   //蓝灯常亮
AT(.rodata.led.cfg)
const led_cfg_t led_cfg_poweroff    = {0x00, 0x00, 1, 0};   //红灯，蓝灯灭
AT(.rodata.led.cfg)
const led_cfg_t led_cfg_idle        = {0x00, 0xff, 1, 0};   //蓝灯常亮
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_btinit      = {0x00, 0xaa, 10, 0};  //蓝灯500ms周期闪烁，默认跟w4pwrkey一样
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_pairing     = {0xf0, 0x0f, 1, 3};   //亮300ms灭450ms，蓝红交替闪
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_reconnect   = {0x00, 0xaa, 6, 0};   //蓝灯300ms周期闪烁
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_connected   = {0x00, 0x02, 2, 86};   //亮100ms, 灭5S
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_playing     = {0x00, 0x02, 2, 86};   //亮100ms, 灭5S
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_ring        = {0x00, 0x02, 2, 86};  //亮100ms, 灭5S
AT(.rodata.led.cfg.bt)
const led_cfg_t led_cfg_calling     = {0x00, 0x0a, 2, 86};  //亮100ms, 灭5S, 亮两次
AT(.rodata.led.cfg.music)
const led_cfg_t led_cfg_music_busy  = {0x00, 0xaa, 4, 0};   //蓝灯200ms周期快闪烁
AT(.rodata.led.cfg.music)
const led_cfg_t led_cfg_music_play  = {0x00, 0xaa, 20, 0};  //蓝灯1s周期闪烁
AT(.rodata.led.cfg.aux)
const led_cfg_t led_cfg_aux_play    = {0x00, 0xaa, 20, 0};  //蓝灯1s周期闪烁
AT(.rodata.led.cfg.fm)
const led_cfg_t led_cfg_fm_play     = {0x00, 0xaa, 20, 0};  //蓝灯1s周期闪烁
AT(.rodata.led.cfg.fm)
const led_cfg_t led_cfg_fm_scan     = {0x00, 0xaa, 20, 0};  //蓝灯1s周期闪烁
AT(.rodata.led.cfg.rec)
const led_cfg_t led_cfg_record      = {0x00, 0xaa, 20, 0};  //蓝灯1s周期闪烁
AT(.rodata.led.cfg.lowbat)
const led_cfg_t led_cfg_lowbat          = {0xaa, 0x00, 0x06, 0x00};     //红灯300ms周期闪烁
AT(.rodata.led.cfg.lowbat)
const led_cfg_t led_cfg_lowbat_follow   = {0x0a, 0x00, 0x02, 0xff};     //红灯间隔100ms闪2次

void led_port_init(void)
{
    if(xcfg_cb.led_sta_config_en == 0) {
        memcpy(&xcfg_cb.led_poweron,    &led_cfg_poweron,   sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_poweroff,   &led_cfg_poweroff,  sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_btinit,     &led_cfg_btinit,    sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_reconnect,  &led_cfg_reconnect, sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_pairing,    &led_cfg_pairing,   sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_connected,  &led_cfg_connected, sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_playing,    &led_cfg_playing,   sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_ring,       &led_cfg_ring,      sizeof(led_cfg_t));
        memcpy(&xcfg_cb.led_calling,    &led_cfg_calling,   sizeof(led_cfg_t));
#if (LED_LOWBAT_EN || RLED_LOWBAT_FOLLOW_EN)
        memcpy(&xcfg_cb.led_lowbat,     &led_cfg_lowbat,    sizeof(led_cfg_t));
#endif
    }
}

AT(.text.led_disp)
void led_power_up(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_poweron);
}

AT(.text.led_disp)
void led_power_down(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_poweroff);
}

AT(.text.led_disp)
void led_idle(void)
{
    led_set_sta(&led_cfg_idle);
}

AT(.text.led_disp.bt)
void led_bt_init(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_btinit);
}

AT(.text.led_disp.bt)
void led_bt_idle(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_pairing);
}

AT(.text.led_disp.bt)
void led_bt_reconnect(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_reconnect);
}

AT(.text.led_disp.bt)
void led_bt_connected(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_connected);
}

AT(.text.led_disp.bt)
void led_bt_play(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_playing);
}

AT(.text.led_disp.bt)
void led_bt_ring(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_ring);
}

AT(.text.led_disp.bt)
void led_bt_call(void)
{
    led_set_sta((led_cfg_t *)&xcfg_cb.led_calling);
}

AT(.text.led_disp.music)
void led_music_busy(void)
{
     led_set_sta(&led_cfg_music_busy);
}

AT(.text.led_disp.music)
void led_music_play(void)
{
    led_set_sta(&led_cfg_music_play);
}

AT(.text.led_disp.aux)
void led_aux_play(void)
{
     led_set_sta(&led_cfg_aux_play);
}

AT(.text.led_disp.fm)
void led_fm_play(void)
{
     led_set_sta(&led_cfg_fm_play);
}

AT(.text.led_disp.fm)
void led_fm_scan(void)
{
    led_set_sta(&led_cfg_fm_scan);
}

AT(.text.led_disp.rec)
void led_record(void)
{
    led_set_sta(&led_cfg_record);
}

AT(.text.led_disp.bt)
void led_bt_scan(void)
{
    led_bt_idle();
}

#if (LED_LOWBAT_EN || RLED_LOWBAT_FOLLOW_EN)
AT(.text.led_disp.lowbat)
void led_lowbat_do(void)
{
    led_set_sta_lowbat((const led_cfg_t *)&xcfg_cb.led_lowbat);
}

AT(.text.led_disp.lowbat)
void led_lowbat_follow_do(void)
{
    led_set_sta_lowbat(&led_cfg_lowbat_follow);
}
#endif

AT(.sleep_text.led)
bool led_bt_sleep(void)
{
#if BT_TWS_SLEEP_LED_SYNC_EN
    #define SYNC_TINE       10      //ÉÁË¸Ê±¼ä£¬µ¥Î»500ms
    if (bt_tws_is_connected()) {
        u32 clock = bt_tws_master_clock_get() / 1600;
        if ((clock % SYNC_TINE) == 0) {
            LED_INIT();
            bled_set_on();
            return true;
        } else {
            bled_set_off();
            return false;
        }
    } else
#endif
    {
        static u8 cnt = 0;

        cnt++;
        if (cnt >= 10) {
            cnt = 0;
            LED_INIT();
            bled_set_on();
            return true;
        } else {
            bled_set_off();
            return false;
        }
    }
}

AT(.com_text.bsp.sys)
bool is_led_scan_enable(void)
{
    return (LED_DISP_EN && sys_cb.led_scan_en);
}

void led_func_init(void)
{
#if UART0_PRINTF_SEL == PRINTF_PB3
    if (xcfg_cb.bled_io_sel == 12) {
        xcfg_cb.bled_io_sel = 0;
    }
    if (xcfg_cb.rled_io_sel == 12) {
        xcfg_cb.rled_io_sel = 0;
    }
#endif
    bsp_gpio_cfg_init(&bled_gpio, xcfg_cb.bled_io_sel);
    bsp_gpio_cfg_init(&rled_gpio, xcfg_cb.rled_io_sel);

#if LED_PWR_EN
    if (xcfg_cb.bled_io_sel == xcfg_cb.rled_io_sel) {
        //1个IO推两个灯
        if (bled_gpio.sfr != NULL) {
            sys_cb.port2led_en = 1;
        }
        bled_func.port_init = led2_port_init;
        bled_func.set_on = bled2_set_on;
        bled_func.set_off = bled2_set_off;

        rled_func.port_init = led2_port_init;
        rled_func.set_on = rled2_set_on;
        rled_func.set_off = rled2_set_off;
    } else
#endif // LED_PWR_EN
    {
        sys_cb.port2led_en = 0;
        bled_func.port_init = led_cfg_port_init;
        bled_func.set_on = led_cfg_set_on;
        bled_func.set_off = led_cfg_set_off;
#if USER_ADKEY_MUX_LED
        bled_func.port_init = led_mux_cfg_port_init;
        bled_func.set_on = led_mux_cfg_set_on;
        bled_func.set_off = led_mux_cfg_set_off;
#endif // USER_ADKEY_MUX_LED

        rled_func.port_init = led_cfg_port_init;
        rled_func.set_on = led_cfg_set_on;
        rled_func.set_off = led_cfg_set_off;
    }
}

u8 led_get_cfg_port(void)
{
    if(xcfg_cb.led_disp_en) {
        return xcfg_cb.bled_io_sel; //优先选择蓝灯
    } else if(xcfg_cb.led_pwr_en) {
        return xcfg_cb.rled_io_sel; //其次选择红灯
    }
    return 0;                       //不显示
}
#endif
