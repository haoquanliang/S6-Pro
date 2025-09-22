#ifndef _API_KARAOK_H
#define _API_KARAOK_H

#define FIX_BIT(x)          ((int)(x * (1 << 28)))

typedef struct {
    u16 res[2];
    s16 *buf_1;
    s16 *buf_2;
    s16 *buf_3;
    s16 *buf_4;
    s16 *buf_5;
    s16 *buf_6;
    char *buf_7;
} mav_cfg_t;

typedef struct {
    const s32 (* lp_coef)[2][8];    //滤波器参数组
    s16 *echo_cache;                //混响缓存
    mav_cfg_t *mav;                 //魔音配置
    u8 res[2];
    u16 echo_en      :1;             //ECHO使能
    u16 mic_dnr      :1;             //MIC降噪使能
    u16 rec_en       :1;             //karaok录音使能
    u16 magic_voice  :1;             //魔音功能使能
    u16 rec_no_echo_en  :1;          //录音是否不录混响
    u16 howling_en   :1;             //是否打开防啸叫功能
}kara_cfg_t;
extern kara_cfg_t kara_cfg;

void karaok_disable_mic_first(void);
void karaok_set_mic_volume(u16 vol);
void karaok_set_music_volume(u16 vol);
void karaok_init(void *cfg);
void karaok_var_init(void);
void karaok_exit(void);
void karaok_reset_buf(void *cfg);

void karaok_mic_mute(void);
void karaok_mic_unmute(void);
void karaok_msc_mute(void);
void karaok_msc_unmute(void);
void karaok_process(void);

void echo_set_level(const u16 *vol);    //混响等级设置
void echo_set_delay(u16 delay);     //混响间隔

void hifi4_pitch_shift2_init(u32 SampleRate, s32 shift, u32 idx);
void hifi4_echo_set_level(void *arg, u32 ch_index);
void hifi4_reverb_set_level(void *arg, u32 ch_index);
void hifi4_bass_treble_set_gain(s8 bass_gain, s8 treble_gain, u32 ch_index);    //gain: -12 ~ +12
void hifi4_howling_init(u32 sample_rate, s32 shift_hz, u32 idx);    //shift_hz: 移频HZ(例如：-15, -10, -5, 5, 10, 15)

void karaok_voice_rm_enable(void);  //开启消人声
void karaok_voice_rm_disable(void); //关闭消人声
void karaok_voice_rm_switch(void);  //消人声切换
bool karaok_get_voice_rm_sta(void); //获取消人声状态

void karaok_rec_init(u8 nchannel);
void karaok_rec_start(void);
void karaok_rec_stop(void);

void karaok_set_mav(u8 type, u8 level);  //type:0(高音), 1(低音)， level[0:7]

void kara_sco_start(void);
void kara_sco_stop(void);

void mic_dnr_init(u8 v_cnt, u16 v_pow, u8 s_cnt, u16 s_pow);   //MIC动态降噪 //10ms检测一次 //连续超过 v_cnt 次大于v_pow就认为有声，连续超过s_cnt次低于s_pow就认为无声
void karaok_enable_mic_first(u16 vol);  //mic优先使能， 同时设置mic_first时， 音量的音量值（0~65535）
void mic_first_init(u8 v_cnt, u16 v_pow, u8 s_cnt, u16 s_pow);  //连续超过 v_cnt 次大于 v_pow 就开启mic_first，连续超过 s_cnt 次低于 s_pow 就关闭 mic_first
u16 get_karaok_mic_maxpow(bool rekick);    //karaok中的mic 能量值
void karaok_mic_first_music_vol_fade_enable(u16 fade_in_step,  u16 fade_out_step);  //ms

#endif
