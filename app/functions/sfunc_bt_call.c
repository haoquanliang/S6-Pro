#include "include.h"
#include "func.h"
#include "func_bt.h"

static call_cfg_t call_cfg AT(.sco_data);
#if ANC_ALG_EN
static u8 sco_anc_alg_en AT(.sco_data);
#endif // ANC_ALG_EN
#if ASR_EN
static u8 bt_asr_sta;
#endif
#if SYS_KARAOK_EN
static u8 sco_karaok_en AT(.sco_data);
#endif
bool mic_eq_test_get_status(void);


#if BT_SCO_DBG_EN
void sco_audio_set_param(u8 type, u16 value)
{
//    aec_cfg_t *aec = &call_cfg.aec;
//    nr_cb_t *nr = &call_cfg.nr;
    //printf("set param[%d]:%d\n", type, value);
    if (type == AEC_PARAM_NOISE) {
//        xcfg_cb.bt_noise_threshoid = value;
//        nr->threshoid = value;
    } else if (type == AEC_PARAM_LEVEL) {
        xcfg_cb.bt_echo_level = value;
//        aec->echo_level = value;
    } else if (type == AEC_PARAM_OFFSET) {
//       aec->far_offset = value;
    } else if (type == AEC_PARAM_MIC_ANL_GAIN) {
        xcfg_cb.mic0_anl_gain = value;
//        set_mic_analog_gain(value, (CHANNEL_0 | CHANNEL_1 | CHANNEL_2));
    } else if (type == AEC_PARAM_MIC_DIG_GAIN) {
        xcfg_cb.bt_mic0_dig_gain = value;
//        sdadc_set_digital_gain((CHANNEL_1 | CHANNEL_2 | CHANNEL_3), (value&0x3f));
    } else if (type == AEC_PARAM_MIC_POST_GAIN) {
        xcfg_cb.bt_sco_post_gain = value & 0x0f;
    }
}
#endif

static void bt_sco_eq_init(call_cfg_t *p)
{
#if BT_MIC_DRC_EN
    u8 *mic_drc_addr;
    u32 mic_drc_len;
#endif
#if SDADC_EQ_EN
    u32 micl_soft_eq_addr, micl_soft_eq_len;
    u32 micr_soft_eq_addr, micr_soft_eq_len;
#endif
    u32 mic_eq_addr, mic_eq_len;
    if (bt_sco_is_msbc()) {
        mic_eq_addr = RES_BUF_EQ_BT_MIC_16K_EQ;
        mic_eq_len  = RES_LEN_EQ_BT_MIC_16K_EQ;
    #if SDADC_EQ_EN
        micl_soft_eq_addr = RES_BUF_EQ_SDADCL_16K_EQ;
        micl_soft_eq_len  = RES_LEN_EQ_SDADCL_16K_EQ;
        micr_soft_eq_addr = RES_BUF_EQ_SDADCR_16K_EQ;
        micr_soft_eq_len  = RES_LEN_EQ_SDADCR_16K_EQ;
    #endif
    #if BT_MIC_DRC_EN
        mic_drc_addr = (u8 *)RES_BUF_EQ_BT_MIC_16K_DRC;
        mic_drc_len = RES_LEN_EQ_BT_MIC_16K_DRC;
    #endif
    } else {
        mic_eq_addr = RES_BUF_EQ_BT_MIC_8K_EQ;
        mic_eq_len  = RES_LEN_EQ_BT_MIC_8K_EQ;
    #if SDADC_EQ_EN
        micl_soft_eq_addr = RES_BUF_EQ_SDADCL_8K_EQ;
        micl_soft_eq_len  = RES_LEN_EQ_SDADCL_8K_EQ;
        micr_soft_eq_addr = RES_BUF_EQ_SDADCR_8K_EQ;
        micr_soft_eq_len  = RES_LEN_EQ_SDADCR_8K_EQ;
    #endif
    #if BT_MIC_DRC_EN
        mic_drc_addr = (u8 *)RES_BUF_EQ_BT_MIC_8K_DRC;
        mic_drc_len = RES_LEN_EQ_BT_MIC_8K_DRC;
    #endif
    }
#if SDADC_EQ_EN
    sdadcl_set_soft_eq_by_res(micl_soft_eq_addr, micl_soft_eq_len);
    sdadcr_set_soft_eq_by_res(micr_soft_eq_addr, micr_soft_eq_len);
#endif

    p->post_gain = (mic_gain_tbl[xcfg_cb.bt_sco_post_gain] >> 8);

    if (mic_set_eq_by_res(mic_eq_addr, mic_eq_len)) {
        call_cfg.mic_eq_en = 1;
    }
#if BT_MIC_DRC_EN
    mic_set_drc_by_res((u32)mic_drc_addr, mic_drc_len);
#endif
    mic_set_post_gain(mic_gain_tbl[xcfg_cb.bt_sco_post_gain]);

#ifdef RES_BUF_EQ_CALL_NORMAL_EQ
    music_set_eq_by_res(RES_BUF_EQ_CALL_NORMAL_EQ, RES_LEN_EQ_CALL_NORMAL_EQ);
#else
    music_eq_off();
#endif

#if DAC_DRC_EN
    music_drc_off();
#endif
}

static void bt_sco_eq_exit(void)
{
#if BT_MIC_DRC_EN
    mic_drc_off();
#endif
    mic_eq_off();
    music_set_eq_by_num(sys_cb.eq_mode);

    if(app_eq_set() == 0){
        music_set_eq_by_num(sys_cb.eq_mode);
    }

#if DAC_DRC_EN
    music_drc_on();
#endif
}

static void bt_call_alg_init(void)
{
    u8 sysclk = SYS_24M;
    memset(&call_cfg, 0, sizeof(call_cfg_t));

    sys_clk_req(INDEX_VOICE, SYS_100M);

    clkgat_hwmath_en();     //算法初始化会用到，需要放前面
    clkgat_plc_en();        //硬件sqrt64需要用到

    ///AEC初始化
    bt_sco_aec_init(&sysclk, &call_cfg.aec, &call_cfg.alc);

    ///通话MIC端降噪算法初始化
    bt_sco_nr_init(&sysclk, &call_cfg.nr);

    ///通话DAC端降噪算法初始化
    bt_sco_far_nr_init(&sysclk, &call_cfg.nr);

    ///DAC、MIC EQ初始化
    bt_sco_eq_init(&call_cfg);

    if (SYS_100M != sysclk) {
        sys_clk_req(INDEX_VOICE, sysclk);
    }

    bt_call_init(&call_cfg);
}

static void bt_call_alg_exit(void)
{
    bt_sco_nr_exit();

    bt_call_exit();

    bt_sco_eq_exit();

    clkgat_hwmath_dis();
    clkgat_plc_dis();       //plc和sqrt64同时关闭clkgate

    sys_clk_free(INDEX_VOICE);
}

ALIGNED(64)
void sco_set_incall_flag(u8 bit)
{
    GLOBAL_INT_DISABLE();
    sys_cb.incall_flag |= bit;
    GLOBAL_INT_RESTORE();
}

ALIGNED(128)
bool sco_clr_incall_flag(u8 bit)
{
    bool ret = false;
    GLOBAL_INT_DISABLE();
    if(sys_cb.incall_flag == INCALL_FLAG_FADE) {
        ret = true;
    }
    sys_cb.incall_flag &= ~bit;
    GLOBAL_INT_RESTORE();
    return ret;
}

#if FUNC_BT_EN
void sco_audio_init(void)
{
    fot_update_pause();

#if DAC_DNC_EN
    msg_queue_detach(EVT_DNC_START, 0);
    msg_queue_detach(EVT_DNC_STOP, 0);
    msg_enqueue(EVT_DNC_STOP);
#endif

#if ASR_EN
    bt_asr_sta = bsp_asr_get_statue();
    bsp_asr_stop();
#endif

#if TINY_TRANSPARENCY_EN
    bsp_ttp_stop();
#endif

#if ANC_ALG_EN
    sco_anc_alg_en = sys_cb.anc_alg_en;
    sys_cb.anc_alg_type_bak = bsp_anc_alg_get_type();
    bsp_anc_alg_stop();
    anc_alg_enter_critical();
#endif // ANC_ALG_EN

#if SYS_KARAOK_EN
    sco_karaok_en = sys_cb.karaok_init;
    bsp_karaok_exit();
#endif

#if BT_MUSIC_EFFECT_EN
    music_effect_sco_audio_init_do();
#endif // BT_MUSIC_EFFECT_EN

    sco_set_incall_flag(INCALL_FLAG_SCO);
    if (!bt_sco_karaok_is_en(1)) {
        bt_call_alg_init();
        dac_set_anl_offset(1);
#if IIS_RX2SCO_EN
        bsp_iis_sco_init();
        iis_start();
#else
        audio_path_init(AUDIO_PATH_BTMIC);
        audio_path_start(AUDIO_PATH_BTMIC);
#endif
        bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
        dac_fade_in();
    }

#if ANC_ALG_EN
    anc_alg_exit_critical();
#endif // ANC_ALG_EN
}

void sco_audio_exit(void)
{
    sco_clr_incall_flag(INCALL_FLAG_SCO);
    if (!bt_sco_karaok_is_en(0)) {
        dac_fade_out();
        dac_aubuf_clr();
        dac_set_anl_offset(0);
        bsp_change_volume(sys_cb.vol);
#if IIS_RX2SCO_EN
        iis_stop();
#else
        audio_path_exit(AUDIO_PATH_BTMIC);
#endif
    }
    bt_call_alg_exit();

#if SYS_KARAOK_EN
    if (sco_karaok_en) {
        bsp_karaok_init();
    }
#endif

#if TINY_TRANSPARENCY_EN
    bsp_ttp_init();
    if(!sco_is_connected()){
        bsp_ttp_start();
    }
#endif

#if BT_MUSIC_EFFECT_EN
    music_effect_sco_audio_exit_do();
#endif // BT_MUSIC_EFFECT_EN

#if ANC_ALG_EN
    if (sco_anc_alg_en) {
        bsp_anc_alg_start(sys_cb.anc_alg_type_bak);
        sys_cb.anc_alg_type_bak = 0;
    }
#endif // ANC_ALG_EN

    fot_update_continue();

#if DAC_DNC_EN
    msg_queue_detach(EVT_DNC_START, 0);
    msg_queue_detach(EVT_DNC_STOP, 0);
    msg_enqueue(EVT_DNC_START);
#endif
#if ASR_EN
    if (bt_asr_sta) {
        bsp_asr_start();
		bt_asr_sta = 0;
    }
#endif
}

static void sfunc_bt_call_process(void)
{
    func_process();
#if BT_TWS_MS_SWITCH_EN
    if (xcfg_cb.bt_tswi_sco_en && bt_tws_need_switch(0)) {
        printf("AUDIO SWITCH\n");
        bt_tws_switch();
    }
#endif
    func_bt_sub_process();
    func_bt_status();
}

static void sfunc_bt_call_enter(void)
{
    sco_set_incall_flag(INCALL_FLAG_CALL);
    if(sys_cb.incall_flag == INCALL_FLAG_FADE) {
        bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
        dac_fade_in();
    }
}

static void sfunc_bt_call_exit(void)
{
    bool vol_change = sco_clr_incall_flag(INCALL_FLAG_CALL);
    if(vol_change) {
        bsp_change_volume(sys_cb.vol);
    }
}

AT(.text.func.bt)
void sfunc_bt_call(void)
{
    printf("%s\n", __func__);

    sfunc_bt_call_enter();

    while ((f_bt.disp_status >= BT_STA_OUTGOING) && (func_cb.sta == FUNC_BT)) {
        sfunc_bt_call_process();
        sfunc_bt_call_message(msg_dequeue());
        func_bt_display();
    }
    sfunc_bt_call_exit();
}

#else

void sco_audio_init(void){}
void sco_audio_exit(void){}

#endif //FUNC_BT_EN
