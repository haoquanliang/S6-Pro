#ifndef _API_EQ_H
#define _API_EQ_H

#define EQ_CHL          BIT(0)
#define EQ_CHR          BIT(1)

//EQ
struct eq_coef_tbl_t {
    const int * coef_0;            //tbl_alpha
    const int * coef_1;            //tbl_cos_w0
};

void eq_var_init(void);

//dac eq/drc
bool music_set_eq_by_res(u32 addr, u32 len);
bool music_set_eq_by_res_ch(u8 ch, u32 addr, u32 len);          //用于DAC EQ分频
void music_set_eq_by_num(u8 num);
void music_eq_off(void);
void music_set_eq(u8 band_cnt, const u32 *eq_param);
void music_set_eq_by_ch(u8 ch, u8 band_cnt, const u32 *eq_param);//用于DAC EQ分频
void music_set_eq_gain(u32 gain);
bool music_set_eq_is_done(void);    //判断上一次设置EQ是否完成，1：已完成
bool music_set_drc_by_res(u32 addr, u32 len);
void music_set_drc(u8 band_cnt, const u32 *drc_param);
void music_drc_on(void);            //使能music drc
void music_drc_off(void);           //关闭music drc
bool music_eq_divband_init(u8 ch, u8 div_band, u32 *param);
void music_eq_divband_exit(void);
void music_eq_set_after_vol(void);  //设置DAC为先过音量，再过music EQ

//peri eq/drc
void mic_set_eq(u8 band_cnt, const u32 *eq_param);
bool mic_set_eq_by_res(u32 addr, u32 len);
void mic_set_eq_gain(u32 gain);
bool mic_set_eq_is_done(void);      //判断上一次设置EQ是否完成，1：已完成
void mic_eq_proc(u16 *buffer, u16 samples);
void mic_eq_off(void);
bool mic_set_drc_by_res(u32 addr, u32 len);
void mic_set_drc(u8 band_cnt, const u32 *drc_param);
void mic_drc_off(void);
void mic_set_post_gain(u32 gain);

void bass_treble_coef_cal(void *eq_coef, int gain, int mode);                           //gain:-12dB~12dB, mode:0(bass), 1(treble)
void bass_treble_coef_cal_ext(void *eq_coef, int gain, int gain_fraction, int mode);    //gain:-12dB~12dB, mode:0(bass), 1(treble), gain_fraction小数部分:-9~9代表(-0.9dB ~ 0.9dB)
void eq_coef_cal(void *eq_coef, int gain);
void eq_coef_cal_ext(void *eq_coef, int gain, int gain_fraction);                       //gain:-12~12代表-12dB~12dB, gain_fraction小数部分:-9~9代表-0.9dB ~ 0.9dB
#endif
