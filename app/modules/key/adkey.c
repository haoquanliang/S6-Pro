#include "include.h"

#if USER_ADKEY_MUX_SDCLK
typedef struct {
    uint8_t sdclk_val;
    uint8_t sdclk_convert : 1,                //ADC转换标志
       sdclk_valid   : 1;                //ADC转换数据有效
} adkey_t;

static adkey_t adkey;
#endif

void adkey_init(void)
{
    uint16_t adc_ch = 0;
#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        adc_ch |= BIT(ADKEY_CH);
    #if ADKEY_PU10K_EN
        adcch_io_pu10k_enable(ADKEY_CH);        //开内部10K上拉
    #endif // ADKEY_PU10K_EN
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        adc_ch |= BIT(ADKEY2_CH);
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        adc_ch |= BIT(SDCLK_AD_CH);
    }
    memset(&adkey, 0x00, sizeof(adkey));
#endif // USER_ADKEY_MUX_SDCLK

#if USER_NTC
    if (xcfg_cb.ntc_user_inner_pu) {
        adcch_io_pu10k_enable(ADCCH_NTC);        //开内部10K上拉
    }
    if (xcfg_cb.ntc_en) {
        adc_ch |= BIT(ADCCH_NTC);
    }
#endif
    saradc_set_channel(adc_ch);
}

#if (USER_ADKEY || USER_ADKEY_MUX_SDCLK)
AT(.com_text.port.key)
static uint8_t adkey_get_key_do(uint8_t key_val)
{
    uint8_t num = 0;
    uint8_t *ptr;

    while (key_val > adkey_table[num].adc_val) {
        num++;
    }

    //工具配置了第一组ADKEY的按键定义？
    ptr = get_adkey_configure(num);
    if (ptr != NULL) {
        if ((num > 11) || (adkey_table[num].adc_val == 0xff)) {
            return NO_KEY;
        }
        return key_config_table[*(ptr+num)];
    }

    return adkey_table[num].usage_id;
}
#endif

#if USER_ADKEY
AT(.com_text.port.adkey)
static uint8_t adkey_get_key(void)
{
    return adkey_get_key_do(saradc_get_value8(ADKEY_CH));
}
#endif

#if USER_ADKEY2
AT(.com_text.port.key)
static uint8_t adkey2_get_key(void)
{
    uint8_t num = 0;
    uint8_t *ptr;

    //配置工具是否使能了第二组ADKEY2？
    if (!xcfg_cb.user_adkey2_en) {
        return NO_KEY;
    }

    while (saradc_get_value8(ADKEY2_CH) > adkey2_table[num].adc_val) {
        num++;
    }
    return adkey2_table[num].usage_id;
}
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
AT(.com_text.key.adkey)
void adkey_mux_convert_done(void)
{
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        if (adkey.sdclk_convert) {
            adkey.sdclk_valid = 1;
        } else {
            adkey.sdclk_valid = 0;
        }

        if (sdcard_detect_is_busy()) {
            saradc_clr_channel(BIT(SDCLK_AD_CH));
            adkey.sdclk_convert = 0;
        } else {
            saradc_set_channel(BIT(SDCLK_AD_CH));
            adkey.sdclk_convert = 1;
        }
    }
}

AT(.com_text.port.key)
void adkey_mux_sdclk_w4_convert(void)
{
    if ((xcfg_cb.user_adkey_mux_sdclk_en) && (adkey.sdclk_convert)) {
        while (!saradc_is_finish()) {
            WDT_CLR();
        }
    }
}

AT(.com_text.port.adkey)
static uint8_t adkey_mux_get_key(void)
{
    if (!adkey.sdclk_valid) {
        return NO_KEY;
    }
    return adkey_get_key_do(saradc_get_value8(SDCLK_AD_CH));
}
#endif // USER_ADKEY_MUX_SDCLK

AT(.com_text.adkey.get)
uint8_t adkey_get_val(void)
{
    uint8_t key_val = NO_KEY;
#if USER_ADKEY
    if (key_val == NO_KEY && xcfg_cb.user_adkey_en) {
        key_val = adkey_get_key();
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (key_val == NO_KEY) {
        key_val = adkey2_get_key();
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    //需要放到最后处理,当没进行adc convert需要返回
    if (key_val == NO_KEY && xcfg_cb.user_adkey_mux_sdclk_en) {
        key_val = adkey_mux_get_key();
    }
#endif // USER_ADKEY_MUX_SDCLK

    return key_val;
}
