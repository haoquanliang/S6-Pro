#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif // FUNC_MUSIC_EN

#if FUNC_BT_EN
    FUNC_BT,
#endif

#if FUNC_BTHID_EN
    FUNC_BTHID,
#endif // FUNC_BTHID

#if FUNC_AUX_EN
    FUNC_AUX,
#endif // FUNC_AUX_EN

#if FUNC_CLOCK_EN
    FUNC_CLOCK,
#endif

#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif // FUNC_USBDEV_EN

#if FUNC_SPEAKER_EN
    FUNC_SPEAKER,
#endif // FUNC_SPEAKER_EN

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

u32 getcfg_vddbt_capless_en(void)
{
    return xcfg_cb.vddbt_capless_en;
}

u32 getcfg_buck_mode_en(void)
{
    uint8_t buck_mode_en = 0;
    if (BUCK_MODE_EN) {
        buck_mode_en |= BIT(0);
        if (xcfg_cb.vddcore_buck_en) {
            buck_mode_en |= BIT(1);
        }
    }
    return buck_mode_en;
}

u32 getcfg_mic_bias_method(u8 mic_ch)
{
    if (mic_ch == MIC0) {
        return xcfg_cb.mic0_pwr_sel << 4 | xcfg_cb.mic0_bias_method;
    }
    if (mic_ch == MIC1) {
        return xcfg_cb.mic1_pwr_sel << 4 | xcfg_cb.mic1_bias_method;
    }
    if (mic_ch == MIC2) {
        return xcfg_cb.mic2_pwr_sel << 4 | xcfg_cb.mic2_bias_method;
    }
    if (mic_ch == MIC3) {
        return xcfg_cb.mic3_pwr_sel << 4 | xcfg_cb.mic3_bias_method;
    }
    if (mic_ch == MIC4) {
        return xcfg_cb.mic4_pwr_sel << 4 | xcfg_cb.mic4_bias_method;
    }
    return 0;
}

u32 getcfg_mic_gain(u8 mic_ch)
{
    if (mic_ch == MIC0) {
        return xcfg_cb.mic0_anl_gain << 8 | xcfg_cb.bt_mic0_dig_gain;
    }
    if (mic_ch == MIC1) {
        return xcfg_cb.mic1_anl_gain << 8 | xcfg_cb.bt_mic1_dig_gain;
    }
    if (mic_ch == MIC2) {
        return xcfg_cb.mic2_anl_gain << 8 | xcfg_cb.bt_mic2_dig_gain;
    }
    if (mic_ch == MIC3) {
        return xcfg_cb.mic3_anl_gain << 8 | xcfg_cb.bt_mic3_dig_gain;
    }
    if (mic_ch == MIC4) {
        return xcfg_cb.mic4_anl_gain << 8 | xcfg_cb.bt_mic4_dig_gain;
    }
    return 0;
}

u32 getcfg_mic_power_level(void)
{
    return xcfg_cb.mic_pwr_level;
}

#if BT_A2DP_LDAC_AUDIO_EN
u8 *getcfg_soft_key(void)
{
    return xcfg_cb.soft_key1;
}
#endif
