#ifndef _BSP_AUX_H
#define _BSP_AUX_H

typedef struct {
    u8 flag;                    //当前状态与计数
    u8 dig_fade;
    u16 cnt;

    u16 voice_cnt;               //从无声变有声的计数与幅值
    u16 voice_pow;

    u16 silence_cnt;             //从有声变回无声的计数与幅值
    u16 silence_pow;
} dnr_cb_t;

void bsp_aux_start(void);
void bsp_aux_stop(void);
void aux_channel_mute(u8 ch_sel);
void aux_channel_unmute(u8 ch_sel);
void set_aux_analog_vol(u8 level, u8 auxlr_sel);
#endif
