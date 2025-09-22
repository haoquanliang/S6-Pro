#include "include.h"

#if FUNC_AUX_EN
/****************************** AUX NOTICE **************************************
///无模拟直通，通路保持audio->adc->dac，配置固定：
* AUXL0_ADC0：       输入：PE6              输出：DACL\DACL&R
* AUXR0_ADC1：       输入：PE7              输出：DACR\DACL&R
* AUXL1_ADC0：       输入：PA6              输出：DACL\DACL&R
* AUXR1_ADC1：       输入：PA7              输出：DACR\DACL&R

///模拟增益范围：
* ANL_GAIN:          AUX_P12DB~AUX_N12DB，-12dB~+12dB 共8级
 *****************************************************************************************/

u8 aux_cfg_feature = -1;

AT(.rodata.aux)
static const u16 auxl_adc_tbl[3] = {
    0, CH_AUXL0, CH_AUXL1,
};

AT(.rodata.aux)
static const u16 auxr_adc_tbl[3] = {
    0, CH_AUXR0, CH_AUXR1,
};

u16 bsp_aux_ch_getcfg(void)
{
#if FUNC_AUX_EN
    u16 aux_cfg = 0;
    if (aux_cfg_feature & BIT(0)) {
        aux_cfg = (ADC4 << 4 | AUXL1);
    } else if (aux_cfg_feature & BIT(1)) {
        aux_cfg = auxl_adc_tbl[xcfg_cb.auxl_sel] | (auxr_adc_tbl[xcfg_cb.auxr_sel] << 8);
    }
    return aux_cfg;
#else
    return 0;
#endif // FUNC_AUX_EN
}

AT(.text.bsp.aux)
void bsp_aux_start(void)
{
 //   printf("%s\n", __func__);
    aux_cfg_feature = (xcfg_cb.aux_comb_en & 0x01) | ((xcfg_cb.aux_diff_en & 0x01) << 1) | (xcfg_cb.aux_comb_sel << 4);
    if (aux_cfg_feature <= 0) {
        printf("AUX setting null, aux feature %x\n", aux_cfg_feature);
        return;
    }
    dac_fade_out();
    dac_fade_wait();                                            //等待淡出完成
    audio_path_init(AUDIO_PATH_AUX);
    audio_path_start(AUDIO_PATH_AUX);
    dac_fade_in();
}

AT(.text.bsp.aux)
void bsp_aux_stop(void)
{
 //   printf("%s\n", __func__);
    dac_fade_out();
    dac_fade_wait();
    audio_path_exit(AUDIO_PATH_AUX);
    dac_aubuf_clr();
}

AT(.text.bsp.aux)
void bsp_aux_mute(u8 ch)    //BIT(0)-AUXL; BIT(1)-AUXR
{
    aux_channel_mute(ch);
}

AT(.text.bsp.aux)
void bsp_aux_unmute(u8 ch)  //BIT(0)-AUXL; BIT(1)-AUXR
{
    aux_channel_unmute(ch);
}

// level: 0-AUX_N12DB; 1-AUX_N9DB; 2-AUX_N6DB; 3-AUX_N3DB; 4-AUX_p0DB; 5-AUX_P6DB; 6-AUX_P9DB; 7-AUX_P12DB;
AT(.text.bsp.aux)
void bsp_aux_set_vol(u8 level, u8 ch)  //BIT(0)-AUXL; BIT(1)-AUXR
{
    set_aux_analog_vol(level, ch);
}

#define ANA_ADC0                  0x00
#define ANA_ADC1                  0x01
#define ANA_ADC2                  0x02
#define ANA_ADC3                  0x03
#define ANA_ADC4                  0x04

//设置AUXL\AUXR ana_adc通路
AT(.text.aux)
u8 aux_ana_adc_ch_hook(void)
{
    u8 auxl_adc = ANA_ADC4;
    u8 auxr_adc = ANA_ADC1;
    return (auxl_adc | (auxr_adc << 4));
}

#endif



