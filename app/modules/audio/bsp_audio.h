#ifndef __BSP_AUDIO_H__
#define __BSP_AUDIO_H__

#define AUDIO_PATH_AUX             0
#define AUDIO_PATH_SPEAKER         1
#define AUDIO_PATH_BTMIC           2
#define AUDIO_PATH_USBMIC          3
#define AUDIO_PATH_KARAOK          4
#define AUDIO_PATH_OPUS            5
#define AUDIO_PATH_TTP             6
#define AUDIO_PATH_ANC_ALG         7
#define AUDIO_PATH_IODM_MIC_TEST   8
#define AUDIO_PATH_ASR             9

#define FPAG_ADC_TEST              0


void audio_path_init(u8 path_idx);
void audio_path_exit(u8 path_idx);
void audio_path_start(u8 path_idx);
u16 bsp_aux_ch_getcfg(void);

void audio_pdm_mic_init(void);
void audio_pdm_mic_exit(void);

#endif //__BSP_AUDIO_H__

