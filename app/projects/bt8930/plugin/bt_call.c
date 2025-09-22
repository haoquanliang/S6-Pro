#include "include.h"

#if BT_SNDP_SMIC_AI_EN
    static sndp_sm_cb_t sndp_sm_cb AT(.sndp_buf);
#elif BT_SNDP_DM_AI_EN
    static sndp_dm_cb_t sndp_dm_cb AT(.sndp_dm_buf);
    uint8_t cfg_micr_phase_en;
#elif BT_SNDP_FBDM_EN
    static sndp_fbdm_cb_t sndp_fbdm_cb AT(.sndp_fbdm_buf);
#elif BT_SCO_SMIC_AI_EN
    static dnn_cb_t dnn_cb AT(.dnn_buf);
#elif BT_SCO_SMIC_AI_PLUS_EN
    static dnn_plus_cb_t dnn_plus_cb AT(.dnn_plus_buf);
#elif BT_SCO_SMIC_AI_PRO_EN
    static dnn_pro_cb_t dnn_pro_cb AT(.dnn_pro_buf);
#elif BT_SCO_AIAEC_DNN_EN
    static dnn_aec_ns_cb_t aiaec_dnn_cb AT(.aiaec_dnn_buf);
#elif BT_SCO_DMIC_AIAEC_EN
    static dmdnn_aiaec_cb_t aiaec_dmdnn_cb AT(.aiaec_dmdnn_buf);
    uint8_t cfg_micr_phase_en;
#elif BT_SCO_FFDM_AI_PRO_EN
    static ffdm_pro_cb_t ffdm_pro_cb AT(.ffdm_pro_buf);
    uint8_t cfg_micr_phase_en;
#elif BT_SCO_FFDM_AI_EN
    static ffdm_cb_t ffdm_cb AT(.ffdm_buf);
    uint8_t cfg_micr_phase_en;
#elif BT_SCO_DMIC_AI_EN
    static dmns_cb_t dmns_cb AT(.dmdnn_buf);
    uint8_t cfg_micr_phase_en;
#elif BT_SCO_LDMIC_AI_EN
    static ldmdnn_cb_t ldmdnn_cb AT(.ldmdnn_buf);
    uint8_t cfg_micr_phase_en;
#elif BT_SCO_NR_EN
    static ains_cb_t ains_cb AT(.ains_buf);
#endif

#if BT_SCO_FAR_NR_EN
static peri_nr_cfg_t far_cfg AT(.nr_buf.far);
#endif

void bt_sco_rec_exit(void);

AT(.text.sco.gain.tbl)
const int mic_gain_tbl[32] = {
    SOFT_DIG_P0DB,
    SOFT_DIG_P1DB,
    SOFT_DIG_P2DB,
    SOFT_DIG_P3DB,
    SOFT_DIG_P4DB,
    SOFT_DIG_P5DB,
    SOFT_DIG_P6DB,
    SOFT_DIG_P7DB,
    SOFT_DIG_P8DB,
    SOFT_DIG_P9DB,
    SOFT_DIG_P10DB,
    SOFT_DIG_P11DB,
    SOFT_DIG_P12DB,
    SOFT_DIG_P13DB,
    SOFT_DIG_P14DB,
    SOFT_DIG_P15DB,
    SOFT_DIG_P16DB,
    SOFT_DIG_P17DB,
    SOFT_DIG_P18DB,
    SOFT_DIG_P19DB,
    SOFT_DIG_P20DB,
    SOFT_DIG_P21DB,
    SOFT_DIG_P22DB,
    SOFT_DIG_P23DB,
    SOFT_DIG_P24DB,
    SOFT_DIG_P25DB,
    SOFT_DIG_P26DB,
    SOFT_DIG_P27DB,
    SOFT_DIG_P28DB,
    SOFT_DIG_P29DB,
    SOFT_DIG_P30DB,
    SOFT_DIG_P31DB,
};

void bt_sco_aec_init(u8 *sysclk, aec_cfg_t *aec, alc_cb_t *alc)
{
    if (xcfg_cb.bt_aec_en) {
    #if BT_AEC_EN
        aec->type           = AEC_TYPE_DEFAULT;
        aec->mode           = 1;
        aec->nlp_bypass     = BT_AEC_NLP_BYPASS_EN;
        aec->nlp_only       = 0;
        aec->nlp_level      = BT_ECHO_LEVEL;
        aec->nlp_part       = 1;
        aec->comforn_level  = 10;
        aec->comforn_floor  = 300;
        aec->comforn_en     = 0;
        aec->xe_add_corr    = 16384;
        aec->upbin          = 63;
        aec->lowbin         = 7;
        aec->diverge_th     = 15;
        aec->echo_th        = 66666;
        aec->gamma          = 27852;
        aec->aggrfact       = 16384;
        aec->depre_mode     = 1;
        aec->mic_ch         = 0;
        aec->qidx           = (16384 * (aec->upbin - aec->lowbin + 1)) >> 15;
        aec->dig_gain       = mic_gain_tbl[xcfg_cb.aec_dig_gain] >> 8;
        *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
    #endif
    } else {
        aec->type         = AEC_TYPE_NONE;
    }
    aec->far_offset     = xcfg_cb.aec_far_offset;           //声加1+1会用到这个参数，单独放到外面初始化
	aec->ff_mic_ref_en  = BT_AEC_FF_MIC_REF_EN;
}

void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr)
{
#if BT_SCO_FAR_NR_EN
    nr->nr_cfg_en |= NR_CFG_FAR_EN;
    memset(&far_cfg, 0, sizeof(peri_nr_cfg_t));
    nr->far_nr = &far_cfg;
    far_cfg.level = BT_SCO_FAR_NOISE_LEVEL;
    far_cfg.min_range1 = 30;
    far_cfg.min_range2 = 50;
    far_cfg.min_range3 = 80;
    far_cfg.min_range4 = 120;
    far_cfg.nera_val = 0x7eb8;
#endif

#if BT_SCO_DAC_DRC_EN
    nr->nr_cfg_en |= NR_CFG_DAC_DRC_EN;
    bt_sco_dac_drc_init((u8 *)RES_BUF_EQ_CALL_DAC_DRC, RES_LEN_EQ_CALL_DAC_DRC);
#endif

#if BT_SCO_DAC_DNR_EN
    nr->nr_cfg_en |= NR_CFG_DAC_DNR_EN;
    nr->dac_dnr_thr = BT_SCO_DAC_DNR_THR;
#endif

#if BT_SCO_CALLING_NR_EN
    nr->calling_voice_cnt = BT_SCO_CALLING_VOICE_CNT;
    nr->calling_voice_pow = BT_SCO_CALLING_VOICE_POW;
#endif
}

void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr)
{
#if BT_TRUMPET_NR_EN
    nr->nr_cfg_en |= NR_CFG_TRUMPET_EN;
    trumpet_denoise_init(BT_TRUMPET_NR_LEVEL);
#endif

#if BT_SCO_FADE_EN
    nr->nr_cfg_en |= NR_CFG_SCO_FADE_EN;
#endif

    bt_sco_near_nr_init(sysclk, nr);

    if (!xcfg_cb.bt_nr_en) {
        nr->nr_type = ((nr->nr_type & ~NR_CFG_TYPE_MASK) | NR_TYPE_NONE);
    }
}

void bt_sco_nr_exit(void)
{
    bt_sco_near_nr_exit();
}

#if BT_SNDP_DMIC_EN
void bt_sco_sndp_dm_init(u8 *sysclk, nr_cb_t *nr)
{
}
#endif

#if BT_SNDP_SMIC_AI_EN
void bt_sco_sndp_sm_init(u8 *sysclk, nr_cb_t *nr)
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
    bt_sndp_sm_init(&sndp_sm_cb);

    *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
}
#endif

#if BT_SNDP_FBDM_EN
void bt_sco_sndp_fbdm_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_sndp_fbdm_en) {
        printf("SNDP FBDM xcfg init warning!\n");
        return;
    }
    nr->nr_type             = NR_TYPE_SNDP_FBDM;
    memset(&sndp_fbdm_cb, 0, sizeof(sndp_fbdm_cb_t));
    sndp_fbdm_cb.param_printf   = 0;
    sndp_fbdm_cb.level          = xcfg_cb.sndp_fbdm_level;
    sndp_fbdm_cb.dnn_level      = xcfg_cb.sndp_fbdm_dnn_level;
    sndp_fbdm_cb.noiserms_th0   = xcfg_cb.sndp_fbdm_noiserms_th0;

#ifdef RES_BUF_SPECCFGPARAST_F_BIN
    if (RES_LEN_SPECCFGPARAST_F_BIN > 0) {
        sndp_fbdm_cb.coef = (const u32 *)RES_BUF_SPECCFGPARAST_F_BIN;
    }
#endif
    bt_sndp_fbdm_init(&sndp_fbdm_cb);

    *sysclk = *sysclk < SYS_100M ? SYS_100M : *sysclk;
}
#endif

#if BT_SNDP_DM_AI_EN
void bt_sco_sndp_dm_init(u8 *sysclk, nr_cb_t *nr)
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
    sndp_dm_cb.distance     = xcfg_cb.sndp_dm_distance;
    sndp_dm_cb.degree       = xcfg_cb.sndp_dm_degree;
    sndp_dm_cb.degree1      = xcfg_cb.sndp_dm_degree1;
    sndp_dm_cb.bf_upper     = 128;

    if (sndp_dm_cb.degree1 < sndp_dm_cb.degree) {
        sndp_dm_cb.degree1 = sndp_dm_cb.degree;
    } else {
        sndp_dm_cb.degree1 = sndp_dm_cb.degree1 - sndp_dm_cb.degree;    //相减得出过渡角
    }

    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }

#ifdef RES_BUF_SPECCFGPARAST_F_BIN
    if (RES_LEN_SPECCFGPARAST_F_BIN > 0) {
        sndp_dm_cb.coef = (const u32 *)RES_BUF_SPECCFGPARAST_F_BIN;
    }
#endif
    bt_sndp_dm_init(&sndp_dm_cb);

    *sysclk = *sysclk < SYS_100M ? SYS_100M : *sysclk;
}
#endif

#if BT_SCO_DMIC_AI_EN
void bt_sco_dmns_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dmns_en) {  //配置上面的DMNS双麦降噪功能未打开
        printf("DMNS xcfg init warning!\n");
    }
	memset(&dmns_cb, 0, sizeof(dmns_cb_t));
	nr->nr_type             = NR_TYPE_DMIC_AI;

	//辅助参数
	dmns_cb.param_printf                = 0;
	dmns_cb.ai_postF_en                 = 1;
	dmns_cb.show_out                    = 0;  //0表示没有特殊输出,1:YAM,2:YBM,3:f1_tmp,4:f2_tmp,5:talk

	//基本参数
	dmns_cb.distance                    = xcfg_cb.bt_dmns_distance;
	dmns_cb.nt                          = xcfg_cb.bt_dmns_level;

	//线性相位差参数
    dmns_cb.linear_en                   = 1;
	dmns_cb.linear_len                  = 150;
	dmns_cb.linear_level                = 5;
	dmns_cb.linear_gain                 = 0;
	dmns_cb.grad_mu                     = 327;

	// 风噪参数
	dmns_cb.wind_sig_choose             = 1;//0时代表副mic受风小  1代表主mic受风小
	dmns_cb.atf_smr                     = 30084;
	dmns_cb.wind_len_used               = 100;
	dmns_cb.wind_sup_open               = 1;
	dmns_cb.wind_state_lim              = 25000;
	dmns_cb.low_windstate_len           = 40;
	dmns_cb.low_windstate_gain          = 1024;
	dmns_cb.wind_or_en                  = 1;
	dmns_cb.wind_sup_floor              = 23000;
	dmns_cb.Gcoh_thr                    = 13666;
	dmns_cb.Gcoh_sum_thr                = 9666;
	dmns_cb.wind_pick_mode              = 2;
	dmns_cb.after_tf_norm               = 324;
	dmns_cb.gcoh_gain                   = 0;

	//白噪编辑
	dmns_cb.white_detect                = 32767;
	dmns_cb.white_nomal_scale           = 4;
	dmns_cb.highFcanc_256_to            = 0;
    dmns_cb.all_smr                     = 26635;    //全额补偿平滑系数 X_AMAM_sm_w[i] X_BMBM_sm_w[i]
    dmns_cb.white_type_len		        = 0;

	//baseline参数
	dmns_cb.comp_mic_fre_L              = 0;
	dmns_cb.comp_mic_fre_H              = 255;
	dmns_cb.opt_limit				    = 32767;
	dmns_cb.bf_choose				    = 1;
	dmns_cb.adp_smr                     = 26635;    //自适应补偿平滑系数 X_BMBM_sm[i] X_AMAM_sm[i]

	//RTF参数
	dmns_cb.tf_en					    = 1;
	dmns_cb.tf_len				        = 256; //smart: range:1-140   zoom:1:256
	dmns_cb.tf_norm_en			        = 1;
	dmns_cb.tf_norm_only_en		        = 0;
	dmns_cb.white_approach_shape        = 1;
	dmns_cb.v_white_en			        = 1;
	dmns_cb.tf_SMrate                   = 30000;
	dmns_cb.rtf_post_thres              = 2048;
	dmns_cb.sup180rtf                   = 40;

	//相位贴合
	dmns_cb.phase_to_w                  = 0;  //如果为0则最终相位为糅合，1为使用talk相位，2为使用ff相位
	dmns_cb.phase_to_nw                 = 0;

	// 波束增强模块选择
	dmns_cb.bfrein3_en		            = 1;//if set "1", the "cb->bfrein_en" must be "0".
	dmns_cb.bfrein3_intensity           = 1024;  //噪声估计强度，q10越大去噪越强最大不超过u16
	dmns_cb.sup180                      = 20;
	dmns_cb.grad_shift                  = 0;
	dmns_cb.frame_G_opt                 = 0;
	dmns_cb.bfrein3_ns		            = 3;

	dmns_cb.bfrein_en			        = 0;//if set "1", the "cb->bfrein3_en" must be "0".
	dmns_cb.b_frein2_floor		        = 19666;
	dmns_cb.bf2_used_len                = 0;

    //整形
	dmns_cb.high_tf_Llim                = 255;  //大于这个频率的tf用的gpost和平滑率略有不同
	dmns_cb.high_tf_rate                = 13000;
	dmns_cb.high_gpost_appd             = 10000;

	// AI参数
	dmns_cb.prior_opt_ada_en            = 1;
	dmns_cb.noise_ps_rate               = 1;
	dmns_cb.music_lev			        = 11;
    dmns_cb.gain_expand			        = 1024;
	dmns_cb.nn_only				        = 0;
	dmns_cb.nn_only_len			        = 16;
	dmns_cb.hi_fre_en 			        = 1;
    dmns_cb.gain_assign			        = 26666;
	dmns_cb.sin_gain_post_en		    = 1;
	dmns_cb.sin_gain_post_len	        = 0;
    dmns_cb.sin_gain_post_len_f	        = 256;
	dmns_cb.enlarge_v				    = 1;//1 or 2 or 3
	dmns_cb.mask_floor			        = 1000;
	dmns_cb.mask_floor_hi		        = 160;
	dmns_cb.low_fre_range               = 16;
	dmns_cb.low_fre_range0              = 0;
	dmns_cb.low_fre_ns0_range	        = 16;//range:1-200
	dmns_cb.prior_opt_idx               = 3;

	dmns_cb.sp_thres				    = 12000;
	dmns_cb.mic1_gain_idx               = 10;
	dmns_cb.mic2_gain_idx               = 10;
	dmns_cb.smooth_en				    = 1;
	dmns_cb.only_postfilter             = 0;

    bt_dmns_init(&dmns_cb);

    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }

    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
}
#endif

#if BT_SCO_LDMIC_AI_EN
void bt_sco_ldmdnn_init(u8 *sysclk, nr_cb_t *nr)
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
    //cb->wind_type				 = 1;

    bt_ldmdnn_init(&ldmdnn_cb);

    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }

    *sysclk = *sysclk < SYS_100M ? SYS_100M : *sysclk;
}
#endif

#if BT_SCO_FFDM_AI_EN
void bt_sco_ffdm_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_ffdm_en) {  //配置上面的耳夹（1+1）降噪功能未打开
        printf("FFDM xcfg init warning!\n");
    }
	memset(&ffdm_cb, 0, sizeof(ffdm_cb_t));
	nr->nr_type             = NR_TYPE_DMIC_AI;

	ffdm_cb.param_printf                = 0;
	ffdm_cb.distance			        = xcfg_cb.bt_ffdm_distance;
	ffdm_cb.noise_ps_rate               = 1;
	ffdm_cb.prior_opt_idx	            = 3;
	ffdm_cb.prior_opt_ada_en	        = 1;

	ffdm_cb.low_fre_range               = 16;
	ffdm_cb.low_fre_range0              = 0;

	ffdm_cb.mask_floor			        = 1100;
	ffdm_cb.noise_ceil			        = 0;
	ffdm_cb.music_lev			        = 11;
	ffdm_cb.gain_expand			        = 1024;
	ffdm_cb.nn_only				        = 0;
	ffdm_cb.nn_only_len			        = 16;

	ffdm_cb.gain_assign			        = 21666;
	ffdm_cb.sin_gain_post_en	        = 0;
	ffdm_cb.sin_gain_post_len	        = 128;
	ffdm_cb.sin_gain_post_len_f	        = 256;
	ffdm_cb.sp_thres			        = 6000;

	ffdm_cb.wind_len_used			    = 256;
	ffdm_cb.Gcoh_lim				    = 10000;
	ffdm_cb.wind_state_lim			    = 25000;
	ffdm_cb.wind_chg_thr			    = 10000; //多大风开始进融合
	ffdm_cb.wind_alluse_thr			    = 180000; //多大风开始全部用风小的mic，不再融合
	ffdm_cb.mix_fre_len				    = 128;//range:3-196
	ffdm_cb.wind_vaild_len			    = 43;
	ffdm_cb.wind_vaild_end			    = 313;
	ffdm_cb.wind_or_en				    = 0;
	ffdm_cb.low_windstate_len		    = 40;
	ffdm_cb.low_windstate_gain          = 1024;
	ffdm_cb.alluse_en				    = 1;
	ffdm_cb.phase_alluse_en			    = 0;
	ffdm_cb.wind_sm_factor			    = 0;

    bt_ffdm_init(&ffdm_cb);

    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }

    *sysclk = *sysclk < SYS_80M ? SYS_80M : *sysclk;
}
#endif

#if BT_SCO_FFDM_AI_PRO_EN
void bt_sco_ffdm_pro_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_ffdm_en) {  //配置上面的耳夹（1+1）降噪功能未打开
        printf("FFDM xcfg init warning!\n");
    }
	memset(&ffdm_pro_cb, 0, sizeof(ffdm_pro_cb_t));
	nr->nr_type             = NR_TYPE_DMIC_AI;

	ffdm_pro_cb.param_printf           	= 1;
	ffdm_pro_cb.distance                = xcfg_cb.bt_ffdm_distance;
	ffdm_pro_cb.gain_floor              = 1100;
    ffdm_pro_cb.noise_ps_rate			= 1;
	ffdm_pro_cb.prior_opt_idx			= 10;
	ffdm_pro_cb.prior_opt_ada_en        = 1;
	ffdm_pro_cb.low_fre_range			= 15;
	ffdm_pro_cb.music_lev				= 11;
	ffdm_pro_cb.nn_only					= 0;
	ffdm_pro_cb.nn_only_len				= 16;
	ffdm_pro_cb.gain_assign				= 16666;
	ffdm_pro_cb.sin_gain_post_en        = 0;
	ffdm_pro_cb.sin_gain_post_len		= 128;
	ffdm_pro_cb.sin_gain_post_len_f		= 256;
	ffdm_pro_cb.wind_len_used			= 256;
	ffdm_pro_cb.Gcoh_lim                = 10000;
	ffdm_pro_cb.wind_state_lim			= 25000;
	ffdm_pro_cb.wind_chg_thr            = 10000; //多大风开始进融合
	ffdm_pro_cb.wind_alluse_thr			= 180000; //多大风开始全部用风小的mic，不再融合
	ffdm_pro_cb.mix_fre_len				= 128;//range:3-196
	ffdm_pro_cb.wind_vaild_len			= 43;
	ffdm_pro_cb.wind_vaild_end			= 313;
    ffdm_pro_cb.wind_or_en				= 0;
	ffdm_pro_cb.low_windstate_len		= 40;
	ffdm_pro_cb.low_windstate_gain      = 1024;
	ffdm_pro_cb.alluse_en				= 1;
	ffdm_pro_cb.phase_alluse_en			= 0;
	ffdm_pro_cb.wind_sm_factor			= 0;
    bt_ffdm_pro_init(&ffdm_pro_cb);

    if(xcfg_cb.micr_phase_en == 1){
        cfg_micr_phase_en = 1;
    }

    *sysclk = *sysclk < SYS_160M ? SYS_160M : *sysclk;
}
#endif

void bt_sco_near_nr_dft_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->nr_type     = NR_TYPE_NONE;
    *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;

#if BT_SCO_NR_EN
    nr->nr_type     = NR_TYPE_AINS;
    memset(&ains_cb, 0, sizeof(ains_cb_t));
    ains_cb.nt              = BT_SCO_NR_LEVEL;
    ains_cb.prior_opt_idx   = 10;
    ains_cb.ns_ps_rate		= 1;
    ains_cb.trumpet_en		= 0;
	ains_cb.nt_post         = 0;    //0-6 >0才起效 0为不开gain指数化 开的话默认为3
	ains_cb.exp_range	    = 90;
	bt_ains_init(&ains_cb);
#endif
}

#if BT_SCO_SMIC_AI_EN
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dnn_en) {
        printf("DNN xcfg init warning!\n");
        return;
    }
    nr->nr_type = NR_TYPE_SMIC_AI;
    memset(&dnn_cb, 0, sizeof(dnn_cb_t));

	dnn_cb.param_printf           	= 0;
	dnn_cb.noise_ps_rate          	= 1;
	dnn_cb.prior_opt_idx	       	= 3;
	dnn_cb.prior_opt_ada_en	   		= 1;
	dnn_cb.low_fre_range          	= 16;
	dnn_cb.low_fre_range0         	= 0;
	dnn_cb.ps_lowlimt               = 0;
	dnn_cb.mask_floor			   	= 1000;
	dnn_cb.mask_floor_r		   		= 0;
	dnn_cb.music_lev			   	= 11;
	dnn_cb.gain_expand			   	= 1024;
	dnn_cb.nn_only				   	= 0;
	dnn_cb.nn_only_len			   	= 16;
	dnn_cb.gain_assign			   	= 16666;
	dnn_cb.sin_gain_post_en	   		= 0;
	dnn_cb.sin_gain_post_len	   	= 128;
	dnn_cb.sin_gain_post_len_f	   	= 256;
	dnn_cb.sp_thres				    = 8000;

	bt_dnn_init(&dnn_cb);
    *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
}
#endif


#if BT_SCO_SMIC_AI_PLUS_EN
void bt_sco_dnn_plus_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dnn_en) {
        printf("DNN xcfg init warning!\n");
        return;
    }
    nr->nr_type = NR_TYPE_SMIC_AI;
    memset(&dnn_plus_cb, 0, sizeof(dnn_plus_cb_t));

	dnn_plus_cb.param_printf           	= 0;
//	dnn_plus_cb.nt                     	= BT_SCO_SMIC_AI_PLUS_LEVEL;
//	dnn_plus_cb.nt_post                	= 0;
//	dnn_plus_cb.exp_range_H			   	= 1;
//	dnn_plus_cb.exp_range_L			   	= 0;
	dnn_plus_cb.noise_ps_rate          	= 1;
	dnn_plus_cb.prior_opt_idx	       	= 3;
	dnn_plus_cb.prior_opt_ada_en	   	= 1;
//	dnn_plus_cb.wind_level			   	= 0;
//	dnn_plus_cb.wind_range			   	= 0;
	dnn_plus_cb.low_fre_range          	= 11;
//	dnn_plus_cb.low_fre_range0         	= 0;
	dnn_plus_cb.mask_floor			   	= 1000;
//	dnn_plus_cb.mask_floor_r		   	= 0;
	dnn_plus_cb.music_lev			   	= 11;
//	dnn_plus_cb.comforN_level		   	= 1;
//	dnn_plus_cb.gain_expand			   	= 1024;
	dnn_plus_cb.nn_only_len				= 0;
	dnn_plus_cb.nn_only_len_no0			= 0;
	dnn_plus_cb.gain_min_len		    = 256;
	dnn_plus_cb.gain_assign			   	= 32767;
	dnn_plus_cb.sin_gain_post_en	   	= 0;
	dnn_plus_cb.sin_gain_post_len	   	= 128;
	dnn_plus_cb.sin_gain_post_len_f	   	= 256;
	dnn_plus_cb.spp_adaptive_thr	    = 10000;

	bt_dnn_plus_init(&dnn_plus_cb);
    *sysclk = *sysclk < SYS_60M ? SYS_60M : *sysclk;
}
#endif

#if BT_SCO_SMIC_AI_PRO_EN
void bt_sco_dnn_pro_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dnn_en) {
        printf("DNN_PRO xcfg init warning!\n");
        return;
    }
    nr->nr_type = NR_TYPE_SMIC_PRO_AI;
    memset(&dnn_pro_cb, 0, sizeof(dnn_pro_cb_t));

	dnn_pro_cb.param_printf           	= 0;
	dnn_pro_cb.gain_floor          	    = 300;
//	dnn_pro_cb.nt_post                	= 0;
//	dnn_pro_cb.exp_range_H			   	= 1;
//	dnn_pro_cb.exp_range_L			   	= 0;
	dnn_pro_cb.noise_ps_rate          	= 1;
	dnn_pro_cb.prior_opt_idx	       	= 10;
	dnn_pro_cb.prior_opt_ada_en	   		= 1;
//	dnn_pro_cb.wind_level			   	= 0;
//	dnn_pro_cb.wind_range			   	= 0;
	dnn_pro_cb.low_fre_range          	= 15;
	dnn_pro_cb.music_lev			   	= 11;
	dnn_pro_cb.nn_only				   	= 0;
	dnn_pro_cb.nn_only_len			   	= 16;
	dnn_pro_cb.gain_assign			   	= 16666;
	dnn_pro_cb.sin_gain_post_en	   		= 0;
	dnn_pro_cb.sin_gain_post_len	   	= 128;
	dnn_pro_cb.sin_gain_post_len_f	   	= 256;

	bt_dnn_pro_init(&dnn_pro_cb);
    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
}
#endif

#if BT_SCO_AIAEC_DNN_EN
void bt_sco_aiaec_dnn_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dnn_en) {
        printf("DNN xcfg init warning!\n");
        return;
    }
    nr->nr_type = NR_TYPE_SMIC_AIAEC;
    memset(&aiaec_dnn_cb, 0, sizeof(dnn_aec_ns_cb_t));

	aiaec_dnn_cb.param_printf           	= 0;
	aiaec_dnn_cb.nt                     	= BT_SCO_AIAEC_DNN_LEVEL;
//	aiaec_dnn_cb.exp_range_H			   	= 1;
//	aiaec_dnn_cb.exp_range_L			   	= 0;
	aiaec_dnn_cb.noise_ps_rate          	= 1;
	aiaec_dnn_cb.prior_opt_idx	       	    = 10;
	aiaec_dnn_cb.prior_opt_ada_en           = 1;
	aiaec_dnn_cb.wind_level			   	    = 0;
	aiaec_dnn_cb.wind_range			   	    = 0;
	aiaec_dnn_cb.low_fre_range          	= 15;
	aiaec_dnn_cb.mask_floor			   	    = 600;
//	aiaec_dnn_cb.mask_floor_r		   		= 0;
	aiaec_dnn_cb.music_lev			   	    = 11;
//	aiaec_dnn_cb.gain_expand			   	= 1024;
	aiaec_dnn_cb.nn_only				   	= 0;
	aiaec_dnn_cb.nn_only_len			   	= 16;
	aiaec_dnn_cb.gain_assign                = 16666;
	aiaec_dnn_cb.sin_gain_post_en           = 0;
	aiaec_dnn_cb.sin_gain_post_len          = 128;
	aiaec_dnn_cb.sin_gain_post_len_f        = 0;
    aiaec_dnn_cb.gamma                      = 27852;
	aiaec_dnn_cb.nlp_en                     = 1;
	aiaec_dnn_cb.nlp_level                  = 3;
    aiaec_dnn_cb.smooth_en				    = 1;
    aiaec_dnn_cb.far_post_thr				= 1024;         //Q8
    aiaec_dnn_cb.dtd_post_thr				= 80000000;     //Q15
    aiaec_dnn_cb.echo_gain_floor			= 100;
    aiaec_dnn_cb.dtd_smooth				    = 29491;
	aiaec_dnn_cb.single_floor				= 0;
	aiaec_dnn_cb.gain_assign_nlp		    = 16000;
	aiaec_dnn_cb.gain_st_thr		        = 12000;
	aiaec_dnn_cb.intensity					= 1;
	aiaec_dnn_cb.aec_gain_db				= 6;            //AEC增益 0-30（放大0到30db）
	aiaec_dnn_cb.nlms_en					= 0;
	aiaec_dnn_cb.nlms_len					= 0;
	nr->aiaec_pinknoise_en                  = BT_SCO_AIAEC_PINK_NOISE_EN;

    bt_aiaec_dnn_init(&aiaec_dnn_cb);
    *sysclk = *sysclk < SYS_60M ? SYS_60M : *sysclk;
}
#endif

#if BT_SCO_DMIC_AIAEC_EN
void bt_sco_aiaec_dmdnn_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dmns_en) {
        printf("DMDNN xcfg init warning!\n");
    }
    nr->nr_type = NR_TYPE_DMIC_AIAEC;
    memset(&aiaec_dmdnn_cb, 0, sizeof(dmdnn_aiaec_cb_t));

    //辅助参数
	aiaec_dmdnn_cb.param_printf            = 0;
	aiaec_dmdnn_cb.ai_postF_en             = 1;
	aiaec_dmdnn_cb.show_out                = 0;

	//基本参数
	aiaec_dmdnn_cb.distance                 = xcfg_cb.bt_dmns_distance;
	aiaec_dmdnn_cb.nt                       = xcfg_cb.bt_dmns_level;
	// 风噪参数
	aiaec_dmdnn_cb.wind_sig_choose		    = 1;     //0时代表副mic受风小  1代表主mic受风小
	aiaec_dmdnn_cb.wind_len_used            = 66;    //风噪抑制以及判断使用范围66
	aiaec_dmdnn_cb.wind_sup_open            = 1;
	aiaec_dmdnn_cb.wind_state_lim           = 25000;
	aiaec_dmdnn_cb.wind_or_en			    = 1;
	aiaec_dmdnn_cb.wind_sup_floor           = 23000;
	aiaec_dmdnn_cb.Gcoh_thr			        = 13666;
	aiaec_dmdnn_cb.Gcoh_sum_thr		        = 9666;
	aiaec_dmdnn_cb.wind_pick_mode           = 0;

	//线性相位差参数
	aiaec_dmdnn_cb.linear_en                = 1;
	aiaec_dmdnn_cb.linear_len               = 100;
	aiaec_dmdnn_cb.linear_level             = 3;
	aiaec_dmdnn_cb.white_detect             = 32767;
	aiaec_dmdnn_cb.white_nomal_scale        = 4;
	aiaec_dmdnn_cb.highFcanc_256_to         = 130;

	// baseline参数
	aiaec_dmdnn_cb.comp_mic_fre_L           = 0;
	aiaec_dmdnn_cb.comp_mic_fre_H           = 256;
	aiaec_dmdnn_cb.opt_limit                = 32767;
	aiaec_dmdnn_cb.bf_choose			    = 1;

	//RTF参数
	aiaec_dmdnn_cb.tf_en				    = 1;
	aiaec_dmdnn_cb.tf_len				    = 256; //smart: range:1-140   zoom:1:256
	aiaec_dmdnn_cb.tf_norm_en			    = 1;
	aiaec_dmdnn_cb.tf_norm_only_en		    = 0;
	aiaec_dmdnn_cb.white_approach_shape     = 1;
	aiaec_dmdnn_cb.v_white_en			    = 1;
	aiaec_dmdnn_cb.tf_SMrate                = 30000;
	aiaec_dmdnn_cb.rtf_post_thres           = 2048;
	aiaec_dmdnn_cb.white_type_len		    = 0;
	aiaec_dmdnn_cb.sup180rtf                = 4;

	//相位贴合
	aiaec_dmdnn_cb.phase_to                 = 0;

	// 波束增强模块选择
	aiaec_dmdnn_cb.bfrein_en			    = 0;
	aiaec_dmdnn_cb.b_frein2_floor		    = 19666;
	aiaec_dmdnn_cb.bfrein3_en		        = 1;
	aiaec_dmdnn_cb.bfrein3_ns		        = 2;
	aiaec_dmdnn_cb.bfrein3_intensity        = 1024;
	aiaec_dmdnn_cb.sup180                   = 4;
	aiaec_dmdnn_cb.grad_shift               = 0;
	aiaec_dmdnn_cb.bf2_used_len             = 0;

	// AI参数
	aiaec_dmdnn_cb.prior_opt_ada_en	        = 1;
	aiaec_dmdnn_cb.noise_ps_rate            = 1;
	aiaec_dmdnn_cb.music_lev			    = 11;
	aiaec_dmdnn_cb.gain_expand			    = 1024;
	aiaec_dmdnn_cb.nn_only				    = 0;
	aiaec_dmdnn_cb.nn_only_len			    = 16;
	aiaec_dmdnn_cb.hi_fre_en 			    = 1;
	aiaec_dmdnn_cb.gain_assign			    = 26666;
	aiaec_dmdnn_cb.sin_gain_post_en	        = 0;
	aiaec_dmdnn_cb.sin_gain_post_len	    = 128;
	aiaec_dmdnn_cb.sin_gain_post_len_f	    = 256;
	aiaec_dmdnn_cb.enlarge_v			    = 1;//1 or 2 or 3
	aiaec_dmdnn_cb.mask_floor			    = 1000;
	aiaec_dmdnn_cb.mask_floor_hi		    = 160;
	aiaec_dmdnn_cb.low_fre_range            = 16; //
	aiaec_dmdnn_cb.low_fre_range0           = 0;
	aiaec_dmdnn_cb.low_fre_ns0_range	    = 16;//range:1-200
	aiaec_dmdnn_cb.prior_opt_idx            = 3;

	aiaec_dmdnn_cb.sp_thres				    = 6000;
	aiaec_dmdnn_cb.mic1_gain_idx            = 10;
	aiaec_dmdnn_cb.mic2_gain_idx            = 10;
	aiaec_dmdnn_cb.rtf_post_thres			= 2048;
	aiaec_dmdnn_cb.nlp_level                = 3;
	aiaec_dmdnn_cb.gamma                    = 27852;
    aiaec_dmdnn_cb.nlp_en                   = 1;
	aiaec_dmdnn_cb.far_post_thr		        = 1024;//Q8
	aiaec_dmdnn_cb.dtd_post_thr		        = 80000000;//Q15
	aiaec_dmdnn_cb.echo_gain_floor			= 100;
	aiaec_dmdnn_cb.dtd_smooth			    = 29491;
	aiaec_dmdnn_cb.single_floor		        = 0;
	aiaec_dmdnn_cb.gain_assign_nlp			= 16000;
	aiaec_dmdnn_cb.gain_st_thr				= 12000;
	aiaec_dmdnn_cb.aec_gain_db				= 0;
	aiaec_dmdnn_cb.refer_dfw				= 0;
	aiaec_dmdnn_cb.nlms_en					= 0;
	aiaec_dmdnn_cb.nlms_len				    = 0;
	aiaec_dmdnn_cb.smooth_en				= 1;
	nr->aiaec_pinknoise_en                  = BT_SCO_AIAEC_PINK_NOISE_EN;

    bt_aiaec_dmdnn_init(&aiaec_dmdnn_cb);
    *sysclk = *sysclk < SYS_100M ? SYS_100M : *sysclk;
}
#endif

#if BT_SCO_NR_USER_SMIC_EN || BT_SCO_NR_USER_DMIC_EN
AT(.bt_voice.sco_nr.user)
void bt_nr_user_proc_do(s16 *mic1, s16 *mic2, nr_cb_t *nr_cb)
{
    //define your nr proc func here, and define nr_input by yourself

    //if you need aec ref signal, you can refer to ↓↓↓
//    if (!bt_sco_far_cache_read((u8*)&nr_input[nr_cb->nr_samples*2], nr_cb->nr_samples*2)) {
//        memset(&nr_input[nr_samples*2], 0, nr_samples * 2);
//    }
}

AT(.bt_voice.sco_nr.user)
void bt_nr_user_init(void *alg_cb, nr_cb_t *nr_cb)
{
    //define your nr init func here
    printf("nr user\n");
}

void bt_sco_nr_user_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->nr_type = NR_TYPE_USER;

    *sysclk = *sysclk < SYS_60M ? SYS_60M : *sysclk;
}
#endif
