#ifndef __BSP_ABP_H
#define __BSP_ABP_H

enum {
    ABP_MODE_CLOSE,
    ABP_MODE_ALPHA,
    ABP_MODE_BETA,
    ABP_MODE_PINK,
} ABP_MODE;

void bsp_abp_set_mode(u8 mode);
void bsp_abp_set_vol(u32 vol);
void abp_start(s32 wave_type);
void abp_stop(void);
bool abp_is_playing(void);
void abp_stop_fade_out(void);
void abp_start_fade_in(void);

#endif // __BSP_ABP_H
