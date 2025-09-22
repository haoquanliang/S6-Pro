#include "include.h"


#if ABP_EN

#define TRACE_EN                0

#define ABP_INTENSITY           15
#define ABP_PCM_BUF_SAMPLES     128

#if BT_TWS_EN
#define ABP_PCM_BUF_LEN         ABP_PCM_BUF_SAMPLES
#else
#define ABP_PCM_BUF_LEN         (ABP_PCM_BUF_SAMPLES * 2)
#endif // BT_TWS_EN

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

ABP_generation_init_cb_t ABP_cb AT(.music_buff.abp);
static s16 ABP_pcm[ABP_PCM_BUF_LEN] AT(.music_buff.abp);
static volatile u8 ABP_start = 0;

void alg_abp_process(void);
void abp_kick_start(void);
void dac1_fade_in_real(void);
void dac1_fade_out_real(void);
void dac1_vol_set_fade(u32 vol);
void music_effect_abp_start(void);
void music_effect_abp_stop(void);


static void abp_init(void)
{
    if (music_effect_get_state(MUSIC_EFFECT_ABP)) {
        return;
    }
    music_effect_abp_start();
	dac1_aubuf_init();
	aubuf1_dma_init();
	dac1_fade_out();
	dac1_fade_wait();
    dac1_vol_set(0x7fff);
}
#if BT_TWS_EN
void bsp_abp_set_mode(u8 mode)
{
#if ABP_MUSIC_DIS_PINK_EN
    if ((mode == ABP_MODE_PINK) && (f_bt.disp_status == BT_STA_PLAYING)) {
        return;
    }
#endif // ABP_MUSIC_DIS_PINK_EN
    sys_cb.abp_mode = mode;
    if (bt_tws_is_slave()) {
        bt_tws_sync_setting();
    } else {
        bt_tws_req_alarm_abp(sys_cb.abp_mode);
    }
    TRACE("abp_mode: %d\n", sys_cb.abp_mode);
}

AT(.com_text.abp.alarm)
void abp_stop_fade_out(void)
{
    if (music_effect_get_state(MUSIC_EFFECT_ABP) && ABP_start) {
        dac1_fade_out_real();
    }
}

AT(.com_text.abp.alarm)
void abp_start_fade_in(void)
{
    if (sco_is_connected()) {
        return;
	}
	dac1_fade_in_real();
}

void abp_start(s32 wave_type)
{
	if (sco_is_connected()) {
        return;
	}
    if (ABP_start) {
        dac1_fade_out_real();
        dac1_fade_wait();
    } else {
        abp_init();
        ABP_start = 1;
    }
    memset(&ABP_cb, 0, sizeof(ABP_generation_init_cb_t));
    ABP_cb.wave_type = wave_type;       //0:alpha 1:beta 2:pink
    ABP_cb.intensity = ABP_INTENSITY;
    ABP_cb.plfsr_l = 0x5EEED1F5;
    ABP_cb.plfsr_r = 0xE5A756F3;
    alpha_beta_pink_generation_init(&ABP_cb);
	dac1_spr_set(SPR_48000);
	alg_abp_process();
	delay_5ms(1);                       //先让数据推起来
    if (!bt_tws_is_slave()) {
        bt_tws_req_alarm_abp(0x10);
    }
    TRACE("abp_start: %d\n", sys_cb.abp_mode);
}

void abp_stop(void)
{
    if (!music_effect_get_state(MUSIC_EFFECT_ABP)) {
        return;
    }
    dac1_fade_wait();
    ABP_start = 0;
    music_effect_abp_stop();
    TRACE("abp_stop: %d\n", sys_cb.abp_mode);
}
#else
void bsp_abp_set_mode(u8 mode)
{
#if ABP_MUSIC_DIS_PINK_EN
    if ((mode == ABP_MODE_PINK) && (f_bt.disp_status == BT_STA_PLAYING)) {
        return;
    }
#endif // ABP_MUSIC_DIS_PINK_EN
    sys_cb.abp_mode = mode;
    if (sys_cb.abp_mode == 0) {
        abp_stop();
    } else {
        abp_start(sys_cb.abp_mode - 1);
    }
    TRACE("abp_mode: %d\n", sys_cb.abp_mode);
}

AT(.com_text.abp.alarm)
void abp_stop_fade_out(void)
{
}

void abp_start(s32 wave_type)
{
	if (sco_is_connected()) {
        return;
	}
    if (ABP_start) {
        dac1_fade_out_real();
        dac1_fade_wait();
    } else {
        abp_init();
        ABP_start = 1;
    }
    memset(&ABP_cb, 0, sizeof(ABP_generation_init_cb_t));
    ABP_cb.wave_type = wave_type;       //0:alpha 1:beta 2:pink
    ABP_cb.intensity = ABP_INTENSITY;
    ABP_cb.plfsr_l = 0x5EEED1F5;
    ABP_cb.plfsr_r = 0xE5A756F3;
    alpha_beta_pink_generation_init(&ABP_cb);
	dac1_spr_set(SPR_48000);
	alg_abp_process();
	delay_5ms(1);                       //先让数据推起来
    dac1_fade_in_real();
    TRACE("abp_start: %d\n", sys_cb.abp_mode);
}

void abp_stop(void)
{
    if (!music_effect_get_state(MUSIC_EFFECT_ABP)) {
        return;
    }
    dac1_fade_out_real();
    dac1_fade_wait();
    ABP_start = 0;
    music_effect_abp_stop();
    TRACE("abp_stop: %d\n", sys_cb.abp_mode);
}
#endif // BT_TWS_EN

void bsp_abp_set_vol(u32 vol)
{
    if (ABP_start) {
        dac1_vol_set_fade(vol);
    }
}

bool abp_is_playing(void)
{
    return (bool)ABP_start;
}

AT(.com_text.aubuf1)
void aubuf1_dma_done_callback(void)
{
    if ((!sco_is_connected()) && ABP_start) {
        abp_kick_start();
    }
}

AT(.audio_text.process)
void alg_abp_process(void)
{
#if BT_TWS_EN
    u32 ch_idx = sys_cb.tws_left_channel ? 0 : 1;
    if (ABP_start) {
        alpha_beta_pink_generation((s16*)ABP_pcm, ABP_PCM_BUF_SAMPLES, ch_idx);
        aubuf1_dma_kick(ABP_pcm, ABP_PCM_BUF_SAMPLES, 1, 0);
    }
#else
    if (ABP_start) {
        alpha_beta_pink_generation_stereo((s16*)ABP_pcm, ABP_PCM_BUF_SAMPLES);
        aubuf1_dma_kick(ABP_pcm, ABP_PCM_BUF_SAMPLES, 2, 0);
    }
#endif // BT_TWS_EN
}
#endif
