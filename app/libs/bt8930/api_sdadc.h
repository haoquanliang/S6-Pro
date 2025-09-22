#ifndef _API_SDADC_H
#define _API_SDADC_H

#define CHANNEL_L           0x00ff
#define CHANNEL_R           0xff00

#define ADC2DAC_EN          0x01        //ADC-->DAC
#define ADC2SRC_EN          0x02        //ADC-->SRC
#define ADC2IRQ_EN          0x03        //ADC-->IRQ（测试用）
#define ADC2DIR_EN          0x04        //ADC-->DAC（测试用）
#define ADC2SANC_EN         0x05        //ADC-->SANC
#define ADC2ASR_EN          0x06        //ADC-->ASR
#define ADC_ALIGN_EN        0x07        //ADC DMA ALIGN

typedef void (*pcm_callback_t)(u8 *ptr, u32 samples, int ch_mode);

typedef struct {
    u16 channel;
    u8 sample_rate;
    u32 anl_gain;
    u32 dig_gain;
    u8 bits_mode;                       //ADC BITS选择；0: 24bits, 1: 16bits, 0xff: 跟随DAC的BIT MODE
    u8 out_ctrl;
    u16 samples;
    pcm_callback_t callback;
} sdadc_cfg_t;

enum {
    SPR_48000,
    SPR_44100,
    SPR_38000,
    SPR_32000,
    SPR_24000,
    SPR_22050,
    SPR_16000,
    SPR_12000,
    SPR_11025,
    SPR_8000,
    SPR_6000,
    SPR_4000,

    SPR_96000,
    SPR_88200,
    SPR_76000,
    SPR_64000,
    SPR_384000,
    SPR_352800,
    SPR_192000,
    SPR_176400,
};

void set_mic_analog_gain(u16 level, u16 channel);         //0~23(共24级), step 3DB (-6db ~ +63db)
void sdadc_set_digital_gain(u16 ch, u16 gain);
void sdadc_set_soft_gain(u16 ch, u32 gain);
void set_aux_analog_vol(u8 level, u8 auxlr_sel);
void set_aux_analog_gain(u32 gain, u16 channel);
void sdadc_dummy(u8 *ptr, u32 samples, int ch_mode);
void sdadc_pcm_2_dac(u8 *ptr, u32 samples, int ch_mode);
void sdadc_pcm_2_dac1(u8 *ptr, u32 samples, int ch_mode);
void sdadc_var_init(void);

int sdadc_init(const sdadc_cfg_t *p_cfg);
int sdadc_start(u16 channel);
int sdadc_exit(u16 channel);
u32 sdadc_channel_enable_meanwhile(u32 adc_ch, u16 anc_dma_ch); //控制多个DMA同时启动，必须在 sdadc_start 之后调用
void sdadc_set_5ch_en(void);

bool sdadc_drc_init(u8 *drc_addr, int drc_len);

//mic
void mic_mute(void);
void mic_unmute(void);

//anc
typedef enum {
	MODE_TWS_FFFB,      //TWS耳机mic2(FF/FB)输入，DACL输出
	MODE_FFFB,          //非TWS mic2、3(FF/FB)输入，DACL/R输出(mic2->dacl，mic3->dacr)

	MODE_TWS_HYBRID = 8,//TWS耳机mic2、3(Hybrid)输入，DACL输出
	MODE_HYBRID,        //非TWS mic0、1、2、3(Hybrid)输入，DACL/R输出(mic01->dacl，mic23->dacr)
} ANC_MODE;

//ANC算法
enum ANC_ALG_TYPE {
    ANC_ALG_WIND_NOISE_FF            = 0x01,    //自研单MIC(FF)传统特征风噪和能量检测算法
    ANC_ALG_WIND_NOISE_FF_TALK,                 //自研双MIC(FF+TALK)传统特征风噪和能量检测算法
    ANC_ALG_WIND_NOISE_FF_FB,                   //自研双MIC(FF+FB)传统特征风噪和能量检测算法
    ANC_ALG_ASM_SIM_FF,                         //自研单MIC(FF)降增噪算法
    ANC_ALG_HOWLING_FB,                         //自研防啸叫(FB)ANC算法
    ANC_ALG_FIT_DETECT_FF_FB,                   //自研贴合度检测(FF+FB)ANC算法
    ANC_ALG_HOWLING_FF,                         //自研防啸叫(FF)ANC算法
    ANC_ALG_AI_WN_FF,                           //自研单MIC(FF)AI风噪检测算法
    ANC_ALG_LIMITER_FF,                         //自研单MIC(FF)瞬态噪声检测算法
    ANC_ALG_DYVOL_FF,                           //自研单MIC(FF)动态音量算法
    ANC_ALG_MSC_ADP_FB,                         //自研单MIC(FB)自适应音乐补偿算法
    ANC_ALG_ADP_EQ_FF_FB,                       //自研双MIC(FF+FB)自适应EQ算法
    ANC_ALG_ASM_FF,                             //自研单MIC(FF)环境自适应ANC算法
    ANC_ALG_AEM_RT_FF_FB,                       //自研双MIC(FF+FB)半入耳耳道自适应算法
    ANC_ALG_AI_WN_MINI_FF,                      //自研单MIC(FF)AI风噪(小模型)检测算法
    SNDP_SAE_SHIELD_LEAK             = 0x80,    //声加泄露补偿自适应ANC算法(SAE_EL01)
    SNDP_SAE_ADAPTER_ANC,                       //声加泄露补偿自适应ANC算法(SAE_EL01)
    SNDP_SAE_WIND_DETECT,                       //声加ANC风噪检测算法
    ANC_ALG_DUMP_TYPE                = 0xFE,
    ANC_ALG_USER                     = 0xFF,    //用户自定义ANC算法
};

#define ANC_ALG_DUMP_DATA_FF        BIT(0)
#define ANC_ALG_DUMP_DATA_FB        BIT(1)
#define ANC_ALG_DUMP_DATA_TALK      BIT(2)
#define ANC_ALG_DUMP_DATA_SPK       BIT(3)
#define ANC_ALG_DUMP_DATA_ANCOUT    BIT(4)
#define ANC_ALG_DUMP_DATA_FF_R      BIT(5)
#define ANC_ALG_DUMP_DATA_FB_R      BIT(6)

//ANC DS DMA数据
enum {
    ANC_DS_DATA_ADC0_ANC_DOUT = 1,
    ANC_DS_DATA_ADC1_ANC_DOUT,
    ANC_DS_DATA_ADC2_ANC_DOUT,
    ANC_DS_DATA_ADC3_ANC_DOUT,
    ANC_DS_DATA_DACL_MUSIC,
    ANC_DS_DATA_DACR_MUSIC,
    ANC_DS_DATA_ANC0_FF_DOUT,
    ANC_DS_DATA_ANC0_FB_DOUT,
    ANC_DS_DATA_ANC1_FF_DOUT,
    ANC_DS_DATA_ANC1_FB_DOUT,
    ANC_DS_DATA_DACL_SPK,
    ANC_DS_DATA_DACR_SPK,
};

typedef struct {
    u8 drc_en           :1;
    u8 drc_after_eq     :1; //0：adc->drc->eq  1：adc->eq->drc
    u8 filter_en        :1;
    u8 filter_band;
    u8 rfu[2];
    const u32 *filter_coef;
    const u32 *drc_coef;
} anc_drc_t;

typedef struct {
	u32 nos_gain;
	const u32 *nos_param;
	u32 msc_gain;
	const u32 *msc_param;
	anc_drc_t drc;
    u8 mic_ch;
	u8 nos_band;            //ANC降噪eq条数
	u8 msc_band;            //ANC音乐补偿eq条数
    u8 bypass           :1; //ANC EQ Bypass，置1 EQ无效
    u8 msc_dly          :2; //music sample delay
    u8 rmdc_sel         :3; //rmdc level:0(5.269Hz),1(168.6Hz),2(42.15Hz),3(21.07Hz),4(10.53Hz),5(2.634Hz),6(1.317Hz),7(0.658Hz)
} anc_channel_t;

struct anc_cfg_t {
	ANC_MODE anc_mode;
	u8 spr;
	u8 rfu[2];
	anc_channel_t *ch[4];
	u8 fade_in_step;        //淡入速度，step * 85ms(384k)
	u8 fade_out_step;       //淡出速度，step * 85ms(384k)
    u16 change_en       :1; //change是否使能
    u16 dc_rm           :1;
    u16 fade_en         :1; //淡入淡出使能
    u16 order           :2;
    u16 output_swap     :1; //DACLR互换输出
};

//ANC算法初始化参数
typedef struct {
    u8    mic_cfg;
    u8    dump_en;
    u8    pcm_interleave;
    u8    alg_nch;
    u16   alg_samples;
    u16   start_delay;
    u8    type[4];
    void* alg_param[4];
    u32   resv[4];
} anc_alg_param_cb;

typedef struct {
    u16 channel;                        //bit[0:7] ANC0DMA, bit[7:15] ANC1DMA
    u16 dig_gain;                       //bit[0:7] ANC0DMA, bit[7:15] ANC1DMA
    u8 sample_rate;
    u8 bits_mode;                       //0: 24bits, 1: 16bits
    u16 samples;
    pcm_callback_t callback;
    u32 resv;
} anc_dma_cfg_cb;


#define ANC_FIX_BIT         23
#define EQ_GAIN_MAX         0x07eca9cd      //24dB
#define EQ_GAIN_MIN         0x00081385      //-24dB
void anc_var_init(void);
void anc_init(void *cfg);
void anc_start(void);
void anc_stop(void);
void anc_exit(void);
bool anc_set_param(void);
void anc_set_eq_param_change(u8 ch);
u8 anc_set_param_is_busy(void);         //ret: BIT(0):lch is busy; BIT(1):rch is busy
void anc_set_tansparency_mode(u8 en);   //使能/关闭通透模式，默认关闭,调用后需要更新参数才起作用
bool anc_is_tansparency_mode(void);     //当前是否处于通透模式
void anc_mic_mute(u8 ch, u8 mute);
void anc_dig_mic_mute(u8 ch, u8 mute);
void anc_set_mic_gain(u8 ch, u8 anl, u8 gain);
u32 anc_pow10_cal(int index);           //10^(dB/20)*2^23, index = 10*dB, -12.0<dB<12.0
void anc_set_drc_param(void);
void anc_vol_set(u8 ch, u16 vol, u8 step, u8 direct_set, u8 fade_step);    //vol:Q15, step:0~15

//ANC DMA
int anc_dma_start(anc_dma_cfg_cb* cfg, u8 kick_start);
int anc_dma_exit(void);
bool anc_dma_is_use(void);

void ttp_init(u32 param);
void ttp_exit(void);

void sdadc_pdm_mic_init(u8 mapping);
void sdadc_pdm_mic_exit(u8 mapping);
void lefmic_zero_detect_do(u8 *ptr, u32 samples);

bool sdadcl_set_soft_eq_by_res(u32 addr, u32 len);
bool sdadcr_set_soft_eq_by_res(u32 addr, u32 len);

#endif //_API_SDADC_H
