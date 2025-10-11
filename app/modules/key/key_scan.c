#include "include.h"


void knob_init(void);
void knob_process(void);
u16 knob_process2(u16 *key_val);

#if USER_MULTI_PRESS_EN
const u16 tbl_double_key[] = USER_KEY_DOUBLE;
#endif

u8 bsp_tkey_scan(void);

AT(.com_text.key.table)
const key_shake_tbl_t key_shake_table = {
    .scan_cnt = KEY_SCAN_TIMES,
    .up_cnt   = KEY_UP_TIMES,
    .long_cnt = KEY_LONG_TIMES,
    .hold_cnt = KEY_LONG_HOLD_TIMES,
};

AT(.text.bsp.key.init)
void key_init(void)
{
    key_var_init();

#if USER_IOKEY
    io_key_init();
#endif

#if USER_ADKEY || USER_ADKEY2 || USER_NTC
    adkey_init();
#endif

#if USER_KEY_KNOB_EN
    knob_init();
#endif

    pwrkey_init();

#if VBAT_DETECT_EN
    vbat_init();
#endif

    bsp_saradc_init();

    bsp_tkey_init();
}

#if USER_MULTI_PRESS_EN
AT(.com_text.bsp.key)
bool check_key_return(u16 key_return)
{
    u8 i;
    if (key_return == NO_KEY) {
        return false;
    }

    //工具配置了哪些按键支持双击功能？
    i = check_key_double_configure(key_return);
    if (i > 0) {
        return (i - 1);
    }

    for (i=0; i<sizeof(tbl_double_key)/2; i++) {
        if (key_return == tbl_double_key[i]) {
            return true;
        }
    }
    return false;
}
#else
AT(.com_text.bsp.key)
bool check_key_return(u16 key_return)
{
    return false;
}
#endif // USER_KEY_DOUBLE_EN

AT(.text.bsp.key)
u8 get_pwroff_pressed_time(void)
{
    return PWROFF_PRESS_TIME;
}

AT(.text.bsp.key)
u8 get_double_key_time(void)
{
    if(DOUBLE_KEY_TIME > 7) {
        return 60;
    } else {
        return (u8)((u8)(DOUBLE_KEY_TIME + 2) * 20 + 1);
    }
}

AT(.com_text.bsp.key)
bool get_poweron_flag(void)
{
    return sys_cb.poweron_flag;
}

AT(.com_text.bsp.key)
void set_poweron_flag(bool flag)
{
    sys_cb.poweron_flag = flag;
}

AT(.com_text.bsp.key)
u16 bsp_key_process(u16 key_val)
{
#if USER_KEY_KNOB2_EN
 	static u8 timer1ms_cnt=0;
    timer1ms_cnt++;
    u16 key_ret = knob_process2(&key_val);
    if (key_ret != 0xffff && key_ret != NO_KEY) {
        return key_ret;
    }
    if(timer1ms_cnt < 5) {
        return NO_KEY;              //貌似有问题
    }
    timer1ms_cnt=0;
#endif

    u16 key_return = key_process(key_val);

    //双击处理
#if USER_MULTI_PRESS_EN
    //配置工具是否使能了按键2/3/4/5击功能？
    if (xcfg_cb.user_key_multi_press_en) {
        key_return = key_multi_press_process(key_return);
    }
#endif
    return key_return;
}

AT(.com_rodata.bsp.key)
const char key_enqueue_str[] = "enqueue: %04x\n";

AT(.com_text.bsp.key)
static bool bsp_key_pwron_filter(u16 key_val)
{
    if ((sys_cb.poweron_flag) && ((key_val & K_PWR_MASK) == K_PWR)) {
        return true;           //剔除开机过程PWRKEY产处的按键消息
    }
    return false;
}

AT(.com_text.bsp.key)
u8 bsp_key_scan_do(void)
{
    u8 key_val = NO_KEY;

    if (!bsp_saradc_process()) {
        return NO_KEY;
    }

#if USER_KEY_KNOB_EN
    knob_process();
#endif

#if USER_TKEY
    key_val = bsp_tkey_scan();
#endif

#if USER_PWRKEY
    if ((key_val == NO_KEY) && (!PWRKEY_2_HW_PWRON)) {
        key_val = pwrkey_get_val();
    }
#endif

#if USER_IOKEY
    if (key_val == NO_KEY) {
        key_val = get_iokey();
    }
#endif

#if USER_ADKEY || USER_ADKEY2 ||USER_ADKEY_MUX_SDCLK
    if (key_val == NO_KEY) {
        key_val = adkey_get_val();
    }
#endif
    return key_val;
}


AT(.com_text.bsp.key)
u8 bsp_key_scan(void)
{
    u8 key_val;
    u16 key = NO_KEY;

    key_val = bsp_key_scan_do();

#if !SWETZ_KEY_TONE
    static key_temp = 0;

    if(key_temp != key_val){
        key_temp = key_val;
        if(key_val == NO_KEY){//抬起响应
                

        }else {
               
                msg_enqueue(EVT_KEY_PRESS);
        }

    }

#endif

    key = bsp_key_process(key_val);
    if ((key != NO_KEY) && (!bsp_key_pwron_filter(key))) {
        //防止enqueue多次HOLD消息
        if ((key & KEY_TYPE_MASK) == KEY_LONG) {
            sys_cb.kh_vol_msg = (key & 0xff) | KEY_HOLD;
        } else if ((key & KEY_TYPE_MASK) == KEY_LONG_UP) {
            msg_queue_detach(sys_cb.kh_vol_msg, 0);
            sys_cb.kh_vol_msg = NO_KEY;
        } else if (sys_cb.kh_vol_msg == key) {
            msg_queue_detach(key, 0);
        }
//        printf(key_enqueue_str, key);
        msg_enqueue(key);
    }
    return key_val;
}

uint8_t bsp_key_pwr_scan(void)
{
    uint8_t key_val = NO_KEY;

#if USER_TKEY
    key_val = bsp_tkey_scan();
#endif

#if USER_PWRKEY
    if (key_val == NO_KEY) {
        key_val = pwrkey_get_val();
    }
#endif // USER_PWRKEY
    return key_val;
}

