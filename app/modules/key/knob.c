#include "include.h"


#if USER_KEY_KNOB_EN
//16级旋钮
AT(.com_rodata.port.key)
const u8 tbl_knob_level_16[16 + 1] = {
    0x02, 0x13, 0x24, 0x36, 0x47, 0x59, 0x6A, 0x7B,
    0x8B, 0x9A, 0xA0, 0xB2, 0xC3, 0xD5, 0xE6, 0xF8,
    0xFF,
};

void knob_init(void)
{
    u16 adc_ch = 0;
    adc_ch |= BIT(ADCCH_PE5);       //旋钮1
    adc_ch |= BIT(ADCCH_PE6);       //旋钮2

    saradc_set_channel(adc_ch);
}


AT(.com_text.bsp.key)
void knob_get_level(u16 adc_val, const u8 *knob_level, u8 *key_val)
{
    u8 step = 0;
    if (s_abs((int)knob_level[*key_val] - (int)adc_val) > 4) {    //取绝对值消抖
        while (adc_val > knob_level[step]) {
            step++;
        }
        *key_val = step;
    }
}

AT(.com_text.port.key)
void knob_process(void)
{
    u8 mic_vol_val = saradc_get_value8(ADCCH_PE5);
    u8 echo_level_val = saradc_get_value8(ADCCH_PE6);

    static u8 echo_level = USER_KEY_KNOB_LEVEL, mic_vol = USER_KEY_KNOB_LEVEL;

    knob_get_level(echo_level_val, tbl_knob_level_16, &echo_level);
    if (echo_level != sys_cb.echo_level) {
        sys_cb.echo_level = echo_level;
        msg_enqueue(EVT_ECHO_LEVEL);
    }

    knob_get_level(mic_vol_val, tbl_knob_level_16, &mic_vol);
    if (mic_vol != sys_cb.mic_vol) {
        sys_cb.mic_vol = mic_vol;
        msg_enqueue(EVT_MIC_VOL);
    }
}

#if USER_KEY_KNOB2_EN
AT(.com_text.bsp.key)
u16 knob_process2(u16 *key_val)
{
    static u8 key_flag = 0;
    static u8 key_cnt_0 = 0, key_cnt_1 = 0;
//    my_printf(test_printf,*key_val);
    if (*key_val != KEY_VOL_DOWN_PREV && *key_val != KEY_VOL_UP_NEXT) {     //过滤掉其他按键
        key_flag = 0;
        if (*key_val == NO_KEY) {
            return NO_KEY;
        }
        return 0xffff;
    }

    if (*key_val == KEY_VOL_DOWN_PREV) {
        if (key_cnt_0 > 1) {    //消抖
            key_cnt_0 = 0;
            if (key_flag & BIT(1)) {
                key_flag = 0;
                return KEY_VOL_UP_NEXT | KEY_SHORT_UP;   //顺时针
            } else {
                key_flag |= BIT(0);
            }
        } else {
            key_cnt_0++;
        }
    }
    if (*key_val == KEY_VOL_UP_NEXT) {
        if (key_cnt_1 > 1) {    //消抖
            key_cnt_1 = 0;
            if (key_flag & BIT(0)) {
                key_flag = 0;
                return KEY_VOL_DOWN_PREV | KEY_SHORT_UP; //逆时针
            } else {
                key_flag |= BIT(1);
            }
        } else {
            key_cnt_1++;
        }
    }
    *key_val = NO_KEY;
    return NO_KEY;
}
#endif // USER_KEY_KNOB2_EN

#endif // USER_KEY_KNOB_EN
