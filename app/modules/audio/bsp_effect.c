/*****************************************************************************
 * Copyright (c) 2023 Shenzhen Bluetrum Technology Co.,Ltd. All rights reserved.
 * File      : bsp_effect.c
 * Function  : 音乐音效模块
 * History   :
 * Created by yutao on 2023-12-1.
 *****************************************************************************/
#include "include.h"


#if BT_MUSIC_EFFECT_EN

#define TRACE_EN                1
#define DUMP_EN                 0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


void load_code_audio_comm(void);
void huart_wait_txdone(void);
int music_effect_set_state(MUSIC_EFFECT_ALG alg, u8 state);
void bsp_set_effect_by_abt(void);

void alg_user_effect_process(u8 *buf, u32 samples, u32 nch, u32 is_24bit, u32 pcm_info);
void alg_user_effect_init(void);
void music_dbb_audio_start_do(u8 is_def_level);
void music_dbb_audio_stop_do(void);
void music_spatial_audio_param_init(void);
void music_spatial_audio_start_do(void);
void music_spatial_audio_stop_do(void);
void music_vbass_audio_param_init(void);
void music_dyeq_audio_param_init(void);
void music_dyeq_audio_start_do(void);
void music_dyeq_audio_stop_do(void);
void music_xdrc_audio_param_init(void);
void music_xdrc_audio_start_do(void);
void music_xdrc_audio_stop_do(void);
void music_hrtf_rt_param_init(void);
void music_hrtf_rt_start_do(void);
void music_hrtf_rt_stop_do(void);

void vbass_frame_process(u32* buf, u32 samples, u32 ch_idx, u32 in_24bits);
void dynamic_eq_frame_process(u32* buf, u32 samples, u32 ch_idx, u32 in_24bits);
void dynamic_eq_clear_cache(void);
void xdrc_frame_process(u32* buf, u32 samples, u32 ch_idx, u32 in_24bits);
void xdrc_clear_cache(void);

extern const u16 dac_dvol_tbl_db[61];
extern const u8 *dac_dvol_table;


typedef struct {
    //音效状态机
    volatile u16 music_effect_state;
    u16 music_effect_state_set;
    volatile u8 process_flag;
    volatile u8 audio_comm_init_flag;
    //主频
    u8 sys_clk;
    //主线程process flag
    u32 func_process_flag;
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
    u8 spatial_audio_eq_state;
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN
#if BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN || BT_MUSIC_EFFECT_HRTF_RT_EQ_EN
    u8 spatial_audio_fix_eq;
#endif
#if BT_MUSIC_EFFECT_HRTF_RT_EN
    volatile u8 hrtf_rt_angle_update_flag;
#endif // BT_MUSIC_EFFECT_HRTF_RT_EN
} music_effect_t;

music_effect_t music_effect;
#if DUMP_EN
u32 music_dump_buffer[256 * 4] AT(.music_exbuff.dump);
u32 music_tx_buffer[256 * 4] AT(.music_exbuff.dump);
#endif // DUMP_EN

//状态位
#define DBB_STA_BIT             BIT(MUSIC_EFFECT_DBB)
#define SPATIAL_AUDIO_STA_BIT   BIT(MUSIC_EFFECT_SPATIAL_AUDIO)
#define VBASS_STA_BIT           BIT(MUSIC_EFFECT_VBASS)
#define DYEQ_STA_BIT            BIT(MUSIC_EFFECT_DYEQ)
#define XDRC_STA_BIT            BIT(MUSIC_EFFECT_XDRC)
#define ABP_STA_BIT             BIT(MUSIC_EFFECT_ABP)
#define HRTF_RT_STA_BIT         BIT(MUSIC_EFFECT_HRTF_RT)
#define USER_ALG_STA_BIT        BIT(MUSIC_EFFECT_ALG_USER)
#define DBB_EN(x)               ((x) & (DBB_STA_BIT))
#define SPATIAL_AUDIO_EN(x)     ((x) & (SPATIAL_AUDIO_STA_BIT))
#define VBASS_EN(x)             ((x) & (VBASS_STA_BIT))
#define DYEQ_EN(x)              ((x) & (DYEQ_STA_BIT))
#define XDRC_EN(x)              ((x) & (XDRC_STA_BIT))
#define ABP_STA_EN(x)           ((x) & (ABP_STA_BIT))
#define HRTF_RT_STA_EN(x)       ((x) & (HRTF_RT_STA_BIT))
#define USER_ALG_EN(x)          ((x) & (USER_ALG_STA_BIT))
//主频设置
#define DBB_SYSCLK_SEL          SYS_24M
#define SPATIAL_AU_SYSCLK_SEL   SYS_100M
#define VBASS_SYSCLK_SEL        SYS_48M
#define DYEQ_SYSCLK_SEL         (BT_MUSIC_EFFECT_DYEQ_VBASS_EN ? SYS_100M : SYS_60M)
#define XDRC_SYSCLK_SEL         SYS_100M
#define ABP_SYSCLK_SEL          SYS_48M
#define HRTF_RT_SYSCLK_SEL      SYS_160M
#define USER_ALG_SYSCLK_SEL     SYS_48M
//主线程process flag bit
#define PROC_FLAG_ALG_REINIT    BIT(0)
#define PROC_FLAG_ALG_SUSPEND   BIT(1)


#define EFFECT_COMM_START_CHECK()       if (sco_is_connected()) {return;}
#define EFFECT_COMM_STOP_CHECK()        if (sco_is_connected()) {return;}



///音效算法处理函数
AT(.com_text.codecs.pcm)
void msc_pcm_effect_process(u8 *buf, u32 samples, u32 nch, u32 is_24bit, u32 pcm_info)
{
    music_effect_t* cb = &music_effect;
    u16 state = cb->music_effect_state;

//    if (pcm_info & BIT(0)) {
//        //first frame
//    }

#if DUMP_EN
    if (is_24bit) {
        s32* in = (s32*)buf;
        s32* out =  (s32*)music_dump_buffer;
        for (int i = 0; i < samples; i++) {
            out[4 * i + 0] = in[2 * i + 0];
            out[4 * i + 1] = in[2 * i + 1];
        }
    } else {
        s16* in = (s16*)buf;
        s16* out =  (s16*)music_dump_buffer;
        for (int i = 0; i < samples; i++) {
            out[4 * i + 0] = in[2 * i + 0];
            out[4 * i + 1] = in[2 * i + 1];
        }
    }
#endif // DUMP_EN

    if (state) {
        cb->process_flag = 1;
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
        if (SPATIAL_AUDIO_EN(state)) {
#if BT_TWS_EN
            u32 ch = sys_cb.tws_left_channel ? 0 : 1;     //in_nch: 0->L, 1->R, 2->stereo
#else
            u32 ch = 2;
#endif // BT_TWS_EN
            if (pcm_info & BIT(0)) {
                v3d_clear_cache();
#if BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
                v3d_delay_buf_clear_cache();
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
            }
            v3d_frame_process((u32*)buf, samples, ch, is_24bit);
        }
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN
#if BT_MUSIC_EFFECT_TWS_ALG_EN
        u32 ch_idx = sys_cb.tws_left_channel ? 0 : 1;     //in_nch: 0=L, 1=R, 2=(L+R)/2
        if (!bt_tws_is_connected()) {
            ch_idx = 2;
        }
#if BT_MUSIC_EFFECT_VBASS_EN
        if (VBASS_EN(state)) {          //只跑TWS单声道
            vbass_frame_process((u32*)buf, samples, ch_idx, is_24bit);
        }
#endif // BT_MUSIC_EFFECT_VBASS_EN
#if BT_MUSIC_EFFECT_DYEQ_EN
        if (DYEQ_EN(state)) {           //只跑TWS单声道
            if (pcm_info & BIT(0)) {
                dynamic_eq_clear_cache();
            }
            dynamic_eq_frame_process((u32*)buf, samples, ch_idx, is_24bit);
        }
#endif // BT_MUSIC_EFFECT_DYEQ_EN
#if BT_MUSIC_EFFECT_XDRC_EN
        if (XDRC_EN(state)) {           //只跑TWS单声道
            if (pcm_info & BIT(0)) {
                xdrc_clear_cache();
            }
            xdrc_frame_process((u32*)buf, samples, ch_idx, is_24bit);
        }
#endif // BT_MUSIC_EFFECT_XDRC_EN
#endif // BT_MUSIC_EFFECT_TWS_ALG_EN
#if BT_MUSIC_EFFECT_HRTF_RT_EN
        if (HRTF_RT_STA_EN(state)) {
            hrtf_rt_audio_frame_process((u32*)buf, samples, 2, is_24bit);
        }
#endif // BT_MUSIC_EFFECT_HRTF_RT_EN
#if BT_MUSIC_EFFECT_USER_EN
        if (USER_ALG_EN(state)) {       //用户自定义的算法处理
            alg_user_effect_process(buf, samples, nch, is_24bit, pcm_info);
        }
#endif // BT_MUSIC_EFFECT_USER_EN
        cb->process_flag = 0;
    }

#if DUMP_EN
    if (is_24bit) {
        s32* in = (s32*)buf;
        s32* out =  (s32*)music_dump_buffer;
        for (int i = 0; i < samples; i++) {
            out[4 * i + 2] = in[2 * i + 0];
            out[4 * i + 3] = in[2 * i + 1];
        }
    } else {
        s16* in = (s16*)buf;
        s16* out =  (s16*)music_dump_buffer;
        for (int i = 0; i < samples; i++) {
            out[4 * i + 2] = in[2 * i + 0];
            out[4 * i + 3] = in[2 * i + 1];
        }
    }
    huart_wait_txdone();
    memcpy(music_tx_buffer, music_dump_buffer, samples * (is_24bit + 1) * 2 * 4);
    huart_tx(music_tx_buffer, samples * (is_24bit + 1) * 2 * 4);

//    huart_wait_txdone();
//    memcpy(music_dump_buffer, buf, samples * (is_24bit + 1) * 2 * 2);
//    huart_tx(music_dump_buffer, samples * (is_24bit + 1) * 2 * 2);
#endif // DUMP_EN
}

///音效部分公共接口
//初始化
void music_effect_init(void)
{
    memset(&music_effect, 0, sizeof(music_effect_t));
    music_effect.sys_clk = SYS_24M;
    load_code_audio_comm();
    music_effect.audio_comm_init_flag = 1;
}

//music线程开始播放
void msc_pcm_out_start_callback(u8 codec)
{
    TRACE("msc_pcm_out_start_callback %d\n", codec);

    music_effect_t* cb = &music_effect;

    if ((codec == 2) || (codec == 3)) {         //LHDC LDAC
        cb->audio_comm_init_flag = 0;
        cb->music_effect_state &= DBB_STA_BIT;  //只保留动态低音，其他关闭
        if (cb->music_effect_state_set) {
            cb->func_process_flag |= PROC_FLAG_ALG_SUSPEND;
        }
    } else if ((codec == 0) || (codec == 1)) {  //AAC SBC
        if (cb->audio_comm_init_flag == 0) {
            cb->func_process_flag |= PROC_FLAG_ALG_REINIT;
        }
#if BT_MUSIC_PAUSE_CLK_BACK_EN
        sys_clk_req(INDEX_KARAOK, cb->sys_clk);
#endif // BT_MUSIC_PAUSE_CLK_BACK_EN
    }
}

//music线程停止播放
void msc_pcm_out_stop_callback(u8 codec)
{
#if BT_MUSIC_PAUSE_CLK_BACK_EN
    if ((codec == 0) || (codec == 1)) {  //AAC SBC
        sys_clk_free(INDEX_KARAOK);
    }
#endif // BT_MUSIC_PAUSE_CLK_BACK_EN
    TRACE("msc_pcm_out_stop_callback\n");
}

//进入通话
void music_effect_sco_audio_init_do(void)
{
    music_effect.audio_comm_init_flag = 0;
    music_effect_alg_suspend(MUSIC_EFFECT_SUSPEND_FOR_SCO);
#if ABP_EN
    if (abp_is_playing()) {
        abp_stop();
    }
#endif // ABP_EN
}

//退出通话
void music_effect_sco_audio_exit_do(void)
{
    music_effect_alg_reinit();
    music_effect_alg_restart();
#if ABP_EN
    if (sys_cb.abp_mode) {
        bsp_abp_set_mode(sys_cb.abp_mode);
    }
#endif // ABP_EN
}

//进出休眠
void music_effect_sfunc_sleep_do(u8 enter)
{
    if (enter == 0) {                    //退出休眠
        sys_clk_req(INDEX_KARAOK, music_effect.sys_clk);
    } else {                             //进入休眠

    }
}

//设置MUSIC EQ的总增益
void music_effect_get_music_eq_total_gain(u32* gain)
{
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
    if (SPATIAL_AUDIO_EN(music_effect.music_effect_state_set)) {
        s32 temp = (s32)*gain;
        s32 base_gain = (s32)(1.0f * (1 << 23));
        if (music_effect.spatial_audio_eq_state == 1) {
            base_gain = (s32)(1.3335f * (1 << 23));   //+2.5db
        } else if (music_effect.spatial_audio_eq_state == 2) {
            base_gain = (s32)(1.7782f * (1 << 23));   //+5db
        } else if (music_effect.spatial_audio_eq_state == 3) {
            base_gain = (s32)(2.3713f * (1 << 23));   //+7.5db
        } else if (music_effect.spatial_audio_eq_state == 4) {
            base_gain = (s32)(3.1622f * (1 << 23));   //+10db
        }
//        printf("base_gain: %08x\n", base_gain);
        s64 res = (s64)__builtin_muls(temp, base_gain);
        *gain = (u32)(res >> 23);
    }
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN
}

//重新初始化算法的软件部分
void music_effect_alg_reinit(void)
{
    if (bt_decode_is_lhdc() || bt_decode_is_ldac()) {
        return;
    }

    if (music_effect.audio_comm_init_flag == 0) {
        load_code_audio_comm();
        music_effect.audio_comm_init_flag = 1;
    }

    if (music_effect.music_effect_state_set == 0) {
        return;
    }
#if ASR_EN
    if (sys_cb.asr_enable) {
        bsp_asr_stop();
        sys_cb.asr_enable = 0;
    }
#endif
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
    if (SPATIAL_AUDIO_EN(music_effect.music_effect_state_set)) {
        music_spatial_audio_param_init();
    }
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN
#if BT_MUSIC_EFFECT_VBASS_EN
    if (VBASS_EN(music_effect.music_effect_state_set)) {
        music_vbass_audio_param_init();
    }
#endif // BT_MUSIC_EFFECT_VBASS_EN
#if BT_MUSIC_EFFECT_DYEQ_EN
    if (DYEQ_EN(music_effect.music_effect_state_set)) {
        music_dyeq_audio_param_init();
    }
#endif // BT_MUSIC_EFFECT_DYEQ_EN
#if BT_MUSIC_EFFECT_XDRC_EN
    if (XDRC_EN(music_effect.music_effect_state_set)) {
        music_xdrc_audio_param_init();
    }
#endif // BT_MUSIC_EFFECT_XDRC_EN
#if BT_MUSIC_EFFECT_HRTF_RT_EN
    if (HRTF_RT_STA_EN(music_effect.music_effect_state_set)) {
        music_hrtf_rt_param_init();
    }
#endif // BT_MUSIC_EFFECT_HRTF_RT_EN
#if BT_MUSIC_EFFECT_USER_EN
    if (USER_ALG_EN(music_effect.music_effect_state_set)) {
        alg_user_effect_init();
    }
#endif // BT_MUSIC_EFFECT_USER_EN
    TRACE("%s\n", __func__);
}

//重启算法，打开算法硬件部分
void music_effect_alg_restart(void)
{
    if (music_effect.music_effect_state_set == 0) {
        return;
    }
#if BT_MUSIC_EFFECT_DBB_EN
    if (DBB_EN(music_effect.music_effect_state_set)) {
        music_dbb_audio_start_do(0);
        music_effect_set_state(MUSIC_EFFECT_DBB, 1);
    }
#endif // BT_MUSIC_EFFECT_DBB_EN
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
    if (SPATIAL_AUDIO_EN(music_effect.music_effect_state_set)) {
        music_spatial_audio_start_do();
        music_effect_set_state(MUSIC_EFFECT_SPATIAL_AUDIO, 1);
    }
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN
#if BT_MUSIC_EFFECT_VBASS_EN
    if (VBASS_EN(music_effect.music_effect_state_set)) {
        music_effect_set_state(MUSIC_EFFECT_VBASS, 1);
    }
#endif // BT_MUSIC_EFFECT_VBASS_EN
#if BT_MUSIC_EFFECT_DYEQ_EN
    if (DYEQ_EN(music_effect.music_effect_state_set)) {
        music_dyeq_audio_start_do();
        music_effect_set_state(MUSIC_EFFECT_DYEQ, 1);
    }
#endif // BT_MUSIC_EFFECT_DYEQ_EN
#if BT_MUSIC_EFFECT_XDRC_EN
    if (XDRC_EN(music_effect.music_effect_state_set)) {
        music_xdrc_audio_start_do();
        music_effect_set_state(MUSIC_EFFECT_XDRC, 1);
    }
#endif // BT_MUSIC_EFFECT_XDRC_EN
#if BT_MUSIC_EFFECT_HRTF_RT_EN
    if (HRTF_RT_STA_EN(music_effect.music_effect_state_set)) {
        music_hrtf_rt_start_do();
        music_effect_set_state(MUSIC_EFFECT_HRTF_RT, 1);
    }
#endif // BT_MUSIC_EFFECT_HRTF_RT_EN

    TRACE("%s sys_clk %d\n", __func__, music_effect.sys_clk);
}

//暂停算法，关闭算法硬件部分
void music_effect_alg_suspend(u8 reason)
{
    if (music_effect.music_effect_state_set == 0) {
        return;
    }
#if BT_MUSIC_EFFECT_DBB_EN
    if (reason != MUSIC_EFFECT_SUSPEND_FOR_HIRES_DEC) {
        if (DBB_EN(music_effect.music_effect_state_set)) {
            music_effect_set_state(MUSIC_EFFECT_DBB, 0);
            music_dbb_audio_stop_do();
        }
    }
#endif // BT_MUSIC_EFFECT_DBB_EN
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
    if (SPATIAL_AUDIO_EN(music_effect.music_effect_state_set)) {
        music_effect_set_state(MUSIC_EFFECT_SPATIAL_AUDIO, 0);
        music_spatial_audio_stop_do();
    }
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN
#if BT_MUSIC_EFFECT_VBASS_EN
    if (VBASS_EN(music_effect.music_effect_state_set)) {
        music_effect_set_state(MUSIC_EFFECT_VBASS, 0);
    }
#endif // BT_MUSIC_EFFECT_VBASS_EN
#if BT_MUSIC_EFFECT_DYEQ_EN
    if (DYEQ_EN(music_effect.music_effect_state_set)) {
        music_effect_set_state(MUSIC_EFFECT_DYEQ, 0);
        music_dyeq_audio_stop_do();
    }
#endif // BT_MUSIC_EFFECT_DYEQ_EN
#if BT_MUSIC_EFFECT_XDRC_EN
    if (XDRC_EN(music_effect.music_effect_state_set)) {
        music_effect_set_state(MUSIC_EFFECT_XDRC, 0);
        music_xdrc_audio_stop_do();
    }
#endif // BT_MUSIC_EFFECT_XDRC_EN
#if BT_MUSIC_EFFECT_HRTF_RT_EN
    if (HRTF_RT_STA_EN(music_effect.music_effect_state_set)) {
        music_effect_set_state(MUSIC_EFFECT_HRTF_RT, 0);
        music_hrtf_rt_stop_do();
    }
#endif // BT_MUSIC_EFFECT_HRTF_RT_EN
    TRACE("%s\n", __func__);
}

//主线程process
AT(.com_text.effect)
void music_effect_func_process(void)
{
    music_effect_t* cb = &music_effect;
    if (cb->func_process_flag) {
        if (cb->func_process_flag & PROC_FLAG_ALG_REINIT) {
            cb->func_process_flag &= ~PROC_FLAG_ALG_REINIT;
            music_effect_alg_reinit();
            music_effect_alg_restart();
        }
        if (cb->func_process_flag & PROC_FLAG_ALG_SUSPEND) {
            cb->func_process_flag &= ~PROC_FLAG_ALG_SUSPEND;
            music_effect_alg_suspend(MUSIC_EFFECT_SUSPEND_FOR_HIRES_DEC);
        }
    }
}

//设置音效状态
int music_effect_set_state(MUSIC_EFFECT_ALG alg, u8 state)
{
    if (alg >= MUSIC_EFFECT_MAX) {
        return -1;
    }

    music_effect_t* cb = &music_effect;
    u8 delay_cnt = 0;
    u8 sys_clk_req_set = SYS_24M;
    u8 sys_clk_req_max = SYS_24M;
    u16 new_state = cb->music_effect_state;

    if (state) {
        if (new_state & BIT(alg)) {
            return -2;
        }
        new_state |= BIT(alg);
    } else {
        if ((new_state & BIT(alg)) == 0) {
            return -2;
        }
        new_state &= ~BIT(alg);
    }

    //调整音效的主频设置
    if (new_state) {
        for (int i = 0; i < MUSIC_EFFECT_MAX; i++) {
            if ((new_state & BIT(i)) == 0) {
                continue;
            }
            switch (i) {
#if BT_MUSIC_EFFECT_DBB_EN
            case MUSIC_EFFECT_DBB:
                sys_clk_req_set = DBB_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_DBB_EN

#if BT_MUSIC_EFFECT_SPATIAL_AU_EN
            case MUSIC_EFFECT_SPATIAL_AUDIO:
                sys_clk_req_set = SPATIAL_AU_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN

#if BT_MUSIC_EFFECT_VBASS_EN
            case MUSIC_EFFECT_VBASS:
                sys_clk_req_set = VBASS_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_VBASS_EN

#if BT_MUSIC_EFFECT_DYEQ_EN
            case MUSIC_EFFECT_DYEQ:
                sys_clk_req_set = DYEQ_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_DYEQ_EN

#if BT_MUSIC_EFFECT_XDRC_EN
            case MUSIC_EFFECT_XDRC:
                sys_clk_req_set = XDRC_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_XDRC_EN

#if ABP_EN
            case MUSIC_EFFECT_ABP:
                sys_clk_req_set = ABP_SYSCLK_SEL;
                break;
#endif // ABP_EN

#if BT_MUSIC_EFFECT_HRTF_RT_EN
            case MUSIC_EFFECT_HRTF_RT:
                sys_clk_req_set = HRTF_RT_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_HRTF_RT_EN

#if BT_MUSIC_EFFECT_USER_EN
            case MUSIC_EFFECT_ALG_USER:
                sys_clk_req_set = USER_ALG_SYSCLK_SEL;
                break;
#endif // BT_MUSIC_EFFECT_USER_EN

            default:
                break;
            }

            if (sys_clk_req_max < sys_clk_req_set) {
                sys_clk_req_max = sys_clk_req_set;
            }
        }

        if (sys_clk_req_max > cb->sys_clk) {
            TRACE("music effect sys_clk_req %d %d\n", sys_clk_req_max, cb->sys_clk);
            sys_clk_req(INDEX_KARAOK, sys_clk_req_max);
            cb->sys_clk = sys_clk_req_max;
        }
    }

    //设置状态
    cb->music_effect_state = new_state;

    //释放音效的主频设置
    if (sys_clk_req_max < cb->sys_clk) {
        while (cb->process_flag) {
            WDT_CLR();
            delay_5ms(1);
            delay_cnt++;
            if (!codecs_pcm_is_start()) {   //音乐播放结束就不等了
                break;
            }
            if (delay_cnt >= 200) {
                printf("music_effect_set_state time out!\n");
                return -3;
            }
        }
        if (new_state == 0) {
            sys_clk_free(INDEX_KARAOK);
            cb->sys_clk = SYS_24M;
        } else {
            TRACE("music effect sys_clk_req %d %d\n", sys_clk_req_max, cb->sys_clk);
            sys_clk_req(INDEX_KARAOK, sys_clk_req_max);
            cb->sys_clk = sys_clk_req_max;
        }
    }

    TRACE("music_effect_set_state 0x%x\n", new_state);

    return 0;
}

//获取音效是否已经设置打开（不包含因通话、提示音等暂停的状态）
AT(.com_text.effect)
bool music_effect_get_state(MUSIC_EFFECT_ALG alg)
{
    music_effect_t* cb = &music_effect;
    return (bool)((cb->music_effect_state_set & BIT(alg)) > 0);
}

//获取音效实际的状态
AT(.com_text.effect)
bool music_effect_get_state_real(MUSIC_EFFECT_ALG alg)
{
    music_effect_t* cb = &music_effect;
    return (bool)((cb->music_effect_state & BIT(alg)) > 0);
}

///动态低音音效
#if BT_MUSIC_EFFECT_DBB_EN

#if BT_A2DP_VENDOR_AUDIO_EN     //96K DAC
const unsigned char dbb_coef_param[457] = {
/*--------------------------------------------------------------------------------------------------
  [DAC Sample Rate]  88.2KHz/96KHz
  [Level Count]      11
  [Band Count]       1
----------------------------------------------------------------------------------------------------
  [Band]             1
  [Frequency]        50
  [Q]                0.75
  [Filter Type]      peak
  [Gain of Level  0] -10
  [Gain of Level  1] -8
  [Gain of Level  2] -6
  [Gain of Level  3] -4
  [Gain of Level  4] -2
  [Gain of Level  5] 0
  [Gain of Level  6] 2
  [Gain of Level  7] 4
  [Gain of Level  8] 6
  [Gain of Level  9] 8
  [Gain of Level 10] 10
--------------------------------------------------------------------------------------------------*/
	0x04, 0x01, 0x0b, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x90,
	0x96, 0xfa, 0x07, 0x1b, 0x95, 0xf5, 0x07, 0x13, 0x26, 0xf0, 0x0f, 0xee, 0xd9, 0x0f, 0xf0, 0x56,
	0xd4, 0x0f, 0xf8, 0xa2, 0xc0, 0xfb, 0x07, 0x35, 0x22, 0xf6, 0x07, 0x3e, 0xdd, 0xf1, 0x0f, 0xc3,
	0x22, 0x0e, 0xf0, 0x2b, 0x1d, 0x0e, 0xf8, 0x8d, 0xdc, 0xfc, 0x07, 0x02, 0x8e, 0xf6, 0x07, 0xf6,
	0x64, 0xf3, 0x0f, 0x0b, 0x9b, 0x0c, 0xf0, 0x72, 0x95, 0x0c, 0xf8, 0x0b, 0xee, 0xfd, 0x07, 0xe3,
	0xd9, 0xf6, 0x07, 0x54, 0xc2, 0xf4, 0x0f, 0xad, 0x3d, 0x0b, 0xf0, 0x13, 0x38, 0x0b, 0xf8, 0xb0,
	0xf8, 0xfe, 0x07, 0xd0, 0x06, 0xf7, 0x07, 0xe6, 0xf9, 0xf5, 0x0f, 0x1b, 0x06, 0x0a, 0xf0, 0x81,
	0x00, 0x0a, 0xf8, 0x00, 0x00, 0x00, 0x08, 0x59, 0x15, 0xf7, 0x07, 0xbe, 0x0f, 0xf7, 0x0f, 0x43,
	0xf0, 0x08, 0xf0, 0xa8, 0xea, 0x08, 0xf8, 0x72, 0x07, 0x01, 0x08, 0xa8, 0x05, 0xf7, 0x07, 0x7f,
	0x07, 0xf8, 0x0f, 0x82, 0xf8, 0x07, 0xf0, 0xe7, 0xf2, 0x07, 0xf8, 0x7f, 0x12, 0x02, 0x08, 0x84,
	0xd7, 0xf6, 0x07, 0x68, 0xe4, 0xf8, 0x0f, 0x99, 0x1b, 0x07, 0xf0, 0xfe, 0x15, 0x07, 0xf8, 0xae,
	0x24, 0x03, 0x08, 0x4c, 0x8a, 0xf6, 0x07, 0x5f, 0xa9, 0xf9, 0x0f, 0xa2, 0x56, 0x06, 0xf0, 0x07,
	0x51, 0x06, 0xf8, 0xa1, 0x41, 0x04, 0x08, 0xf5, 0x1c, 0xf6, 0x07, 0xfa, 0x58, 0xfa, 0x0f, 0x07,
	0xa7, 0x05, 0xf0, 0x6b, 0xa1, 0x05, 0xf8, 0x1b, 0x6d, 0x05, 0x08, 0x0a, 0x8e, 0xf5, 0x07, 0x8a,
	0xf5, 0xfa, 0x0f, 0x77, 0x0a, 0x05, 0xf0, 0xdc, 0x04, 0x05, 0xf8, 0x8f, 0x1c, 0xfa, 0x07, 0x40,
	0xaa, 0xf4, 0x07, 0x2f, 0xc0, 0xee, 0x0f, 0xd2, 0x3f, 0x11, 0xf0, 0x32, 0x39, 0x11, 0xf8, 0xd5,
	0x60, 0xfb, 0x07, 0xad, 0x43, 0xf5, 0x07, 0xe1, 0x9d, 0xf0, 0x0f, 0x20, 0x62, 0x0f, 0xf0, 0x80,
	0x5b, 0x0f, 0xf8, 0xbd, 0x95, 0xfc, 0x07, 0xe3, 0xb8, 0xf5, 0x07, 0xfe, 0x47, 0xf2, 0x0f, 0x03,
	0xb8, 0x0d, 0xf0, 0x62, 0xb1, 0x0d, 0xf8, 0x50, 0xbf, 0xfd, 0x07, 0x61, 0x0b, 0xf6, 0x07, 0x10,
	0xc4, 0xf3, 0x0f, 0xf1, 0x3b, 0x0c, 0xf0, 0x50, 0x35, 0x0c, 0xf8, 0x77, 0xe1, 0xfe, 0x07, 0x33,
	0x3c, 0xf6, 0x07, 0x08, 0x17, 0xf5, 0x0f, 0xf9, 0xe8, 0x0a, 0xf0, 0x57, 0xe2, 0x0a, 0xf8, 0x00,
	0x00, 0x00, 0x08, 0xf5, 0x4b, 0xf6, 0x07, 0x52, 0x45, 0xf6, 0x0f, 0xaf, 0xba, 0x09, 0xf0, 0x0c,
	0xb4, 0x09, 0xf8, 0xb1, 0x1e, 0x01, 0x08, 0xd5, 0x3a, 0xf6, 0x07, 0xe3, 0x52, 0xf7, 0x0f, 0x1e,
	0xad, 0x08, 0xf0, 0x7b, 0xa6, 0x08, 0xf8, 0x52, 0x41, 0x02, 0x08, 0x93, 0x08, 0xf6, 0x07, 0x41,
	0x43, 0xf8, 0x0f, 0xc0, 0xbc, 0x07, 0xf0, 0x1c, 0xb6, 0x07, 0xf8, 0xb9, 0x6b, 0x03, 0x08, 0x7e,
	0xb4, 0xf5, 0x07, 0x93, 0x19, 0xf9, 0x0f, 0x6e, 0xe6, 0x06, 0xf0, 0xca, 0xdf, 0x06, 0xf8, 0xd8,
	0xa1, 0x04, 0x08, 0x75, 0x3d, 0xf5, 0x07, 0xa9, 0xd8, 0xf9, 0x0f, 0x58, 0x27, 0x06, 0xf0, 0xb3,
	0x20, 0x06, 0xf8, 0xca, 0xe7, 0x05, 0x08, 0xe2, 0xa1, 0xf4, 0x07, 0x07, 0x83, 0xfa, 0x0f, 0xfa,
	0x7c, 0x05, 0xf0, 0x55, 0x76, 0x05, 0xf8, 0xb3, 0x7b,
};
#else                           //44.1K/48K DAC


/* 该文件代码由Dynamic Bass Boost Tool生成。 */
const u8 dbb_coef_param[248] = {
/* 等级总数：6    	*/
/* 滤波器类型：low shelf  	*/
/* 中心频率：200    	*/
/* Q值：0.70      	*/
/* 增益：0, 1, 6, 9, 12, 13,   	*/
	0x02, 0x06, 0x00, 0x01, 0x06, 0x09, 0x0c, 0x0d, 0x00, 0x00, 0x00, 0x08, 0xc7, 0xd1, 0xb4, 0x07, 
	0x0d, 0x71, 0xb3, 0x0f, 0xf4, 0x8e, 0x4c, 0xf0, 0x3a, 0x2e, 0x4b, 0xf8, 0x7c, 0x34, 0x02, 0x08, 
	0xc1, 0xc9, 0xb4, 0x07, 0xbe, 0x9c, 0xb5, 0x0f, 0x97, 0x77, 0x4a, 0xf0, 0x18, 0x16, 0x49, 0xf8, 
	0x7c, 0x54, 0x0d, 0x08, 0x64, 0xb6, 0xb3, 0x07, 0xca, 0x93, 0xbf, 0x0f, 0xd9, 0xe8, 0x40, 0xf0, 
	0xc3, 0x71, 0x3f, 0xf8, 0xfa, 0x26, 0x14, 0x08, 0x4c, 0x53, 0xb2, 0x07, 0x6d, 0xe7, 0xc4, 0x0f, 
	0x6c, 0xd8, 0x3b, 0xf0, 0x93, 0x45, 0x3a, 0xf8, 0xf4, 0x25, 0x1b, 0x08, 0x1e, 0x5f, 0xb0, 0x07, 
	0x61, 0xca, 0xc9, 0x0f, 0x91, 0x3e, 0x37, 0xf0, 0xe0, 0x83, 0x35, 0xf8, 0x34, 0x87, 0x1d, 0x08, 
	0x9c, 0x97, 0xaf, 0x07, 0xf4, 0x53, 0xcb, 0x0f, 0x0a, 0xcf, 0x35, 0xf0, 0x2e, 0x04, 0x34, 0xf8, 
	0x00, 0x00, 0x00, 0x08, 0x1c, 0x4e, 0xae, 0x07, 0xeb, 0xac, 0xac, 0x0f, 0x16, 0x53, 0x53, 0xf0, 
	0xe5, 0xb1, 0x51, 0xf8, 0x67, 0x66, 0x02, 0x08, 0x63, 0x45, 0xae, 0x07, 0xab, 0x09, 0xaf, 0x0f, 
	0x61, 0x0e, 0x51, 0xf0, 0x42, 0x6c, 0x4f, 0xf8, 0x23, 0x83, 0x0e, 0x08, 0x99, 0x1a, 0xad, 0x07, 
	0xfb, 0xe1, 0xb9, 0x0f, 0x79, 0xb1, 0x46, 0xf0, 0xb9, 0xf5, 0x44, 0xf8, 0x55, 0xf1, 0x15, 0x08, 
	0x61, 0x99, 0xab, 0x07, 0x10, 0xae, 0xbf, 0x0f, 0xef, 0x34, 0x41, 0xf0, 0x48, 0x58, 0x3f, 0xf8, 
	0x8b, 0x90, 0x1d, 0x08, 0xd5, 0x7a, 0xa9, 0x07, 0x87, 0xff, 0xc4, 0x0f, 0xfe, 0x39, 0x3c, 0xf0, 
	0x24, 0x2e, 0x3a, 0xf8, 0x6e, 0x28, 0x20, 0x08, 0x71, 0xa2, 0xa8, 0x07, 0xdf, 0xab, 0xc6, 0x0f, 
	0x7a, 0xac, 0x3a, 0xf0, 0x7b, 0x8d, 0x38, 0xf8, 
};

#endif

void music_dbb_eq_index_init(u32* coef_l, u32* coef_r)
{
    dbb_param_cb_t cb;

    cb.dbb_param = dbb_coef_param;
    cb.param_len = sizeof(dbb_coef_param);

    cb.coef_l = (s32*)coef_l;
    cb.coef_r = (s32*)coef_r;
    cb.dac_band_cnt = BT_MUSIC_EFFECT_DBB_BAND_CNT;
    music_dbb_init(&cb);
}

void music_dbb_audio_start_do(u8 is_def_level)
{
    u8 vol_level = dac_dvol_table[sys_cb.vol] + sys_cb.gain_offset;
    u8 bass_level = is_def_level ? BT_MUSIC_EFFECT_DBB_DEF_LEVEL : music_dbb_get_bass_level();
    if (vol_level > 60) {
        vol_level = 60;
    }
    music_dbb_update_param(vol_level, bass_level);
    music_set_eq_by_num(sys_cb.eq_mode);
    TRACE("%s\n", __func__);
}

void music_dbb_audio_stop_do(void)
{
    music_dbb_stop();
    music_set_eq_by_num(sys_cb.eq_mode);
    TRACE("%s\n", __func__);
}

void music_dbb_audio_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (DBB_EN(cb->music_effect_state_set)) {
        return;
    }

    cb->music_effect_state_set |= DBB_STA_BIT;

    music_dbb_audio_start_do(1);

    music_effect_set_state(MUSIC_EFFECT_DBB, 1);

    TRACE("music_dbb_audio_start\n");
}

void music_dbb_audio_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (DBB_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    cb->music_effect_state_set &= ~DBB_STA_BIT;

    music_dbb_audio_stop_do();

    music_effect_set_state(MUSIC_EFFECT_DBB, 0);

    TRACE("music_dbb_audio_stop\n");
}

void music_dbb_audio_set_vol_do(u8 vol_level)
{
    music_effect_t* cb = &music_effect;

    if ((DBB_EN(cb->music_effect_state_set) == 0) || (DBB_EN(cb->music_effect_state) == 0)) {
        return;
    }

    u8 bass_level = music_dbb_get_bass_level();
#if SWETZ_DBB_LEVEL_FOLLOW_VOL
    if(sys_cb.vol == 16){
            bass_level = 0;
    }
    else if(sys_cb.vol == 15){
            bass_level = 1;
    }
    else if(sys_cb.vol == 14){
            bass_level = 2;
    }
    else if(sys_cb.vol == 13){
            bass_level = 3;
    }
    else if(sys_cb.vol == 12){
            bass_level = 4;
    }
    else {
             bass_level = 5;

    }
#endif

    int res = music_dbb_update_param(vol_level, bass_level);
 



    //0:不需要change eq，1:先change eq再调音量，2:先调音量再change eq
    if (res == 1) {
        music_set_eq_by_num(sys_cb.eq_mode);
        dac_vol_set(dac_dvol_tbl_db[vol_level]);
    } else if (res == 2) {
        dac_vol_set(dac_dvol_tbl_db[vol_level]);
        music_set_eq_by_num(sys_cb.eq_mode);
    } else {
        dac_vol_set(dac_dvol_tbl_db[vol_level]);
    }
    TRACE("bass_level: %d, vol: -%d dB\n", bass_level, vol_level);
}

void music_dbb_audio_set_bass_level(u8 bass_level)
{
    music_effect_t* cb = &music_effect;

    if ((DBB_EN(cb->music_effect_state_set) == 0) || (DBB_EN(cb->music_effect_state) == 0)) {
        return;
    }

    u8 vol_level = dac_dvol_table[sys_cb.vol] + sys_cb.gain_offset;

    if (vol_level > 60) {
        vol_level = 60;
    }
    music_dbb_update_param(vol_level, bass_level);
    music_set_eq_by_num(sys_cb.eq_mode);
    TRACE("bass_level: %d, vol: -%d dB\n", bass_level, vol_level);
}

#endif // BT_MUSIC_EFFECT_DBB_EN

///空间音效
#if BT_MUSIC_EFFECT_SPATIAL_AU_EN

#if BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
#define SPATIAL_AU_DL_SAMPLES_L         10
#define SPATIAL_AU_DL_SAMPLES_R         10
s32 spatial_audio_delay_buf_l[256 + SPATIAL_AU_DL_SAMPLES_L] AT(.music_exbuff.headset_v3d);
s32 spatial_audio_delay_buf_r[256 + SPATIAL_AU_DL_SAMPLES_R] AT(.music_exbuff.headset_v3d);
#if (SPATIAL_AU_DL_SAMPLES_L >= 256) || (SPATIAL_AU_DL_SAMPLES_R >= 256)
#error "EFFECT: DL_SAMPLES overflow."
#endif
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_DLEN

#if BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN
u8 music_spatial_audio_get_fix_eq_sta(void)
{
    return music_effect.spatial_audio_fix_eq;
}
#endif // BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN

void music_spatial_audio_start_do(void)
{
#if BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN
    music_effect.spatial_audio_fix_eq = 1;
#endif // BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN

    if (!codecs_pcm_is_start()) {
        music_effect.spatial_audio_eq_state = 4;
        music_set_eq_by_num(sys_cb.eq_mode);
    } else {
        music_effect.spatial_audio_eq_state = 0;
        for (int i = 0; i < 4; i++) {   //分四次增加增益
            music_effect.spatial_audio_eq_state++;
            music_set_eq_by_num(sys_cb.eq_mode);
//            delay_5ms(4);
        }
    }

    TRACE("%s\n", __func__);
}

void music_spatial_audio_stop_do(void)
{
#if BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN
    music_effect.spatial_audio_fix_eq = 0;
#endif // BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN

    if (!codecs_pcm_is_start()) {
        music_effect.spatial_audio_eq_state = 0;
        music_set_eq_by_num(sys_cb.eq_mode);
    } else {
        music_effect.spatial_audio_eq_state = 4;
        for (int i = 0; i < 4; i++) {   //分四次减小增益
            music_effect.spatial_audio_eq_state--;
            music_set_eq_by_num(sys_cb.eq_mode);
//            delay_5ms(4);
        }
    }

    TRACE("%s\n", __func__);
}

void music_spatial_audio_param_init(void)
{
    v3d_init();
#if BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
    if (!v3d_delay_buf_init(spatial_audio_delay_buf_l, sizeof(spatial_audio_delay_buf_l), SPATIAL_AU_DL_SAMPLES_L,
                            spatial_audio_delay_buf_r, sizeof(spatial_audio_delay_buf_r), SPATIAL_AU_DL_SAMPLES_R)) {
        printf("V3D delay buffer ERROR\n");
    }
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
    v3d_set_fade(0);
    TRACE("%s\n", __func__);
}

void music_spatial_audio_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (SPATIAL_AUDIO_EN(cb->music_effect_state_set)) {
        return;
    }

    cb->music_effect_state_set |= SPATIAL_AUDIO_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        music_spatial_audio_param_init();
        music_effect_set_state(MUSIC_EFFECT_SPATIAL_AUDIO, 1);
        if (codecs_pcm_is_start()) {
//            delay_5ms(10);
            u16 delay_cnt = 10;
            while (delay_cnt) {
                WDT_CLR();
                bt_thread_check_trigger();
                delay_5ms(1);
                delay_cnt--;
            }
        }
        music_spatial_audio_start_do();
    }

    TRACE("music_spatial_audio_start %d\n", music_effect.spatial_audio_eq_state);
}

void music_spatial_audio_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (SPATIAL_AUDIO_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    v3d_set_fade(1);
    if (codecs_pcm_is_start()) {
//        delay_5ms(10);
        u16 delay_cnt = 10;
        while (delay_cnt) {
            WDT_CLR();
            bt_thread_check_trigger();
            delay_5ms(1);
            delay_cnt--;
        }
    }

    music_spatial_audio_stop_do();

    while (!v3d_fade_is_done(1)) {
        WDT_CLR();
        delay_5ms(1);
        bt_thread_check_trigger();
        if (!codecs_pcm_is_start()) {   //音乐播放结束就不等了
            break;
        }
    }

    music_effect_set_state(MUSIC_EFFECT_SPATIAL_AUDIO, 0);

    cb->music_effect_state_set &= ~SPATIAL_AUDIO_STA_BIT;

    TRACE("music_spatial_audio_stop %d\n", music_effect.spatial_audio_eq_state);
}
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_EN

///虚拟低音
#if BT_MUSIC_EFFECT_VBASS_EN

#define VBASS_CAL_INTENSITY(x,bits)     ((s32)(.5+(0.01f * (x))*(((s32)1)<<(bits))))

u8 vbass_cb[232] AT(.music_buff.vbass);

AT(.audio_text.vbass)
void vbass_frame_process(u32* buf, u32 samples, u32 ch_idx, u32 in_24bits)
{
    int i;
    s32 tmp32;
    s16* pcm16 = (s16*)buf;
    s32* pcm32 = (s32*)buf;

    if (in_24bits) {
        if (ch_idx == 0 || ch_idx == 1) {
            for (i = 0; i < samples; i++) {
                tmp32 = pcm32[2*i+ch_idx];
                tmp32 = vbass_process(vbass_cb, tmp32);
                pcm32[2*i+0] = tmp32;
                pcm32[2*i+1] = tmp32;
            }
        } else if (ch_idx == 2) {
            for (i = 0; i < samples; i++) {
                tmp32 = (pcm32[2*i+0] + pcm32[2*i+1]) >> 1;
                tmp32 = vbass_process(vbass_cb, tmp32);
                pcm32[2*i+0] = tmp32;
                pcm32[2*i+1] = tmp32;
            }
        }
    } else {
        if (ch_idx == 0 || ch_idx == 1) {
            for (i = 0; i < samples; i++) {
                tmp32 = ((s32)pcm16[2*i+ch_idx]) << 8;
                tmp32 = vbass_process(vbass_cb, tmp32);
                pcm16[2*i+0] = (s16)(tmp32 >> 8);
                pcm16[2*i+1] = (s16)(tmp32 >> 8);
            }
        } else if (ch_idx == 2) {
            for (i = 0; i < samples; i++) {
                tmp32 = ((s32)pcm16[2*i+0] + pcm16[2*i+1]) << 7;
                tmp32 = vbass_process(vbass_cb, tmp32);
                pcm16[2*i+0] = (s16)(tmp32 >> 8);
                pcm16[2*i+1] = (s16)(tmp32 >> 8);
            }
        }
    }
}

void music_vbass_audio_param_init(void)
{
    vbass_param_cb_t vbass_param_cb;
    vbass_param_cb.cutoff_frequency = 2;
    vbass_param_cb.intensity_set = 50;
    vbass_param_cb.vbass_high_frequency_set = 2;
    vbass_param_cb.pregain = 0.707 * (1 << 23);     //-3db
    vbass_param_cb.intensity = VBASS_CAL_INTENSITY(vbass_param_cb.intensity_set, 15);
    vbass_init(vbass_cb, &vbass_param_cb);
    TRACE("%s\n", __func__);
}

void music_vbass_audio_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (VBASS_EN(cb->music_effect_state_set)) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set |= VBASS_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        music_vbass_audio_param_init();
        music_effect_set_state(MUSIC_EFFECT_VBASS, 1);
    }

    //delay一下再淡入
    delay_5ms(14);
    dac_fade_in();

    TRACE("music_vbass_audio_start\n");
}

void music_vbass_audio_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (VBASS_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set &= ~VBASS_STA_BIT;

    music_effect_set_state(MUSIC_EFFECT_VBASS, 0);

    //delay一下再淡入
    delay_5ms(8);
    dac_fade_in();

    TRACE("music_vbass_audio_stop\n");
}

void music_vbass_set_param(u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set, u32 pregain)
{
    vbass_set_param(vbass_cb, cutoff_frequency, intensity, vbass_high_frequency_set, pregain);
    TRACE("music_vbass_set_param %d %d %d\n", cutoff_frequency, intensity, vbass_high_frequency_set);
}
#endif // BT_MUSIC_EFFECT_VBASS_EN

///动态EQ
#if BT_MUSIC_EFFECT_DYEQ_EN

#define VBASS_CAL_INTENSITY(x, bits)    ((s32)(.5+(0.01f * (x))*(((s32)1)<<(bits))))

u8 dyeq_cb[200] AT(.music_buff.dyeq);
u8 dyeq_drc_cb[84] AT(.music_buff.dyeq);
soft_vol_t dyeq_soft_vol AT(.music_buff.dyeq);
#if BT_MUSIC_EFFECT_DYEQ_VBASS_EN
u8 dyeq_vbass_cb[232] AT(.music_buff.vbass);
#endif // BT_MUSIC_EFFECT_DYEQ_VBASS_EN

AT(.audio_rodata.dyeq.trace_str)
const char dyeq_set_vol_str[] = "music_dyeq_audio_set_vol_by_dvol %d %d\n";


AT(.audio_text.dyeq)
ALWAYS_INLINE s32 dyeq_mono_process(s32 data)
{
    s32 tmp32;
    soft_vol_process_mono_24bits_one_sample(&dyeq_soft_vol, &data);
#if BT_MUSIC_EFFECT_DYEQ_VBASS_EN
    tmp32 = vbass_process(dyeq_vbass_cb, data);
#else
    tmp32 = (s32)data;
#endif // BT_MUSIC_EFFECT_DYEQ_VBASS_EN
    tmp32 = dynamic_eq_process(dyeq_cb, tmp32);
    tmp32 = dyeq_drc_v3_calc(tmp32, dyeq_drc_cb);
    return tmp32;
}

AT(.audio_text.dyeq)
void dynamic_eq_frame_process(u32* buf, u32 samples, u32 ch_idx, u32 in_24bits)
{
    int i;
    s32 tmp32;
    s32 *pcm32 = (s32*)buf;
    s16 *pcm16 = (s16*)buf;

    if (in_24bits) {
        if (ch_idx == 0 || ch_idx == 1) {
            for (i = 0; i < samples; i++) {
                tmp32 = pcm32[2*i+ch_idx];
                tmp32 = dyeq_mono_process(tmp32);
                pcm32[2*i+0] = tmp32;
                pcm32[2*i+1] = tmp32;
            }
        } else if (ch_idx == 2) {
            for (i = 0; i < samples; i++) {
                tmp32 = (pcm32[2*i+0] + pcm32[2*i+1]) >> 1;
                tmp32 = dyeq_mono_process(tmp32);
                pcm32[2*i+0] = tmp32;
                pcm32[2*i+1] = tmp32;
            }
        }
    } else {
        if (ch_idx == 0 || ch_idx == 1) {
            for (i = 0; i < samples; i++) {
                tmp32 = ((s32)pcm16[2*i+ch_idx]) << 8;
                tmp32 = dyeq_mono_process(tmp32);
                pcm16[2*i+0] = (s16)(tmp32 >> 8);
                pcm16[2*i+1] = (s16)(tmp32 >> 8);
            }
        } else if (ch_idx == 2) {
            for (i = 0; i < samples; i++) {
                tmp32 = ((s32)pcm16[2*i+0] + pcm16[2*i+1]) << 7;
                tmp32 = dyeq_mono_process(tmp32);
                pcm16[2*i+0] = (s16)(tmp32 >> 8);
                pcm16[2*i+1] = (s16)(tmp32 >> 8);
            }
        }
    }
}

void music_dyeq_audio_start_do(void)
{
    dac_vol_set(DIG_N0DB);
    TRACE("%s\n", __func__);
}

void music_dyeq_audio_stop_do(void)
{
    u8 vol_level = dac_dvol_table[sys_cb.vol] + sys_cb.gain_offset;
    if (vol_level > 60) {
        vol_level = 60;
    }
    dac_vol_set(dac_dvol_tbl_db[vol_level]);
    TRACE("%s\n", __func__);
}

void music_dyeq_audio_param_init(void)
{
    memset(dyeq_cb, 0, sizeof(dyeq_cb));
    memset(dyeq_drc_cb, 0, sizeof(dyeq_drc_cb));
#if BT_MUSIC_EFFECT_DYEQ_VBASS_EN
    memset(dyeq_vbass_cb, 0, sizeof(dyeq_vbass_cb));
#endif // BT_MUSIC_EFFECT_DYEQ_VBASS_EN
    bsp_set_effect_by_abt();

    u8 vol_level = dac_dvol_table[sys_cb.vol] + sys_cb.gain_offset;
    if (vol_level > 60) {
        vol_level = 60;
    }
    soft_vol_init(&dyeq_soft_vol);
    soft_vol_set_vol_param(&dyeq_soft_vol, dac_dvol_tbl_db[vol_level], 1);

    TRACE("%s\n", __func__);
}

void music_dyeq_audio_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (DYEQ_EN(cb->music_effect_state_set)) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set |= DYEQ_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        music_dyeq_audio_param_init();
        music_dyeq_audio_start_do();
        music_effect_set_state(MUSIC_EFFECT_DYEQ, 1);
    }

    //delay一下再淡入
    delay_5ms(14);
    dac_fade_in();

    TRACE("music_dyeq_audio_start\n");
}

void music_dyeq_audio_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (DYEQ_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set &= ~DYEQ_STA_BIT;

    music_dyeq_audio_stop_do();

    music_effect_set_state(MUSIC_EFFECT_DYEQ, 0);

    //delay一下再淡入
    delay_5ms(8);
    dac_fade_in();

    TRACE("music_dyeq_audio_stop\n");
}

void music_dyeq_set_param(u8 *buf)
{
    dyeq_coef_update(dyeq_cb, buf);
}

void music_dyeq_drc_set_param(void *buf)
{
    dyeq_drc_v3_set_param(buf, dyeq_drc_cb);
}

void music_dyeq_audio_set_vol_do(u8 vol_level)
{
    u8 vol_direct_set_flag = codecs_pcm_is_start() ? 0 : 1;
    if (vol_level > 60) {
        vol_level = 60;
    }
    soft_vol_set_vol_param(&dyeq_soft_vol, dac_dvol_tbl_db[vol_level], vol_direct_set_flag);
    TRACE("music_dyeq_audio_set_vol_do %d %d\n", vol_level, vol_direct_set_flag);
}

AT(.audio_text.dyeq.soft_vol)
void music_dyeq_audio_set_vol_by_dvol(u16 vol)
{
    u8 vol_direct_set_flag = codecs_pcm_is_start() ? 0 : 1;
    soft_vol_set_vol_param(&dyeq_soft_vol, vol, vol_direct_set_flag);
    TRACE(dyeq_set_vol_str, vol, vol_direct_set_flag);
}

AT(.audio_text.dyeq)
void dynamic_eq_clear_cache(void)
{
    dyeq_clear_cache(dyeq_cb);
}

#if BT_MUSIC_EFFECT_DYEQ_VBASS_EN
void music_vbass_set_param(u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set, u32 pregain)
{
    vbass_set_param(dyeq_vbass_cb, cutoff_frequency, intensity, vbass_high_frequency_set, pregain);
    TRACE("music_vbass_set_param %d %d %d %d\n", cutoff_frequency, intensity, vbass_high_frequency_set, pregain);
}
#endif // BT_MUSIC_EFFECT_DYEQ_VBASS_EN

#endif // BT_MUSIC_EFFECT_DYEQ_EN

///2段DRC
#if BT_MUSIC_EFFECT_XDRC_EN

soft_vol_t xdrc_soft_vol AT(.music_buff.xdrc);
#if BT_MUSIC_EFFECT_XDRC_EQ_EN
s32 xdrc_preeq_cb[BT_MUSIC_EFFECT_XDRC_EQ_BAND*7 + 6] AT(.music_buff.xdrc);  //*coef, *zpara, band, pre_gain, coef(5*BAND), calc_buf((BAND+1)*2)
#endif
#if BT_MUSIC_EFFECT_XDRC_DELAY_CNT
u8  xdrc_delay_cb[16] AT(.music_buff.xdrc);
s32 xdrc_delay_buf[256] AT(.music_buff.xdrc);
#endif
//lp_cb
s32 xdrc_lp_cb[1*7 + 6] AT(.music_buff.xdrc);
s32 xdrc_lp_cb2[1*7 + 6] AT(.music_buff.xdrc);   //分频点过两次
//hp_cb
s32 xdrc_hp_cb[1*7 + 6] AT(.music_buff.xdrc);
s32 xdrc_hp_cb2[1*7 + 6] AT(.music_buff.xdrc);   //分频点过两次
//lp_cb
s32 xdrc_lp_exp_cb[1*7 + 6] AT(.music_buff.xdrc);
//hp_cb
s32 xdrc_hp_exp_cb[1*7 + 6] AT(.music_buff.xdrc);
//drc_cb
u8 xdrc_drclp_cb[14 * 4] AT(.music_buff.xdrc);
//drc_cb
u8 xdrc_drchp_cb[14 * 4] AT(.music_buff.xdrc);
//drc_cb
u8 xdrc_drcall_cb[14 * 4] AT(.music_buff.xdrc);

AT(.audio_rodata.xdrc.trace_str)
const char xdrc_set_vol_str[] = "music_xdrc_audio_set_vol_by_dvol %d %d\n";


AT(.audio_text.xdrc)
ALWAYS_INLINE s32 xdrc_process(s16 data)
{
    s32 pcm32;
    s32 pcm32_lp;
    s32 pcm32_hp;
    s32 pcm32_lp_exp, drc_gain_lp;
    s32 pcm32_hp_exp, drc_gain_hp;

    soft_vol_process_mono_one_sample(&xdrc_soft_vol, &data);
    pcm32 = (s32)data;

#if BT_MUSIC_EFFECT_XDRC_EQ_EN
    //软件EQ
    pcm32 = xdrc_softeq_proc(xdrc_preeq_cb,pcm32);
#endif // BT_MUSIC_EFFECT_XDRC_EQ_EN
    //LP/HP filter for DrcGain
    pcm32_lp_exp = xdrc_softeq_proc(xdrc_lp_exp_cb, pcm32);
    pcm32_hp_exp = xdrc_softeq_proc(xdrc_hp_exp_cb, pcm32);
    //expand freq to get drc_gain
    drc_gain_lp = xdrc_drc_process_gain(xdrc_drclp_cb, pcm32_lp_exp);
    drc_gain_hp = xdrc_drc_process_gain(xdrc_drchp_cb, pcm32_hp_exp);

#if BT_MUSIC_EFFECT_XDRC_DELAY_EN
    pcm32 = pcmdelay_mono_s32(xdrc_delay_cb, pcm32);
#endif // BT_MUSIC_EFFECT_XDRC_DELAY_EN

    //全频pcm32 过 drc_gain
    pcm32_lp = xdrc_drc_get_pcm32_s(pcm32, drc_gain_lp);
    pcm32_hp = xdrc_drc_get_pcm32_s(pcm32, drc_gain_hp);
    //LP/HP filter
    pcm32_lp = xdrc_softeq_proc(xdrc_lp_cb, pcm32_lp);
    pcm32_lp = xdrc_softeq_proc(xdrc_lp_cb2, pcm32_lp);
    pcm32_hp = xdrc_softeq_proc(xdrc_hp_cb, pcm32_hp);
    pcm32_hp = xdrc_softeq_proc(xdrc_hp_cb2, pcm32_hp);
    //LP/HP mix
    pcm32 = pcm32_lp + pcm32_hp;
    //all DRC
    pcm32 = xdrc_drc_process_s16(xdrc_drcall_cb, pcm32);

    return pcm32;
}

AT(.audio_text.xdrc)
void xdrc_frame_process(u32* buf, u32 samples, u32 ch_idx, u32 in_24bits)
{
    int i;
    s16 tmp16;
    s32 tmp32;
    s16 *pcm16 = (s16*)buf;
    s32 *pcm32 = (s32*)buf;

    if (in_24bits) {
        if (ch_idx == 0 || ch_idx == 1) {
            for (i = 0; i < samples; i++) {
                tmp16 = (s16)(pcm32[2*i+ch_idx] >> 8);
                tmp32 = xdrc_process(tmp16);
                pcm32[2*i+0] = (s32)(tmp32 << 8);
                pcm32[2*i+1] = (s32)(tmp32 << 8);
            }
        } else if (ch_idx == 2) {
            for (i = 0; i < samples; i++) {
                tmp16 = (s16)((pcm32[2*i+0] + pcm32[2*i+1]) >> 9);
                tmp32 = xdrc_process(tmp16);
                pcm32[2*i+0] = (s32)(tmp32 << 8);
                pcm32[2*i+1] = (s32)(tmp32 << 8);
            }
        }
    } else {
        if (ch_idx == 0 || ch_idx == 1) {
            for (i = 0; i < samples; i++) {
                tmp16 = pcm16[2*i+ch_idx];
                tmp32 = xdrc_process(tmp16);
                pcm16[2*i+0] = (s16)tmp32;
                pcm16[2*i+1] = (s16)tmp32;
            }
        } else if (ch_idx == 2) {
            for (i = 0; i < samples; i++) {
                tmp16 = (s16)(((s32)pcm16[2*i+0] + pcm16[2*i+1]) >> 1);
                tmp32 = xdrc_process(tmp16);
                pcm16[2*i+0] = (s16)tmp32;
                pcm16[2*i+1] = (s16)tmp32;
            }
        }
    }
}

void music_xdrc_audio_start_do(void)
{
    dac_vol_set(DIG_N0DB);
    TRACE("%s\n", __func__);
}

void music_xdrc_audio_stop_do(void)
{
    u8 vol_level = dac_dvol_table[sys_cb.vol] + sys_cb.gain_offset;
    dac_vol_set(dac_dvol_tbl_db[vol_level]);
    TRACE("%s\n", __func__);
}

void music_xdrc_audio_param_init(void)
{
#if BT_MUSIC_EFFECT_XDRC_DELAY_EN
    pcmdelay_init(xdrc_delay_cb, xdrc_delay_buf,sizeof(xdrc_delay_buf), 4, BT_MUSIC_EFFECT_XDRC_DELAY_CNT);
#endif // BT_MUSIC_EFFECT_XDRC_DELAY_EN

#if BT_MUSIC_EFFECT_XDRC_EQ_EN
    memset(xdrc_preeq_cb, 0, sizeof(xdrc_preeq_cb));
#endif // BT_MUSIC_EFFECT_XDRC_EQ_EN
    memset(xdrc_lp_cb, 0, sizeof(xdrc_lp_cb));
    memset(xdrc_lp_cb2, 0, sizeof(xdrc_lp_cb2));
    memset(xdrc_hp_cb, 0, sizeof(xdrc_hp_cb));
    memset(xdrc_hp_cb2, 0, sizeof(xdrc_hp_cb2));
    memset(xdrc_lp_exp_cb, 0, sizeof(xdrc_lp_exp_cb));
    memset(xdrc_hp_exp_cb, 0, sizeof(xdrc_hp_exp_cb));
    memset(xdrc_drclp_cb, 0, sizeof(xdrc_drclp_cb));
    memset(xdrc_drchp_cb, 0, sizeof(xdrc_drchp_cb));
    memset(xdrc_drcall_cb, 0, sizeof(xdrc_drcall_cb));
    bsp_set_effect_by_abt();

    u8 vol_level = dac_dvol_table[sys_cb.vol] + sys_cb.gain_offset;
    if (vol_level > 60) {
        vol_level = 60;
    }
    soft_vol_init(&xdrc_soft_vol);
    soft_vol_set_vol_param(&xdrc_soft_vol, dac_dvol_tbl_db[vol_level], 1);

    TRACE("%s\n", __func__);
}

void music_xdrc_audio_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (XDRC_EN(cb->music_effect_state_set)) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set |= XDRC_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        music_xdrc_audio_param_init();
        music_xdrc_audio_start_do();
        music_effect_set_state(MUSIC_EFFECT_XDRC, 1);
    }

    //delay一下再淡入
    delay_5ms(14);
    dac_fade_in();

    TRACE("music_xdrc_start\n");
}

void music_xdrc_audio_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (XDRC_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set &= ~XDRC_STA_BIT;

    music_xdrc_audio_stop_do();

    music_effect_set_state(MUSIC_EFFECT_XDRC, 0);

    //delay一下再淡入
    delay_5ms(8);
    dac_fade_in();

    TRACE("music_xdrc_stop\n");
}

void music_xdrc_audio_set_vol_do(u8 vol_level)
{
    u8 vol_direct_set_flag = codecs_pcm_is_start() ? 0 : 1;
    if (vol_level > 60) {
        vol_level = 60;
    }
    soft_vol_set_vol_param(&xdrc_soft_vol, dac_dvol_tbl_db[vol_level], vol_direct_set_flag);
    TRACE("music_xdrc_audio_set_vol_do %d %d\n", vol_level, vol_direct_set_flag);
}

AT(.audio_text.xdrc.soft_vol)
void music_xdrc_audio_set_vol_by_dvol(u16 vol)
{
    u8 vol_direct_set_flag = codecs_pcm_is_start() ? 0 : 1;
    soft_vol_set_vol_param(&xdrc_soft_vol, vol, vol_direct_set_flag);
    TRACE(xdrc_set_vol_str, vol, vol_direct_set_flag);
}

AT(.audio_text.xdrc)
void xdrc_clear_cache(void)
{
#if BT_MUSIC_EFFECT_XDRC_EQ_EN
    xdrc_softeq_clear_cache(xdrc_preeq_cb);
#endif // BT_MUSIC_EFFECT_XDRC_EQ_EN
    xdrc_softeq_clear_cache(xdrc_lp_cb);
    xdrc_softeq_clear_cache(xdrc_lp_cb2);
    xdrc_softeq_clear_cache(xdrc_lp_exp_cb);
    xdrc_softeq_clear_cache(xdrc_hp_cb);
    xdrc_softeq_clear_cache(xdrc_hp_cb2);
    xdrc_softeq_clear_cache(xdrc_hp_exp_cb);
}

#if BT_MUSIC_EFFECT_XDRC_DELAY_EN
void music_xdrc_set_delay(u16 delay_samples)
{
    pcmdelay_coef_update(xdrc_delay_cb, delay_samples);
    TRACE("music_xdrc_set_delay %d\n", delay_samples);
}
#endif // BT_MUSIC_EFFECT_XDRC_DELAY_EN

#endif // BT_MUSIC_EFFECT_XDRC_EN

///舒适噪声（alpha波、beta波、pink）
#if ABP_EN

void music_effect_abp_start(void)
{
    music_effect_t* cb = &music_effect;

    if (ABP_STA_EN(cb->music_effect_state_set)) {
        return;
    }

    cb->music_effect_state_set |= ABP_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        music_effect_set_state(MUSIC_EFFECT_ABP, 1);
    }

    TRACE("music_effect_abp_start\n");
}

void music_effect_abp_stop(void)
{
    music_effect_t* cb = &music_effect;

    if (ABP_STA_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    cb->music_effect_state_set &= ~ABP_STA_BIT;

    music_effect_set_state(MUSIC_EFFECT_ABP, 0);

    TRACE("music_effect_abp_stop\n");
}
#endif // ABP_EN

///动态空间音频
#if BT_MUSIC_EFFECT_HRTF_RT_EN

void music_hrtf_rt_param_init(void)
{
    hrtf_rt_cb_t hrtf_rt_cb;
    hrtf_rt_cb.decay_factor = 32767;
    hrtf_rt_init(&hrtf_rt_cb);
#if BT_MUSIC_EFFECT_HRTF_RT_RST_EN
    hrtf_rt_angle_filter_init(300, 10, 2);
#endif // BT_MUSIC_EFFECT_HRTF_RT_RST_EN
    hrtf_rt_update_angle_process(0, 90);

    TRACE("%s\n", __func__);
}

#if BT_MUSIC_EFFECT_HRTF_RT_EQ_EN
u8 music_hrtf_rt_get_fix_eq_sta(void)
{
    return music_effect.spatial_audio_fix_eq;
}
#endif // BT_MUSIC_EFFECT_HRTF_RT_EQ_EN

void music_hrtf_rt_start_do(void)
{
#if BT_MUSIC_EFFECT_HRTF_RT_EQ_EN
    music_effect.spatial_audio_fix_eq = 1;
    music_set_eq_by_res(RES_BUF_EQ_SPATIAL_AUDIO_EQ, RES_LEN_EQ_SPATIAL_AUDIO_EQ);
#endif // BT_MUSIC_EFFECT_HRTF_RT_EQ_EN

    TRACE("%s\n", __func__);
}

void music_hrtf_rt_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (HRTF_RT_STA_EN(cb->music_effect_state_set)) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set |= HRTF_RT_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        music_hrtf_rt_param_init();
        music_hrtf_rt_start_do();
        music_effect_set_state(MUSIC_EFFECT_HRTF_RT, 1);
    }

    //delay一下再淡入
    delay_5ms(14);
    dac_fade_in();

    TRACE("music_hrtf_rt_start\n");
}

void music_hrtf_rt_stop_do(void)
{
#if BT_MUSIC_EFFECT_HRTF_RT_EQ_EN
    music_effect.spatial_audio_fix_eq = 0;
    music_set_eq_by_num(sys_cb.eq_mode);
#endif // BT_MUSIC_EFFECT_HRTF_RT_EQ_EN

    music_effect_t* cb = &music_effect;
    u16 delay_cnt = 100;    //10ms
    while (cb->hrtf_rt_angle_update_flag && delay_cnt) {
        delay_us(100);
        WDT_CLR();
        delay_cnt--;
    }

    TRACE("%s\n", __func__);
}

void music_hrtf_rt_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (HRTF_RT_STA_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    //MUTE一下去掉杂音
    dac_fade_out();
    dac_fade_wait();

    cb->music_effect_state_set &= ~HRTF_RT_STA_BIT;

    music_hrtf_rt_stop_do();

    music_effect_set_state(MUSIC_EFFECT_HRTF_RT, 0);

    //delay一下再淡入
    delay_5ms(14);
    dac_fade_in();

    TRACE("music_hrtf_rt_stop\n");
}

void music_hrtf_rt_update_angle(s16 az, s16 el)
{
    if (music_effect_get_state_real(MUSIC_EFFECT_HRTF_RT)) {
        music_effect_t* cb = &music_effect;
        cb->hrtf_rt_angle_update_flag = 1;
#if !BT_MUSIC_EFFECT_HRTF_RT_AC_EN
        if (az < 0) {
            az += 360;
        }
//        az = 360 - az;  //取对应的组角输入算法
//        printf("X:%d ", az);
#if BT_MUSIC_EFFECT_HRTF_RT_RST_EN
        az = hrtf_rt_angle_filter(az);
#endif // BT_MUSIC_EFFECT_HRTF_RT_RST_EN
//        el = el + 120;
//        if (el > 180) {
//            el -= 180;
//        }
//        printf("X:%d Y:%d\n", az, el);
//        printf("Y:%d\n", az);
//        hrtf_rt_update_angle_process(az, el);
        hrtf_rt_update_angle_process(az, 90);
#else
        static s16 test = 0;
        test++;
        if (test >= 360) {
            test = 0;
        }
//        printf("X:%d\n", test);
        hrtf_rt_update_angle_process(test, 90);
#endif
        cb->hrtf_rt_angle_update_flag = 0;
    }
}

#endif // BT_MUSIC_EFFECT_HRTF_RT_EN

///用户自定义音效算法
#if BT_MUSIC_EFFECT_USER_EN

AT(.audio_text.user)
void alg_user_effect_process(u8 *buf, u32 samples, u32 nch, u32 is_24bit, u32 pcm_info)
{
    //用户自定义音效算法处理示例函数
    //算法中用到的函数可以放在audio_text段或者com_text段，以提高算法效率
    //算法中用到的buffer需要放到music_buff段或者music_exbuff段

}

void alg_user_effect_init(void)
{
    //用户自定义音效算法初始化示例函数
    printf("alg_user_effect_init\n");

}

void music_effect_alg_user_start(void)
{
    EFFECT_COMM_START_CHECK();

    music_effect_t* cb = &music_effect;

    if (USER_ALG_EN(cb->music_effect_state_set)) {
        return;
    }

    cb->music_effect_state_set |= USER_ALG_STA_BIT;

    if (cb->audio_comm_init_flag) {         //已初始化audio comm，直接开启
        alg_user_effect_init();
        music_effect_set_state(MUSIC_EFFECT_ALG_USER, 1);
    }

    TRACE("music_effect_alg_user_start\n");
}

void music_effect_alg_user_stop(void)
{
    EFFECT_COMM_STOP_CHECK();

    music_effect_t* cb = &music_effect;

    if (USER_ALG_EN(cb->music_effect_state_set) == 0) {
        return;
    }

    cb->music_effect_state_set &= ~USER_ALG_STA_BIT;

    music_effect_set_state(MUSIC_EFFECT_ALG_USER, 0);

    TRACE("music_effect_alg_user_stop\n");
}
#endif // BT_MUSIC_EFFECT_USER_EN

#endif // BT_MUSIC_EFFECT_EN
