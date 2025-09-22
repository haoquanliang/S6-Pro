#ifndef _API_NR_H
#define _API_NR_H

#define _MAX_GAIN                           (0x800000)
#define SOFT_DIG_P0DB                       (_MAX_GAIN * 1.000000)
#define SOFT_DIG_P1DB                     	(_MAX_GAIN * 1.122018)
#define SOFT_DIG_P2DB                     	(_MAX_GAIN * 1.258925)
#define SOFT_DIG_P3DB                     	(_MAX_GAIN * 1.412538)
#define SOFT_DIG_P4DB                     	(_MAX_GAIN * 1.584893)
#define SOFT_DIG_P5DB                     	(_MAX_GAIN * 1.778279)
#define SOFT_DIG_P6DB                     	(_MAX_GAIN * 1.995262)
#define SOFT_DIG_P7DB                     	(_MAX_GAIN * 2.238721)
#define SOFT_DIG_P8DB                     	(_MAX_GAIN * 2.511886)
#define SOFT_DIG_P9DB                     	(_MAX_GAIN * 2.818383)
#define SOFT_DIG_P10DB                    	(_MAX_GAIN * 3.162278)
#define SOFT_DIG_P11DB                    	(_MAX_GAIN * 3.548134)
#define SOFT_DIG_P12DB                    	(_MAX_GAIN * 3.981072)
#define SOFT_DIG_P13DB                    	(_MAX_GAIN * 4.466836)
#define SOFT_DIG_P14DB                    	(_MAX_GAIN * 5.011872)
#define SOFT_DIG_P15DB                    	(_MAX_GAIN * 5.623413)
#define SOFT_DIG_P16DB                      (_MAX_GAIN * 6.309573)
#define SOFT_DIG_P17DB                      (_MAX_GAIN * 7.079458)
#define SOFT_DIG_P18DB                      (_MAX_GAIN * 7.943282)
#define SOFT_DIG_P19DB                      (_MAX_GAIN * 8.912509)
#define SOFT_DIG_P20DB                      (_MAX_GAIN * 10.000000)
#define SOFT_DIG_P21DB                      (_MAX_GAIN * 11.220185)
#define SOFT_DIG_P22DB                      (_MAX_GAIN * 12.589254)
#define SOFT_DIG_P23DB                      (_MAX_GAIN * 14.125375)
#define SOFT_DIG_P24DB                      (_MAX_GAIN * 15.848932)
#define SOFT_DIG_P25DB                      (_MAX_GAIN * 17.782794)
#define SOFT_DIG_P26DB                      (_MAX_GAIN * 19.952623)
#define SOFT_DIG_P27DB                      (_MAX_GAIN * 22.387211)
#define SOFT_DIG_P28DB                      (_MAX_GAIN * 25.118864)
#define SOFT_DIG_P29DB                      (_MAX_GAIN * 28.183829)
#define SOFT_DIG_P30DB                      (_MAX_GAIN * 31.622777)
#define SOFT_DIG_P31DB                      (_MAX_GAIN * 35.481339)

enum {
    DUMP_MIC_TALK               = 0,    //主MIC数据
    DUMP_MIC_FF,                        //副MIC数据
    DUMP_MIC_NR,                        //降噪后数据
    DUMP_AEC_INPUT,                     //AEC输入数据
    DUMP_AEC_FAR,                       //AEC远端数据
    DUMP_AEC_OUTPUT,                    //AEC输出数据
    DUMP_FAR_NR_INPUT,                  //远端降噪输入数据
    DUMP_FAR_NR_OUTPUT,                 //远端降噪输出数据
    DUMP_EQ_OUTPUT,                     //MIC EQ输出数据
    DUMP_SCO_MAX_NB,
};

#define DUMP_SNDP_MASK                      0x000f
#define DUMP_AEC_MASK                       0x00f0

#define NR_CFG_TYPE_MASK                    0x1F

//call param
enum NR_TYPE {
    NR_TYPE_NONE            = 0,
    NR_TYPE_AINS,
    NR_TYPE_SNDP_SM,
    NR_TYPE_SNDP_DM,
    NR_TYPE_SNDP_FBDM,
    NR_TYPE_SMIC_AI,                            //自研单MIC AI降噪算法
    NR_TYPE_SMIC_PRO_AI,                        //自研单MIC 大模型AI降噪算法
    NR_TYPE_SMIC_AIAEC,                         //自研单MIC + AIAEC降噪算法
    NR_TYPE_DMIC_AIAEC,                         //自研双MIC + AIAEC降噪算法
    NR_TYPE_DMIC_AI,                            //NR_TYPE_DMDNN
    NR_TYPE_USER,
};

enum AEC_TYPE {
    AEC_TYPE_NONE           = 0,
    AEC_TYPE_DEFAULT,                           //默认硬件频域AEC
};

enum AEC_NR_MODE {
    AEC_NR_MODE0            = 0,                //AEC(线性+非线性) + NR
    AEC_NR_MODE1,                               //AEC(线性) + NR + AEC(非线性)
    AEC_NR_MODE2,                               //NR + AEC(线性) + AEC(非线性)
};

typedef struct {
    u8 level;                                   //稳态最大降噪量，范围0~29dB
    u8 dnn_level;                               //dnn最大降噪量，范围0~29dB
    u8 param_printf;

    const u32 *coef;                            //声加配置资源，非0：使用声加工具配置（从资源读取）
} sndp_sm_cb_t;

typedef struct {
    u8 distance;
    //小于保护角的语音被保留，大于抑制角的语音被抑制，中间平滑过渡
    u8 degree;                                  //语音保护角，范围0~90度
    u8 degree1;                                 //语音抑制角 = degree + degree1，范围0~90度

    u8 level;                                   //稳态最大降噪量，范围0~29dB
    u8 dnn_level;                               //dnn最大降噪量，范围0~29dB
    u8 param_printf;
    int bf_upper;                               //固定波束频率上限, 默认128
    const u32 *coef;                            //声加配置资源，非0：使用声加工具配置（从资源读取）
} sndp_dm_cb_t;

typedef struct {
    u8 level;                                   //稳态最大降噪量，范围0~29dB
    u8 dnn_level;                               //dnn最大降噪量，范围0~29dB
    u8 param_printf;
    u8 noiserms_th0;                            //混音阈值，范围0~100

    const u32 *coef;                            //声加配置资源，非0：使用声加工具配置（从资源读取）
} sndp_fbdm_cb_t;

enum NR_CFG_EN {
    NR_CFG_FAR_EN           = BIT(0),           //使能远端降噪
    NR_CFG_NEAR_AINS2_EN    = BIT(1),           //使能声加单麦前AINS2
    NR_CFG_TRUMPET_EN       = BIT(2),           //使能汽车喇叭声降噪
    NR_CFG_SCO_FADE_EN      = BIT(3),           //使能通话前500ms淡入
    NR_CFG_DAC_DNR_EN       = BIT(4),           //使能通话下行————动态降噪
    NR_CFG_DAC_DRC_EN       = BIT(5),           //使能通话下行————DRC
};

typedef struct {
    u32 nt;
    u8  prior_opt_idx;
	u8  ns_ps_rate;
	u8  trumpet_en;
	u8  nt_post;
	u8  exp_range;
} ains_cb_t;

typedef struct {
    u8 distance;                                //双mic间距：mm（步进1mm）
    u8 degree;                                  //语音角度（小于90度）:10+5*N (5度步进，N<17)
    u8 max_degree;                              //最大保护角度（小于90度）:10+5*N (5度步进，N<17)
    u8 level;                                   //降噪强度：2*N（2dB步进，N<30）
    u8 wind_level;
    u8 param_printf;                            //使能参数打印
    u8 rfu[1];
    const int *filter_coef;                     //int coef[512]
    int maxIR;                                  //Q18
    int maxIR2;                                 //Q18
    u16 windnoise_conditioned_eq_x0;            //风噪抑制起始频段(Hz)
    u16 windnoise_conditioned_eq_x1;            //风噪抑制终止频段(Hz)
    int windnoise_conditioned_eq_y1;            //风噪抑制的终止大小(db), 最小为0db
    u8 snr_x0_level;                            //如果在一般信噪比下，降噪不足，就适当调大（0~0.4 步进0.05），默认0.1
    u8 snr_x1_level;                            //如果低信噪比下，语音断断续续，就适当调小（0.5~1 步进0.05），默认0.8
} dmic_cb_t;

typedef struct {
	//u16 nt;
	//s16 exp_range_H;
	//s16 exp_range_L;
	//u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	u8  wind_thr;
	u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印
   // u8  wind_level;
	//u16 wind_range;
	u16 low_fre_range;
	u16 low_fre_range0;
	u8  pitch_filter_en;
	u16 mask_floor;
	u16 noise_ceil;
	u16 ps_lowlimt;
	u8  mask_floor_r;
	u8  music_lev;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 sp_thres;
	//u16 pitch_filter_range;
} dnn_cb_t;

typedef struct {
	u16 nt;
	//s16 exp_range_H;
	//s16 exp_range_L;
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	u8  wind_thr;
	u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印
    //u8  wind_level;
	//u16 wind_range;
	u16 low_fre_range;
	//u16 low_fre_range0;
	//u8  pitch_filter_en;
	u16 mask_floor;
	//u8  mask_floor_r;
	u8  music_lev;
	//u16 gain_expand;
	//u16 spp_len;
	u16 nn_only_len;
	u16 nn_only_len_no0;
	u16 gain_assign;
	u16 gain_min_len;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 spp_adaptive_thr;
	u8  enr_mean_max_en;
	s32 enr_nr_thr;
} dnn_plus_cb_t;

typedef struct {
	u16 gain_floor;
	//u8  nt_post;
	//s16 exp_range_H;
	//s16 exp_range_L;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
	//u8  wind_level;
	//u16 wind_range;
	u16 low_fre_range;
	u8  param_printf;                           //使能参数打印
	u8  music_lev;
	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
} dnn_pro_cb_t;

typedef struct {
	s16	gamma;
	u8  nlp_en;
	u8  nlp_level;

	s16 nt;
	u8 param_printf;
	u8  music_lev;
	//s16 exp_range_H;
	//s16 exp_range_L;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
	u8  wind_level;
	u16 wind_range;
	u16 mask_floor;
	u16 low_fre_range;
	u8  nn_only;
	u16 nn_only_len;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	u8  smooth_en;
	u16 far_post_thr;
	u32 dtd_post_thr;
	u16 gain_assign;
	u16 echo_gain_floor;
    s16 dtd_smooth;
	s16 single_floor;
	s16 gain_assign_nlp;
	u16 gain_st_thr;
	s32 intensity;
	s16 aec_gain_db;
	u8  nlms_en;
	u16 nlms_len;
} dnn_aec_ns_cb_t;

typedef struct {
	u8  param_printf;                           //使能参数打印
	u8  mic_cfg;                                //主副麦选择
	u8  bf_type;                                //beamforming类型
	s16 distance;				                //双麦间距
	s16 nt;						                //普通降噪量
	u8  nt_post;						        //后滤波降噪量
	s16 exp_range;
	//u8  fast_convergence_en;                    //快速收敛功能，对人声有影响，默认不开
	//u16 lowside_corr;                           //下支路相关系数

	//	u8  trumpet_en;                             //喇叭声抑制
	u8  cmp_tbl_dis;
	u8  dm_dnn_en;                              //是否使能双麦AI算法
	u8  noise_ps_rate;
	u8  comp_mic_fre;                           //双麦+AI的参数
	u16 comp_mic_fre_L;                         //传统双麦的参数
	u16 comp_mic_fre_H;                         //传统双麦的参数
	u8  prior_opt_idx;
	u16 low_fre_ns0_range;
	u8  wind_sig_choose;
	u8  bf_choose;
	u8  tf_en;
	u16 tf_len;
	u8  tf_norm_en;
	u8  tf_norm_only_en;
	s32 opt_limit;
	u8	music_lev;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	u8  hi_fre_en;
	u8  prior_opt_ada_en;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 b_frein2_floor;
	u16 Gcoh_thr;
	u16 Gcoh_sum_thr;
	u8  wind_or_en;
	u8  wind_pick_mode;
	u8  v_white_en;
	//u8  wind_type;
	s16 sin_gain_post_len_f;
	s16 sin_gain_post_len;
	u8  enlarge_v;
	s16 mask_floor;
	u16 mask_floor_hi;
	u16 low_fre_range;
	u16 low_fre_range0;
	s16 wind_state_lim;
	s16 wind_sup_floor;
	u8  wind_sup_open;
	u8  bfrein_en;
	u8  bf2_used_len;
	//u8  bfrein2_len;
	//u16 bf2_gain_sm;
	//u16 bf2_frameG_floor;
	u8  bfrein3_ns;
	u16 bfrein3_intensity;
	u16 wind_len_used;
	u8  bfrein3_en;
	u8  ai_postF_en;
	u8  phase_to;
	u8  show_out;
	u8  white_approach_shape;
	u8  linear_len;
	u8  linear_level;
	s32 white_detect;
	u8  white_nomal_scale;
	u8  highFcanc_256_to;
	s16 tf_SMrate;
	u8  sup180;
	u16 sup180rtf;
	u8  white_type_len;
	s16 sp_thres;
	u8  mic1_gain_idx;
	u8  mic2_gain_idx;
	s16 rtf_post_thres;
	u8  smooth_en;
	u8  linear_en;
	s32 grad_shift;

	s16	gamma;
	u8  nlp_en;
	u8  nlp_level;

	u16 far_post_thr;
	u32 dtd_post_thr;
	u16 echo_gain_floor;
	s16 dtd_smooth;
	s16 single_floor;
	s16 gain_assign_nlp;
	u16 gain_st_thr;
	s16 aec_gain_db;
	u8  nlms_en;
	u16 nlms_len;
	u8  refer_dfw;
	u8  nlp_choose;
} dmdnn_aiaec_cb_t;

typedef struct {
    u8  param_printf;                           //使能参数打印
	u16 min_value;
	u16 nostation_floor;
	u8  wind_thr;
	u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
	s16 distance;
	u16 low_fre_range;
	u16 low_fre_range0;
	u8  pitch_filter_en;
	u16 mask_floor;
	u16 noise_ceil;
	u16 ps_lowlimt;
	u8  mask_floor_r;
	u8  music_lev;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 sp_thres;

	s16 wind_len_used;
	s16 Gcoh_lim;
	s16 wind_state_lim;
	s32 wind_chg_thr;
	u8  alluse_en;
	u8  phase_alluse_en;
	s16 mix_fre_len;
	s32 wind_vaild_len;
	s32 wind_vaild_end;
	s32 wind_alluse_thr;
	u8  wind_or_en;
	u8  low_windstate_len;
	s16 low_windstate_gain;
	s32 wind_sm_factor;
} ffdm_cb_t;

typedef struct {
	s16  distance;
	u16 gain_floor;
	//u8  nt_post;
	//s16 exp_range_H;
	//s16 exp_range_L;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
	//u8  wind_level;
	//u16 wind_range;
	u16 low_fre_range;
	u8  param_printf;                           //使能参数打印
	u8  music_lev;
	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 wind_len_used;
	s16 Gcoh_lim;
	s16 wind_state_lim;
	s32 wind_chg_thr;
	s32 wind_alluse_thr;
	s16 mix_fre_len;
	s32 wind_vaild_len;
	s32 wind_vaild_end;
	u8  alluse_en;
	u8  phase_alluse_en;
	u8  wind_or_en;
	u8  low_windstate_len;
	s16 low_windstate_gain;
	s32 wind_sm_factor;
} ffdm_pro_cb_t;

typedef struct {
	u8  param_printf;                           //使能参数打印
	u8  mic_cfg;                                //主副麦选择
	u8  bf_type;                                //beamforming类型
	s16 distance;				                //双麦间距
	s16 nt;						                //普通降噪量
	u8  nt_post;						        //后滤波降噪量
	s16 exp_range;
	//u8  fast_convergence_en;                    //快速收敛功能，对人声有影响，默认不开
	//u16 lowside_corr;                           //下支路相关系数

	//	u8  trumpet_en;                             //喇叭声抑制
	u8  cmp_tbl_dis;
	u8  dm_dnn_en;                              //是否使能双麦AI算法
	u8  noise_ps_rate;
	u8  comp_mic_fre;                           //双麦+AI的参数
	u16 comp_mic_fre_L;                         //传统双麦的参数
	u16 comp_mic_fre_H;                         //传统双麦的参数
	u8  prior_opt_idx;
	u16 low_fre_ns0_range;
	u8  wind_sig_choose;
	u8  bf_choose;
	u8  tf_en;
	u16 tf_len;
	u8  tf_norm_en;
	u8  tf_norm_only_en;
	s32 opt_limit;
	u8	music_lev;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	u8  hi_fre_en;
	u8  prior_opt_ada_en;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 b_frein2_floor;
	u16 Gcoh_thr;
	u16 Gcoh_sum_thr;
	u8  wind_or_en;
	u8  wind_pick_mode;
	s16 after_tf_norm;
	u8  v_white_en;
	//u8  wind_type;
	s16 sin_gain_post_len_f;
	s16 sin_gain_post_len;
	u8  enlarge_v;
	s16 mask_floor;
	u16 mask_floor_hi;
	u16 low_fre_range;
	u16 low_fre_range0;
	s16 wind_state_lim;
	s16 wind_sup_floor;
	u8  wind_sup_open;
	u8  bfrein_en;
	u8  bf2_used_len;
	//u8  bfrein2_len;
	//u16 bf2_gain_sm;
	//u16 bf2_frameG_floor;
	u8  bfrein3_ns;
	u16 bfrein3_intensity;
	u16 wind_len_used;
	u8  bfrein3_en;
	u8  ai_postF_en;
	u8  phase_to_w;
	u8  phase_to_nw;
	u8  show_out;
	u8  white_approach_shape;
	u8  linear_len;
	u8  linear_level;
	s32 white_detect;
	u8  white_nomal_scale;
	u8  highFcanc_256_to;
	s16 tf_SMrate;
	u8  sup180;
	u16 sup180rtf;
	u8  white_type_len;
	s16 sp_thres;
	u8  mic1_gain_idx;
	u8  mic2_gain_idx;
	s16 rtf_post_thres;
	u8  smooth_en;
	u8  linear_en;
	s32 grad_shift;
	s32 adp_smr;
	s32 all_smr;
	s32 atf_smr;
	s32 grad_mu;
	s16 high_tf_Llim;
	s32 high_tf_rate;
	s32 high_gpost_appd;
	u8  frame_G_opt;
	s16 gcoh_gain;
	u8  low_windstate_len;
	s16 low_windstate_gain;
	u8  only_postfilter;
	s32  linear_gain;
} dmns_cb_t;

typedef struct {
	u8  param_printf;                           //使能参数打印
	u8  mic_cfg;                                //主副麦选择
	s16 distance;				                //双麦间距
	s16 nt;						                //普通降噪量
	u8  nt_post;						        //后滤波降噪量
	s16 exp_range;

	u8  cmp_tbl_dis;
	u8  dm_dnn_en;                              //是否使能双麦AI算法
	u8  noise_ps_rate;
	u8  comp_mic_fre;                           //双麦+AI的参数
	u16 comp_mic_fre_L;                         //传统双麦的参数
	u16 comp_mic_fre_H;                         //传统双麦的参数
	u8  prior_opt_idx;
	u16 low_fre_ns0_range;
	u8  wind_sig_choose;
	u8  wind_sm_len;
	s16 wind_state_lim;
	u16 wind_assign;
	s16 wind_sup_floor;
	u8  wind_sup_open;
	u8  wind_a_pow_en;
	u8  bf_choose;
	u8  tf_en;

	u16 tf_len;
	u8  tf_norm_en;
	u8  tf_norm_only_en;
	s16 exp_range_H;
	s16 exp_range_L;
	u8	music_lev;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	u8  hi_fre_en;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 b_frein2_floor;
	u16 Gcoh_thr;
	u16 Gcoh_sum_thr;
	u8  wind_or_en;
	u8  protect_Agl;
	s16 sin_gain_post_len_f;
	u8  enlarge_v;
	s16 mask_floor;
    u16 mask_floor_hi;

	u8  lowF_change_en;
	u16 wind_len_used;
	u16 Ftrust;
	u8  AI_sig_ch_en;
	u16 frame_gain_f;
	u16 gain_used;
	u16 gain_sm;
	u8 huawu_en;
	u8 counter_en;
	u8 counter_lim;
	u16 counter_substep;
	u32 opt_limt;

	u8  single_mic_en;
	u8  single_mic_gain;
} ldmdnn_cb_t;

typedef struct {
    u8 mode;                                    //auto or manual mode
    u8 level;                                   // 降噪量
    u8 min_range1;                              // 噪声谱更新系数
    u8 min_range2;                              // 噪声谱更新系数
    u8 min_range3;                              // 噪声谱更新系数
    u8 min_range4;                              // 噪声谱更新系数
    u8 anksnr_en;                               // 是否使能ank snr
    u16 nera_val;                               // anksnr_en为0时有效，alp1固定值
    u32 block_cnt;
} peri_nr_cfg_t;

typedef struct {
    u8  agc_en;
    u16 sampleHzIn;
    u8  bit;
    u8  compress_agcDb;
    u8  target_agcDbfs;
    u8  low_signal_en;
	u8  max_gain;
} agc_cb_t;

typedef struct {
    u8 nr_type;                                 //近端降噪类型
    u8 nr_cfg_en;                               //远端降噪、喇叭声降噪、500ms淡入等降噪配置
    u32 dump_en;
    u8 rfu[2];
    u8 calling_voice_pow;                       //用于呼出电话，响铃之前动态降噪
    u8 calling_voice_cnt;
    u8 calling_voice_temp_cnt;
    void *far_nr;                               //远端降噪算法配置
    int dac_dnr_thr;
    agc_cb_t agc_cb;
    u8 aiaec_pinknoise_en;
} nr_cb_t;

typedef struct {
    u32 type            :4;
    u32 mode            :1;                     //auto or manual mode
    u32 nlp_bypass      :1;                     //aec nlp bypass
    u32 nlp_only        :1;                     //aec nlp only select
    u32 nlp_level       :4;
    u32 nlp_part        :2;
    u32 comforn_level   :4;
    u32 comforn_floor   :10;
    u32 comforn_en      :1;
    u32 xe_add_corr     :16;
    u32 upbin           :8;
    u32 lowbin          :8;
    u32 diverge_th      :4;
    u32 gamma           :16;
    u32 aggrfact        :16;
    u32 depre_mode      :1;
    u32 mic_ch          :1;
    u32 dig_gain        :24;
    u32 echo_th         ;
    u16 far_offset;
    u8  ff_mic_ref_en;
    u32 qidx;
} aec_cfg_t;

typedef struct {
    u8 alc_en;
    u8 rfu[1];
    u8 fade_in_step;
    u8 fade_out_step;
    u8 fade_in_delay;
    u8 fade_out_delay;
    s32 far_voice_thr;
} alc_cb_t;

typedef struct {
    aec_cfg_t aec;
    alc_cb_t alc;
    nr_cb_t nr;

    u8 rfu[3];
    u8 mic_eq_en        : 1;
    u8 mic_drc_en       : 1;
    int post_gain;
} call_cfg_t;

void bt_call_init(call_cfg_t *p);
void bt_call_exit(void);
bool bt_sco_dnn_en(void);

bool bt_sco_dac_drc_init(u8 *drc_addr, int drc_len);

bool bt_sco_far_cache_write(u8 *inbuf, u32 wlen);
bool bt_sco_far_cache_read(u8 *outbuf, u32 rlen);
void bt_sco_far_cache_init(void);

void bt_ldmdnn_init(void *alg_cb);
void bt_ldmdnn_exit(void);

void bt_dmns_init(void *alg_cb);
void bt_dmns_exit(void);

void bt_dnn_init(void *alg_cb);
void bt_dnn_exit(void);

void bt_dnn_plus_init(void *alg_cb);
void bt_dnn_plus_exit(void);

void bt_dnn_pro_init(void *alg_cb);
void bt_dnn_pro_exit(void);

void bt_aiaec_dnn_init(void *alg_cb);
void bt_aiaec_dnn_exit(void);

void bt_aiaec_dmdnn_init(void *alg_cb);
void bt_aiaec_dmdnn_exit(void);

void bt_ffdm_init(void *alg_cb);
void bt_ffdm_exit(void);

void bt_ffdm_pro_init(void *alg_cb);
void bt_ffdm_pro_exit(void);

void bt_sndp_sm_init(void *alg_cb);
void bt_sndp_sm_exit(void);

void bt_ains_init(void *alg_cb);
void bt_ains_exit(void);

void bt_sndp_fbdm_init(void *alg_cb);
void bt_sndp_fbdm_exit(void);

void bt_sndp_dm_init(void *alg_cb);
void bt_sndp_dm_exit(void);

void bt_agc_init(void);
void bt_sco_agc_proc(s16 *buf);

void trumpet_denoise_init(u8 level);
void trumpet_denoise(int *fft_in);

void nr_init(u16 nt);
void nr_process(s16 *data);    //data:64个点
u32 nr_gain_radio_convert(u8 dB);

void bt_peri_nr_init(void);
void bt_peri_nr_exit(void);
bool bt_peri_nr_wait_done(void);
void bt_sco_far_nr_int(void *cfg);
void bt_sco_far_peri_nr_process(s16 *ptr, u16 samples);
void peri_nr_isr(void);


void wnr_init(void);
void wnr_detect(s32 *p);
void wnr_process(s32 *re, s32 *im);

void aec_nlms_process(u8 mic_sel);
void aec_nlp_process(void);
void sco_aec_far_process(void);
void aec_init(aec_cfg_t *aec);
bool aec_isr(void);
void aec_exit(void);

void alc_init(void);
bool alc_isr(void);
void alc_exit(void);


#endif
