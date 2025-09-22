#ifndef _USB_CALL_H
#define _USB_CALL_H

typedef void (*audio_callback_t)(u8 *ptr, u32 samples, int channel, void *param);
typedef s32 (*nr_kick_func_t)(s32 *efw0, s32 *efw1, s32 *dfw, s32 *xfw, s16 *hnl, u32 *aeccon2);

void src_var_init(void);

void usbmic_aec_process(u8 *ptr, u32 samples, int ch_mode);
void uda_mic_alg_init(void);
void uda_mic_alg_exit(void);
void uda_aec_pcm_init(void);
void uda_aec_pcm_exit(void);
void aec_pcm_set_nr_kick_func(nr_kick_func_t nr_kick_func);
void aec_pcm_nr_cb_init(nr_cb_t *nr);

void alg_sndp_sm_init(void *alg_cb);
void alg_sndp_sm_exit(void);
s32 bt_sndp_sm_process(s32 *efw0, s32 *efw1, s32 *dfw, s32 *xfw, s16 *hnl, u32 *aeccon2);
void uda_alg_sndp_sm_init(nr_cb_t *nr, u8 *sysclk);
void uda_alg_sndp_sm_exit(void);

void alg_sndp_dm_init(void *alg_cb);
void alg_sndp_dm_exit(void);
s32 bt_sndp_dm_process(s32 *efw0, s32 *efw1, s32 *dfw, s32 *xfw, s16 *hnl, u32 *aeccon2);
void uda_alg_sndp_dm_init(nr_cb_t *nr, u8 *sysclk);
void uda_alg_sndp_dm_exit(void);

void alg_ldmdnn_init(void *alg_cb);
void alg_ldmdnn_exit(void);
s32 bt_ldmdnn_process(s32 *efw0, s32 *efw1, s32 *dfw, s32 *xfw, s16 *hnl, u32 *aeccon2);
void uda_alg_ldmdnn_init(nr_cb_t *nr, u8 *sysclk);
void uda_alg_ldmdnn_exit(void);

void sco_huart_init(void);
#endif
