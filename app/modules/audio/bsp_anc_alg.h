#ifndef __BSP_ANC_ALG_H__
#define __BSP_ANC_ALG_H__

#define ANC_ALG_DYVOL_FF_LOCAL_VOL_EN       0           //动态音量是否调本地音量

u32 bsp_anc_alg_get_type(void);
bool bsp_anc_alg_get_sta_by_type(u8 type);
bool bsp_anc_alg_add_alg_to_type(u32* input_type, u8 type);
bool bsp_anc_alg_remove_alg_from_type(u32* input_type, u8 type);
void bsp_anc_alg_start(u32 type);
void bsp_anc_alg_stop(void);
void anc_alg_process(void);
s16 anc_alg_get_gain(u8 ch);

///自研双MIC(FF+TALK)传统特征风噪检测
void bsp_anc_alg_wn_ff_talk_start(void);
void bsp_anc_alg_wn_ff_talk_stop(void);

///自研双MIC(FF+FB)传统特征风噪检测
void bsp_anc_alg_wn_ff_fb_start(void);
void bsp_anc_alg_wn_ff_fb_stop(void);

///自研单MIC(FF)降增噪
void bsp_anc_alg_asm_sim_ff_start(void);
void bsp_anc_alg_asm_sim_ff_stop(void);

///自研防啸叫(FB)
void bsp_anc_alg_howling_fb_start(void);
void bsp_anc_alg_howling_fb_stop(void);

///自研贴合度检测(FF+FB)
void bsp_anc_alg_fit_detect_ff_fb_start(void);
void bsp_anc_alg_fit_detect_ff_fb_stop(void);

///自研防啸叫(FF)
void bsp_anc_alg_howling_ff_start(void);
void bsp_anc_alg_howling_ff_stop(void);

///自研单MIC(FF)AI风噪检测
void bsp_anc_alg_ai_wn_ff_start(void);
void bsp_anc_alg_ai_wn_ff_stop(void);

///自研单MIC(FF)瞬态噪声检测
void bsp_anc_alg_limiter_ff_start(void);
void bsp_anc_alg_limiter_ff_stop(void);

///自研单MIC(FF)动态音量算法
void bsp_anc_alg_dyvol_ff_start(void);
void bsp_anc_alg_dyvol_ff_stop(void);
void bsp_anc_alg_dyvol_set_vol(void);
void bsp_anc_alg_dyvol_gain_cal(u16 dac_vol);
void bsp_anc_alg_dyvol_set_local_gain(void);

///自研单MIC(FB)自适应音乐补偿算法
void bsp_anc_alg_msc_adp_fb_start(void);
void bsp_anc_alg_msc_adp_fb_stop(void);

///自研双MIC(FF+FB)自适应EQ算法
void bsp_anc_alg_adp_eq_ff_fb_start(void);
void bsp_anc_alg_adp_eq_ff_fb_stop(void);

///自研单MIC(FF)环境自适应ANC算法
void bsp_anc_alg_asm_ff_start(void);
void bsp_anc_alg_asm_ff_stop(void);

///自研双MIC(FF+FB)半入耳耳道自适应算法
void bsp_anc_alg_aem_rt_ff_fb_start(void);
void bsp_anc_alg_aem_rt_ff_fb_stop(void);

///自研单MIC(FF)AI风噪(小模型)检测
void bsp_anc_alg_ai_wn_mini_ff_start(void);
void bsp_anc_alg_ai_wn_mini_ff_stop(void);

///用户自定义ANC算法
void bsp_anc_alg_user_start(void);
void bsp_anc_alg_user_stop(void);

///在线调试
bool bsp_anc_alg_dbg_rx_done(u8* rx_buf, u8 type);
void bsp_anc_alg_parse_cmd(void);

enum ANC_ALG_TODO_FLAG {
    ANC_ALG_TODO_WIND_LEVEL_0,      //风噪检测：无风
    ANC_ALG_TODO_WIND_LEVEL_1,      //风噪检测：一档风
    ANC_ALG_TODO_WIND_LEVEL_2,      //风噪检测：二档风
    ANC_ALG_TODO_WIND_LEVEL_3,      //风噪检测：三档风
    ANC_ALG_TODO_ASM_SIM_NORMAL,    //降增噪：普通
    ANC_ALG_TODO_ASM_SIM_DEEP,      //降增噪：深度
    ANC_ALG_TODO_ASM_LIGHT,         //环境自适应：轻度
    ANC_ALG_TODO_ASM_NORMAL,        //环境自适应：普通
    ANC_ALG_TODO_ASM_DEEP,          //环境自适应：深度
    ANC_ALG_TODO_FIT_DET_FLAG_0,    //贴合度检测：较差
    ANC_ALG_TODO_FIT_DET_FLAG_1,    //贴合度检测：良好
    ANC_ALG_TODO_HOWLING_FB_SET0,   //FB防啸叫
    ANC_ALG_TODO_HOWLING_FB_SET1,   //FB防啸叫
    ANC_ALG_TODO_HOWLING_FF_SET0,   //FF防啸叫
    ANC_ALG_TODO_HOWLING_FF_SET1,   //FF防啸叫
    ANC_ALG_TODO_LIMITER_FF_SET0,   //FF瞬态噪声
    ANC_ALG_TODO_LIMITER_FF_SET1,   //FF瞬态噪声
    ANC_ALG_TODO_DYVOL_FF_SET,      //FF动态音量
    ANC_ALG_TODO_MSC_ADP_FB_SET,    //MSC自适应
    ANC_ALG_TODO_ADP_EQ_LEAK_SET,   //自适应EQ
    ANC_ALG_TODO_ADP_EQ_MSC_SET,    //自适应EQ
    ANC_ALG_TODO_AEM_RT_SCEN_SET,   //耳道自适应
    ANC_ALG_TODO_AEM_RT_CVT_SET,    //耳道自适应

    //右声道的TODO_FLAG
#if ANC_ALG_STEREO_EN
    ANC_ALG_TODO_HOWLING_FB_SET0_R, //FB防啸叫
    ANC_ALG_TODO_HOWLING_FB_SET1_R, //FB防啸叫
    ANC_ALG_TODO_HOWLING_FF_SET0_R, //FF防啸叫
    ANC_ALG_TODO_HOWLING_FF_SET1_R, //FF防啸叫
    ANC_ALG_TODO_MSC_ADP_FB_SET_R,  //MSC自适应
#endif // ANC_ALG_STEREO_EN
};

#endif // __BSP_ANC_ALG_H__
