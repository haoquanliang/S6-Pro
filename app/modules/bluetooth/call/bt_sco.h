#ifndef _BT_SCO_H
#define _BT_SCO_H


extern bool bt_sco_is_msbc(void);                      //判断当前通话是否是宽带通话

s16 *bt_sco_far_rptr(void);
void bt_sco_far_history(s16 *buf, s16 *temp_rptr, uint16_t fifo_cnt);

void bt_sco_pcm_buf_init(void);

void sco_huart_init(void);
bool sco_huart_putcs(u8 type, const void *buf, uint len, u8 frame_num);
void bt_sco_dump_cb(uint type, void *ptr, uint size);

void mic_post_gain_process_s(s16 *ptr, int gain, int samples);
s16 bt_sco_dac_drc_calc(s32 data);
u16 dnr_buf_maxpow(void *ptr, u16 len);

#endif
