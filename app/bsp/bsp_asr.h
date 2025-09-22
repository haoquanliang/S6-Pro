#ifndef __BSP_ASR_H
#define __BSP_ASR_H

void bsp_vad_start(void);
void bsp_vad_stop(void);
void bsp_asr_start(void);
void bsp_asr_stop(void);
u8 bsp_asr_get_statue(void);
void asr_bt_evt_notice(uint evt, void *params);
void asr_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void asr_callback_process(short *ptr);
u32 asr_get_adc_buff_len(void);
u32 asr_get_adc_buff_addr(void);
#endif
