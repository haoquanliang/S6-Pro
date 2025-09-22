/*****************************************************************************
 * Copyright (c) 2023 Shenzhen Bluetrum Technology Co.,Ltd. All rights reserved.
 * File      : bsp_effect.h
 * Function  : 音乐音效模块接口
 * History   :
 * Created by yutao on 2023-12-1.
 *****************************************************************************/
#ifndef __BSP_EFFECT_H__
#define __BSP_EFFECT_H__

typedef enum {
    MUSIC_EFFECT_DBB            = 0,        //动态低音音效
    MUSIC_EFFECT_SPATIAL_AUDIO,             //空间音效
    MUSIC_EFFECT_VBASS,                     //虚拟低音音效
    MUSIC_EFFECT_DYEQ,                      //动态EQ音效
    MUSIC_EFFECT_XDRC,                      //2段DRC
    MUSIC_EFFECT_ABP,                       //舒适噪声（alpha波、beta波、pink）
    MUSIC_EFFECT_HRTF_RT,                   //动态空间音频
    MUSIC_EFFECT_ALG_USER       = 8,        //用户自定义音效算法
    MUSIC_EFFECT_MAX            = 16,
} MUSIC_EFFECT_ALG;

enum {
    MUSIC_EFFECT_SUSPEND_FOR_SCO,
    MUSIC_EFFECT_SUSPEND_FOR_RES,
    MUSIC_EFFECT_SUSPEND_FOR_HIRES_DEC,
    MUSIC_EFFECT_SUSPEND_FOR_CHARGE,
    MUSIC_EFFECT_SUSPEND_FOR_ASR,
};


void music_effect_init(void);
void music_effect_sco_audio_init_do(void);
void music_effect_sco_audio_exit_do(void);
void music_effect_sfunc_sleep_do(u8 enter);
void music_effect_func_process(void);
void music_effect_get_music_eq_total_gain(u32* gain);
int music_effect_set_state(MUSIC_EFFECT_ALG alg, u8 state); //设置音效状态
bool music_effect_get_state(MUSIC_EFFECT_ALG alg);          //获取音效是否已经设置打开（不包含因通话、提示音等暂停的状态）
bool music_effect_get_state_real(MUSIC_EFFECT_ALG alg);     //获取音效实际的状态
void music_effect_alg_reinit(void);                         //重新初始化算法的软件部分
void music_effect_alg_restart(void);                        //重启算法，打开算法硬件部分
void music_effect_alg_suspend(u8 reason);                   //暂停算法，关闭算法硬件部分

///动态低音音效
void music_dbb_audio_start(void);
void music_dbb_audio_stop(void);
void music_dbb_audio_set_vol_do(u8 vol_level);
void music_dbb_audio_set_bass_level(u8 bass_level);

///空间音效
void music_spatial_audio_start(void);
void music_spatial_audio_stop(void);
u8 music_spatial_audio_get_fix_eq_sta(void);

///虚拟低音
void music_vbass_audio_start(void);
void music_vbass_audio_stop(void);
void music_vbass_set_param(u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set, u32 pregain);

///动态EQ
void music_dyeq_audio_start(void);
void music_dyeq_audio_stop(void);
void music_dyeq_audio_set_vol_do(u8 vol_level);
void music_dyeq_audio_set_vol_by_dvol(u16 vol);
void music_dyeq_set_param(u8 *buf);
void music_dyeq_drc_set_param(void *buf);

///2段DRC
void music_xdrc_audio_start(void);
void music_xdrc_audio_stop(void);
void music_xdrc_audio_set_vol_do(u8 vol_level);
void music_xdrc_audio_set_vol_by_dvol(u16 vol);
void music_xdrc_set_delay(u16 delay_samples);
void xdrc_softeq_set_param(u8* buf);

///动态空间音频
void music_hrtf_rt_start(void);
void music_hrtf_rt_stop(void);
u8 music_hrtf_rt_get_fix_eq_sta(void);
void music_hrtf_rt_update_angle(s16 az, s16 el);

///用户自定义音效算法
void music_effect_alg_user_start(void);
void music_effect_alg_user_stop(void);

#endif //__BSP_EFFECT_H__
