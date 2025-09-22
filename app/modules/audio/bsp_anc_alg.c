#include "include.h"


#if ANC_ALG_EN

#define TRACE_EN                    0
//MIC配置，需要根据实际修改
#if ANC_ALG_STEREO_EN
#define FF_MIC_SDADC_MAPPING        (0)
#define FB_MIC_SDADC_MAPPING        (1)
#define TALK_MIC_SDADC_MAPPING      (4)
#define FF_R_MIC_SDADC_MAPPING      (2)
#define FB_R_MIC_SDADC_MAPPING      (3)
#else
#define FF_MIC_SDADC_MAPPING        (2)
#define FB_MIC_SDADC_MAPPING        (3)
#define TALK_MIC_SDADC_MAPPING      (4)
#define FF_R_MIC_SDADC_MAPPING      (2)
#define FB_R_MIC_SDADC_MAPPING      (3)
#endif // ANC_ALG_STEREO_EN
#define MIC_NUM_MAX                 (5)

#define MIC_CFG_IDX_FF              0
#define MIC_CFG_IDX_FB              1
#define MIC_CFG_IDX_TALK            2
#define MIC_CFG_IDX_FF_R            3
#define MIC_CFG_IDX_FB_R            4

#define SDADC_GET_CH0(x)            (u8)(((x) >> 0) & 0xFF)
#define SDADC_GET_CH1(x)            (u8)(((x) >> 8) & 0xFF)
#define SDADC_GET_CH2(x)            (u8)(((x) >> 16) & 0xFF)

#if TRACE_EN
#define TRACE(...)                  printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif // TRACE_EN


enum {  //比 MIC_CFG_IDX 加一
    ANC_ALG_MIC_NONE,
    ANC_ALG_MIC_FF = 0x01,
    ANC_ALG_MIC_FB,
    ANC_ALG_MIC_TALK,
    ANC_ALG_MIC_FF_R,
    ANC_ALG_MIC_FB_R,
    ANC_ALG_MIC_MAX,
};

enum {
#if ANC_ALG_WIND_NOISE_FF_TALK_EN || ANC_ALG_AI_WN_FF_EN || ANC_ALG_WIND_NOISE_FF_FB_EN || ANC_ALG_AI_WN_DSP_FF_EN || ANC_ALG_AI_WN_MINI_FF_EN
    ANC_ALG_ADJUST_FF_GAIN_WINDNOISE,
#endif
#if ANC_ALG_HOWLING_FF_EN || ANC_ALG_HOWLING_FB_EN
    ANC_ALG_ADJUST_FF_GAIN_HOWLING,
#endif // ANC_ALG_HOWLING_FF_EN
#if ANC_ALG_LIMITER_FF_EN
    ANC_ALG_ADJUST_FF_GAIN_LIMITER,
#endif // ANC_ALG_LIMITER_FF_EN
#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN
    ANC_ALG_ADJUST_FF_GAIN_ASM,
#endif
#if ANC_ALG_AEM_RT_FF_FB_EN
    ANC_ALG_ADJUST_FF_GAIN_AEM_RT,
#endif // ANC_ALG_AEM_RT_FF_FB_EN
    ANC_ALG_ADJUST_FF_GAIN_MAX,
};

enum {
#if ANC_ALG_HOWLING_FB_EN
    ANC_ALG_ADJUST_FB_GAIN_HOWLING,
#endif // ANC_ALG_HOWLING_FB_EN
#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN
    ANC_ALG_ADJUST_FB_GAIN_ASM,
#endif
#if ANC_ALG_MSC_ADP_FB_EN
    ANC_ALG_ADJUST_FB_GAIN_MSC_ADP,
#endif // ANC_ALG_MSC_ADP_FB_EN
#if ANC_ALG_AEM_RT_FF_FB_EN
    ANC_ALG_ADJUST_FB_GAIN_AEM_RT,
#endif // ANC_ALG_AEM_RT_FF_FB_EN
    ANC_ALG_ADJUST_FB_GAIN_MAX,
};

//算法公共参数
typedef struct {
    u8    type;
    u8    mic_ch_cfg;
    u8    pcm_interleave;
    u16   alg_samples;
    u8    anc_dma;
    u16   sdadc_ch;
    u16   sdadc_samples;
    void* alg_param;
} anc_alg_comm_param_cb;

//自研双MIC(FF+TALK)传统特征风噪和能量检测算法参数
typedef struct {
    u8 db_en;
    u8 filt_use;
    u8 power_use_ff;
    s32 danger_thr;
    u8  distance;
    u8  degree;
	u8  wind_range;
	s32 corr_thr;
	u32 wind_enk1_thr;
	u32 wind_enk2_thr;
	u16 wind_Gcoh_sum_thr;
	s32 ene_th1;
	s32 ene_th2;
	u8  energy_en;
	u16 time_up;
	u16 time_down;
	u16 pen_frame;
	s32 wind_vaild_len;
	s32 wind_vaild_end;
} dewind_dm_cb_t;

//自研双MIC(FF+FB)传统特征风噪和能量检测算法参数
typedef struct {
	s32 ht_mode;
	s32 light_wind_detect;
	s32 corr_thr_h;
	s32 diff_thr_h;
	s32 eli_thr_l;
	s32 eli_thr_r;
	s32 diff_abs_thr;
	s32 diff_abs_thr_high;
	s32 wind_count_stay_thr;
	s32 wind_count_clean_thr;
	u8  energy_en;
	s32 diff_bad_thr;
	s32 slope_thr;
	s32 light_thr;
	u32 ene_th1;
	u32 ene_th2;
	u16 time_up;
	u16 time_down;
	u16 pen_frame;
	s32 bit_goal;
	s32 bit_goal_hfre;
	s32 corr_thr;
	s32 diff_thr;
	s32 diff_thr_low;
	s32 smooth_thr;
	s32 E_var_thr;
	s32 E_var_thr_2;
	s32 E_var_frame_thr;
	s32 E_var_frame_thr_2;
	u32 wind_enk1_thr_l;
	u32 wind_enk1_thr_h;
	u32 wind_enk2_thr_l;
	u32 wind_enk2_thr_m;
	u32 wind_enk2_thr_h;
	u32 wind_enk2_thr2;
	s32 bad_thr;
	s32 bad_thr_2;
	s32	pp_save[8];
	s32 sp_h[8 * 2];
	s32 sp_h_2[50 * 2];
	s32 wind_vaild_len;
	s32 wind_vaild_end;
	s32 spk_power_thr;
	s32 fb_power_thr;
} anc_wind_ff_fb_cb_t;

//自研单MIC(FF)环境自适应ANC算法参数
typedef struct {
	u8  db_en;
	s32 thresholds_1;
	s32 thresholds_2;
	s32 power_thr;
	s32 out_thr_1;
	s32 out_thr_2;
	s32 out_thr_3;
	s32 out_thr_4;
	s32 left_alp;
	s32 right_alp;
	u16 double_thr_en;
	u16 time_up;
	u16 time_down;
	s16 bit_tun;
	u16 pen_frame;
	s16 ff_gain_buffer[3];
	s16 fb_gain_buffer[3];
} asm_simple_cb_t;

//自研贴合度检测(FF+FB)ANC算法参数
typedef struct {
	s32 sp_min_thr;
	s32 sp_max_thr;
	s32 aem_fre_s_idx;
	s32 aem_fre_e_idx;
	s32 Iterations_ready_aem;
	s32 Iterations_ready_fd;
	s32 pp_h[16 * 2];
	s32 pp_gain[10];
	s32 thr_buffer[10];
	s32 q_tun;
	s32 w_thr;
} anc_fit_detect_cb_t;

//自研防啸叫(FB/FF)ANC算法
typedef struct {
    u16 frame_len;
    u8 mic_ancout_exchange; // speed = 3 检测单个分支时,哪个分支输出幅度大就用哪个检测，0是ancout，1是mic
	u8 gainback;            //回Gain速度
	u16 gainhold;           //保持帧数，0表示触发下Gain以后无需保持 OWS修改成1000
	u8 framestart;          //起始检测频点 = framestart*31.25
	u8 twsmodel;
	u8 speed;               //1、2、3检测速度依次递增
	s16 ptprthrmic;         //输入需要检测的起始阈值，可以用Audition查看音频并输入对应的512点fft对应幅度谱的dbfs,推荐范围 -20 ~ -10 过高漏检，过低误检
	s16 ptprthrancout;
	s8 gainlight;
	s8 gainmedium;
	s8 gaindeep;
	u8 wndetect;            //白噪检测开启，仅在OWS开启
	u8 numratio;            //白噪检测时域特征
	u8 paprnewpercent;      //paprnew 基波特征占比
	u8 paprnewharpercent;   //paprnew 基波+谐波特征占比
	u8 speechdetect;
} howling_cb_t;

//自研单MIC(FF)AI风噪检测算法
typedef struct {
    u8  aiwn_mode;
    u16 time_down;
    u16 time_up;
	u16 pen_frame;
	u8  enr_en;
	u32 ene_th1;
	u32 ene_th2;
	s32 wind_vaild_len;
	s32 wind_vaild_end;
} ai_wn_init_t;

//自研单MIC(FF)瞬态噪声检测算法
typedef struct {
	u16 enable;
	u8  gainback;
    u8  ratio2to0;
    u8  ratio2to1;
    u32 ancoutmaxvalue;
    u32 ancoutmeanvalue;
    u8  lowfreqratio;
    s32 flatness;
    s8  gain1;
    s8  gain2;
    s8  gain3;
} limiter_config_info_t;

//自研单MIC(FF)动态音量算法
typedef struct {
	s32 bit_tun;
	s32 init_thr;
	s16 enable;
	s32 low_thr;
	s32 high_thr;
	s32 max_adjust_vol;
	s32 min_adjust_vol;
	s32 alp;
	s32 up_thr;
	s32 down_thr;
	s32 delat_tun;
	s32 speed_up;
	s32 speed_down;
} Dyvol_cb_t;

//自研单MIC(FB)自适应音乐补偿算法
typedef struct {
	u8  q_tun;
	s32 w_thr;
	s32 fre_s_idx;
	s32 fre_e_idx;
	s32 fb_tun_max;
	s32 msc_tun_max;
	s32 fb_tun_min;
	s32 msc_tun_min;
	s32 tx_power_1_thr;
	s32 tx_power_2_thr;
	s32 power_max_thr;
	s32 leak_thr_2;
	s32 leak_thr_1;
	s32 fade_time;
	u16 restore_max;
	u16 stable_cnt_thr;
	u16 leak_safe_cnt_thr;
	u16 leak_danger_cnt_thr;
	u16 danger_hold_cnt;
} anc_msc_adp_cb_t;

//自研双MIC(FF+FB)自适应EQ算法
typedef struct {
	s32 pp_h[16 * 2];
	u8  q_tun;
	u16 cal_stay_thr;
	u16 cal_confirm_thr;
	u16 period;
	s32 leak_tun_min;
	s32 leak_tun_max;
	s32 msc_tun_min;
	s32 msc_tun_max;
	s32 leak_thr_2;
	s32 leak_thr_1;
	s32 leak_thr_2_msc;
	s32 leak_thr_1_msc;
	s32 w_thr;
	s32 tx_power_1_thr;
	s32 speed_up;
	s32 speed_down;
} adp_eq_cb_t;

//自研双MIC(FF+FB)半入耳耳道自适应算法
typedef struct {
	s32 alp;
	s32 open_thr_2;
	s32 gears_use;
	s32 cvt_en;
	s32 fix_idx;
	s32 power_diff_enable;
	s32 ft_thr_fb;
	s32 ft_thr_ff;
	s32 pp_idx_1;
	s32 pp_idx_2;
	s32 cvt_sp_step;
	s32 gear_gain_step_ff;
	s32 gear_gain_step_fb;
	s32 gear_gain_step_msc;
	s32 cvt_cnt_thr;
	s32 erp_cal_cnt_thr;
	s32 erp_cal_cnt_thr_2;
	s32 wear_detection_cnt_thr;
	s32 open_thr;
	s32 diff_thr;
	s32 sp_delta;
	s32 tx_thr;
	s32 var_thr;
	s32 self_time_max;
	s32 thr_buffer[20];
	s32 ff_gains_buffer[20 + 2];
	s32 pen_frame;
	s32 q_tun;
	s32 tolerate_thr;
	s32 aem_fre_s_idx;
	s32 aem_fre_e_idx;
	s32 aem_voice_e_idx;
	s32 Iterations_ready;
	s32 change_up_thr;
	s32 change_down_thr;
	s32 change_hold_len;
	s32 change_hold_len_2;
	s32 shift_cnt_thr;
	s32 diff_shift_thr;
	s32 w_thr;
	s32 silence_thr;
	s32 pp_h[16 * 2];
	s32 pp_h_2[16 * 2];
	s32 pp_h_3[16 * 2];
} anc_aem_rt_cb_t;

//声加泄露补偿自适应ANC算法(SAE_EL01)参数
typedef struct {
    int SHIELD_LEAKGEAR[7];
    int SHIELD_CORR_CORRECT;
    int SHIELD_ShieldStartFrm;
    int SHIELD_ShieldEndFrm;
    int SHIELD_ShieldOverFrm;
    int SHIELD_ShieldHalfDetFrm;
} g_Cfg_SpEc_ShieldLeakageDetect;

//声加泄露补偿自适应ANC算法(SAE_EL01)参数
typedef struct {
    int DataScaleff;
    int DataScalefb;
    int THR_FF;
    int THR_FB;
    int THR_Est0;
    int THR_Est1;
    int THR_Est2;
    int THR_Est3;
    int THR_Est4;
    int THR_Est5;
    short GainMax;
    short GainMin;
    short ModeMax;
    int LogSwitch; // LOG 切换，1 打开，0 关闭
} g_Cfg_SpEc_AdapANC;

typedef struct {
    int FFGain;
    int FBGain;
    int ATT_WindFFFB_FrmNum;
    int DEC_WindFFFB_FrmNum;
    int WindFFFB_TH;
    int noWindFFFB_TH;
    int Hcomp[4][34];
    int Hoff[4][34];
    int PWRDIFTHH[4];
} g_Cfg_SpEc_WindDetect;

void anc_alg_ram_clear(void);
int anc_alg_init(anc_alg_param_cb* p);
int anc_alg_exit(anc_alg_param_cb* p);
uint calc_crc(void *buf, uint len, uint seed);
unsigned int s_bcnt(unsigned int rs1);
void anc_alg_ancdma_process(u8* ptr, u32 samples, int ch_mode);
void anc_alg_sdadc_process(u8* ptr, u32 samples, int ch_mode);
void anc_alg_sdadc_process_3ch_talk(u8* ptr, u32 samples, int ch_mode);
void anc_alg_sdadc_process_fffb_r(u8* ptr, u32 samples, int ch_mode);
void dyvol_set_enable(u8 en);
void anc_adp_eq_set_gain(int gain, int gain_fraction);
void anc_msc_gain_set(u8 ch, u32 gain, u8 step, u8 direct_set, u8 fade_step);
void bsp_anc_alg_aem_rt_set_gain(void);
void bsp_anc_aem_set_fb_param(void);
void bsp_anc_set_param(u8 tp);
void bsp_anc_aem_set_ff_param(u8 idx_num, bool change_en, bool cvt_flag);
void bsp_anc_aem_set_ff_cvt(s16 ff_gain);

bool alg_anc_windnoise_ff_talk_set(void* ptr, u16 size, u8 ch);
bool alg_anc_windnoise_ff_fb_set(void* ptr, u16 size, u8 ch);
bool alg_anc_asm_simple_set(void* ptr, u16 size, u8 ch);
bool alg_anc_howling_fb_set(void* ptr, u16 size, u8 ch);
bool alg_anc_fit_detect_ff_fb_set(void* ptr, u16 size, u8 ch);
bool alg_anc_howling_ff_set(void* ptr, u16 size, u8 ch);
bool alg_anc_msc_adp_fb_set(void* ptr, u16 size, u8 ch);
bool alg_anc_adp_eq_ff_fb_set(void* ptr, u16 size, u8 ch);
bool alg_anc_asm_set(void* ptr, u16 size, u8 ch);
bool alg_anc_ai_wn_ff_set(void* ptr, u16 size, u8 ch);
bool alg_anc_ai_wn_mini_ff_set(void* ptr, u16 size, u8 ch);


#define QCONST32(x, bits)            ((s32)(.5+(x)*(((s32)1)<<(bits))))
#define CAL_FIX(x)                   ((int)(x * (1 << 27)))

#if ANC_ALG_WIND_NOISE_FF_TALK_EN

#define WIND_NOISE_DM_FF_MIC            ANC_ALG_MIC_FF
#define WIND_NOISE_DM_SHIFT             15

#define WIND_NOISE_FF_TALK_SIZE         1428
static u8 wind_noise_ff_talk_buf[WIND_NOISE_FF_TALK_SIZE] AT(.anc_data.anc_windnoise_dm.buf);
//双MIC(FF+TALK)传统特征风噪检测
const dewind_dm_cb_t dewind_dm = {
    .db_en = 1,
    .filt_use = 1,
    .power_use_ff = 0,
    .danger_thr = QCONST32(-7.5, WIND_NOISE_DM_SHIFT),
	.distance = 20,
	.degree = 0,
	.wind_range = 20,
	.wind_Gcoh_sum_thr = 1200,
	.wind_enk1_thr = 170000,
	.wind_enk2_thr = QCONST32(-0.77, WIND_NOISE_DM_SHIFT),
	.ene_th1 = QCONST32(23.6, WIND_NOISE_DM_SHIFT),
	.ene_th2 = QCONST32(41.5, WIND_NOISE_DM_SHIFT),
	.energy_en = 1,
	.corr_thr = 16500,
	.time_down = 50,
	.time_up = 50,
	.pen_frame = 150,
	.wind_vaild_len = 43,
	.wind_vaild_end = 313,
};
#endif // ANC_ALG_WIND_NOISE_FF_TALK_EN

#if ANC_ALG_WIND_NOISE_FF_FB_EN

#define WIND_NOISE_FF_FB_SIZE     832 * (ANC_ALG_STEREO_EN + 1)
static u8 wind_noise_ff_fb_buf[WIND_NOISE_FF_FB_SIZE] AT(.anc_data.ff_fb_wind.data);
//双MIC(FF+FB)传统特征风噪检测
const anc_wind_ff_fb_cb_t dewind_fffb = {
    .ht_mode = 0,
    .energy_en = 1,
    .light_wind_detect = 1,
    .ene_th1 = 3200,
    .ene_th2 = 30000,
    .time_up = 43,
    .time_down = 50,
    .pen_frame = 2000,
    .wind_enk1_thr_l = 65536,
    .wind_enk1_thr_h = 16384,
    .wind_enk2_thr_l = 4260,
    .wind_enk2_thr_h = 196608,
    .wind_enk2_thr_m = 32768,
    .wind_enk2_thr2 = 1838400,
    .E_var_thr = 1638,
    .E_var_thr_2 = 3932,
    .E_var_frame_thr = 1,
    .E_var_frame_thr_2 = 66,
    .bad_thr = -154010,
    .diff_bad_thr = -163840,
    .slope_thr = 32768 * 2,
    .bit_goal = 18,
    .bit_goal_hfre = 25,
    .corr_thr = 28836,
    .diff_thr = 111411,
    .smooth_thr = 24576,
    .light_thr = 98304,
    .eli_thr_l = 6554,
    .eli_thr_r = 180224,
    .diff_thr_low = -65536,
    .diff_abs_thr = 73728,
    .diff_abs_thr_high = 90112,
    .fb_power_thr = 12500000,
    .spk_power_thr = 2500000,
    .wind_count_clean_thr = 150,
    .wind_count_stay_thr = 15,
    .wind_vaild_len = 43,
    .wind_vaild_end = 313,
    .pp_save = {
        -156814, -180179, -207297, -242980, -263019, -294556, -316976, -330132,
    },
    .sp_h = {
        -36245, 8209,  -32751, 11379, -29364, 13053, -26704, 13798,
        -24454, 14153, -22437, 14032, -20441, 13711, -19192, 13128,
    },
    .sp_h_2 = {
        19045,  -13782, 18509,  -13612, 18258,  -13255, 17819,  -13162, 17626,  -12779, 17389,  -12573, 17027,
        -12358, 17042,  -12205, 16868,  -12110, 16802,  -12158, 16721,  -11841, 16783,  -12039, 16764,  -12157,
        16539,  -12293, 16554,  -12465, 16434,  -12700, 16179,  -12908, 15848,  -13151, 15493,  -13439, 15251,
        -13571, 14904,  -13718, 14508,  -13901, 14231,  -13970, 13782,  -14051, 13391,  -14177, 13086,  -14183,
        12641,  -14220, 12334,  -14202, 11977,  -14111, 11653,  -14171, 11340,  -14076, 11050,  -13922, 10738,
        -14000, 10453,  -13978, 10314,  -13917, 9961,   -13881, 9740,   -13793, 9412,   -13665, 9226,   -13583,
        8955,   -13553, 8717,   -13624, 8548,   -13481, 8242,   -13347, 8041,   -13342, 7826,   -13236, 7601,
        -13154, 7457,   -13052, 7227,   -13088, 7058,   -12987, 6903,   -12944,
    },
};
#endif // ANC_ALG_WIND_NOISE_FF_FB_EN

#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN

#define ASM_SIM_FF_SIZE     1096 * (ANC_ALG_STEREO_EN + 1)
static u8 asm_sim_ff_buf[ASM_SIM_FF_SIZE] AT(.anc_data.anc_asm_simple.data);

#if ANC_ALG_ASM_SIM_FF_EN
//降增噪
const asm_simple_cb_t asm_simple = {
    .bit_tun = 12,
    .time_up = 312*2,
    .time_down = 312,
    .power_thr = 100,
    .pen_frame = 100,
    .thresholds_1 = 480,
    .thresholds_2 = 15500,
    .left_alp = 29491,
    .right_alp = 36045,
    .double_thr_en = 1,
    .out_thr_1 = 480,
    .out_thr_2 = 4800,
    .out_thr_3 = 15500,
    .out_thr_4 = 15500,
    .db_en = 0,
    .ff_gain_buffer[0] = 0,
    .ff_gain_buffer[1] = 0,
    .ff_gain_buffer[2] = 0,
    .fb_gain_buffer[0] = 0,
    .fb_gain_buffer[1] = 0,
    .fb_gain_buffer[2] = 0,
};
#endif // ANC_ALG_ASM_SIM_FF_EN
#if ANC_ALG_ASM_FF_EN
//环境自适应
const asm_simple_cb_t asm_cb = {
    .bit_tun = 12,
    .time_up = 312,
    .time_down = 312,
    .power_thr = 100,
    .pen_frame = 100,
    .thresholds_1 = 580,
    .thresholds_2 = 10200,
    .left_alp = 29491,
    .right_alp = 36045,
    .double_thr_en = 0,
    .out_thr_1 = 0,
    .out_thr_2 = 0,
    .out_thr_3 = 0,
    .out_thr_4 = 0,
    .db_en = 1,
    .ff_gain_buffer[0] = -20,
    .ff_gain_buffer[1] = -10,
    .ff_gain_buffer[2] = 0,
    .fb_gain_buffer[0] = -20,
    .fb_gain_buffer[1] = -10,
    .fb_gain_buffer[2] = 0,
};
#endif // ANC_ALG_ASM_FF_EN
#endif

#if ANC_ALG_HOWLING_FB_EN

#define HOWLING_FB_BUF_SIZE     4448 * (ANC_ALG_STEREO_EN + 1)
static u8 howling_fb_buf[HOWLING_FB_BUF_SIZE] AT(.anc_data.anc_howling.data);

#define HOWLING_LEN             256         //只能为64，128，256。越大越省算力，但啸叫检测的时间会越长
#define HOWLING_SPEED           1
//防啸叫(FB)
const howling_cb_t howling_fb = {
    .frame_len = HOWLING_LEN,
    .mic_ancout_exchange = 0,
    .gainback = 20,
    .gainhold = 1000,
    .framestart = 20,
    .twsmodel = 1,
    .speed = HOWLING_SPEED,
    .ptprthrmic = -15,
    .ptprthrancout = -15,
    .gainlight = -10,
    .gainmedium = -15,
    .gaindeep = -24,
    .wndetect = 1,
    .numratio = 25,
    .paprnewpercent = 70,
    .paprnewharpercent = 80,
    .speechdetect = 0,
};
#endif // ANC_ALG_HOWLING_FB_EN

#if ANC_ALG_FIT_DETECT_FF_FB_EN

#define FIT_DETECT_BUF_SIZE     3396
static u8 fit_detect_buf[FIT_DETECT_BUF_SIZE] AT(.anc_data.anc_fit_detect.data);
//贴合度检测(FF+FB)
const anc_fit_detect_cb_t anc_fit_detect = {
	.sp_min_thr = -147456,
	.sp_max_thr = 989824,
	.aem_fre_s_idx = 3,
	.aem_fre_e_idx = 11,
	.Iterations_ready_aem = 185,
	.Iterations_ready_fd = 185,
	.pp_h = {
         38022,       0,   21942,   -5510,   20703,   -6239,   21663,   -7164,   19886,  -12354,   18224,  -11904,   12705,
        -10522,   12715,   -9411,   11483,  -12560,    7513,  -10366,    7062,   -7878,    9142,   -7640,    9446,   -8450,
          8202,  -10466,    6650,  -10124,    4910,   -9811
	},
	.pp_gain = {
         32768,   32768,   32768,   32768,   32768,   32768,   32768,   32768,   32768,   32768
	},
	.thr_buffer = {
             0,   65536,  131072,  196608,  262144,  327680,  393216,  458752,  524288,  589824
	},
	.q_tun = 8,
	.w_thr = 683,
};
#endif // ANC_ALG_FIT_DETECT_FF_FB_EN

#if ANC_ALG_HOWLING_FF_EN

#define HOWLING_FF_BUF_SIZE     4448 * (ANC_ALG_STEREO_EN + 1)
static u8 howling_ff_buf[HOWLING_FF_BUF_SIZE] AT(.anc_data.anc_howling.data);

#define HOWLING_LEN             64          //只能为64，128，256。越大越省算力，但啸叫检测的时间会越长
#define HOWLING_SPEED           3
//防啸叫(FF)
const howling_cb_t howling_ff = {
    .frame_len = HOWLING_LEN,
    .mic_ancout_exchange = 1,
    .gainback = 20,
    .gainhold = 1000,
    .framestart = 20,
    .twsmodel = 0,
    .speed = HOWLING_SPEED,
    .ptprthrmic = -30,
    .ptprthrancout = -35,
    .gainlight = -10,
    .gainmedium = -15,
    .gaindeep = -24,
    .wndetect = 1,
    .numratio = 25,
    .paprnewpercent = 70,
    .paprnewharpercent = 80,
    .speechdetect = 0,
};
#endif // ANC_ALG_HOWLING_FF_EN

#if ANC_ALG_AI_WN_FF_EN || ANC_ALG_AI_WN_DSP_FF_EN

#if ANC_ALG_AI_WN_DSP_FF_EN
#define AI_WN_FF_BUG_SIZE       9068*(ANC_ALG_STEREO_EN + 1) + 23240
u8 ai_wn_ff_buf[AI_WN_FF_BUG_SIZE] AT(.anc_data.aiwn.data);
#endif // ANC_ALG_AI_WN_DSP_FF_EN
//单MIC(FF)AI风噪检测
const ai_wn_init_t ai_wn_ff = {
    .aiwn_mode      = ANC_ALG_AI_WN_FF_EN,
    .time_down      = 150,
    .time_up        = 50,
    .pen_frame      = 1,
    .enr_en         = 1,
    .ene_th1        = 100000,
    .ene_th2        = 400000,
    .wind_vaild_len = 50,
    .wind_vaild_end = 200,
};
#endif // ANC_ALG_AI_WN_FF_EN

#if ANC_ALG_LIMITER_FF_EN

#define LIMITER_FF_SHIFT                    15
//单MIC(FF)瞬态噪声检测
const limiter_config_info_t limiter_ff = {
    .enable = 1,
	.gainback = 9,
	.ratio2to0 = 5,
	.ratio2to1 = 32,
	.ancoutmaxvalue = 32768,
	.ancoutmeanvalue = 256,
	.lowfreqratio = 95,
	.flatness = QCONST32(-4, LIMITER_FF_SHIFT),
	.gain1 = -5,
	.gain2 = -10,
	.gain3 = -20,
};
#endif // ANC_ALG_LIMITER_FF_EN

#if ANC_ALG_DYVOL_FF_EN

#define	DYVOL_FF_SHIFT					    15
//单MIC(FF)动态音量
const Dyvol_cb_t dyvol_ff = {
    .bit_tun = 5,
    .init_thr = 5,
    .enable = 1,
	.low_thr = QCONST32(-75, DYVOL_FF_SHIFT),
	.high_thr = QCONST32(-35, DYVOL_FF_SHIFT),
	.max_adjust_vol = QCONST32(40, DYVOL_FF_SHIFT),
	.min_adjust_vol = QCONST32(-40, DYVOL_FF_SHIFT),
	.alp = QCONST32(0.6, DYVOL_FF_SHIFT),
	.up_thr = 5,
	.down_thr = 5,
	.delat_tun = 10,
	.speed_up = 2,
	.speed_down = 2,
};
#endif // ANC_ALG_DYVOL_FF_EN

#if ANC_ALG_MSC_ADP_FB_EN

#define MSC_ADP_FB_BUF_SIZE         216 * (ANC_ALG_STEREO_EN + 1)
static u8 msc_adp_fb_buf[MSC_ADP_FB_BUF_SIZE] AT(.anc_data.anc_msc_adp.data);

#define SHIFT                       15
#define Q_TUN                       7
//自适应音乐补偿
const anc_msc_adp_cb_t msc_adp_fb = {
    .q_tun = Q_TUN,
    .w_thr = 30,
    .fre_s_idx = 3,
	.fre_e_idx = 11,
	.fb_tun_max = 24,
	.msc_tun_max = 24,
	.fb_tun_min = 0,
	.msc_tun_min = 0,
	.tx_power_1_thr = QCONST32(0.000047, SHIFT+ Q_TUN),
	.tx_power_2_thr = QCONST32(0.065, SHIFT + Q_TUN),
	.power_max_thr  = QCONST32(0.7, SHIFT),
	.leak_thr_2 = QCONST32(-14, SHIFT),
	.leak_thr_1 = QCONST32(-10, SHIFT),
	.fade_time = 2,
	.restore_max = 47,
	.stable_cnt_thr = 47*2,
	.leak_safe_cnt_thr = 47,
	.leak_danger_cnt_thr = 25,
	.danger_hold_cnt = 313/2,
};
#endif // ANC_ALG_MSC_ADP_FB_EN

#if ANC_ALG_ADP_EQ_FF_FB_EN

#define ADP_EQ_FF_FB_BUF_SIZE   300
static u8 adp_eq_ff_fb_buf[ADP_EQ_FF_FB_BUF_SIZE] AT(.anc_data.adp_eq.data);

#define SHIFT           	    15

//自适应EQ
static const int adp_eq_tbl[2] = {
    CAL_FIX(0.0492847860),    CAL_FIX(0.9979453926),     //Band:(450Hz) Q:(0.65)
};

const adp_eq_cb_t adp_eq_ff_fb = {
    .pp_h = {
         16597,       0,   14848,   -1270,   14784,   -1150,   14891,    -931,   14928,    -769,   14973,    -953,   15262,
         -1018,   15431,   -1311,   15104,   -1836,   15784,    -859,   15619,   -1174,   15298,   -1499,   16013,   -1593,
         14721,    -820,   14900,    -928,   15869,   -1169
    },
    .q_tun = 7,
    .cal_stay_thr = 30,
    .cal_confirm_thr = 40,
    .period = 47,
    .leak_tun_min = 0,
    .leak_tun_max = 24,
    .msc_tun_min = 0,
    .msc_tun_max = -24,
    .leak_thr_2 = QCONST32(-3.9, SHIFT),
    .leak_thr_1 = QCONST32(7.2, SHIFT),
    .leak_thr_2_msc = QCONST32(-3.9, SHIFT),
    .leak_thr_1_msc = QCONST32(7.2, SHIFT),
    .w_thr = 0,
    .tx_power_1_thr = 60,
    .speed_up = 20,
    .speed_down = 35,
};

#endif // ANC_ALG_ADP_EQ_FF_FB_EN

#if ANC_ALG_AEM_RT_FF_FB_EN

#define SHIFT               15
#define INT_MIN             (-2147483647 - 1)

//半入耳耳道自适应
const anc_aem_rt_cb_t aem_ff_fb = {
    .alp = QCONST32(0.7, SHIFT),
    .open_thr_2 = QCONST32(-20, SHIFT),
    .gears_use = 10,
    .cvt_en = 1,
    .fix_idx = 10,
    .power_diff_enable = 0,
    .ft_thr_fb = QCONST32(0.25, SHIFT),
    .ft_thr_ff =  QCONST32(0.33, SHIFT),
    .pp_idx_1 = 1,
    .pp_idx_2 = 8,
    .cvt_sp_step = QCONST32(0.1, SHIFT),
    .gear_gain_step_ff = 1,
    .gear_gain_step_fb = 0,
    .gear_gain_step_msc = 0,
    .cvt_cnt_thr = 50000,
    .erp_cal_cnt_thr = 80,
    .erp_cal_cnt_thr_2 = 40,
    .wear_detection_cnt_thr = 150,
    .open_thr = QCONST32(-100, SHIFT),
    .diff_thr = QCONST32(2.4, SHIFT),
    .sp_delta = QCONST32(0.5, SHIFT),
    .tx_thr = QCONST32(13, SHIFT),
    .var_thr = QCONST32(0.5, SHIFT),
    .self_time_max = 80,
    .thr_buffer = {
         361162,  292901,  224640,  156379,   88118,   19857,  -39980,  -99816, -159652, -219489, INT_MIN, INT_MIN,
        INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN
    },
    .ff_gains_buffer = {
        -446001, -368998, -291995, -215559, -139741,  -64571,    9934,   79814,  151609,  224160,  296475,  368790,
              0,       0,       0,       0,       0,       0,       0,       0,       0,       0
    },
    .pen_frame = 150,
    .q_tun = 10,
    .tolerate_thr = 25,
    .aem_fre_s_idx = 3,
    .aem_fre_e_idx = 11,
    .aem_voice_e_idx = 97,
    .Iterations_ready = 287,
    .change_up_thr = 250,
    .change_down_thr = 250,
    .change_hold_len = 30,
    .change_hold_len_2 = 60,
    .shift_cnt_thr = 56,
    .diff_shift_thr = 32768,
    .w_thr = QCONST32(0.00001, SHIFT+10),
    .silence_thr = QCONST32(0.0001, SHIFT),
    .pp_h = {
           9821,       0,  -25271,    3448,  -32463,    1956,  -31853,    3376,  -32094,    5767,  -31900,    7995,
         -30667,   10496,  -30666,   12659,  -29163,   15024,  -28845,   17846,  -26272,   19293,  -25196,   20663,
         -23138,   23578,  -20490,   25328,  -18150,   25397,  -15464,   26185
    },
    .pp_h_2 = {
         -23927,       0,  -29680,   -1691,  -31970,   -1117,  -32525,    1050,  -31384,    2325,  -32515,    3165,
         -32686,    5246,  -32380,    6338,  -32357,    7868,  -32954,    8600,  -31755,   10827,  -33138,   12748,
         -34280,   14505,  -32206,   15537,  -32179,   18708,  -31631,   19046
    },
    .pp_h_3 = {
         -16849,       0,  -30038,   -2234,  -31526,   -1661,  -31867,    -291,  -31394,     440,  -32031,    1088,
         -31647,    2256,  -32085,    3073,  -31469,    3315,  -32164,    3603,  -32409,    3985,  -33064,    5131,
         -33588,    6040,  -33874,    7142,  -33975,    7776,  -34794,    8313
    },
};
#endif // ANC_ALG_AEM_RT_FF_FB_EN

///自研单MIC(FF)AI风噪(小模型)检测
#if ANC_ALG_AI_WN_MINI_FF_EN

#define AI_WN_MINI_FF_BUG_SIZE          3964 * (ANC_ALG_STEREO_EN + 1) + 9264
u8 ai_wn_mini_ff_buf[AI_WN_MINI_FF_BUG_SIZE] AT(.anc_data.aiwn_mini.data);

const ai_wn_init_t ai_wn_mini_ff = {
    .time_down      = 150,
    .time_up        = 50,
    .pen_frame      = 1,
    .enr_en         = 1,
    .ene_th1        = 100000,
    .ene_th2        = 400000,
    .wind_vaild_len = 50,
    .wind_vaild_end = 200,
};
#endif // ANC_ALG_AI_WN_MINI_FF_EN

#if ANC_SNDP_SAE_SHIELD_ADAPTER_EN

#define _IQ24(A)                ((int)((A) * ((int)1 << 24)))
#define _IQ21(A)                ((int)((A) * ((int)1 << 21)))

AT(.sndp_sae_rodata.SAE_EL01)
g_Cfg_SpEc_ShieldLeakageDetect gc_Para_ShieldLeakDet_Cfg = {
    .SHIELD_LEAKGEAR = {97000,77000,59800,44731,35000,30000,15000},
    .SHIELD_CORR_CORRECT = _IQ24(31.36),
    .SHIELD_ShieldStartFrm = 228,
    .SHIELD_ShieldEndFrm = 234,
    .SHIELD_ShieldOverFrm = 237,
    .SHIELD_ShieldHalfDetFrm = 7,
};

AT(.sndp_sae_rodata.SAE_EL01)
g_Cfg_SpEc_AdapANC gc_Para_AdapANC_Cfg = {
    .DataScaleff = _IQ24(0.1), // 不要低于 0.1
    .DataScalefb = _IQ24(0.75),
    .THR_FF = _IQ21(5.0f),
    .THR_FB = _IQ21(-10.0f),
    .THR_Est0 = _IQ21(2.0f),
    .THR_Est1 = _IQ21(18.0f),
    .THR_Est2 = _IQ21(5.0f),
    .THR_Est3 = _IQ21(16.0f),
    .THR_Est4 = _IQ21(17.0f),
    .THR_Est5 = _IQ21(10.5f),
    .GainMax = 0 * 64,
    .GainMin = -4 * 64,
    .ModeMax = 5,
    .LogSwitch = 1,
};

int FB_total_gain = 321;
#endif // ANC_SNDP_SAE_SHIELD_ADAPTER_EN

#if ANC_SNDP_SAE_WIND_DETECT_EN

#define _IQ25(A)                ((int)((A) * ((int)1 << 25)))
#define _IQ30(A)                ((int)((A) * ((int)1 << 30)))

AT(.sndp_sae_rodata.dwind)
const g_Cfg_SpEc_WindDetect gc_Para_WindDetect_Cfg = {
    // Calibration gain for FF and FB mic signals.
    .FFGain = _IQ25(17.5792f),      // +8.8dB ( 10^(+8.8/20) = 2.7542 )
    .FBGain = _IQ25(1.7783f),      // +1dB ( 10^(+1/20) = 1.2220 )

    .ATT_WindFFFB_FrmNum = 135,
    .DEC_WindFFFB_FrmNum = 600,

    .WindFFFB_TH = _IQ30(0.55f),
    .noWindFFFB_TH = _IQ30(0.1f),

    .Hcomp = {
        {    // Only spectrum below 500Hz is used. Transparancy mode (with compensation for FB銆€ANC) for ziyan earphone.
            -64497159, 973517474, -46210455,  29241855, -14855326,  26127703,  -5476212,   2420901, -16422022, -10319346,
            -22801208,  -8209959, -19622954,  -9969027, -21782378, -19612238, -32361140, -24275477, -40094765, -21425144,
            -43356557, -20850606, -51332590, -22833442, -63558880, -17690154, -69886033,  -5291593, -69098636,   5087389,
            -68493927,  12340450, -67316590,  22402872
        },
        {   // Only spectrum below 500Hz is used. Strong ANC mode for ziyan earphone.
            -83659349, 325611177, -66018007,  30275697, -33746675,  30051843, -19962514,   6775569, -28371308, -11148726,
            -38993987, -14671973, -43166333, -16494349, -50185533, -22541341, -64453543, -24024265, -77758278, -16093006,
            -84895913,  -4769325, -89480511,   6451685, -91917411,  20973163, -86420197,  37373238, -73343825,  47099857,
            -61947795,  47627043, -57037488,  46589744
        },
        {   // Only spectrum below 500Hz is used. Middle ANC mode for ziyan earphone.
            -15791929, -97130471, -19621773, -18660709, -32158353, -35170456, -53422822, -43958754, -77527982, -38970300,
            -94085747, -20718063, -96552926,   1894789, -87791666,  19093750, -75246775,  28295996, -62445453,  32343724,
            -48811573,  31979639, -36189094,  25122595, -29502667,  13267455, -29742326,   2478432, -31890282,  -4237758,
            -32598072, -10489878, -35138523, -19357396
        },
        {   // Only spectrum below 500Hz is used. Light ANC mode for ziyan earphone.
            44661217,  51119989,  35422915,   8624165,  26277941,  40596009,  46335911,  83749564,  94748246, 105406057,
            143165047,  92869970, 171044066,  61860328, 180651113,  29930339, 179475473,    228041, 166495051, -27116469,
            141001264, -44393018, 111705163, -44546821,  89479309, -30360394,  78299251,  -9463681,  77751449,  12996893,
            87360773,  32880702, 104166916,  45434720
        },
    },

    .Hoff = {
        {   // Only spectrum below 500Hz is used. FF ANC off (FB ANC on only) mode for ziyan earphone.
            68243380, 702547923,  49577280, -29561036,  17190628, -26050802,   7236011,   -581904,  19528572,  14030756,
            27997668,  11650120,  24680557,  11919243,  24984254,  22457976,  36085413,  30453723,  47392666,  28995990,
            52761397,  26673164,  59729530,  28511174,  72982726,  27152633,  85922410,  17086239,  92511019,   2460823,
            93784970, -12380565,  90204235
        },
        {   // Only spectrum below 500Hz is used. FF ANC off (FB ANC on only) mode for ziyan earphone.
            112680614, -373663250,   83682155,  -48516294,   32926486,  -45932184,   16193809,   -8788899,   34057179,   12776368,
            47495488,    8742277,   44688405,    7908710,   48685194,   20419198,   68869264,   25507897,   86693228,   12563746,
            90542764,   -3886817,   90501768,  -13682649,   93349096,  -24619182,   90838644,  -40755245,   79356457,  -52646700,
            68169979,  -55849457,   61750592,  -58139469
        },
        {   // Only spectrum below 500Hz is used. FF ANC off (FB ANC on only) mode for ziyan earphone.
            110027806, 109160996, 104737953, -15531418,  94854202, -22502171,  89634249, -23566142,  89024386, -27055996,
            86054373, -34233101,  78905529, -39050079,  73167753, -39168448,  71926486, -39821498,  70442360, -45024443,
            63640420, -50814617,  54225100, -51075322,  48969435, -46716746,  48994818, -44455875,  48322591, -47257504,
            42511221, -50306845,  34904748, -48024520
        },
        {   // Only spectrum below 500Hz is used. FF ANC off (FB ANC on only) mode for ziyan earphone.
            140515460, 400189913, 120072017, -30389771,  87940765, -22339800,  86925972,   7128357, 112360274,  16685089,
            129502219,   1712683, 129855738, -11872643, 132158442, -16827424, 140238155, -28761678, 138249543, -49356025,
            124134283, -61902999, 114034667, -62801978, 112302507, -66415463, 106059278, -76578408,  92265561, -80762565,
            83783344, -76069862,  84015681, -75635534
        },
    },
    .PWRDIFTHH = {
        214748, 1073742, 4294967,  711032
    },
};
#endif // ANC_SNDP_SAE_WIND_DETECT_EN

#define STEREO_MIC                  ANC_ALG_STEREO_EN

#define ANC_ALG_WIND_NOISE_DM_CH    (WIND_NOISE_DM_FF_MIC | ANC_ALG_MIC_TALK << 8)      //FF + TALK
#define ANC_ALG_WIND_NOISE_FFFB_CH  (ANC_ALG_MIC_FF | ANC_ALG_MIC_FB << 8)              //FF + FB
#define ANC_ALG_ASM_SIM_FF_CH       (ANC_ALG_MIC_FF)                                    //FF
#define ANC_ALG_HOWLING_FB_CH       (ANC_ALG_MIC_FB)                                    //FB
#define ANC_ALG_FIT_DETECT_CH       (ANC_ALG_MIC_FF | ANC_ALG_MIC_FB << 8)              //FF + FB
#define ANC_ALG_HOWLING_FF_CH       (ANC_ALG_MIC_FF)                                    //FF
#define ANC_ALG_AI_WN_FF_CH         (ANC_ALG_MIC_FF)                                    //FF
#define ANC_ALG_LIMITER_FF_CH       (ANC_ALG_MIC_FF)                                    //FF
#define ANC_ALG_DYVOL_FF_CH         (ANC_ALG_MIC_FF)                                    //FF
#define ANC_ALG_MSC_ADP_FB_CH       (ANC_ALG_MIC_FB)                                    //FB
#define ANC_ALG_ADP_EQ_FF_FB_CH     (ANC_ALG_MIC_FF | ANC_ALG_MIC_FB << 8)              //FF + FB
#define ANC_ALG_AEM_RT_FF_FB_CH     (ANC_ALG_MIC_FF | ANC_ALG_MIC_FB << 8)              //FF + FB
#define ANC_ALG_AI_WN_MINI_FF_CH    (ANC_ALG_MIC_FF)
#define SNDP_SAE_SHIELD_LEAK_CH     (ANC_ALG_MIC_FB)                                    //FB
#define SNDP_SAE_ADAPTER_ANC_CH     (ANC_ALG_MIC_FF | ANC_ALG_MIC_FB << 8)              //FF + FB
#define SNDP_SAE_DWIND_ANC_CH       (ANC_ALG_MIC_FF | ANC_ALG_MIC_FB << 8)              //FF + FB
#define ANC_ALG_USER_CH             (ANC_ALG_MIC_FF)                                    //FF

AT(.rodata.anc_alg)
const anc_alg_comm_param_cb anc_alg_comm_param_tbl[] = {
/*  type                        mic_ch_cfg  pcm_interleave  alg_samples anc_dma sdadc_ch                    sdadc_samples    alg_param*/
#if ANC_ALG_WIND_NOISE_FF_TALK_EN
    {ANC_ALG_WIND_NOISE_FF_TALK,0,          0,              512,        0,      ANC_ALG_WIND_NOISE_DM_CH,   256,             (void*)&dewind_dm},
#endif // ANC_ALG_WIND_NOISE_FF_TALK_EN
#if ANC_ALG_WIND_NOISE_FF_FB_EN
    {ANC_ALG_WIND_NOISE_FF_FB,  STEREO_MIC, 0,              512,        1,      ANC_ALG_WIND_NOISE_FFFB_CH, 256,             (void*)&dewind_fffb},
#endif // ANC_ALG_WIND_NOISE_FF_FB_EN
#if ANC_ALG_ASM_SIM_FF_EN
    {ANC_ALG_ASM_SIM_FF,        STEREO_MIC, 0,              256,        0,      ANC_ALG_ASM_SIM_FF_CH,      256,             (void*)&asm_simple},
#endif // ANC_ALG_ASM_SIM_FF_EN
#if ANC_ALG_HOWLING_FB_EN
    {ANC_ALG_HOWLING_FB,        STEREO_MIC, 0,              HOWLING_LEN,1,      ANC_ALG_HOWLING_FB_CH,      HOWLING_LEN,     (void*)&howling_fb},
#endif // ANC_ALG_HOWLING_FB_EN
#if ANC_ALG_FIT_DETECT_FF_FB_EN
    {ANC_ALG_FIT_DETECT_FF_FB,  0,          0,              256,        1,      ANC_ALG_FIT_DETECT_CH,      256,             (void*)&anc_fit_detect},
#endif // ANC_ALG_FIT_DETECT_FF_FB_EN
#if ANC_ALG_HOWLING_FF_EN
    {ANC_ALG_HOWLING_FF,        STEREO_MIC, 0,              HOWLING_LEN,1,      ANC_ALG_HOWLING_FF_CH,      HOWLING_LEN,     (void*)&howling_ff},
#endif // ANC_ALG_HOWLING_FF_EN
#if ANC_ALG_AI_WN_FF_EN || ANC_ALG_AI_WN_DSP_FF_EN
    {ANC_ALG_AI_WN_FF,          STEREO_MIC, 0,              512,        0,      ANC_ALG_AI_WN_FF_CH,        256,             (void*)&ai_wn_ff},
#endif // ANC_ALG_AI_WN_FF_EN
#if ANC_ALG_LIMITER_FF_EN
    {ANC_ALG_LIMITER_FF,        0,          0,              256,        1,      ANC_ALG_LIMITER_FF_CH,      256,             (void*)&limiter_ff},
#endif // ANC_ALG_LIMITER_FF_EN
#if ANC_ALG_DYVOL_FF_EN
    {ANC_ALG_DYVOL_FF,          0,          0,              512,        0,      ANC_ALG_DYVOL_FF_CH,        256,             (void*)&dyvol_ff},
#endif // ANC_ALG_DYVOL_FF_EN
#if ANC_ALG_MSC_ADP_FB_EN
    {ANC_ALG_MSC_ADP_FB,        STEREO_MIC, 0,              512,        1,      ANC_ALG_MSC_ADP_FB_CH,      256,             (void*)&msc_adp_fb},
#endif // ANC_ALG_MSC_ADP_FB_EN
#if ANC_ALG_ADP_EQ_FF_FB_EN
    {ANC_ALG_ADP_EQ_FF_FB,      0,          0,              512,        1,      ANC_ALG_ADP_EQ_FF_FB_CH,    256,             (void*)&adp_eq_ff_fb},
#endif // ANC_ALG_ADP_EQ_FF_FB_EN
#if ANC_ALG_ASM_FF_EN
    {ANC_ALG_ASM_FF,            STEREO_MIC, 0,              256,        0,      ANC_ALG_ASM_SIM_FF_CH,      256,             (void*)&asm_cb},
#endif // ANC_ALG_ASM_FF_EN
#if ANC_ALG_AEM_RT_FF_FB_EN
    {ANC_ALG_AEM_RT_FF_FB,      0,          0,              256,        1,      ANC_ALG_AEM_RT_FF_FB_CH,    256,             (void*)&aem_ff_fb},
#endif // ANC_ALG_AEM_RT_FF_FB_EN
#if ANC_ALG_AI_WN_MINI_FF_EN
    {ANC_ALG_AI_WN_MINI_FF,     STEREO_MIC, 0,              256,        0,      ANC_ALG_AI_WN_MINI_FF_CH,   256,             (void*)&ai_wn_mini_ff},
#endif // ANC_ALG_AI_WN_MINI_FF_EN
#if ANC_SNDP_SAE_SHIELD_ADAPTER_EN
    {SNDP_SAE_SHIELD_LEAK,      0,          0,              120,        0,      SNDP_SAE_SHIELD_LEAK_CH,    240,             (void*)&FB_total_gain},
    {SNDP_SAE_ADAPTER_ANC,      0,          1,              128,        0,      SNDP_SAE_ADAPTER_ANC_CH,    256,             0},
#endif // ANC_SNDP_SAE_SHIELD_ADAPTER_EN
#if ANC_SNDP_SAE_WIND_DETECT_EN
    {SNDP_SAE_WIND_DETECT,      0,          1,              240,        0,      SNDP_SAE_DWIND_ANC_CH,      480,             0},
#endif // ANC_SNDP_SAE_WIND_DETECT_EN
#if ANC_ALG_DUMP_EN && ANC_ALG_DUMP_FOR_ANC_MODE
    {ANC_ALG_DUMP_TYPE,         0,          0,              256,        1,      0,                          256,             0},
#endif // ANC_ALG_DUMP_EN
#if ANC_ALG_USER_EN
    {ANC_ALG_USER,              0,          0,              256,        0,      ANC_ALG_USER_CH,            256,             0},
#endif // ANC_ALG_USER_EN
};

AT(.rodata.anc_alg)
static const u8 sdadc_ch_tbl[MIC_NUM_MAX] = {
    FF_MIC_SDADC_MAPPING, FB_MIC_SDADC_MAPPING, TALK_MIC_SDADC_MAPPING, FF_R_MIC_SDADC_MAPPING, FB_R_MIC_SDADC_MAPPING,
};


#define ANC_GAIN_TBL_MAX_IDX        (242)
#define ANC_GAIN_TBL_MUTE_IDX       (241)
#define ANC_GAIN_MIN_DB             (-24)

AT(.anc_rodata.res.dvol_tbl)
const s16 dvol_tbl_Q15_step0P1db[ANC_GAIN_TBL_MAX_IDX] = {   //0.0~-24.0dB, step: 0.1dB
    0x7fff, 0x7e88, 0x7d15, 0x7ba7, 0x7a3c, 0x78d6, 0x7774, 0x7616,
    0x74bc, 0x7366, 0x7214, 0x70c5, 0x6f7b, 0x6e34, 0x6cf1, 0x6bb2,
    0x6a76, 0x693e, 0x680a, 0x66d9, 0x65ac, 0x6482, 0x635b, 0x6238,
    0x6118, 0x5ffc, 0x5ee3, 0x5dcc, 0x5cba, 0x5baa, 0x5a9d, 0x5994,
    0x588d, 0x578a, 0x5689, 0x558c, 0x5491, 0x5399, 0x52a4, 0x51b2,
    0x50c3, 0x4fd6, 0x4eec, 0x4e05, 0x4d20, 0x4c3e, 0x4b5f, 0x4a82,
    0x49a7, 0x48d0, 0x47fa, 0x4727, 0x4657, 0x4589, 0x44bd, 0x43f3,
    0x432c, 0x4268, 0x41a5, 0x40e5, 0x4026, 0x3f6a, 0x3eb1, 0x3df9,
    0x3d43, 0x3c90, 0x3bde, 0x3b2f, 0x3a81, 0x39d6, 0x392c, 0x3885,
    0x37df, 0x373c, 0x369a, 0x35fa, 0x355c, 0x34bf, 0x3425, 0x338c,
    0x32f5, 0x325f, 0x31cc, 0x313a, 0x30aa, 0x301b, 0x2f8e, 0x2f03,
    0x2e79, 0x2df1, 0x2d6a, 0x2ce5, 0x2c62, 0x2bdf, 0x2b5f, 0x2ae0,
    0x2a62, 0x29e6, 0x296b, 0x28f2, 0x287a, 0x2803, 0x278e, 0x271a,
    0x26a7, 0x2636, 0x25c6, 0x2558, 0x24ea, 0x247e, 0x2413, 0x23a9,
    0x2341, 0x22d9, 0x2273, 0x220e, 0x21ab, 0x2148, 0x20e6, 0x2086,
    0x2027, 0x1fc8, 0x1f6b, 0x1f0f, 0x1eb4, 0x1e5a, 0x1e01, 0x1da9,
    0x1d52, 0x1cfd, 0x1ca8, 0x1c54, 0x1c01, 0x1baf, 0x1b5d, 0x1b0d,
    0x1abe, 0x1a70, 0x1a22, 0x19d6, 0x198a, 0x193f, 0x18f5, 0x18ac,
    0x1864, 0x181c, 0x17d6, 0x1790, 0x174b, 0x1706, 0x16c3, 0x1680,
    0x163e, 0x15fd, 0x15bd, 0x157d, 0x153e, 0x1500, 0x14c2, 0x1485,
    0x1449, 0x140e, 0x13d3, 0x1399, 0x135f, 0x1327, 0x12ef, 0x12b7,
    0x1280, 0x124a, 0x1214, 0x11df, 0x11ab, 0x1177, 0x1144, 0x1112,
    0x10e0, 0x10ae, 0x107d, 0x104d, 0x101d, 0x0fee, 0x0fbf, 0x0f91,
    0x0f63, 0x0f36, 0x0f0a, 0x0ede, 0x0eb2, 0x0e87, 0x0e5d, 0x0e32,
    0x0e09, 0x0de0, 0x0db7, 0x0d8f, 0x0d67, 0x0d40, 0x0d19, 0x0cf3,
    0x0ccd, 0x0ca7, 0x0c82, 0x0c5d, 0x0c39, 0x0c15, 0x0bf2, 0x0bcf,
    0x0bac, 0x0b8a, 0x0b68, 0x0b47, 0x0b26, 0x0b05, 0x0ae5, 0x0ac5,
    0x0aa5, 0x0a86, 0x0a67, 0x0a49, 0x0a2b, 0x0a0d, 0x09f0, 0x09d2,
    0x09b6, 0x0999, 0x097d, 0x0961, 0x0946, 0x092b, 0x0910, 0x08f5,
    0x08db, 0x08c1, 0x08a7, 0x088e, 0x0875, 0x085c, 0x0844, 0x082b,
    0x0813, 0x0000,
};

u32 anc_alg_todo_flag = 0;
anc_alg_param_cb ada_param_cb AT(.anc_data.ada_param_cb);
static anc_dma_cfg_cb anc_dma_cfg AT(.anc_data.comm_param);
static sdadc_cfg_t sdadc_cfg_3ch_talk AT(.anc_data.comm_param);
#if ANC_ALG_STEREO_EN
static sdadc_cfg_t sdadc_cfg_fffb_r AT(.anc_data.comm_param);
#endif // ANC_ALG_STEREO_EN
static u8 anc_alg_sdadc_ch[MIC_NUM_MAX] AT(.anc_data.comm_param);
static u8 alg_idx[4] AT(.anc_data.comm_param);
static u8 alg_nums AT(.anc_data.comm_param);
static u16 sdadc_samples AT(.anc_data.comm_param);
static u8 anc_dma_nch AT(.anc_data.comm_param);
s16 anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FF_GAIN_MAX] = {
#if ANC_ALG_WIND_NOISE_FF_TALK_EN || ANC_ALG_AI_WN_FF_EN || ANC_ALG_WIND_NOISE_FF_FB_EN || ANC_ALG_AI_WN_DSP_FF_EN || ANC_ALG_AI_WN_MINI_FF_EN
    [ANC_ALG_ADJUST_FF_GAIN_WINDNOISE]  = 0x7FFF,
#endif
#if ANC_ALG_HOWLING_FF_EN || ANC_ALG_HOWLING_FB_EN
    [ANC_ALG_ADJUST_FF_GAIN_HOWLING]    = 0x7FFF,
#endif // ANC_ALG_HOWLING_FF_EN
#if ANC_ALG_LIMITER_FF_EN
    [ANC_ALG_ADJUST_FF_GAIN_LIMITER]    = 0x7FFF,
#endif // ANC_ALG_LIMITER_FF_EN
#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN
    [ANC_ALG_ADJUST_FF_GAIN_ASM]        = 0x7FFF,
#endif // ANC_ALG_ASM_SIM_FF_EN
#if ANC_ALG_AEM_RT_FF_FB_EN
    [ANC_ALG_ADJUST_FF_GAIN_AEM_RT]     = 0x7FFF,
#endif // ANC_ALG_AEM_RT_FF_FB_EN
};
s16 anc_alg_adjust_fb_gain[ANC_ALG_ADJUST_FB_GAIN_MAX] = {
#if ANC_ALG_HOWLING_FB_EN
    [ANC_ALG_ADJUST_FB_GAIN_HOWLING]    = 0x7FFF,
#endif // ANC_ALG_HOWLING_FB_EN
#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN
    [ANC_ALG_ADJUST_FB_GAIN_ASM]        = 0x7FFF,
#endif // ANC_ALG_ASM_SIM_FF_EN
#if ANC_ALG_MSC_ADP_FB_EN
    [ANC_ALG_ADJUST_FB_GAIN_MSC_ADP]    = 0x7FFF,
#endif // ANC_ALG_MSC_ADP_FB_EN
#if ANC_ALG_AEM_RT_FF_FB_EN
    [ANC_ALG_ADJUST_FB_GAIN_AEM_RT]     = 0x7FFF,
#endif // ANC_ALG_AEM_RT_FF_FB_EN
};
#if ANC_ALG_STEREO_EN
s16 anc_alg_adjust_ff_gain_r[ANC_ALG_ADJUST_FF_GAIN_MAX] = {
#if ANC_ALG_WIND_NOISE_FF_TALK_EN || ANC_ALG_AI_WN_FF_EN || ANC_ALG_WIND_NOISE_FF_FB_EN || ANC_ALG_AI_WN_DSP_FF_EN || ANC_ALG_AI_WN_MINI_FF_EN
    [ANC_ALG_ADJUST_FF_GAIN_WINDNOISE]  = 0x7FFF,
#endif
#if ANC_ALG_HOWLING_FF_EN || ANC_ALG_HOWLING_FB_EN
    [ANC_ALG_ADJUST_FF_GAIN_HOWLING]    = 0x7FFF,
#endif // ANC_ALG_HOWLING_FF_EN
#if ANC_ALG_LIMITER_FF_EN
    [ANC_ALG_ADJUST_FF_GAIN_LIMITER]    = 0x7FFF,
#endif // ANC_ALG_LIMITER_FF_EN
#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN
    [ANC_ALG_ADJUST_FF_GAIN_ASM]        = 0x7FFF,
#endif // ANC_ALG_ASM_SIM_FF_EN
};
s16 anc_alg_adjust_fb_gain_r[ANC_ALG_ADJUST_FB_GAIN_MAX] = {
#if ANC_ALG_HOWLING_FB_EN
    [ANC_ALG_ADJUST_FB_GAIN_HOWLING]    = 0x7FFF,
#endif // ANC_ALG_HOWLING_FB_EN
#if ANC_ALG_ASM_SIM_FF_EN || ANC_ALG_ASM_FF_EN
    [ANC_ALG_ADJUST_FB_GAIN_ASM]        = 0x7FFF,
#endif // ANC_ALG_ASM_SIM_FF_EN
#if ANC_ALG_MSC_ADP_FB_EN
    [ANC_ALG_ADJUST_FB_GAIN_MSC_ADP]    = 0x7FFF,
#endif // ANC_ALG_MSC_ADP_FB_EN
};
#endif // ANC_ALG_STEREO_EN

#define wind_noise_adjust_gain      anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FF_GAIN_WINDNOISE]
#define wind_noise_adjust_gain_r    anc_alg_adjust_ff_gain_r[ANC_ALG_ADJUST_FF_GAIN_WINDNOISE]

#if ANC_ALG_HOWLING_FB_EN
#define howling_adjust_ff_gain      anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FB_GAIN_HOWLING]
#define howling_adjust_fb_gain      anc_alg_adjust_fb_gain[ANC_ALG_ADJUST_FB_GAIN_HOWLING]
#define howling_adjust_ff_r_gain    anc_alg_adjust_ff_gain_r[ANC_ALG_ADJUST_FF_GAIN_HOWLING]
#define howling_adjust_fb_r_gain    anc_alg_adjust_fb_gain_r[ANC_ALG_ADJUST_FB_GAIN_HOWLING]
#elif ANC_ALG_HOWLING_FF_EN
#define howling_adjust_ff_gain      anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FF_GAIN_HOWLING]
#define howling_adjust_ff_r_gain    anc_alg_adjust_ff_gain_r[ANC_ALG_ADJUST_FF_GAIN_HOWLING]
#endif

#define limiter_adjust_gain         anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FF_GAIN_LIMITER]

#define asm_sim_adjust_ff_gain      anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FF_GAIN_ASM]
#define asm_sim_adjust_fb_gain      anc_alg_adjust_fb_gain[ANC_ALG_ADJUST_FB_GAIN_ASM]
#define asm_sim_adjust_ff_r_gain    anc_alg_adjust_ff_gain_r[ANC_ALG_ADJUST_FF_GAIN_ASM]
#define asm_sim_adjust_fb_r_gain    anc_alg_adjust_fb_gain_r[ANC_ALG_ADJUST_FB_GAIN_ASM]

#define msc_adp_adjust_fb_gain      anc_alg_adjust_fb_gain[ANC_ALG_ADJUST_FB_GAIN_MSC_ADP]
#define msc_adp_adjust_fb_r_gain    anc_alg_adjust_fb_gain_r[ANC_ALG_ADJUST_FB_GAIN_MSC_ADP]

#define msc_aem_adjust_ff_gain      anc_alg_adjust_ff_gain[ANC_ALG_ADJUST_FF_GAIN_AEM_RT]
#define msc_aem_adjust_fb_gain      anc_alg_adjust_fb_gain[ANC_ALG_ADJUST_FB_GAIN_AEM_RT]

#if ANC_ALG_WIND_NOISE_FF_TALK_EN || ANC_ALG_AI_WN_FF_EN || ANC_ALG_WIND_NOISE_FF_FB_EN || ANC_ALG_AI_WN_DSP_FF_EN || ANC_ALG_AI_WN_MINI_FF_EN
#define WIND_LEVEL_1_GAIN           (-6)        //(0dB ~ -24dB)
#define WIND_LEVEL_2_GAIN           (-12)       //(0dB ~ -24dB)

#if (WIND_LEVEL_1_GAIN < -24 || WIND_LEVEL_1_GAIN > 0) || (WIND_LEVEL_2_GAIN < -24 || WIND_LEVEL_2_GAIN > 0)
#error "WIND_LEVEL_x_GAIN overflow"
#endif
#endif

#if ANC_ALG_DYVOL_FF_EN
s8 dyvol_vol_change;
#if ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
u16 dyvol_vol_gain = 0x7fff;
u8 dyvol_en = 0;
#endif // ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
#endif // ANC_ALG_DYVOL_FF_EN

#if ANC_ALG_ADP_EQ_FF_FB_EN
s16 adp_eq_leak_tun;
s16 adp_eq_msc_tun;
#endif // ANC_ALG_ADP_EQ_FF_FB_EN

#if ANC_ALG_MSC_ADP_FB_EN
s16 msc_adp_msc_tun;
#if ANC_ALG_STEREO_EN
s16 msc_adp_r_msc_tun;
#endif // ANC_ALG_STEREO_EN
#endif // ANC_ALG_MSC_ADP_FB_EN

#if ANC_ALG_AEM_RT_FF_FB_EN
u8 aem_rt_idx_num;
s16 aem_cvt_ff_gain;
s16 aem_cvt_fb_gain;
s16 aem_cvt_msc_gain;
#endif // ANC_ALG_AEM_RT_FF_FB_EN


///******************* 公共部分 *******************///

void anc_alg_adjust_ff_gain_set_vol(u8 step, u8 direct_set, u8 fade_step)
{
    u16 vol = 0x7FFF;
    for (int i = 0; i < ANC_ALG_ADJUST_FF_GAIN_MAX; i++) {
        if (anc_alg_adjust_ff_gain[i] < vol) {
            vol = anc_alg_adjust_ff_gain[i];
        }
    }
    anc_vol_set(0, vol, step, direct_set, fade_step);
    TRACE("anc_alg_adjust_ff_gain_set_vol %x\n", vol);
}

void anc_alg_adjust_fb_gain_set_vol(u8 step, u8 direct_set, u8 fade_step)
{
    u16 vol = 0x7FFF;
    for (int i = 0; i < ANC_ALG_ADJUST_FB_GAIN_MAX; i++) {
        if (anc_alg_adjust_fb_gain[i] < vol) {
            vol = anc_alg_adjust_fb_gain[i];
        }
    }
#if ANC_MAX_VOL_DIS_FB_EN
    if (bsp_anc_max_vol_dis_fb_get_sta()) {
        vol = 0;
    }
#endif // ANC_MAX_VOL_DIS_FB_EN
    anc_vol_set(1, vol, step, direct_set, fade_step);
    TRACE("anc_alg_adjust_fb_gain_set_vol %x\n", vol);
}

#if ANC_ALG_STEREO_EN
void anc_alg_adjust_ff_r_gain_set_vol(u8 step, u8 direct_set, u8 fade_step)
{
    u16 vol = 0x7FFF;
    for (int i = 0; i < ANC_ALG_ADJUST_FF_GAIN_MAX; i++) {
        if (anc_alg_adjust_ff_gain_r[i] < vol) {
            vol = anc_alg_adjust_ff_gain_r[i];
        }
    }
    anc_vol_set(2, vol, step, direct_set, fade_step);
    TRACE("anc_alg_adjust_ff_r_gain_set_vol %x\n", vol);
}

void anc_alg_adjust_fb_r_gain_set_vol(u8 step, u8 direct_set, u8 fade_step)
{
    u16 vol = 0x7FFF;
    for (int i = 0; i < ANC_ALG_ADJUST_FB_GAIN_MAX; i++) {
        if (anc_alg_adjust_fb_gain_r[i] < vol) {
            vol = anc_alg_adjust_fb_gain_r[i];
        }
    }
#if ANC_MAX_VOL_DIS_FB_EN
    if (bsp_anc_max_vol_dis_fb_get_sta()) {
        vol = 0;
    }
#endif // ANC_MAX_VOL_DIS_FB_EN
    anc_vol_set(3, vol, step, direct_set, fade_step);
    TRACE("anc_alg_adjust_fb_r_gain_set_vol %x\n", vol);
}
#endif // ANC_ALG_STEREO_EN

s16 anc_alg_get_gain(u8 ch)
{
    s16 gain_min = 0x7FFF;

    if (ch == 0) {
        for (int i = 0; i < ANC_ALG_ADJUST_FF_GAIN_MAX; i++) {
            if (anc_alg_adjust_ff_gain[i] < gain_min) {
                gain_min = anc_alg_adjust_ff_gain[i];
            }
        }
    } else if (ch == 1) {
        for (int i = 0; i < ANC_ALG_ADJUST_FB_GAIN_MAX; i++) {
            if (anc_alg_adjust_fb_gain[i] < gain_min) {
                gain_min = anc_alg_adjust_fb_gain[i];
            }
        }
    }

#if ANC_ALG_STEREO_EN
    if (ch == 2) {
        for (int i = 0; i < ANC_ALG_ADJUST_FF_GAIN_MAX; i++) {
            if (anc_alg_adjust_ff_gain_r[i] < gain_min) {
                gain_min = anc_alg_adjust_ff_gain_r[i];
            }
        }
    } else if (ch == 3) {
        for (int i = 0; i < ANC_ALG_ADJUST_FB_GAIN_MAX; i++) {
            if (anc_alg_adjust_fb_gain_r[i] < gain_min) {
                gain_min = anc_alg_adjust_fb_gain_r[i];
            }
        }
    }
#endif // ANC_ALG_STEREO_EN

    return gain_min;
}

static s8 get_anc_alg_param_idx_by_type(u8 type)
{
    int total = sizeof(anc_alg_comm_param_tbl) / sizeof(anc_alg_comm_param_cb);
//    printf("total %d %d %d\n", total, sizeof(anc_alg_comm_param_tbl), sizeof(anc_alg_comm_param_cb));
    for (int i = 0; i < total; i++) {
        if (type == anc_alg_comm_param_tbl[i].type) {
            return i;
        }
    }
    return -1;
}

static void anc_alg_sdadc_gain_set(sdadc_cfg_t* cfg)
{
    //模拟增益
    cfg->anl_gain = ((xcfg_cb.mic0_anl_gain)     |
                     (xcfg_cb.mic1_anl_gain<<6)  |
                     (xcfg_cb.mic2_anl_gain<<12) |
                     (xcfg_cb.mic3_anl_gain<<18) |
                     (xcfg_cb.mic4_anl_gain<<24));
    //数字增益
    if (xcfg_cb.anc_alg_dgain_en) {
        cfg->dig_gain = ((xcfg_cb.anc_mic0_dig_gain)      |
                         (xcfg_cb.anc_mic1_dig_gain<<6)   |
                         (xcfg_cb.anc_mic2_dig_gain<<12)  |
                         (xcfg_cb.anc_mic3_dig_gain<<18)  |
                         (xcfg_cb.anc_mic4_dig_gain<<24));
    } else {
        cfg->dig_gain = ((xcfg_cb.bt_mic0_dig_gain)       |
                         (xcfg_cb.bt_mic1_dig_gain<<6)    |
                         (xcfg_cb.bt_mic2_dig_gain<<12)   |
                         (xcfg_cb.bt_mic3_dig_gain<<18)   |
                         (xcfg_cb.bt_mic4_dig_gain<<24));
    }
}

bool anc_alg_get_type_sta(u32 type, u8 alg_type)
{
    bool res = false;
    for (int i = 0; i < 4; i++) {
        if (((type >> (8 * i)) & 0xFF) == alg_type) {
            res = true;
        }
    }

    return res;
}

u32 bsp_anc_alg_get_type(void)
{
    if (sys_cb.anc_alg_en == 0) {
        return 0;
    }
    u32* tptr = (u32*)ada_param_cb.type;
    u32 type_all = *tptr;
    return type_all;
}

bool bsp_anc_alg_get_sta_by_type(u8 type)
{
    if (sys_cb.anc_alg_en == 0) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        if (ada_param_cb.type[i] == type) {
            return true;
        }
    }
    return false;
}

bool bsp_anc_alg_add_alg_to_type(u32* input_type, u8 type)
{
    u8 num = 0;
    u8 temp_type;

    if (type == 0) {
        return true;
    }

    for (int i = 0; i < 4; i++) {
        temp_type = (u8)((*input_type) >> (i * 8)) & 0xFF;
        if (temp_type) {
            num++;
            if (temp_type == type) {
                return true;        //已存在，直接返回
            }
        }
    }

    if (num >= 4) {
        return false;               //已满
    } else {
        (*input_type) |= (type << (8 * num));
        return true;
    }
}

bool bsp_anc_alg_remove_alg_from_type(u32* input_type, u8 type)
{
    u32 new_type = 0;
    u8 temp_type = 0;
    u8 new_alg_nums = 0;

    if (type == 0) {
        return true;
    }

    for (int i = 0; i < 4; i++) {
        temp_type = (u8)((*input_type) >> (i * 8)) & 0xFF;
        if ((temp_type != type) && (temp_type != 0)) {
            new_type |= (temp_type << (8 * new_alg_nums));
            new_alg_nums++;
        }
    }

    (*input_type) = new_type;

    return true;
}

static u8 sdadc_get_ch_by_mic_mapping(u8 mic_mapping)
{
    if (mic_mapping >= 5) {
        return 0;
    }

    u8 mic_mapping_tbl[5] = {CH_MIC0, CH_MIC1, CH_MIC2, CH_MIC3, CH_MIC4};

    //这里注意要和 bsp_audio.c 的代码同步
#if ANC_EN
    if ((xcfg_cb.anc_en) && (xcfg_cb.anc_mode != MODE_HYBRID)) {        //如果打开anc，重定义mic->adc路径
        mic_mapping_tbl[0] = ADC2 << 4 | MIC0;
        mic_mapping_tbl[1] = ADC3 << 4 | MIC1;
        mic_mapping_tbl[2] = ADC0 << 4 | MIC2;
        mic_mapping_tbl[3] = ADC1 << 4 | MIC3;
    }
#endif

    return mic_mapping_tbl[mic_mapping];
}

#if ANC_DS_DMA_EN
u8 anc_alg_init_get_anc_dma_nch(void)
{
    return anc_dma_nch;
}
#endif // ANC_DS_DMA_EN

//开启ANC算法
void bsp_anc_alg_start(u32 type)
{
#if BT_A2DP_LHDC_AUDIO_EN
    if (bt_decode_is_lhdc()) {
        return;
    }
#endif // BT_A2DP_LHDC_AUDIO_EN

#if BT_A2DP_LDAC_AUDIO_EN
    if (bt_decode_is_ldac()) {
        return;
    }
#endif // BT_A2DP_LDAC_AUDIO_EN

    if (type == 0) {
        return;
    }

    anc_alg_enter_critical();

    if (sys_cb.anc_alg_en == 1) {
        anc_alg_exit_critical();
        return;
    }

    if (sco_is_connected()) {
        anc_alg_exit_critical();
        return;
    }

    //算法个数计算
    anc_alg_ram_clear();
    alg_nums = 0;
    for (int i = 0; i < 4; i++) {
        if ((type >> (8 * i)) & 0xFF) {
            ada_param_cb.type[alg_nums] = (u8)((type >> (8 * i)) & 0xFF);
            s8 res = get_anc_alg_param_idx_by_type(ada_param_cb.type[alg_nums]);
            if (res < 0) {
                printf("ANC ALG ERROR: %s: %d error!\n", __func__, __LINE__);
                anc_alg_exit_critical();
                return;
            }
            alg_idx[alg_nums] = (u8)res;
            alg_nums++;
        } else {
            break;
        }
    }
    TRACE("ANC ALG nums: %d\n", alg_nums);
    for (int i = 0; i < alg_nums; i++) {
        TRACE("alg_idx[%d]: %d\n", i, alg_idx[i]);
    }

    //算法公共参数初始化
    memset(anc_alg_sdadc_ch, 0, sizeof(anc_alg_sdadc_ch));
    sdadc_samples = anc_alg_comm_param_tbl[alg_idx[0]].sdadc_samples;
    u8 sdadc_nch = 0;
    u8 ch_temp;
    bool anc_dma_en = false;
    for (int i = 0; i < alg_nums; i++) {
        ada_param_cb.alg_param[i] = anc_alg_comm_param_tbl[alg_idx[i]].alg_param;       //alg_param
        ada_param_cb.resv[i] = anc_alg_comm_param_tbl[alg_idx[i]].alg_samples;          //resv <-> alg_samples
        ch_temp = SDADC_GET_CH0(anc_alg_comm_param_tbl[alg_idx[i]].sdadc_ch);
        if (ch_temp) {
            if ((ada_param_cb.mic_cfg & BIT(ch_temp - 1)) == 0) {
                ada_param_cb.mic_cfg |= BIT(ch_temp - 1);                               //mic_cfg
            }
#if ANC_ALG_STEREO_EN
            if ((anc_alg_comm_param_tbl[alg_idx[i]].mic_ch_cfg) && (ch_temp != ANC_ALG_MIC_TALK)) {
                ada_param_cb.mic_cfg |= BIT(ch_temp - 1 + 3);                           //FF_R or FB_R
            }
#endif // ANC_ALG_STEREO_EN
        }
        ch_temp = SDADC_GET_CH1(anc_alg_comm_param_tbl[alg_idx[i]].sdadc_ch);
        if (ch_temp) {
            if ((ada_param_cb.mic_cfg & BIT(ch_temp - 1)) == 0) {
                ada_param_cb.mic_cfg |= BIT(ch_temp - 1);
            }
#if ANC_ALG_STEREO_EN
            if ((anc_alg_comm_param_tbl[alg_idx[i]].mic_ch_cfg) && (ch_temp != ANC_ALG_MIC_TALK)) {
                ada_param_cb.mic_cfg |= BIT(ch_temp - 1 + 3);                           //FF_R or FB_R
            }
#endif // ANC_ALG_STEREO_EN
        }
        if (anc_alg_comm_param_tbl[alg_idx[i]].anc_dma) {                               //anc_dma
            anc_dma_en = true;
        }
        if (sdadc_samples > anc_alg_comm_param_tbl[alg_idx[i]].sdadc_samples) {         //sdadc_samples取最小的那个
            sdadc_samples = anc_alg_comm_param_tbl[alg_idx[i]].sdadc_samples;
        }
        TRACE("ada_param_cb type[%d]: %x alg_param %08x alg_samples %d\n",
              i, ada_param_cb.type[i], ada_param_cb.alg_param[i], ada_param_cb.resv[i]);
    }
    anc_dma_nch = anc_dma_en ? 2 : 0;                                                   //anc_dma_nch
    sdadc_nch = s_bcnt(ada_param_cb.mic_cfg);                                           //sdadc_nch
    ada_param_cb.alg_nch = sdadc_nch;                                                   //alg_nch <-> sdadc_nch
#if ANC_ALG_DUMP_FOR_ANC_MODE
    ada_param_cb.mic_cfg = 0x7;
    sdadc_nch = ada_param_cb.alg_nch = 3;
#endif // ANC_ALG_DUMP_FOR_ANC_MODE
    for (int i = 0; i < MIC_NUM_MAX; i++) {
        if (ada_param_cb.mic_cfg & BIT(i)) {
            anc_alg_sdadc_ch[i] = sdadc_get_ch_by_mic_mapping(sdadc_ch_tbl[i]);         //根据 mic_cfg 对应放置
        }
    }

    for (int i = 1; i < alg_nums; i++) {
        if (anc_alg_comm_param_tbl[alg_idx[i]].pcm_interleave != anc_alg_comm_param_tbl[alg_idx[i - 1]].pcm_interleave) {
            printf("ANC ALG ERROR: %s: %d error!\n", __func__, __LINE__);
            anc_alg_exit_critical();
            return;
        }
    }
    ada_param_cb.pcm_interleave = anc_alg_comm_param_tbl[alg_idx[0]].pcm_interleave;    //pcm_interleave参数由第一个type的确定

    ada_param_cb.start_delay = 0;                                                       //这些参数手动配置
#if ANC_ALG_DUMP_EN
    ada_param_cb.dump_en = ANC_ALG_DUMP_DATA_TYPE;
#else
    ada_param_cb.dump_en = 0;
#endif // ANC_ALG_DUMP_EN

    TRACE("anc_alg_sdadc_ch:   0x%x 0x%x 0x%x 0x%x 0x%x\n", anc_alg_sdadc_ch[0], anc_alg_sdadc_ch[1], anc_alg_sdadc_ch[2], anc_alg_sdadc_ch[3], anc_alg_sdadc_ch[4]);
    TRACE("sdadc_samples:      %d\n", sdadc_samples);
    TRACE("mic_cfg:            %d\n", ada_param_cb.mic_cfg);
    TRACE("pcm_interleave:     %d\n", ada_param_cb.pcm_interleave);
    TRACE("sdadc_nch:          %d\n", ada_param_cb.alg_nch);
    TRACE("anc_dma_nch:        %d\n", anc_dma_nch);
    TRACE("dump_en:            0x%x\n", ada_param_cb.dump_en);
    TRACE("start_delay:        %d\n", ada_param_cb.start_delay);

    //算法初始化
    if (anc_alg_init(&ada_param_cb) < 0) {
        printf("ANC ALG ERROR: %s: %d error!\n", __func__, __LINE__);
        anc_alg_exit_critical();
        return;
    }

    //调整主频
    if (alg_nums == 1) {
#if (ANC_ALG_HOWLING_FB_EN || ANC_ALG_HOWLING_FF_EN) && (HOWLING_LEN < 256)
        if (anc_alg_get_type_sta(type, ANC_ALG_HOWLING_FB) || anc_alg_get_type_sta(type, ANC_ALG_HOWLING_FF)) {
            sys_clk_req(INDEX_ANC, SYS_60M);
        } else
#endif
        {
            sys_clk_req(INDEX_ANC, SYS_48M);
        }
    } else if (alg_nums == 2) {
        sys_clk_req(INDEX_ANC, SYS_60M);
    } else if (alg_nums >= 3) {
        sys_clk_req(INDEX_ANC, SYS_80M);
    }

    //SDADC初始化
    audio_path_init(AUDIO_PATH_ANC_ALG);
    audio_path_start(AUDIO_PATH_ANC_ALG);
    memset(&sdadc_cfg_3ch_talk, 0, sizeof(sdadc_cfg_t));
    if ((ada_param_cb.alg_nch >= 3) && ((ada_param_cb.mic_cfg & 0x7) == 0x7)) { //FF FB TALK同时使用的时候
        //3ch TALK
        sdadc_cfg_3ch_talk.channel = (u16)anc_alg_sdadc_ch[MIC_CFG_IDX_TALK];
        sdadc_cfg_3ch_talk.samples = sdadc_samples;
        anc_alg_sdadc_gain_set(&sdadc_cfg_3ch_talk);
        sdadc_cfg_3ch_talk.out_ctrl = ADC_ALIGN_EN;      //DMA同步启动
        sdadc_cfg_3ch_talk.sample_rate = SPR_16000;
        sdadc_cfg_3ch_talk.bits_mode = 1;                //16bit
        sdadc_cfg_3ch_talk.callback = anc_alg_sdadc_process_3ch_talk;
        sdadc_init(&sdadc_cfg_3ch_talk);
        sdadc_start(sdadc_cfg_3ch_talk.channel);
    }
#if ANC_ALG_STEREO_EN
    memset(&sdadc_cfg_fffb_r, 0, sizeof(sdadc_cfg_fffb_r));
    if ((ada_param_cb.mic_cfg & BIT(MIC_CFG_IDX_FF_R)) || (ada_param_cb.mic_cfg & BIT(MIC_CFG_IDX_FB_R))) {
        //FF_R FB_R
        u16 channel = 0;
        u8 nch = 0;
        for (int i = 0; i < 2; i++) {
            if (ada_param_cb.mic_cfg & BIT(i + MIC_CFG_IDX_FF_R)) {
                channel |= (anc_alg_sdadc_ch[i + MIC_CFG_IDX_FF_R] << (nch * 8));
                nch++;
                if (nch >= 2) {
                    break;
                }
            }
        }
        sdadc_cfg_fffb_r.channel = channel;
        sdadc_cfg_fffb_r.samples = sdadc_samples;
        anc_alg_sdadc_gain_set(&sdadc_cfg_fffb_r);
        sdadc_cfg_fffb_r.out_ctrl = ADC_ALIGN_EN;       //DMA同步启动
        sdadc_cfg_fffb_r.sample_rate = SPR_16000;
        sdadc_cfg_fffb_r.bits_mode = 1;                 //16bit
        sdadc_cfg_fffb_r.callback = anc_alg_sdadc_process_fffb_r;
        sdadc_init(&sdadc_cfg_fffb_r);
        sdadc_start(sdadc_cfg_fffb_r.channel);
    }
#endif // ANC_ALG_STEREO_EN

    //ANC DMA初始化
#if ANC_DS_DMA_EN
    if (anc_dma_nch) {
        memset(&anc_dma_cfg, 0, sizeof(anc_dma_cfg_cb));
        anc_dma_cfg.bits_mode = 1;
        anc_dma_cfg.callback = anc_alg_ancdma_process;
        anc_dma_cfg.samples = sdadc_samples;
        anc_dma_cfg.sample_rate = SPR_16000;
        anc_dma_cfg.channel = (ANC_DS_DATA_DACL_SPK | (ANC_DS_DATA_ANC0_FF_DOUT << 8));
#if (ANC_ALG_HOWLING_FF_EN || ANC_ALG_HOWLING_FB_EN) && ANC_ALG_STEREO_EN
        if (anc_alg_get_type_sta(type, ANC_ALG_HOWLING_FB)) {
            anc_dma_cfg.channel = (ANC_DS_DATA_ANC1_FF_DOUT | (ANC_DS_DATA_ANC0_FF_DOUT << 8));
        }
#endif
#if ANC_ALG_WIND_NOISE_FF_FB_EN && ANC_ALG_STEREO_EN
        if (anc_alg_get_type_sta(type, ANC_ALG_WIND_NOISE_FF_FB)) {
            anc_dma_cfg.channel = (ANC_DS_DATA_DACL_SPK | (ANC_DS_DATA_DACR_SPK << 8));
        }
#endif
        anc_dma_cfg.dig_gain = 0;
        anc_dma_cfg.resv = 0;
        anc_dma_start(&anc_dma_cfg, 0);
    }
#endif // ANC_DS_DMA_EN

#if ANC_ALG_STEREO_EN
    bool dma_enable_meanwhile = (bool)(anc_dma_nch || sdadc_cfg_3ch_talk.channel || sdadc_cfg_fffb_r.channel);
#else
    bool dma_enable_meanwhile = (bool)(anc_dma_nch || sdadc_cfg_3ch_talk.channel);
#endif // ANC_ALG_STEREO_EN

    if (dma_enable_meanwhile) {
        u32 adc_ch = 0;
        u8 nch = 0;
        for (int i = 0; i < MIC_NUM_MAX; i++) {
            if (anc_alg_sdadc_ch[i]) {
                adc_ch |= ((u32)((anc_alg_sdadc_ch[i] >> 4) & 0xFF) << (4 * nch));
                nch++;
                if (nch >= ada_param_cb.alg_nch) {
                    break;
                }
            }
        }
//        printf("adc_ch %x\n", adc_ch);
        sdadc_channel_enable_meanwhile(adc_ch, anc_dma_cfg.channel);
    }

    sys_cb.anc_alg_en = 1;

    TRACE("ANC alg: start\n");
    anc_alg_exit_critical();
}

//关闭ANC算法
void bsp_anc_alg_stop(void)
{
    anc_alg_enter_critical();

    if (sys_cb.anc_alg_en == 0) {
        anc_alg_exit_critical();
        return;
    }

    sys_cb.anc_alg_en = 0;

    //算法关闭
    anc_alg_exit(&ada_param_cb);

    //ANC DMA关闭
#if ANC_DS_DMA_EN
    if (anc_dma_nch) {
        anc_dma_exit();
    }
#endif // ANC_DS_DMA_EN

    //SDADC关闭
    audio_path_exit(AUDIO_PATH_ANC_ALG);
    if (sdadc_cfg_3ch_talk.channel) {
        sdadc_exit(sdadc_cfg_3ch_talk.channel);
        memset(&sdadc_cfg_3ch_talk, 0, sizeof(sdadc_cfg_t));
    }
#if ANC_ALG_STEREO_EN
    if (sdadc_cfg_fffb_r.channel) {
        sdadc_exit(sdadc_cfg_fffb_r.channel);
        memset(&sdadc_cfg_fffb_r, 0, sizeof(sdadc_cfg_t));
    }
#endif // ANC_ALG_STEREO_EN

    //调整主频
    sys_clk_free(INDEX_ANC);

    TRACE("ANC alg: stop\n");
    anc_alg_exit_critical();
}

//ANC算法主线程process
NO_INLINE void anc_alg_process_do(void)
{
    //FB防啸叫
#if ANC_ALG_HOWLING_FB_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FB_SET0)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FB_SET0);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
        TRACE("howling_adjust_gain: %d %d 0\n", howling_adjust_ff_gain, howling_adjust_fb_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FB_SET1)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FB_SET1);
        anc_alg_adjust_ff_gain_set_vol(0, 1, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 1, 12);
        TRACE("howling_adjust_gain: %d %d 1\n", howling_adjust_ff_gain, howling_adjust_fb_gain);
    }
#if ANC_ALG_STEREO_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FB_SET0_R)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FB_SET0_R);
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
        TRACE("howling_adjust_gain R: %d %d 0\n", howling_adjust_ff_r_gain, howling_adjust_fb_r_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FB_SET1_R)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FB_SET1_R);
        anc_alg_adjust_ff_r_gain_set_vol(0, 1, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 1, 12);
        TRACE("howling_adjust_gain R: %d %d 1\n", howling_adjust_ff_r_gain, howling_adjust_fb_r_gain);
    }
#endif // ANC_ALG_STEREO_EN
#endif // ANC_ALG_HOWLING_FB_EN
    //FF防啸叫
#if ANC_ALG_HOWLING_FF_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FF_SET0)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FF_SET0);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);               //回gain fade
        TRACE("howling_adjust_gain: 0x%x 0\n", howling_adjust_ff_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FF_SET1)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FF_SET1);
        anc_alg_adjust_ff_gain_set_vol(14, 0, 0);               //啸叫时direct set
        TRACE("howling_adjust_gain: 0x%x 1\n", howling_adjust_ff_gain);
    }
#if ANC_ALG_STEREO_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FF_SET0_R)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FF_SET0_R);
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);               //回gain fade
        TRACE("howling_adjust_gain R: 0x%x 0\n", howling_adjust_ff_r_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_HOWLING_FF_SET1_R)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_HOWLING_FF_SET1_R);
        anc_alg_adjust_ff_r_gain_set_vol(14, 0, 0);               //啸叫时direct set
        TRACE("howling_adjust_gain R: 0x%x 1\n", howling_adjust_ff_r_gain);
    }
#endif // ANC_ALG_STEREO_EN
#endif // ANC_ALG_HOWLING_FF_EN
    //FF瞬态噪声
#if ANC_ALG_LIMITER_FF_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_LIMITER_FF_SET0)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_LIMITER_FF_SET0);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);               //回gain fade
        TRACE("limiter_adjust_gain: 0x%x 0\n", limiter_adjust_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_LIMITER_FF_SET1)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_LIMITER_FF_SET1);
        anc_alg_adjust_ff_gain_set_vol(14, 0, 0);               //direct set
        TRACE("limiter_adjust_gain: 0x%x 1\n", limiter_adjust_gain);
    }
#endif // ANC_ALG_LIMITER_FF_EN
    //风噪检测
#if ANC_ALG_WIND_NOISE_FF_TALK_EN || ANC_ALG_AI_WN_FF_EN || ANC_ALG_WIND_NOISE_FF_FB_EN || ANC_ALG_AI_WN_DSP_FF_EN || ANC_ALG_AI_WN_MINI_FF_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_WIND_LEVEL_0)) {   //无风
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_WIND_LEVEL_0);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("wind_level 0: 0x%x\n", wind_noise_adjust_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_WIND_LEVEL_1)) {   //一档风
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_WIND_LEVEL_1);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("wind_level 1: 0x%x\n", wind_noise_adjust_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_WIND_LEVEL_2)) {   //二档风
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_WIND_LEVEL_2);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("wind_level 2: 0x%x\n", wind_noise_adjust_gain);
    }
#endif
    //降增噪
#if ANC_ALG_ASM_SIM_FF_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ASM_SIM_NORMAL)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ASM_SIM_NORMAL);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("asm sim level normal: 0x%x 0x%x\n", asm_sim_adjust_ff_gain, asm_sim_adjust_fb_gain);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ASM_SIM_DEEP)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ASM_SIM_DEEP);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("asm sim level deep: 0x%x 0x%x\n", asm_sim_adjust_ff_gain, asm_sim_adjust_fb_gain);
    }
#endif // ANC_ALG_ASM_SIM_FF_EN
    //动态音量
#if ANC_ALG_DYVOL_FF_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_DYVOL_FF_SET)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_DYVOL_FF_SET);
        sys_cb.dyvol_local += dyvol_vol_change;
#if BT_TWS_EN
        bt_tws_sync_setting();
        if (!bt_tws_is_slave())
#endif // BT_TWS_EN
        {
            bsp_anc_alg_dyvol_set_vol();
        }
        TRACE("dyvol change: %d %d %d %d\n", sys_cb.vol, sys_cb.dyvol_overlap, dyvol_vol_change, sys_cb.dyvol_local);
    }
#endif // ANC_ALG_DYVOL_FF_EN
    //自适应音乐补偿
#if ANC_ALG_MSC_ADP_FB_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_MSC_ADP_FB_SET)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_MSC_ADP_FB_SET);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
        anc_msc_gain_set(1, anc_pow10_cal(msc_adp_msc_tun), 0, 0, 12);
        TRACE("L msc adp set: 0x%x 0x%x\n", msc_adp_adjust_fb_gain, anc_pow10_cal(msc_adp_msc_tun));
    }
#if ANC_ALG_STEREO_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_MSC_ADP_FB_SET_R)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_MSC_ADP_FB_SET_R);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
        anc_msc_gain_set(3, anc_pow10_cal(msc_adp_r_msc_tun), 0, 0, 12);
        TRACE("R msc adp set: 0x%x 0x%x\n", msc_adp_adjust_fb_r_gain, anc_pow10_cal(msc_adp_r_msc_tun));
    }
#endif // ANC_ALG_STEREO_EN
#endif // ANC_ALG_MSC_ADP_FB_EN
    //自适应EQ
#if ANC_ALG_ADP_EQ_FF_FB_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ADP_EQ_LEAK_SET)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ADP_EQ_LEAK_SET);
        anc_adp_eq_set_gain(adp_eq_leak_tun/10, adp_eq_leak_tun%10);
        TRACE("adp eq set leak: %d.%d\n", adp_eq_leak_tun/10, adp_eq_leak_tun%10);
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ADP_EQ_MSC_SET)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ADP_EQ_MSC_SET);
        anc_msc_gain_set(1, anc_pow10_cal(adp_eq_msc_tun), 0, 0, 12);
        TRACE("adp eq set msc: %d.%d\n", adp_eq_msc_tun/10, -adp_eq_msc_tun%10);
    }
#endif // ANC_ALG_ADP_EQ_FF_FB_EN
    //环境自适应
#if ANC_ALG_ASM_FF_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ASM_LIGHT)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ASM_LIGHT);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("asm level light\n");
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ASM_NORMAL)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ASM_NORMAL);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("asm level normal\n");
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_ASM_DEEP)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_ASM_DEEP);
        anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
        anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
        anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN
        TRACE("asm level deep\n");
    }
#endif // ANC_ALG_ASM_FF_EN
    //半入耳耳道自适应
#if ANC_ALG_AEM_RT_FF_FB_EN
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_AEM_RT_SCEN_SET)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_AEM_RT_SCEN_SET);
        bsp_anc_aem_set_ff_param(aem_rt_idx_num, true, (bool)(anc_alg_todo_flag & BIT(ANC_ALG_TODO_AEM_RT_CVT_SET)));
    }
    if (anc_alg_todo_flag & BIT(ANC_ALG_TODO_AEM_RT_CVT_SET)) {
        anc_alg_todo_flag &= ~BIT(ANC_ALG_TODO_AEM_RT_CVT_SET);
        bsp_anc_alg_aem_rt_set_gain();
    }
#endif // ANC_ALG_AEM_RT_FF_FB_EN
}

//ANC算法主线程process
AT(.com_text.anc.alg_process)
void anc_alg_process(void)
{
    if (sys_cb.anc_alg_en && anc_alg_todo_flag) {
        anc_alg_process_do();
    }
}

//ANC算法初始化回调函数
bool anc_alg_init_callback(u8 type)
{
#if ANC_ALG_WIND_NOISE_FF_TALK_EN
    if (type == ANC_ALG_WIND_NOISE_FF_TALK) {
        u8 ch = (WIND_NOISE_DM_FF_MIC == ANC_ALG_MIC_FF) ? 0 : 1;
        return alg_anc_windnoise_ff_talk_set(wind_noise_ff_talk_buf, sizeof(wind_noise_ff_talk_buf), ch);
    }
#endif // ANC_ALG_WIND_NOISE_FF_TALK_EN

#if ANC_ALG_WIND_NOISE_FF_FB_EN
    if (type == ANC_ALG_WIND_NOISE_FF_FB) {
        return alg_anc_windnoise_ff_fb_set(wind_noise_ff_fb_buf, sizeof(wind_noise_ff_fb_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_WIND_NOISE_FF_FB_EN

#if ANC_ALG_ASM_SIM_FF_EN
    if (type == ANC_ALG_ASM_SIM_FF) {
        return alg_anc_asm_simple_set(asm_sim_ff_buf, sizeof(asm_sim_ff_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_ASM_SIM_FF_EN

#if ANC_ALG_HOWLING_FB_EN
    if (type == ANC_ALG_HOWLING_FB) {
        return alg_anc_howling_fb_set(howling_fb_buf, sizeof(howling_fb_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_HOWLING_FB_EN

#if ANC_ALG_FIT_DETECT_FF_FB_EN
    if (type == ANC_ALG_FIT_DETECT_FF_FB) {
        return alg_anc_fit_detect_ff_fb_set(fit_detect_buf, sizeof(fit_detect_buf), 1);
    }
#endif // ANC_ALG_FIT_DETECT_FF_FB_EN

#if ANC_ALG_HOWLING_FF_EN
    if (type == ANC_ALG_HOWLING_FF) {
        return alg_anc_howling_ff_set(howling_ff_buf, sizeof(howling_ff_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_HOWLING_FF_EN

#if ANC_ALG_AI_WN_FF_EN || ANC_ALG_AI_WN_DSP_FF_EN
    if (type == ANC_ALG_AI_WN_FF) {
#if ANC_ALG_AI_WN_FF_EN
        return alg_anc_ai_wn_ff_set(NULL, 0, (ANC_ALG_STEREO_EN + 1));
#else
        return alg_anc_ai_wn_ff_set(ai_wn_ff_buf, sizeof(ai_wn_ff_buf), (ANC_ALG_STEREO_EN + 1));
#endif // ANC_ALG_AI_WN_FF_EN
    }
#endif // ANC_ALG_AI_WN_FF_EN

#if ANC_ALG_MSC_ADP_FB_EN
    if (type == ANC_ALG_MSC_ADP_FB) {
        return alg_anc_msc_adp_fb_set(msc_adp_fb_buf, sizeof(msc_adp_fb_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_MSC_ADP_FB_EN

#if ANC_ALG_ADP_EQ_FF_FB_EN
    if (type == ANC_ALG_ADP_EQ_FF_FB) {
        return alg_anc_adp_eq_ff_fb_set(adp_eq_ff_fb_buf, sizeof(adp_eq_ff_fb_buf), 1);
    }
#endif // ANC_ALG_ADP_EQ_FF_FB_EN

#if ANC_ALG_ASM_FF_EN
    if (type == ANC_ALG_ASM_FF) {
        return alg_anc_asm_set(asm_sim_ff_buf, sizeof(asm_sim_ff_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_ASM_FF_EN

#if ANC_ALG_AI_WN_MINI_FF_EN
    if (type == ANC_ALG_AI_WN_MINI_FF) {
        return alg_anc_ai_wn_mini_ff_set(ai_wn_mini_ff_buf, sizeof(ai_wn_mini_ff_buf), (ANC_ALG_STEREO_EN + 1));
    }
#endif // ANC_ALG_AI_WN_MINI_FF_EN
    return true;
}

//ANC算法结果回调函数
AT(.anc_text.process.comm)
void alg_anc_process_callback(int* res, u32 len, u8 type)
{
//    //for debug
//    printf("type: %d res: ", type);
//    for (int i = 0; i < len; i++) {
//        printf("%d ", res[i]);
//    }
//    printf("\n");

#if ANC_ALG_HOWLING_FB_EN
    if (type == ANC_ALG_HOWLING_FB) {   //ff_gain  fb_gain  howling_reslut  level
//        printf("L ff_gain: %d fb_gain: %d howling_reslut: %d level: %d\n", res[0], res[1], res[2], res[3]);
        if ((res[0] <= 0) && (res[0] >= ANC_GAIN_MIN_DB) && (res[1] <= 0) && (res[1] >= ANC_GAIN_MIN_DB)) {
            s16 new_ff_gain = dvol_tbl_Q15_step0P1db[res[0] * (-10)];
            s16 new_fb_gain = dvol_tbl_Q15_step0P1db[res[1] * (-10)];
            if ((new_ff_gain != howling_adjust_ff_gain) || (new_fb_gain != howling_adjust_fb_gain)) {
                howling_adjust_ff_gain = new_ff_gain;
                howling_adjust_fb_gain = new_fb_gain;
                if (res[2]) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FB_SET1);
                } else {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FB_SET0);
                }
            }
        }
#if ANC_ALG_STEREO_EN
//        printf("R ff_gain: %d fb_gain: %d howling_reslut: %d level: %d\n", res[4], res[5], res[6], res[7]);
        if ((res[4] <= 0) && (res[4] >= ANC_GAIN_MIN_DB) && (res[5] <= 0) && (res[5] >= ANC_GAIN_MIN_DB)) {
            s16 new_ff_gain = dvol_tbl_Q15_step0P1db[res[4] * (-10)];
            s16 new_fb_gain = dvol_tbl_Q15_step0P1db[res[5] * (-10)];
            if ((new_ff_gain != howling_adjust_ff_r_gain) || (new_fb_gain != howling_adjust_fb_r_gain)) {
                howling_adjust_ff_r_gain = new_ff_gain;
                howling_adjust_fb_r_gain = new_fb_gain;
                if (res[6]) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FB_SET1_R);
                } else {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FB_SET0_R);
                }
            }
        }
#endif // ANC_ALG_STEREO_EN
    }
#endif // ANC_ALG_HOWLING_FB_EN

#if ANC_ALG_HOWLING_FF_EN
    if (type == ANC_ALG_HOWLING_FF) {   //ff_gain  fb_gain  howling_reslut  level
//        printf("L ff_gain: %d fb_gain: %d howling_reslut: %d level: %d\n", res[0], res[1], res[2], res[3]);
        if ((res[0] <= 0) && (res[0] >= ANC_GAIN_MIN_DB)) {
            s16 new_ff_gain = dvol_tbl_Q15_step0P1db[res[0] * (-10)];
            if (new_ff_gain != howling_adjust_ff_gain) {
                howling_adjust_ff_gain = new_ff_gain;
                if (res[2]) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FF_SET1);
                } else {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FF_SET0);
                }
            }
        }
#if ANC_ALG_STEREO_EN
//        printf("R ff_gain: %d fb_gain: %d howling_reslut: %d level: %d\n", res[4], res[5], res[6], res[7]);
        if ((res[4] <= 0) && (res[4] >= ANC_GAIN_MIN_DB) && (res[5] <= 0) && (res[5] >= ANC_GAIN_MIN_DB)) {
            s16 new_ff_gain = dvol_tbl_Q15_step0P1db[res[4] * (-10)];
            if (new_ff_gain != howling_adjust_ff_r_gain) {
                howling_adjust_ff_r_gain = new_ff_gain;
                if (res[6]) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FF_SET1_R);
                } else {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_HOWLING_FF_SET0_R);
                }
            }
        }
#endif // ANC_ALG_STEREO_EN
    }
#endif // ANC_ALG_HOWLING_FF_EN

#if ANC_ALG_LIMITER_FF_EN
    if (type == ANC_ALG_LIMITER_FF) {   //ff_gain  limiter_result
//        printf("ff_gain: %d limiter_reslut: %d \n", res[0], res[1]);
        if ((res[0] <= 0) && (res[0] >= ANC_GAIN_MIN_DB)) {
            s16 new_ff_gain = dvol_tbl_Q15_step0P1db[res[0] * (-10)];
            if (new_ff_gain != limiter_adjust_gain) {
                limiter_adjust_gain = new_ff_gain;
                if (res[1]) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_LIMITER_FF_SET1);
                } else {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_LIMITER_FF_SET0);
                }
            }
        }
    }
#endif // ANC_ALG_LIMITER_FF_EN

#if ANC_ALG_DYVOL_FF_EN
    if (type == ANC_ALG_DYVOL_FF) {   //scen_stage  vol_change  enable
//        printf("scen_stage: %d vol_change: %d enable : %d\n", res[0], res[1], res[2]);
        if (sys_cb.vol == 0) {
            if (res[2]) {
                dyvol_set_enable(0);
                sys_cb.dyvol_local = 0;
                sys_cb.dyvol_overlap = 0;
                res[0] = 0;
            }
        } else if (!res[2]) {
            dyvol_set_enable(1);
        } else if (res[0]) {
            dyvol_vol_change = res[1];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_DYVOL_FF_SET);
        }
    }
#endif // ANC_ALG_DYVOL_FF_EN

#if ANC_ALG_WIND_NOISE_FF_TALK_EN
    if (type == ANC_ALG_WIND_NOISE_FF_TALK) {   //wind_level ene_level
//        printf("wind_level: %d ene_level: %d\n", res[0], res[1]);
        s16 new_ff_gain = 0x7FFF;
        if (res[0]) {
            if (res[1] == 1) {
                new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_1_GAIN * (-10)];
            } else if (res[1] == 2) {
                new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_2_GAIN * (-10)];
            }
        }
        if (new_ff_gain != wind_noise_adjust_gain) {
            wind_noise_adjust_gain = new_ff_gain;
#if ANC_ALG_STEREO_EN
            wind_noise_adjust_gain_r = new_ff_gain;
#endif // ANC_ALG_STEREO_EN
            if ((res[0]) && (res[1] == 2)) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_2);
            } else if ((res[0]) && (res[1] == 1)) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_1);
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_0);
            }
        }
    }
#endif // ANC_ALG_WIND_NOISE_FF_TALK_EN

#if ANC_ALG_WIND_NOISE_FF_FB_EN
    if (type == ANC_ALG_WIND_NOISE_FF_FB) {   //wind_level ene_level
//        printf("wind_detect: %d %d ene_level: %d %d\n", res[0], res[2], res[1], res[3]);
#if ANC_ALG_STEREO_EN
        if (res[0] || res[2]) {
            res[0] = 1;
        }
        res[1] = max(res[1], res[3]);
#endif // ANC_ALG_STEREO_EN
        s16 new_ff_gain = 0x7FFF;
        if (res[0]) {
            if (res[1] == 1) {
                new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_1_GAIN * (-10)];
            } else if (res[1] == 2) {
                new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_2_GAIN * (-10)];
            }
        }
        if (new_ff_gain != wind_noise_adjust_gain) {
            wind_noise_adjust_gain = new_ff_gain;
#if ANC_ALG_STEREO_EN
            wind_noise_adjust_gain_r = new_ff_gain;
#endif // ANC_ALG_STEREO_EN
            if (res[0]) {
                if (res[1] == 1) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_1);
                } else if (res[1] == 2) {
                    anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_2);
                }
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_0);
            }
        }
    }
#endif // ANC_ALG_WIND_NOISE_FF_FB_EN

#if ANC_ALG_ASM_SIM_FF_EN
    if (type == ANC_ALG_ASM_SIM_FF) {    //scen_idx_out_num scen_stage asm_simple_ff_gain_out asm_simple_fb_gain_out
#if !ANC_ALG_STEREO_EN
//        printf("scen_idx_out_num: %d scen_stage: %d ff: %d fb: %d\n", res[0], res[1], res[2], res[3]);
        if (res[1]) {
            asm_sim_adjust_ff_gain = dvol_tbl_Q15_step0P1db[-res[2]];
            asm_sim_adjust_fb_gain = dvol_tbl_Q15_step0P1db[-res[3]];
            if (res[0]) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_SIM_DEEP);
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_SIM_NORMAL);
            }
        }
#else
//        printf("scen_idx_out_num L: %d scen_stage: %d ff: %d fb: %d   ", res[0], res[1], res[2], res[3]);
//        printf("scen_idx_out_num R: %d scen_stage: %d ff: %d fb: %d\n", res[4], res[5], res[6], res[7]);
        if (res[1] || res[5]) {
            if ((res[0] == 2) && (res[4] == 2)) {
                asm_sim_adjust_ff_r_gain = asm_sim_adjust_ff_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[2]] : dvol_tbl_Q15_step0P1db[-res[6]]);
                asm_sim_adjust_fb_r_gain = asm_sim_adjust_fb_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[3]] : dvol_tbl_Q15_step0P1db[-res[7]]);
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_SIM_DEEP);
            } else if ((res[0] == 1) && (res[4] == 1)) {
                asm_sim_adjust_ff_r_gain = asm_sim_adjust_ff_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[2]] : dvol_tbl_Q15_step0P1db[-res[6]]);
                asm_sim_adjust_fb_r_gain = asm_sim_adjust_fb_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[3]] : dvol_tbl_Q15_step0P1db[-res[7]]);
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_SIM_NORMAL);
            }
        }
#endif
    }
#endif // ANC_ALG_ASM_SIM_FF_EN

#if ANC_ALG_FIT_DETECT_FF_FB_EN
    if (type == ANC_ALG_FIT_DETECT_FF_FB) {     //stau fit_flag sp_val idx_num
//        printf("stau: %d fit_flag: %d sp_val: %d idx_num: %d\n", res[0], res[1], res[2], res[3]);
    }
#endif // ANC_ALG_FIT_DETECT_FF_FB_EN

#if ANC_ALG_AI_WN_FF_EN || ANC_ALG_AI_WN_DSP_FF_EN
    if (type == ANC_ALG_AI_WN_FF) {
#if ANC_ALG_STEREO_EN
//        printf("L : wind:%d prob:%d ene:%d ai_wind:%d\n", res[0], res[1], res[2], res[3]);
//        printf("R : wind:%d prob:%d ene:%d ai_wind:%d\n", res[4], res[5], res[6], res[7]);
        s16 new_ff_gain = 0x7FFF;
        u8 ai_wind_level = max(res[0], res[4]);
        if (ai_wind_level == 2) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_2_GAIN * (-10)];
        } else if (ai_wind_level == 1) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_1_GAIN * (-10)];
        }
        if (new_ff_gain != wind_noise_adjust_gain) {
            wind_noise_adjust_gain = new_ff_gain;
            wind_noise_adjust_gain_r = new_ff_gain;
            if (ai_wind_level) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_1);
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_0);
            }
        }
#else
//        printf("wind:%d prob:%d ene:%d ai_wind:%d\n", res[0], res[1], res[2], res[3]);
        s16 new_ff_gain = 0x7FFF;
        if (res[0] == 2) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_2_GAIN * (-10)];
        } else if (res[0] == 1) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_1_GAIN * (-10)];
        }
        if (new_ff_gain != wind_noise_adjust_gain) {
            wind_noise_adjust_gain = new_ff_gain;
            if (res[0]) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_1);
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_0);
            }
        }
#endif // ANC_ALG_STEREO_EN
    }
#endif // ANC_ALG_AI_WN_FF_EN

#if ANC_ALG_MSC_ADP_FB_EN
    if (type == ANC_ALG_MSC_ADP_FB) {       //out_msc_tun out_fb_tun update_fb_flag update_msc_flag
        if (res[2] || res[3]) {
//            printf("L out_msc_tun : %d, out_fb_tun : %d, update_fb_flag : %d, update_msc_flag : %d\n", res[0], res[1], res[2], res[3]);
            msc_adp_adjust_fb_gain = dvol_tbl_Q15_step0P1db[res[1]];
            msc_adp_msc_tun = -res[0];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_MSC_ADP_FB_SET);
        }
#if ANC_ALG_STEREO_EN
        if (res[6] || res[7]) {
//            printf("R out_msc_tun : %d, out_fb_tun : %d, update_fb_flag : %d, update_msc_flag : %d\n", res[4], res[5], res[6], res[7]);
            msc_adp_adjust_fb_r_gain = dvol_tbl_Q15_step0P1db[res[5]];
            msc_adp_r_msc_tun = -res[4];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_MSC_ADP_FB_SET_R);
        }
#endif // ANC_ALG_STEREO_EN
    }
#endif // ANC_ALG_MSC_ADP_FB_EN

#if ANC_ALG_ADP_EQ_FF_FB_EN
    if (type == ANC_ALG_ADP_EQ_FF_FB) {     //leak_tun_now update_leak_gain_flag msc_tun_now update_msc_gain_flag
//        printf("leak_tun : %d, leak_flag : %d, msc_tun : %d, msc_flag : %d\n", res[0], res[1], res[2], res[3]);
        if (res[1]) {
            adp_eq_leak_tun = res[0];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ADP_EQ_LEAK_SET);
        }
        if (res[3]) {
            adp_eq_msc_tun = res[2];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ADP_EQ_MSC_SET);
        }
    }
#endif // ANC_ALG_ADP_EQ_FF_FB_EN

#if ANC_ALG_ASM_FF_EN
    if (type == ANC_ALG_ASM_FF) {    //scen_idx_out_num scen_stage asm_simple_ff_gain_out asm_simple_fb_gain_out
#if !ANC_ALG_STEREO_EN
//        printf("scen_idx_out_num : %d scen_stage: %d ff: %d fb: %d\n", res[0], res[1], res[2], res[3]);
        if (res[1]) {
            asm_sim_adjust_ff_gain = dvol_tbl_Q15_step0P1db[-res[2]];
            asm_sim_adjust_fb_gain = dvol_tbl_Q15_step0P1db[-res[3]];
            if (res[0] == 2) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_DEEP);
            } else if (res[0] == 1) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_NORMAL);
            } else if (res[0] == 0) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_LIGHT);
            }
        }
#else
//        printf("scen_idx_out_num L: %d scen_stage: %d ff: %d fb: %d   ", res[0], res[1], res[2], res[3]);
//        printf("scen_idx_out_num R: %d scen_stage: %d ff: %d fb: %d\n", res[4], res[5], res[6], res[7]);
        if (res[1] || res[5]) {
            if ((res[0] == 2) && (res[4] == 2)) {
                asm_sim_adjust_ff_r_gain = asm_sim_adjust_ff_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[2]] : dvol_tbl_Q15_step0P1db[-res[6]]);
                asm_sim_adjust_fb_r_gain = asm_sim_adjust_fb_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[3]] : dvol_tbl_Q15_step0P1db[-res[7]]);
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_DEEP);
            } else if ((res[0] == 1) && (res[4] == 1)) {
                asm_sim_adjust_ff_r_gain = asm_sim_adjust_ff_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[2]] : dvol_tbl_Q15_step0P1db[-res[6]]);
                asm_sim_adjust_fb_r_gain = asm_sim_adjust_fb_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[3]] : dvol_tbl_Q15_step0P1db[-res[7]]);
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_NORMAL);
            } else if ((res[0] == 0) && (res[4] == 0)) {
                asm_sim_adjust_ff_r_gain = asm_sim_adjust_ff_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[2]] : dvol_tbl_Q15_step0P1db[-res[6]]);
                asm_sim_adjust_fb_r_gain = asm_sim_adjust_fb_gain = (res[1] ? dvol_tbl_Q15_step0P1db[-res[3]] : dvol_tbl_Q15_step0P1db[-res[7]]);
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_ASM_LIGHT);
            }
        }
#endif
    }
#endif // ANC_ALG_ASM_FF_EN

#if ANC_ALG_AEM_RT_FF_FB_EN
    if (type == ANC_ALG_AEM_RT_FF_FB) {    //idx_num sp_val self_talk_cnt scen_stage
//        printf("idx_num : %d, sp_val : %d, self_talk_cnt : %d, scen_stage : %d, cvt_ff_gain : %d, cvt_fb_gain : %d, cvt_msc_gain : %d, cvt_stage : %d\n",
//                                                                                            res[0], res[1], res[2], res[3], res[4], res[5], res[6], res[7]);
        if (res[3]) {
            aem_rt_idx_num = res[0];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_AEM_RT_SCEN_SET);
        }
        if (res[7]) {
            aem_cvt_ff_gain  = res[4];
            aem_cvt_fb_gain  = res[5];
            aem_cvt_msc_gain = res[6];
            anc_alg_todo_flag |= BIT(ANC_ALG_TODO_AEM_RT_CVT_SET);
        }
    }
#endif // ANC_ALG_AEM_RT_FF_FB_EN

#if ANC_ALG_AI_WN_MINI_FF_EN
    if (type == ANC_ALG_AI_WN_MINI_FF) {
#if ANC_ALG_STEREO_EN
//        printf("L : wind:%d prob:%d ene:%d ai_wind:%d\n", res[0], res[1], res[2], res[3]);
//        printf("R : wind:%d prob:%d ene:%d ai_wind:%d\n", res[4], res[5], res[6], res[7]);
        s16 new_ff_gain = 0x7FFF;
        u8 ai_wind_level = max(res[0], res[4]);
        if (ai_wind_level == 2) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_2_GAIN * (-10)];
        } else if (ai_wind_level == 1) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_1_GAIN * (-10)];
        }
        if (new_ff_gain != wind_noise_adjust_gain) {
            wind_noise_adjust_gain = new_ff_gain;
            wind_noise_adjust_gain_r = new_ff_gain;
            if (ai_wind_level) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_1);
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_0);
            }
        }
#else
//        printf("wind:%d prob:%d ene:%d ai_wind:%d\n", res[0], res[1], res[2], res[3]);
        s16 new_ff_gain = 0x7FFF;
        if (res[0] == 2) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_2_GAIN * (-10)];
        } else if (res[0] == 1) {
            new_ff_gain = dvol_tbl_Q15_step0P1db[WIND_LEVEL_1_GAIN * (-10)];
        }
        if (new_ff_gain != wind_noise_adjust_gain) {
            wind_noise_adjust_gain = new_ff_gain;
            if (res[0]) {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_1);
            } else {
                anc_alg_todo_flag |= BIT(ANC_ALG_TODO_WIND_LEVEL_0);
            }
        }
#endif // ANC_ALG_STEREO_EN
    }
#endif // ANC_ALG_AI_WN_MINI_FF_EN

#if ANC_SNDP_SAE_SHIELD_ADAPTER_EN
    if (type == SNDP_SAE_SHIELD_LEAK) {
//        printf("leakaga_level: %d, anc_mode: %d, frm_cnt: %d res: %d\n", res[0], res[1], res[2], res[3]);
    }
    if (type == SNDP_SAE_ADAPTER_ANC) {
//        printf("anc_mode: %d, ff_total_gain: %d, frm_cnt: %d res: %d\n", res[0], res[1], res[2], res[3]);
    }
#endif // ANC_SNDP_SAE_SHIELD_ADAPTER_EN

#if ANC_SNDP_SAE_WIND_DETECT_EN
    if (type == SNDP_SAE_WIND_DETECT) {
//       printf("wind_level: %d, frm_cnt: %d\n", res[0], res[1]);
    }
#endif // ANC_SNDP_SAE_WIND_DETECT_EN
}

void anc_alg_audio_path_cfg_set(sdadc_cfg_t* cfg)
{
    //通道和样点数
    u16 channel = 0;
    u8 nch = 0;
    for (int i = 0; i < 3; i++) {
        if (ada_param_cb.mic_cfg & BIT(i)) {
            channel |= (anc_alg_sdadc_ch[i] << (nch * 8));
            nch++;
            if (nch >= 2) {
                break;
            }
        }
    }
    cfg->channel = channel;
    cfg->samples = sdadc_samples;

    //增益设置
    anc_alg_sdadc_gain_set(cfg);

    //通路控制
    if ((anc_dma_nch) || (ada_param_cb.alg_nch > 2)) {
        cfg->out_ctrl = ADC_ALIGN_EN;       //DMA同步启动
    }

//    printf("cfg->channel %x\n", cfg->channel);
//    printf("cfg->samples %x\n", cfg->samples);
//    printf("cfg->anl_gain %x\n", cfg->anl_gain);
//    printf("cfg->dig_gain %x\n", cfg->dig_gain);
//    printf("cfg->out_ctrl %x\n", cfg->out_ctrl);
}



#define ANC_ALG_COMM_START_PROC(type)       {                                                                   \
                                                if (sys_cb.anc_alg_en) {                                        \
                                                    u32 now_type = bsp_anc_alg_get_type();                      \
                                                    u8 now_alg_nums = alg_nums;                                 \
                                                    bsp_anc_alg_stop();                                         \
                                                    now_type |= (type << (8 * now_alg_nums));                   \
                                                    bsp_anc_alg_start(now_type);                                \
                                                } else {                                                        \
                                                    bsp_anc_alg_start(type);                                    \
                                                }                                                               \
                                            }

#define ANC_ALG_COMM_STOP_PROC(type)        {                                                                   \
                                                if (alg_nums > 1) {                                             \
                                                    u32 now_type = bsp_anc_alg_get_type();                      \
                                                    u8 now_alg_nums = alg_nums;                                 \
                                                    u32 new_type = 0;                                           \
                                                    u8 new_alg_nums = 0;                                        \
                                                    u8 type_t = 0;                                              \
                                                    for (int i = 0; i < now_alg_nums; i++) {                    \
                                                        type_t = (u8)((now_type >> (8 * i)) & 0xFF);            \
                                                        if (type_t != type) {                                   \
                                                            new_type |= (type_t << (8 * new_alg_nums));         \
                                                            new_alg_nums++;                                     \
                                                        }                                                       \
                                                    }                                                           \
                                                    bsp_anc_alg_stop();                                         \
                                                    bsp_anc_alg_start(new_type);                                \
                                                } else if (alg_nums == 1) {                                     \
                                                    bsp_anc_alg_stop();                                         \
                                                }                                                               \
                                            }

#define ANC_ALG_COMM_START_CHECK()          (bt_decode_is_lhdc() || bt_decode_is_ldac() || sco_is_connected())


///******************* 自研双MIC(FF+TALK)传统特征风噪检测算法 *******************///
#if ANC_ALG_WIND_NOISE_FF_TALK_EN
void bsp_anc_alg_wn_ff_talk_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_WIND_NOISE_FF_TALK)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_WIND_NOISE_FF_TALK);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_wn_ff_talk_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_WIND_NOISE_FF_TALK)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_WIND_NOISE_FF_TALK);

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_WIND_NOISE_FF_TALK_EN


///******************* 自研双MIC(FF+FB)传统特征风噪检测算法 *******************///
#if ANC_ALG_WIND_NOISE_FF_FB_EN
void bsp_anc_alg_wn_ff_fb_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_WIND_NOISE_FF_FB)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_WIND_NOISE_FF_FB);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_wn_ff_fb_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_WIND_NOISE_FF_FB)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_WIND_NOISE_FF_FB);

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_WIND_NOISE_FF_FB_EN


///******************* 自研单MIC(FF)降增噪算法 *******************///
#if ANC_ALG_ASM_SIM_FF_EN
void bsp_anc_alg_asm_sim_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_ASM_SIM_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    asm_sim_adjust_ff_gain = 0x7FFF;
    asm_sim_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    asm_sim_adjust_ff_r_gain = 0x7FFF;
    asm_sim_adjust_fb_r_gain = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_ASM_SIM_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_asm_sim_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_ASM_SIM_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_ASM_SIM_FF);

    asm_sim_adjust_ff_gain = 0x7FFF;
    asm_sim_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    asm_sim_adjust_ff_r_gain = 0x7FFF;
    asm_sim_adjust_fb_r_gain = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_ASM_SIM_FF_EN


///******************* 自研防啸叫(FB) *******************///
#if ANC_ALG_HOWLING_FB_EN
void bsp_anc_alg_howling_fb_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_HOWLING_FB)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    howling_adjust_ff_gain = 0x7FFF;
    howling_adjust_fb_gain = 0x7FFF;
#if ANC_ALG_STEREO_EN
    howling_adjust_ff_r_gain = 0x7FFF;
    howling_adjust_fb_r_gain = 0x7FFF;
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_HOWLING_FB);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_howling_fb_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_HOWLING_FB)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_HOWLING_FB);

    howling_adjust_ff_gain = 0x7FFF;
    howling_adjust_fb_gain = 0x7FFF;
#if ANC_ALG_STEREO_EN
    howling_adjust_ff_r_gain = 0x7FFF;
    howling_adjust_fb_r_gain = 0x7FFF;
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_HOWLING_FB_EN


///******************* 自研贴合度检测(FF+FB)ANC算法 *******************///
#if ANC_ALG_FIT_DETECT_FF_FB_EN
void bsp_anc_alg_fit_detect_ff_fb_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_FIT_DETECT_FF_FB)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    ANC_ALG_COMM_START_PROC(ANC_ALG_FIT_DETECT_FF_FB);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_fit_detect_ff_fb_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_FIT_DETECT_FF_FB)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_FIT_DETECT_FF_FB);

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_FIT_DETECT_FF_FB_EN


///******************* 自研防啸叫(FF) *******************///
#if ANC_ALG_HOWLING_FF_EN
void bsp_anc_alg_howling_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_HOWLING_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    howling_adjust_ff_gain = 0x7FFF;
#if ANC_ALG_STEREO_EN
    howling_adjust_ff_r_gain = 0x7FFF;
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_HOWLING_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_howling_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_HOWLING_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_HOWLING_FF);

    howling_adjust_ff_gain = 0x7FFF;
#if ANC_ALG_STEREO_EN
    howling_adjust_ff_r_gain = 0x7FFF;
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_HOWLING_FF_EN


///******************* 自研单MIC(FF)AI风噪检测 *******************///
#if ANC_ALG_AI_WN_FF_EN || ANC_ALG_AI_WN_DSP_FF_EN
void bsp_anc_alg_ai_wn_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_AI_WN_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_AI_WN_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_ai_wn_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_AI_WN_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_AI_WN_FF);

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_AI_WN_FF_EN


///******************* 自研单MIC(FF)瞬态噪声检测 *******************///
#if ANC_ALG_LIMITER_FF_EN
void bsp_anc_alg_limiter_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_LIMITER_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    limiter_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);

    ANC_ALG_COMM_START_PROC(ANC_ALG_LIMITER_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_limiter_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_LIMITER_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_LIMITER_FF);

    limiter_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_LIMITER_FF_EN


///******************* 自研单MIC(FF)动态音量 *******************///
#if ANC_ALG_DYVOL_FF_EN
void bsp_anc_alg_dyvol_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_DYVOL_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    dyvol_vol_change = 0;
    sys_cb.dyvol_real = 0;
    sys_cb.dyvol_local = 0;
    sys_cb.dyvol_overlap = 0;
#if ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
    dyvol_vol_gain = 0x7fff;
    dyvol_en = 1;
#endif // ANC_ALG_DYVOL_FF_LOCAL_VOL_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_DYVOL_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_dyvol_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_DYVOL_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_DYVOL_FF);

#if ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
    dyvol_en = 0;
    dyvol_vol_gain = 0x7fff;
#else
    sys_cb.vol -= (sys_cb.dyvol_real - sys_cb.dyvol_overlap);
#endif // ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
    dyvol_vol_change = 0;
    sys_cb.dyvol_real = 0;
    sys_cb.dyvol_local = 0;
    sys_cb.dyvol_overlap = 0;
    bt_music_vol_change();

    TRACE("%s\n", __func__);
}

#if ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
AT(.com_text.dev_vol)
void bsp_anc_alg_dyvol_gain_cal(u16 dac_vol)
{
    if (dyvol_en) {
        dac_vol = clip(muls_shift15(dac_vol, dyvol_vol_gain), 15);
    }
    dac_src_vol_set(0, dac_vol);
}

void bsp_anc_alg_dyvol_set_local_gain(void)
{
    dyvol_vol_gain = (u16)(anc_pow10_cal(sys_cb.dyvol_real*10) >> 8);
}
#endif // ANC_ALG_DYVOL_FF_LOCAL_VOL_EN

void bsp_anc_alg_dyvol_set_vol(void)
{
    s8 vol_change;
#if BT_TWS_EN
    if (bt_tws_is_connected()) {
        vol_change = max(sys_cb.dyvol_local, sys_cb.dyvol_remote) - sys_cb.dyvol_real;
    } else {
        vol_change = sys_cb.dyvol_local - sys_cb.dyvol_real;
    }
#else
    vol_change = sys_cb.dyvol_local - sys_cb.dyvol_real;
#endif // BT_TWS_EN
    sys_cb.dyvol_real += vol_change;
#if BT_TWS_EN
    bt_tws_sync_setting();
#endif // BT_TWS_EN
#if ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
    bsp_anc_alg_dyvol_set_local_gain();
    bt_music_vol_change();
#else
    if (vol_change > 0) {
        if ((sys_cb.vol + vol_change) > VOL_MAX) {
            sys_cb.dyvol_overlap += (sys_cb.vol + vol_change - VOL_MAX);
            sys_cb.vol = VOL_MAX;
        } else {
            sys_cb.vol += vol_change;
        }
        bt_music_vol_change();
    } else {
        if ((sys_cb.dyvol_overlap + vol_change) >= 0) {
            sys_cb.dyvol_overlap += vol_change;
        } else if ((sys_cb.dyvol_overlap > 0) && (sys_cb.vol > 1)) {
            sys_cb.vol += (sys_cb.dyvol_overlap + vol_change);
            sys_cb.dyvol_overlap = 0;
            bt_music_vol_change();
        } else if (sys_cb.vol > 1) {
            sys_cb.vol += vol_change;
            bt_music_vol_change();
        }
    }
#endif // ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
}
#endif // ANC_ALG_DYVOL_FF_EN


///******************* 自研单MIC(FB)自适应音乐补偿 *******************///
#if ANC_ALG_MSC_ADP_FB_EN
void bsp_anc_alg_msc_adp_fb_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_MSC_ADP_FB)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    msc_adp_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
    anc_msc_gain_set(1, 0x800000, 0, 0, 12);
#if ANC_ALG_STEREO_EN
    msc_adp_adjust_fb_r_gain = 0x7FFF;
    anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
    anc_msc_gain_set(3, 0x800000, 0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_MSC_ADP_FB);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_msc_adp_fb_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_MSC_ADP_FB)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_MSC_ADP_FB);

    msc_adp_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
    anc_msc_gain_set(1, 0x800000, 0, 0, 12);
#if ANC_ALG_STEREO_EN
    msc_adp_adjust_fb_r_gain = 0x7FFF;
    anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
    anc_msc_gain_set(3, 0x800000, 0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_MSC_ADP_FB_EN


///******************* 自研双MIC(FF+FB)自适应EQ算法 *******************///
#if ANC_ALG_ADP_EQ_FF_FB_EN
void bsp_anc_alg_adp_eq_ff_fb_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_ADP_EQ_FF_FB)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    adp_eq_leak_tun = 0;
    adp_eq_msc_tun = 0;
    anc_adp_eq_set_gain(0, 0);
    anc_msc_gain_set(1, 0x7fffff, 0, 0, 12);

    ANC_ALG_COMM_START_PROC(ANC_ALG_ADP_EQ_FF_FB);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_adp_eq_ff_fb_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_ADP_EQ_FF_FB)) {
        return;
    }

    adp_eq_leak_tun = 0;
    adp_eq_msc_tun = 0;
    anc_adp_eq_set_gain(0, 0);
    anc_msc_gain_set(1, 0x7fffff, 0, 0, 12);

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_ADP_EQ_FF_FB);

    TRACE("%s\n", __func__);
}

int get_anc_alg_adp_eq_coef(u8 idx)
{
    return adp_eq_tbl[idx];
}
#endif // ANC_ALG_ADP_EQ_FF_FB_EN


///******************* 自研单MIC(FF)环境自适应ANC算法 *******************///
#if ANC_ALG_ASM_FF_EN
void bsp_anc_alg_asm_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_ASM_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    asm_sim_adjust_ff_gain = 0x7FFF;
    asm_sim_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    asm_sim_adjust_ff_r_gain = 0x7FFF;
    asm_sim_adjust_fb_r_gain = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_ASM_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_asm_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_ASM_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_ASM_FF);

    asm_sim_adjust_ff_gain = 0x7FFF;
    asm_sim_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    asm_sim_adjust_ff_r_gain = 0x7FFF;
    asm_sim_adjust_fb_r_gain = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
    anc_alg_adjust_fb_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_ASM_FF_EN


///******************* 自研双MIC(FF+FB)半入耳耳道自适应算法 *******************///
#if ANC_ALG_AEM_RT_FF_FB_EN
void bsp_anc_alg_aem_rt_ff_fb_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_AEM_RT_FF_FB)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    aem_rt_idx_num   = 4;
    aem_cvt_ff_gain  = 0;
    aem_cvt_fb_gain  = 0;
    aem_cvt_msc_gain = 0;
    bsp_anc_aem_set_ff_param(aem_rt_idx_num, false, false);
    bsp_anc_aem_set_fb_param();
    bsp_anc_alg_aem_rt_set_gain();
    ANC_ALG_COMM_START_PROC(ANC_ALG_AEM_RT_FF_FB);

    wav_res_play(RES_BUF_AEM_ON_WAV, RES_LEN_AEM_ON_WAV);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_aem_rt_ff_fb_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_AEM_RT_FF_FB)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_AEM_RT_FF_FB);

    msc_aem_adjust_ff_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
    msc_aem_adjust_fb_gain = 0x7FFF;
    anc_alg_adjust_fb_gain_set_vol(0, 0, 12);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_aem_rt_set_gain(void)
{
    bsp_anc_aem_set_ff_cvt(aem_cvt_ff_gain);
}
#endif // ANC_ALG_AEM_RT_FF_FB_EN

///******************* 自研单MIC(FF)AI风噪(小模型)检测 *******************///
#if ANC_ALG_AI_WN_MINI_FF_EN
void bsp_anc_alg_ai_wn_mini_ff_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_AI_WN_MINI_FF)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    ANC_ALG_COMM_START_PROC(ANC_ALG_AI_WN_MINI_FF);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_ai_wn_mini_ff_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_AI_WN_MINI_FF)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_AI_WN_MINI_FF);

    wind_noise_adjust_gain = 0x7FFF;
    anc_alg_adjust_ff_gain_set_vol(0, 0, 12);
#if ANC_ALG_STEREO_EN
    wind_noise_adjust_gain_r = 0x7FFF;
    anc_alg_adjust_ff_r_gain_set_vol(0, 0, 12);
#endif // ANC_ALG_STEREO_EN

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_AI_WN_MINI_FF_EN


///******************* 用户自定义ANC算法 *******************///
#if ANC_ALG_USER_EN
AT(.anc_text.process.anc_alg_user)
WEAK void alg_anc_user_do_process(s16* ff, s16* fb, s16* talk, int* res, u32* res_len, int num)
{

}

void alg_anc_user_init(void* param)
{
    TRACE("%s\n", __func__);
}

void alg_anc_user_exit(void)
{
    TRACE("%s\n", __func__);
}

void bsp_anc_alg_user_start(void)
{
    if (bsp_anc_alg_get_sta_by_type(ANC_ALG_USER)) {
        return;
    }

    if (ANC_ALG_COMM_START_CHECK()) {
        return;
    }

    ANC_ALG_COMM_START_PROC(ANC_ALG_USER);

    TRACE("%s\n", __func__);
}

void bsp_anc_alg_user_stop(void)
{
    if (!bsp_anc_alg_get_sta_by_type(ANC_ALG_USER)) {
        return;
    }

    ANC_ALG_COMM_STOP_PROC(ANC_ALG_USER);

    TRACE("%s\n", __func__);
}
#endif // ANC_ALG_USER_EN


///******************* 在线调试 *******************///
#if ANC_ALG_DBG_EN

#define SAL_VERSION             '1'
#define ANC_ALG_CRC_SEED        0xFFFF

enum {
    ANC_ALG_DBG_ACK_OK = 0,
    ANC_ALG_DBG_ACK_DATA_CRC_ERR,
    ANC_ALG_DBG_ACK_DATA_LEN_ERR,
    ANC_ALG_DBG_ACK_ALG_TYPE_ERR,
    ANC_ALG_DBG_ACK_ALG_PARAM_ERR,
};

u8 anc_alg_dbg_buf[10];
u8 anc_alg_dbg_type;


static u8 check_sum(u8* buf, u16 size)
{
    u32 i, sum = 0;
    for (i = 0; i < size; i++) {
        sum += buf[i];
    }
    return (u8)(-sum);
}

static void tx_ack(u8* packet, u16 len)
{
    delay_5ms(1);   //延时一段时间再ack
    if (anc_alg_dbg_type == 0) {
#if HUART_EN
        huart_tx(packet, len);
#endif // HUART_EN
    } else if (anc_alg_dbg_type == 1) {
#if BT_SPP_EN
        if (bt_get_status() >= BT_STA_CONNECTED) {
//            printf("tx:\n");
//            print_r(packet, len);
            bt_spp_tx(SPP_SERVICE_CH0, packet, len);
        }
#endif // BT_SPP_EN
    }
}

AT(.com_text.anc_alg_dbg)
bool bsp_anc_alg_dbg_rx_done(u8* rx_buf, u8 type)
{
//    print_r(eq_rx_buf, EQ_BUFFER_LEN);
    if ((rx_buf[0] == 'S') && (rx_buf[1] == 'A') && (rx_buf[2] == 'L')) {
        anc_alg_dbg_type = type;
        if (type == 0) {    //HUART
            msg_enqueue(EVT_ONLINE_SET_ANC_ALG);
        }
        return true;
    }

    return false;
}

void bsp_anc_alg_dbg_tx_ack(u8 ack)
{
    anc_alg_dbg_buf[0] = 'S';
    anc_alg_dbg_buf[1] = 'A';
    anc_alg_dbg_buf[2] = 'L';
    anc_alg_dbg_buf[3] = SAL_VERSION;
    anc_alg_dbg_buf[4] = ack;
    anc_alg_dbg_buf[5] = check_sum(anc_alg_dbg_buf, 5);
    tx_ack(anc_alg_dbg_buf, 6);
}

void bsp_anc_alg_parse_cmd(void)
{
#if TRACE_EN
//    printf("EVT_ONLINE_SET_ANC_ALG:\n");
//    print_r(eq_rx_buf, EQ_BUFFER_LEN);
#endif // TRACE_EN

    if ((eq_rx_buf[0] != 'S') || (eq_rx_buf[1] != 'A') || (eq_rx_buf[2] != 'L')) {
        return;
    }

    u32 param_len = little_endian_read_32(eq_rx_buf, 16);
    if ((param_len + 22) > EQ_BUFFER_LEN) {
        bsp_anc_alg_dbg_tx_ack(ANC_ALG_DBG_ACK_DATA_LEN_ERR);
        return;
    }

    u16 crc_rx = little_endian_read_16(eq_rx_buf, 20 + param_len);
    u16 crc_cal = calc_crc(eq_rx_buf, 20 + param_len, ANC_ALG_CRC_SEED);
    if (crc_cal != crc_rx) {
        bsp_anc_alg_dbg_tx_ack(ANC_ALG_DBG_ACK_DATA_CRC_ERR);
        return;
    }

    bool alg_en = false;
#if ANC_SNDP_SAE_SHIELD_ADAPTER_EN
    if (strcmp((const char*)&eq_rx_buf[4], "SAE_EL01") == 0) {
        if (param_len == (sizeof(gc_Para_ShieldLeakDet_Cfg) + sizeof(gc_Para_AdapANC_Cfg))) {
            if (sys_cb.anc_alg_en) {
                bsp_anc_alg_stop();
            }
            bsp_anc_alg_start(SNDP_SAE_ADAPTER_ANC);
            memcpy(&gc_Para_ShieldLeakDet_Cfg, &eq_rx_buf[20], sizeof(gc_Para_ShieldLeakDet_Cfg));
            memcpy(&gc_Para_AdapANC_Cfg, &eq_rx_buf[20] + sizeof(gc_Para_ShieldLeakDet_Cfg), sizeof(gc_Para_AdapANC_Cfg));
//            printf("---------------------------------\n");
//            printf("gc_Para_ShieldLeakDet_Cfg:\n");
//            printf(" SHIELD_LEAKGEAR: "); for(int i = 0; i < 7; i++) {printf("%d ", gc_Para_ShieldLeakDet_Cfg.SHIELD_LEAKGEAR[i]);}; printf("\n");
//            printf(" SHIELD_CORR_CORRECT: %d\n", gc_Para_ShieldLeakDet_Cfg.SHIELD_CORR_CORRECT);
//            printf(" SHIELD_ShieldStartFrm: %d\n", gc_Para_ShieldLeakDet_Cfg.SHIELD_ShieldStartFrm);
//            printf(" SHIELD_ShieldEndFrm: %d\n", gc_Para_ShieldLeakDet_Cfg.SHIELD_ShieldEndFrm);
//            printf(" SHIELD_ShieldOverFrm: %d\n", gc_Para_ShieldLeakDet_Cfg.SHIELD_ShieldOverFrm);
//            printf(" SHIELD_ShieldHalfDetFrm: %d\n", gc_Para_ShieldLeakDet_Cfg.SHIELD_ShieldHalfDetFrm);
//            printf("gc_Para_AdapANC_Cfg:\n");
//            printf(" DataScaleff: %d\n", gc_Para_AdapANC_Cfg.DataScaleff);
//            printf(" DataScalefb: %d\n", gc_Para_AdapANC_Cfg.DataScalefb);
//            printf(" THR_FF: %d\n", gc_Para_AdapANC_Cfg.THR_FF);
//            printf(" THR_FB: %d\n", gc_Para_AdapANC_Cfg.THR_FB);
//            printf(" THR_Est0: %d\n", gc_Para_AdapANC_Cfg.THR_Est0);
//            printf(" THR_Est1: %d\n", gc_Para_AdapANC_Cfg.THR_Est1);
//            printf(" THR_Est2: %d\n", gc_Para_AdapANC_Cfg.THR_Est2);
//            printf(" THR_Est3: %d\n", gc_Para_AdapANC_Cfg.THR_Est3);
//            printf(" THR_Est4: %d\n", gc_Para_AdapANC_Cfg.THR_Est4);
//            printf(" THR_Est5: %d\n", gc_Para_AdapANC_Cfg.THR_Est5);
//            printf(" GainMax: %d\n", gc_Para_AdapANC_Cfg.GainMax);
//            printf(" GainMin: %d\n", gc_Para_AdapANC_Cfg.GainMin);
//            printf(" ModeMax: %d\n", gc_Para_AdapANC_Cfg.ModeMax);
//            printf("---------------------------------\n");
        } else {
            bsp_anc_alg_dbg_tx_ack(ANC_ALG_DBG_ACK_ALG_PARAM_ERR);
            return;
        }

        alg_en = true;
    }
#endif // ANC_SNDP_SAE_SHIELD_ADAPTER_EN

    if (!alg_en) {
        bsp_anc_alg_dbg_tx_ack(ANC_ALG_DBG_ACK_ALG_TYPE_ERR);
    } else {
        bsp_anc_alg_dbg_tx_ack(ANC_ALG_DBG_ACK_OK);
    }

    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
}

#endif // ANC_ALG_DBG_EN

#endif // ANC_ALG_EN
