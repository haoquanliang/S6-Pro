#include "include.h"
#include "usb_call.h"

#if USB_MIC_NR_EN
aec_cfg_t uda_aec;
nr_cb_t uda_nr;
#if BT_SNDP_SMIC_AI_EN
    static sndp_sm_cb_t sndp_sm_cb AT(.sndp_buf);
#elif BT_SNDP_DM_AI_EN
    static sndp_dm_cb_t sndp_dm_cb AT(.sndp_dm_buf);
#elif BT_SCO_LDMIC_AI_EN
    static ldmdnn_cb_t ldmdnn_cb AT(.ldmdnn_buf);
    uint8_t cfg_micr_phase_en;
#endif

extern s16 bt_sco_dump_buf[4][240];
void load_code_bt_voice(void);

AT(.usbdev.com)
void usbmic_data_process(u8 *ptr, u32 samples, int ch_mode)
{
    if (uda_is_bypass()) {
        return;
    }
    usbmic_aec_process(ptr, samples, ch_mode);
}

void uda_mic_alg_init(void)
{
    printf("uda_mic_alg_init\n");
    u8 sysclk = SYS_24M;
    uda_aec_pcm_init();
    clkgat_hwmath_en();     //算法初始化会用到，需要放前面
    clkgat_plc_en();        //硬件sqrt64需要用到
    memset(&uda_aec, 0, sizeof(uda_aec));
#if USB_AEC_EN
    bt_sco_aec_init(&sysclk, &uda_aec, 0);
    uda_aec.far_offset = 48*3;
#endif
    aec_init(&uda_aec);
    load_code_bt_voice();

    ///算法数据dump初始化
#if BT_SCO_DUMP_EN || BT_AEC_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_EQ_DUMP_EN
    if (!xcfg_cb.huart_en) {
        printf("dump huart xcfg init err!\n");
        while(1);
    }
    memset(bt_sco_dump_buf, 0, sizeof(bt_sco_dump_buf));
    sco_huart_init();
#endif

#if BT_SNDP_SMIC_AI_EN
    uda_alg_sndp_sm_init(&uda_nr, &sysclk);
#elif BT_SNDP_DM_AI_EN
    uda_alg_sndp_dm_init(&uda_nr, &sysclk);
#elif BT_SCO_LDMIC_AI_EN
    uda_alg_ldmdnn_init(&uda_nr, &sysclk);
#endif
    aec_pcm_nr_cb_init(&uda_nr);
    sys_clk_req(INDEX_VOICE, sysclk);
}

void uda_mic_alg_exit(void)
{
    delay_5ms(1);
#if BT_SNDP_SMIC_AI_EN
    uda_alg_sndp_sm_exit();
#elif BT_SNDP_DM_AI_EN
    uda_alg_sndp_dm_exit();
#elif BT_SCO_LDMIC_AI_EN
    uda_alg_ldmdnn_exit();
#endif
    aec_exit();
    clkgat_hwmath_dis();
    clkgat_plc_dis();       //plc和sqrt64同时关闭clkgate
    uda_aec_pcm_exit();
    sys_clk_free(INDEX_VOICE);
}

#if BT_SNDP_SMIC_AI_EN
void uda_alg_sndp_sm_init(nr_cb_t *nr, u8 *sysclk)
{
    if (!xcfg_cb.bt_sndp_smic_ai_en) {
        printf("SNDP DNN xcfg init warning!\n");
        return;
    }
    nr->nr_type             = NR_TYPE_SNDP_SM;
    memset(&sndp_sm_cb, 0, sizeof(sndp_sm_cb_t));
    sndp_sm_cb.param_printf = 0;
    sndp_sm_cb.level        = xcfg_cb.bt_sndp_level;
    sndp_sm_cb.dnn_level    = xcfg_cb.bt_sndp_dnn_level;
#ifdef RES_BUF_SPECCFGPARAST_F_BIN
    if (RES_LEN_SPECCFGPARAST_F_BIN > 0) {
        sndp_sm_cb.coef = (const u32 *)RES_BUF_SPECCFGPARAST_F_BIN;
    }
#endif
    alg_sndp_sm_init(&sndp_sm_cb);

    aec_pcm_set_nr_kick_func(bt_sndp_sm_process);

    *sysclk = *sysclk < SYS_80M ? SYS_80M : *sysclk;
}
void uda_alg_sndp_sm_exit(void)
{
    alg_sndp_sm_exit();
}
#endif

#if BT_SCO_LDMIC_AI_EN
void uda_alg_ldmdnn_init(nr_cb_t *nr, u8 *sysclk)
{
    if (!xcfg_cb.bt_dmns_en) {  //配置上面的DMNS双麦降噪功能未打开
        printf("DMNS xcfg init warning!\n");
    }
	memset(&ldmdnn_cb, 0, sizeof(ldmdnn_cb_t));
	nr->nr_type             = NR_TYPE_DMIC_AI;

    //辅助参数
    ldmdnn_cb.param_printf              = 0;
    ldmdnn_cb.AI_sig_ch_en              = 1;

	//基本参数
    ldmdnn_cb.distance                  = xcfg_cb.bt_dmns_distance;
    ldmdnn_cb.nt                        = xcfg_cb.bt_dmns_level;

	//baseline参数
	ldmdnn_cb.comp_mic_fre_L            = 0;
	ldmdnn_cb.comp_mic_fre_H            = 256;
	ldmdnn_cb.bf_choose                 = 1;

	//入射角度估计
    ldmdnn_cb.Ftrust                    = 54;

	//TF参数
	ldmdnn_cb.tf_en				        = 1;
	ldmdnn_cb.tf_len				    = 256;
	ldmdnn_cb.tf_norm_en			    = 0;
	ldmdnn_cb.tf_norm_only_en		    = 0;
	ldmdnn_cb.lowF_change_en		    = 25;
	ldmdnn_cb.protect_Agl			    = 50;

	//波束增强参数
	ldmdnn_cb.huawu_en                  = 1;
	ldmdnn_cb.opt_limt                  = 42767;
	ldmdnn_cb.frame_gain_f              = 15;
	ldmdnn_cb.counter_en                = 1;
	ldmdnn_cb.counter_lim               = 20;
	ldmdnn_cb.counter_substep           = 5000;
	ldmdnn_cb.gain_used                 = 200;
	ldmdnn_cb.gain_sm                   = 26384;

	//风噪参数
	ldmdnn_cb.wind_len_used             = 40;  //要小于 WIND_LEN 66
	ldmdnn_cb.wind_sm_len               = 40;   //不能大于wind_len_used
	ldmdnn_cb.wind_sig_choose		    = 1;//0时代表副mic受风小  1代表主mic受风小
	ldmdnn_cb.wind_state_lim            = 20000;
	ldmdnn_cb.wind_sup_floor            = 25000;
	ldmdnn_cb.wind_sup_open             = 1;
//	ldmdnn_cb.wind_a_pow_en             = 0;
//
//	ldmdnn_cb.Gcoh_thr			        = 13666;
//	ldmdnn_cb.Gcoh_sum_thr		        = 9666;
	ldmdnn_cb.wind_or_en			    = 1;
	ldmdnn_cb.wind_assign               = 1024;  // q10通道1的缩放倍数，与通道二对齐方便计算风噪

	//AI参数
    ldmdnn_cb.noise_ps_rate             = 1;
    ldmdnn_cb.low_fre_ns0_range	        = 16;//range:1-200
    ldmdnn_cb.prior_opt_idx		        = 3;
    ldmdnn_cb.exp_range_H			    = 0;
    ldmdnn_cb.exp_range_L			    = 0;
    ldmdnn_cb.music_lev		            = 11;
    ldmdnn_cb.gain_expand			    = 1024;
    ldmdnn_cb.nn_only				    = 0;
    ldmdnn_cb.nn_only_len			    = 16;
    ldmdnn_cb.hi_fre_en 			    = 1;
    ldmdnn_cb.gain_assign			    = 26666;
    ldmdnn_cb.sin_gain_post_en          = 1;
    ldmdnn_cb.sin_gain_post_len_f	    = 160;
    ldmdnn_cb.b_frein2_floor		    = 19666;
    ldmdnn_cb.enlarge_v			        = 1;//1 or 2 or 3
    ldmdnn_cb.mask_floor			    = 200;
    ldmdnn_cb.mask_floor_hi		        = 160;

	//单麦模式
	ldmdnn_cb.single_mic_en			    = 0;
	ldmdnn_cb.single_mic_gain			= 2;

    alg_ldmdnn_init(&ldmdnn_cb);

    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }

    aec_pcm_set_nr_kick_func(bt_ldmdnn_process);

    *sysclk = *sysclk < SYS_100M ? SYS_100M : *sysclk;
}

void uda_alg_ldmdnn_exit(void)
{
    alg_ldmdnn_exit();
}
#endif

#if BT_SNDP_DM_AI_EN
void uda_alg_sndp_dm_init(nr_cb_t *nr , u8 *sysclk)
{
    if (!xcfg_cb.bt_sndp_dm_ai_en) {
        printf("SNDP DM xcfg init warning!\n");
        return;
    }
    nr->nr_type             = NR_TYPE_SNDP_DM;
    memset(&sndp_dm_cb, 0, sizeof(sndp_dm_cb_t));
    sndp_dm_cb.param_printf = 0;
    sndp_dm_cb.level        = xcfg_cb.sndp_dm_level;
    sndp_dm_cb.dnn_level    = xcfg_cb.sndp_dm_dnn_level;
    sndp_dm_cb.distance     = 70;
    sndp_dm_cb.degree       = xcfg_cb.sndp_dm_degree;
    sndp_dm_cb.degree1      = xcfg_cb.sndp_dm_degree1;
    sndp_dm_cb.bf_upper     = 128;

    if (sndp_dm_cb.degree1 < sndp_dm_cb.degree) {
        sndp_dm_cb.degree1 = sndp_dm_cb.degree;
    } else {
        sndp_dm_cb.degree1 = sndp_dm_cb.degree1 - sndp_dm_cb.degree;    //相减得出过渡角
    }

//    if(xcfg_cb.micr_phase_en == 1){                                   //当前软件不支持反相
//        cfg_micr_phase_en = 1;
//    }

#ifdef RES_BUF_SPECCFGPARAST_F_BIN
    if (RES_LEN_SPECCFGPARAST_F_BIN > 0) {
        sndp_dm_cb.coef = (const u32 *)RES_BUF_SPECCFGPARAST_F_BIN;
    }
#endif

    alg_sndp_dm_init(&sndp_dm_cb);
    printf("dm level: %d %d, distance: %d\n", sndp_dm_cb.level, sndp_dm_cb.dnn_level, sndp_dm_cb.distance);
    printf("degree: %d %d\n", sndp_dm_cb.degree, sndp_dm_cb.degree1);
    printf("alg_sndp_dm_init\n");

    aec_pcm_set_nr_kick_func(bt_sndp_dm_process);

    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
}

void uda_alg_sndp_dm_exit(void)
{
    alg_sndp_dm_exit();
}
#endif
#endif
