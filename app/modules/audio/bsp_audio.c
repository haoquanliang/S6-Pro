#include "include.h"

void bt_aec_process(u8 *ptr, u32 samples, int ch_mode);
void bt_sco_tx_process(u8 *ptr, u32 samples, int ch_mode);
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode);
void aux_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void speaker_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void usbmic_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void karaok_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void voice_assistant_sdadc_process(u8 *ptr,u32 samples,int ch_mode);
void ttp_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void anc_alg_sdadc_process(u8* ptr, u32 samples, int ch_mode);
u32 get_sysclk_nhz(void);
void anc_alg_audio_path_cfg_set(sdadc_cfg_t* cfg);
void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void usbmic_data_process(u8 *ptr, u32 samples, int ch_mode);

#if FUNC_AUX_EN
    #define aux_sdadc_callback      aux_sdadc_process
#else
    #define aux_sdadc_callback      sdadc_dummy
#endif // FUNC_AUX_EN

#if FUNC_SPEAKER_EN
    #define speaker_sdadc_callback  speaker_sdadc_process
#else
    #define speaker_sdadc_callback  sdadc_dummy
#endif // FUNC_SPEAKER_EN

#if UDE_MIC_EN && USB_MIC_NR_EN
    #define usbmic_sdadc_callback   usbmic_data_process
#elif UDE_MIC_EN
    #define usbmic_sdadc_callback   usbmic_sdadc_process
#else
    #define usbmic_sdadc_callback   sdadc_dummy
#endif // UDE_MIC_EN

#if BT_AEC_EN
    #define bt_sdadc_callback    bt_aec_process
#elif BT_SCO_SMIC_EN || BT_SCO_DMIC_EN
    #define bt_sdadc_callback    bt_aec_process
#else
    #define bt_sdadc_callback    bt_sco_tx_process
#endif

#if SYS_KARAOK_EN
    #define karaok_sdadc_callback   karaok_sdadc_process
#else
    #define karaok_sdadc_callback   sdadc_dummy
#endif

#if TINY_TRANSPARENCY_EN
    #define ttp_sdadc_callback      ttp_sdadc_process
#else
    #define ttp_sdadc_callback      sdadc_dummy
#endif

#if OPUS_ENC_EN
    #define opus_sdadc_callback   		opus_sdadc_process
#else
    #define opus_sdadc_callback   		sdadc_dummy
#endif

#if ANC_ALG_EN
    #define anc_alg_sdadc_callback      anc_alg_sdadc_process
#else
    #define anc_alg_sdadc_callback      sdadc_dummy
#endif

#if IODM_TEST_EN
    #define iodm_test_sdadc_callback     iodm_test_sdadc_process
#else
    #define iodm_test_sdadc_callback     sdadc_dummy
#endif
#if ASR_EN
    #define asr_sdadc_callback     asr_sdadc_process
#else
    #define asr_sdadc_callback     sdadc_dummy
#endif

#if SDADC_SOFT_GAIN_EN
    uint8_t cfg_sdadc_soft_gain_en  = SDADC_SOFT_GAIN_EN;        //ADC 软件增益使能
    uint8_t cfg_sdadc_drc_en        = SDADC_DRC_EN;              //开启ADC DRC
#endif

//MIC analog gain: 0~14(共15级), step 3DB (0db ~ +42db)
//adadc digital gain: 0~63, step 0.5 DB, 保存在gain的低6bit
const sdadc_cfg_t rec_cfg_tbl[] = {
/*   通道,             采样率,    模拟增益, 数字增益,    BITS,    通路控制,    样点数,   回调函数*/
    {AUX_CHANNEL_CFG,  SPR_44100,   2,         0,       0xff,     ADC2DAC_EN,    256,    aux_sdadc_callback},            /* AUX     */
    {MIC_CHANNEL_CFG,  SPR_48000,   10,        0,       0xff,     ADC2DAC_EN,    256,    speaker_sdadc_callback},        /* SPEAKER */
    {MIC_CHANNEL_CFG,  SPR_8000,    12,        0,       1,        ADC2DAC_EN,    480,    bt_sdadc_callback},             /* BTMIC   */
    {MIC_CHANNEL_CFG,  SPR_48000,   12,        0,       1,        0,             256,    usbmic_sdadc_callback},         /* USBMIC  */
    {MIC_CHANNEL_CFG,  SPR_44100,   12,        0,       1,        ADC2SRC_EN,    480,    karaok_sdadc_callback},         /* KARAOK  */
    {MIC_CHANNEL_CFG,  SPR_16000,   12,        0,       1,        0,             256,    opus_sdadc_callback},            /* opus */
    {MIC_CHANNEL_CFG,  SPR_44100,   6,         0,       1,        ADC2SRC_EN,    128,    ttp_sdadc_callback},            /* TRANSPARENCY  */
    {MIC_CHANNEL_CFG,  SPR_16000,   12,        0,       1,        ADC2SANC_EN,   256,    anc_alg_sdadc_callback},        /* ANC ALG */
    {MIC_CHANNEL_CFG,  SPR_48000,   10,        0,       0xff,     ADC2DAC_EN,    256,    iodm_test_sdadc_callback},      /* IODM TEST */
    {MIC_CHANNEL_CFG,  SPR_16000,   10,        0,       1,        ADC2ASR_EN,    256,    asr_sdadc_callback},            /* ASR */
};

#if FUNC_AUX_EN
u16 bsp_aux_ch_getcfg(void);
#endif

/*****************************************************************************
 * 功能   : 根据Settings，获取mic通路
 * 输入   : audio_path_idx,用于判断是否为BT_MIC_PATH
 * 注意   : 若mic通路选取异常，会报err;BT_MIC_PATH可选单、双麦,其他模式只采用单麦即主麦
 * 返回   : 无
 *****************************************************************************/
static u16 bsp_mic_ch_getcfg(u8 audio_path)
{
    u8 mic_cnt = 0;
    u16 ch_sel = 0;
    u16 mic_list[2] = {xcfg_cb.bt_mmic_cfg, xcfg_cb.bt_smic_cfg};
    u8 mic_mapping_tbl[5] = {CH_MIC0, CH_MIC1, CH_MIC2, CH_MIC3, CH_MIC4};

#if ANC_EN
    if (xcfg_cb.anc_en && xcfg_cb.anc_mode != MODE_HYBRID) {        //如果打开anc，重定义mic->adc路径
        mic_mapping_tbl[0] = ADC2 << 4 | MIC0;
        mic_mapping_tbl[1] = ADC3 << 4 | MIC1;
        mic_mapping_tbl[2] = ADC0 << 4 | MIC2;
        mic_mapping_tbl[3] = ADC1 << 4 | MIC3;
    }
#endif

#if BT_SCO_DMIC_EN
    if (audio_path == AUDIO_PATH_BTMIC) {
        mic_cnt = 2;
    } else {
        mic_cnt = 1;
    }
    printf("MMIC --> %d, SMIC --> %d\n", mic_list[0], mic_list[1]);
#else
    mic_cnt = 1;
    printf("MMIC --> %d\n", mic_list[0]);
#endif
#if USB_MIC_NR_EN
    if (audio_path == AUDIO_PATH_USBMIC) {
        mic_cnt = 2;
    }
#endif

    for (u8 i = 0; i < mic_cnt; i++) {
        if ((mic_list[i] + 1) > MIC4) {
            continue;
        }
        ch_sel |= mic_mapping_tbl[mic_list[i]] << (8 * i);
    }
    return ch_sel;
}

/*****************************************************************************
 * 功能   : 初始化对应AUDIO_PATH
 * 输入   : audio_path_idx 对应不同功能使用的audio通路
 * 注意   : 区分bt_call和其他状态
 * 返回   : 无
 *****************************************************************************/
void audio_path_init(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = bsp_aux_ch_getcfg();
        cfg.anl_gain = xcfg_cb.aux_anl_gain | xcfg_cb.aux_anl_gain<<6;      //双声道模拟增益保持一致
        cfg.dig_gain = xcfg_cb.aux_dig_gain | xcfg_cb.aux_dig_gain<<6;      //双声道数字增益保持一致
    }
#endif // FUNC_AUX_EN

    if (path_idx == AUDIO_PATH_BTMIC || path_idx == AUDIO_PATH_KARAOK || path_idx == AUDIO_PATH_OPUS || path_idx == AUDIO_PATH_TTP || path_idx == AUDIO_PATH_SPEAKER || path_idx == AUDIO_PATH_USBMIC || path_idx == AUDIO_PATH_ASR) {
        if (path_idx == AUDIO_PATH_BTMIC) {
            if (sys_cb.hfp_karaok_en) {
                memcpy(&cfg, &rec_cfg_tbl[AUDIO_PATH_KARAOK], sizeof(sdadc_cfg_t));
                cfg.sample_rate = SPR_48000;
            } else {
                if (bt_sco_is_msbc() || bt_sco_dnn_en()) {     //如果开了msbc或dnn，则采样率设为16k
                    cfg.sample_rate = SPR_16000;
                }
            }
        }
        cfg.channel = bsp_mic_ch_getcfg(path_idx);

        cfg.anl_gain = ((xcfg_cb.mic0_anl_gain) |
                        (xcfg_cb.mic1_anl_gain<<6) |
                        (xcfg_cb.mic2_anl_gain<<12)|
                        (xcfg_cb.mic3_anl_gain<<18)|
                        (xcfg_cb.mic4_anl_gain<<24));
        cfg.dig_gain = ((xcfg_cb.bt_mic0_dig_gain) |
                        (xcfg_cb.bt_mic1_dig_gain<<6) |
                        (xcfg_cb.bt_mic2_dig_gain<<12)|
                        (xcfg_cb.bt_mic3_dig_gain<<18)|
                        (xcfg_cb.bt_mic4_dig_gain<<24));
    }

#if ANC_ALG_EN
    if (path_idx == AUDIO_PATH_ANC_ALG) {
        anc_alg_audio_path_cfg_set(&cfg);
    }
#endif // ANC_ALG_EN

#if IODM_TEST_EN
    if (path_idx == AUDIO_PATH_IODM_MIC_TEST){
        iodm_test_loopback_set_mic_ch(&cfg.channel, &cfg.anl_gain, &cfg.dig_gain);
    }
#endif

#if FUNC_USBDEV_EN
    if (path_idx == AUDIO_PATH_USBMIC) {
        cfg.sample_rate = sys_cb.ude_mic_spr;
#if USB_MIC_NR_EN
        cfg.sample_rate = SPR_16000;
        cfg.samples = 480;
#endif
    }
#endif

    sdadc_init(&cfg);

    if (path_idx == AUDIO_PATH_BTMIC) {
        if (!bt_sco_is_msbc() && bt_sco_dnn_en()) {             //部分降噪算法窄带通话时，ADC为16K采样率，DAC为8K采样率
            dac_spr_set(SPR_8000);
        }
    }

#if SDADC_DRC_EN
    sdadc_drc_init((u8 *)RES_BUF_EQ_SDADC_DRC, RES_LEN_EQ_SDADC_DRC);
#endif
}

/*****************************************************************************
 * 功能   : 启动AUDIO采集和DAC数据处理
 * 输入   : audio_path_idx 对应不同功能使用的audio通路
 * 注意   : channel需要和init时保持一致，否则通路会启动失败
 * 返回   : 无
 *****************************************************************************/
void audio_path_start(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = bsp_aux_ch_getcfg();
        cfg.anl_gain = xcfg_cb.aux_anl_gain | xcfg_cb.aux_anl_gain<<6;      //双声道模拟增益保持一致
        cfg.dig_gain = xcfg_cb.aux_dig_gain | xcfg_cb.aux_dig_gain<<6;      //双声道数字增益保持一致
    }
#endif // FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_BTMIC || path_idx == AUDIO_PATH_KARAOK || path_idx == AUDIO_PATH_OPUS || path_idx == AUDIO_PATH_TTP || path_idx == AUDIO_PATH_SPEAKER || path_idx == AUDIO_PATH_USBMIC || path_idx == AUDIO_PATH_ASR) {
        cfg.channel = bsp_mic_ch_getcfg(path_idx);
    }

#if ANC_ALG_EN
    if (path_idx == AUDIO_PATH_ANC_ALG) {
        anc_alg_audio_path_cfg_set(&cfg);
    }
#endif // ANC_ALG_EN

#if IODM_TEST_EN
    if (path_idx == AUDIO_PATH_IODM_MIC_TEST){
        iodm_test_loopback_set_mic_ch(&cfg.channel, &cfg.anl_gain, &cfg.dig_gain);
    }
#endif

    sdadc_start(cfg.channel);
}

/*****************************************************************************
 * 功能   : 关闭对应AUDIO_PATH
 * 输入   : audio_path_idx 对应不同功能使用的audio通路
 * 注意   : 关闭audio后，功耗要和打开audio之前保持一致
 * 返回   : 无
 *****************************************************************************/
void audio_path_exit(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FUNC_AUX_EN
    if (path_idx == AUDIO_PATH_AUX) {
        cfg.channel = bsp_aux_ch_getcfg();
    }
#endif // FUNC_AUX_EN

    if (path_idx == AUDIO_PATH_BTMIC || path_idx == AUDIO_PATH_KARAOK || path_idx == AUDIO_PATH_OPUS || path_idx == AUDIO_PATH_TTP || path_idx == AUDIO_PATH_SPEAKER || path_idx == AUDIO_PATH_ASR || path_idx == AUDIO_PATH_USBMIC) {
        cfg.channel = bsp_mic_ch_getcfg(path_idx);
    }

#if ANC_ALG_EN
    if (path_idx == AUDIO_PATH_ANC_ALG) {
        anc_alg_audio_path_cfg_set(&cfg);
    }
#endif // ANC_ALG_EN

#if IODM_TEST_EN
    if (path_idx == AUDIO_PATH_IODM_MIC_TEST){
        iodm_test_loopback_set_mic_exit(&cfg.channel);
    }
#endif

    sdadc_exit(cfg.channel);

#if ANC_EN
    if (!sys_cb.anc_start)
#endif
    {
        adpll_spr_set(cfg_dac_out_spr);
    }
}


