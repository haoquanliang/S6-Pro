#ifndef _BSP_KARAOK_H
#define _BSP_KARAOK_H

typedef struct {
    const u16 (*echo_level_gain)[2];
    const u16 *echo_delay;
    const u16 *dvol_table;
    const u16 *micvol_table;
} karaok_vol_t;
extern karaok_vol_t karaok_vol;


void bsp_karaok_init(void);
void bsp_karaok_exit(void);
void bsp_karaok_set_mic_volume(void);
void bsp_karaok_set_music_volume(void);
void bsp_echo_set_level(void);
void bsp_echo_set_delay(void);

void karaok_set_mic_volume(u16 vol);

void bsp_karaok_rec_init(void);
void bsp_karaok_rec_exit(void);
void bsp_karaok_music_vol_adjust(u8 up);
void bsp_karaok_mic_vol_adjust(u8 up);
bool bt_sco_karaok_is_en(u8 enter);

#endif
