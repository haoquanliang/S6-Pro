#ifndef _API_EFFECTS_H
#define _API_EFFECTS_H

typedef struct {
    const u8* dbb_param;
    u32 param_len;
    s32* coef_l;
    s32* coef_r;
    u8 dac_band_cnt;
} dbb_param_cb_t;

typedef struct {
    u32 cutoff_frequency;
    u32 intensity_set;
    u8 vbass_high_frequency_set;
    u32 intensity;
    u32 pregain;
} vbass_param_cb_t;

typedef struct {
    const s32* dyeq_param;
    const s32* coef_param;
    u8 vbass_en;
} dyeq_param_cb_t;

typedef struct {
    u16 fade_step;
    volatile s8 fade_dir;
    u16 gain;
    volatile u16 target_gain;
} soft_vol_t;

typedef struct {
	s32 wave_type;
	s32 intensity;
	s32 plfsr_l;
	s32 plfsr_r;
} ABP_generation_init_cb_t;

typedef struct {
	s16 decay_factor;
} hrtf_rt_cb_t;

void codecs_pcm_init(void);
u8 codecs_pcm_is_start(void);

///软件音量
void soft_vol_process_mono_24bits_one_sample(soft_vol_t* p, s32* input);
void soft_vol_process_stereo_24bits_one_sample(soft_vol_t* p, s32* inputl, s32* inputr);
void soft_vol_process_mono_one_sample(soft_vol_t* p, s16* input);
void soft_vol_process_stereo_one_sample(soft_vol_t* p, s16* inputl, s16* inputr);
void soft_vol_set_vol_param(soft_vol_t* p, u16 vol, u8 vol_direct_set);
void soft_vol_init(soft_vol_t* p);

///动态低音音效
int music_dbb_init(dbb_param_cb_t* p);
int music_dbb_update_param(u8 vol_level, u8 bass_level);
int music_dbb_stop(void);
u8 music_dbb_get_bass_level(void);
u8 music_dbb_get_level_cnt(void);

///空间音效
int v3d_get_data_ptr_l1(u32* ptr);
void v3d_frame_process(u32* buf, u32 samples, u32 in_nch, u32 in_24bits);
void v3d_clear_cache(void);
void v3d_delay_buf_clear_cache(void);
void v3d_set_param(int mode, u16 wet, u16 dry);
void v3d_set_fade(u32 fade);            //fade: 0->fade in, 1->fade out
bool v3d_fade_is_done(u32 fade);        //fade: 0->fade in, 1->fade out
bool v3d_delay_buf_init(void* ptr_l, u32 buf_l_size, u32 delay_l_samples, void* ptr_r, u32 buf_r_size, u32 delay_r_samples);
void v3d_init(void);

///虚拟低音
s32 vbass_process(void *cb, s32 data);
void vbass_init(void *cb, vbass_param_cb_t *p);
void vbass_set_param(void *cb, u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set, u32 pregain);

///动态EQ
s32 dynamic_eq_process(void *cb, s32 data);
void dyeq_init(void *cb, dyeq_param_cb_t *p);
void dyeq_coef_update(void *cb, u8 *buf);
void dyeq_clear_cache(void *cb);
s32 dyeq_drc_v3_calc(s32 sample, void *drc_cb);
bool dyeq_drc_v3_init(const void *bin, int bin_size, void *drc_cb);
bool dyeq_drc_v3_set_param(void *buf, void *drc_cb);

///2段DRC
s32 xdrc_softeq_proc(void *cb, s32 input);
int xdrc_softeq_cb_init(void *cb, u32 cb_len, u32 res_addr, u32 res_len);
void xdrc_softeq_coef_update(void *cb, u32 cb_len, u32 *coef, u32 band, bool pregain);
void xdrc_softeq_clear_cache(void *cb);
s32 xdrc_drc_process_gain(void *cb, s32 data_in);
s32 xdrc_drc_get_pcm32_s(s32 data, int g);
s16 xdrc_drc_process_s16(void *cb, s32 data_in);
int xdrc_drcv1_cb_init(void *cb, u32 cb_len, u32 res_addr, u32 res_len);
void xdrc_drcv1_coef_update(void* cb, u32 *tbl);
s32 pcmdelay_mono_s32(void *cb, s32 pcm);
int pcmdelay_init(void *cb,void *delay_buf, u16 delay_buf_len, u16 samples_size, u16 delay_samples);
bool pcmdelay_coef_update(void *cb, u16 delay_samples);

///舒适噪声（alpha波、beta波、pink）
void alpha_beta_pink_generation_init(ABP_generation_init_cb_t *p);
void alpha_beta_pink_generation(s16 *samples, s32 len, u8 idx);
void alpha_beta_pink_generation_stereo(s16 *samples, s32 len);

///动态空间音频
void hrtf_rt_init(hrtf_rt_cb_t *p);
void hrtf_rt_audio_frame_process(u32* buf, u32 samples, u32 in_nch, u32 in_24bits);
void hrtf_rt_update_angle_process(s16 az, s16 el);
s32 hrtf_rt_angle_filter(s32 in_angle);
void hrtf_rt_angle_filter_init(u32 filter_cnt_total, s16 delta_thr, s16 step);

#endif // _API_EFFECTS_H
