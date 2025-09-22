/**********************************************************************
*
*   strong_sco.c
*   定义库里面通话算法部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

/*****************************************************************************
 * Module    : AEC算法强定义
 *****************************************************************************/
#if !BT_AEC_EN
AT(.com_text.isr.aec)
bool aec_isr(void) { return false; }
AT(.com_text.aec_det)
void aec_far_detect(void) {}
AT(.bt_voice.aec)
void aec_nlms_process(u8 mic_sel) {}
AT(.bt_voice.aec)
void aec_nlp_process(void) {}
void aec_init(aec_cfg_t *aec) {}
void aec_exit(void) {}
#endif

#if !BT_ALC_EN
void alc_init(void) {}
void alc_process(void) {}
void alc_fade_in(s16 *buf) {}
void alc_fade_out(s16 *buf) {}
AT(.bt_voice.alc)
bool alc_isr(void) { return false; }
AT(.bt_voice.alc)
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode) {};
#endif


/*****************************************************************************
 * Module    : 通话其他部分强定义
 *****************************************************************************/

#if !BT_SCO_DUMP_EN && !BT_AEC_DUMP_EN && !BT_SCO_FAR_DUMP_EN && !BT_EQ_DUMP_EN
void sco_huart_init(void){}
bool sco_huart_putcs(u8 type, u8 frame_num, const void *buf, uint len){return 0;}
AT(.com_huart.text)
void sco_huart_tx_done(void){}
AT(.bt_voice.sco.dump)
void bt_sco_dump_cb(uint type, void *ptr, uint size) {}
#endif

#if !SDADC_SOFT_GAIN_EN
AT(.com_text.sdadc.soft_gain)
void sdadc_drc_soft_gain_proc(s16 *ptr, u32 samples, int ch_mode) {}
AT(.com_text.sdadc.soft_gain.proc)
bool sdadc_soft_gain_proc(s16 *ptr, u32 samples, int ch_mode){return false;}
bool sdadc_drc_init(u8 *drc_addr, int drc_len){return false;}
void sdadc_set_soft_gain(u16 ch, u32 gain) {}
#else
    void sdadc_set_digital_gain(u16 ch, u16 gain) {}
#endif

#if !SDADC_EQ_EN
AT(.com_text.sdadc.soft_eq)
void sdadc_soft_eq_proc(s16 *ptr, u32 samples, int ch_mode) {}
bool sdadcl_set_soft_eq_by_res(u32 addr, u32 len){return false;}
bool sdadcr_set_soft_eq_by_res(u32 addr, u32 len){return false;}
#endif

#if !SDADC_DRC_EN
bool sdadc_drc_v3_init(u8 *drc_addr, int drc_len){return false;}
AT(.com_text.sdadc.drc_v3)
bool sdadc_drc_v3_calc(s16 *ptr, u32 samples, int ch_mode){return false;}
#endif

/*****************************************************************************
 * Module    : 通话上行降噪算法强定义
 *****************************************************************************/
#if !BT_TRUMPET_NR_EN
void trumpet_denoise_init(u8 level) {}
void trumpet_denoise(int *fft_in) {}
AT(.bt_voice.sco.trumpet)
void bt_sco_trumpet_nr(s32 *fft_in) {}
#endif

#if !BT_SCO_AGC_EN
void bt_sco_agc_proc(s16 *buf) {}
void bt_agc_init(void) {}
void agc_init(agc_cb_t* agc_cb) {}
void agc_proc(s16 *input, s16 *output, s32 pcmLen) {}
void bt_sco_agc_proc_do(s16 *ptr, int samples) {}
#endif

#if !BT_SNDP_SMIC_AI_EN
void bt_sndp_sm_init(void *alg_cb) {}
void sndp_sm_process(void) {}
void bt_sndp_sm_exit(void) {}
#endif

//#if !BT_SNDP_DMIC_EN && !BT_SNDP_DMIC_DNN_EN
//void bt_sndp_dm_proc_do(void) {}
//void bt_sndp_dm_process(s16 *buf) {}
//void bt_sndp_dmic_init(void) {}
//void bt_sndp_dmic_exit(void) {}
//#endif

#if !BT_SCO_SMIC_AI_EN && !BT_SCO_SMIC_AI_PLUS_EN && !BT_SCO_DMIC_AI_EN && !BT_SCO_LDMIC_AI_EN && !BT_SCO_ASM_DNN_EN && !BT_SCO_AIAEC_DNN_EN && !BT_SCO_DMIC_AIAEC_EN && !BT_SCO_SMIC_AI_PRO_EN && !BT_SCO_FFDM_AI_EN && !BT_SCO_FFDM_AI_PRO_EN
AT(.bt_voice.sco)
bool bt_sco_dnn_en(void) {return 0;}
void dnn_far_upsample(s16 *out, s16 *in, u32 samples, u8 step) {}
#endif

#if !BT_SCO_SMIC_AI_EN
void bt_dnn_init(void *alg_cb) {}
void dnn_sm_process(void) {}
void bt_dnn_exit(void) {}
#endif

#if !BT_SCO_SMIC_AI_PLUS_EN
void bt_dnn_plus_init(void *alg_cb) {}
void dnn_plus_sm_process(void) {}
void bt_dnn_plus_exit(void) {}
#endif

#if !BT_SCO_SMIC_AI_PRO_EN
void bt_dnn_pro_init(void *alg_cb) {}
void dnn_pro_sm_process(void) {}
void bt_dnn_pro_exit(void) {}
#endif

#if !BT_SCO_AIAEC_DNN_EN
void bt_aiaec_dnn_init(void *alg_cb) {}
void aiaec_dnn_sm_process(void) {}
void bt_aiaec_dnn_exit(void) {}
#endif

#if !BT_SCO_DMIC_AIAEC_EN
void bt_aiaec_dmdnn_init(void *alg_cb) {}
void aiaec_dmdnn_dm_process(void) {}
void bt_aiaec_dmdnn_exit(void) {}
#endif

#if !BT_SCO_AIAEC_PINK_NOISE_EN
void aiaec_pink_generation_init(s32 plfsr_val) {}
void pink_noise_proc(s16 *pink_buf, s16 *mic_in) {}
void pink_generate16(s16 *out) {}
void aiaec_dnn_pink_buf_init(void) {}
void aiaec_dm_pink_buf_init(void) {}
s16 dm_pink_gen_buf[1];
s16 pink_gen_buf[1];
#endif

#if !BT_SCO_FFDM_AI_EN
void bt_ffdm_init(void *alg_cb) {}
void ffdm_dm_process(void) {}
void bt_ffdm_exit(void) {}
#endif

#if !BT_SCO_FFDM_AI_PRO_EN
void bt_ffdm_pro_init(void *alg_cb) {}
void ffdm_pro_dm_process(void) {}
void bt_ffdm_pro_exit(void) {}
#endif

#if !BT_SCO_DMIC_AI_EN
void bt_dmns_init(void *alg_cb) {}
void dnn_dm_process(void) {}
void bt_dmns_exit(void) {}
#endif

#if !BT_SCO_LDMIC_AI_EN
void bt_ldmdnn_init(void *alg_cb) {}
void ldmdnn_process(void) {}
void bt_ldmdnn_exit(void) {}
#endif

#if !BT_SCO_NR_USER_SMIC_EN && !BT_SCO_NR_USER_DMIC_EN
void bt_nr_user_proc_do(s16 *mic1, s16 *mic2, nr_cb_t *nr_cb) {}
void bt_nr_user_init(void *alg_cb, nr_cb_t *nr_cb) {}
#endif

#if !BT_SCO_AEC_USER_EN
AT(.bt_voice.soc_nr.far)
void bt_sco_far_get(s16 *buf) {}
void bt_sco_far_cache_init(void) {}
#endif

#if !BT_SCO_NR_EN
void bt_ains_init(void *alg_cb) {}
void ains_sm_process(void) {}
void bt_ains_exit(void) {}
#endif

#if !BT_SNDP_FBDM_EN
void bt_sndp_fbdm_init(void *alg_cb) {}
void alg_sndp_fbdm_process(void) {}
void bt_sndp_fbdm_exit(void) {}
#endif

#if !BT_SNDP_DM_AI_EN
void bt_sndp_dm_init(void *alg_cb) {}
void alg_sndp_dm_process(void) {}
void bt_sndp_dm_exit(void) {}
s32 bt_sndp_dm_process(s32 *efw0, s32 *efw1, s32 *xfw, s16 *hnl, u32 *aeccon2){return 0;}
#endif

/*****************************************************************************
 * Module    : 通话下行降噪算法强定义
 *****************************************************************************/
#if !BT_SCO_FAR_NR_EN
AT(.com_text.isr.peri_nr)
void peri_nr_isr(void) {}
AT(.bt_voice.peri_nr)
void bt_sco_far_peri_nr_process(s16 *ptr, u16 samples) {}
void bt_sco_far_nr_int(void *cfg) {}
void bt_peri_nr_exit(void) {}
void bt_peri_nr_init(void) {}
#endif

#if !BT_SCO_CALLING_NR_EN
AT(.bt_voice.sco)
u16 dnr_voice_maxpow_calling(u32 *ptr, u16 len){return 0;}
#endif

#if !BT_SCO_DAC_DRC_EN
bool bt_sco_dac_drc_init(u8 *drc_addr, int drc_len) {return false;}
AT(.bt_voice.dac.drc)
s16 bt_sco_dac_drc_calc(s32 data) {return 0;}
AT(.bt_voice.dac.drc)
void bt_sco_dac_drc_proc(s16 *ptr, u32 samples) {}
#endif

#if !BT_SCO_DAC_DNR_EN
AT(.bt_voice.dac.dnr)
void bt_sco_dac_dnr_proc(s16 *ptr, u32 samples, int dnr_thr) {}
#endif


