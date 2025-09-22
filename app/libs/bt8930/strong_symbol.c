/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

uint32_t cfg_sdk_version = SDK_VERSION;

#if !FUNC_USBDEV_EN
void usb_dev_isr(void){}
void ude_ep_reset(void){}
void ude_control_flow(void){}
void ude_isoc_tx_process(void){}
void ude_isoc_rx_process(void){}
void lock_code_usbdev(void){}
#endif //FUNC_USBDEV_EN

#if !FUNC_REC_EN
bool puts_rec_encbuf(u8 *buf, u16 len) {return false;}
bool gets_rec_obuf(u8 *buf, u16 len) {return false;}
#endif
#if (REC_TYPE_SEL != REC_MP3)
int mpa_encode_frame(void) {return 0;}
#endif //(REC_TYPE_SEL != REC_MP3)

#if (REC_TYPE_SEL != REC_SBC)
#if (!BT_HFP_MSBC_EN)
bool msbc_encode_init(void){return false;}
#endif
bool sbc_encode_init(u8 spr, u8 nch){return false;}
void sbc_encode_process(void){}
#endif

#if (REC_TYPE_SEL != REC_ADPCM && !BT_HFP_REC_EN)
void adpcm_encode_process(void){}
#endif //(REC_TYPE_SEL != REC_ADPCM)

#if !MUSIC_WAV_SUPPORT
int wav_dec_init(void){return 0;}
bool wav_dec_frame(void){return false;}
void lock_code_wavdec(void){}
int wav_decode_init(void){return 0;}
#endif // MUSIC_WAV_SUPPORT

#if !MUSIC_WMA_SUPPORT
int wma_dec_init(void){return 0;}
bool wma_dec_frame(void){return false;}
void lock_code_wmadec(void){}
int wma_decode_init(void){return 0;}
#endif // MUSIC_WMA_SUPPORT

#if !MUSIC_APE_SUPPORT
int ape_dec_init(void){return 0;}
bool ape_dec_frame(void){return false;}
void lock_code_apedec(void){}
int ape_decode_init(void){return 0;}
#else
AT(.com_text.dac)
void obuf_put_one_sample(u32 left, u32 right)
{
    dac_put_sample_16bit(left, right);
}
#endif // MUSIC_APE_SUPPORT

#if !MUSIC_FLAC_SUPPORT
int flac_dec_init(void){return 0;}
bool flac_dec_frame(void){return false;}
void lock_code_flacdec(void){}
int flac_decode_init(void){return 0;}
#endif // MUSIC_FLAC_SUPPORT

#if !MUSIC_SBC_SUPPORT
int sbcio_dec_init(void){return 0;}
bool sbcio_dec_frame(void){return false;}
int sbcio_decode_init(void){return 0;}

AT(.sbcdec.code)
void codec_sbcdec_update(void){}
AT(.sbcdec.code)
void codec_sbcdec_init(void *param){}
#endif // MUSIC_SBC_SUPPORT

#if !MUSIC_AAC_SUPPORT
int aacio_dec_init(void) {return 0;}
int aacio_decode_init(void) {return 0;}
#endif

#if !MUSIC_M4A_SUPPORT
int m4a_decode_init(void){return 0;}
int m4a_dec_init(void){return 0;}
bool aacio_sub_process(void){return true;}
#endif

#if !MUSIC_AAC_SUPPORT && !MUSIC_M4A_SUPPORT
bool aacio_dec_frame(void) {return false;}
#endif

#if (!(MUSIC_WMA_SUPPORT | MUSIC_APE_SUPPORT | MUSIC_FLAC_SUPPORT))
void msc_stream_start(u8 *ptr) {}
void msc_stream_end(void) {}
int msc_stream_read(void *buf, unsigned int size) {return 0;}
bool msc_stream_seek(unsigned int ofs, int whence) {return false;}
void os_stream_fill(void) {}
void os_stream_read(void) {}
void os_stream_seek(void) {}
void os_stream_end(void) {}
AT(.com_text.stream)
void msc_stream_fill(void) {}
#endif

#if !FUNC_AUX_EN
void sdadc_analog_aux_start(u8 adc_ch_get, u8 str_ch, u8 ana_gain){}
void sdadc_analog_aux_exit(u8 adc_ch_get, u8 str_ch){}

u8 noise_detect(void *p, u16 pow) {return 0;}
u8 aux_dnr_process(u8 *ptr, u32 len) {return 0;}
void aux_dnr_init(u16 v_cnt, u16 v_pow, u16 s_cnt, u16 s_pow){}
#endif

#if !FUNC_MUSIC_EN
int mp3_dec_init(void) {return 0;}
int mp3_decode_init(void){return 0;}
int music_decode_init(void){return 0;}
void mp3_get_total_time(void){}
void update_codec_playtime_callback(void *s){}
int mad_layer_I(void) { return 0; }
int mad_layer_II(void) { return 0; }
bool mp3_save_point(void *frame) {return false;}
void mp3_qskip_start(void *frame) {}
bool mp3_qskip_process(void *frame) {return false;}
AT(.text.qskip)
void codec_qskip_init(u32 block_start, u16 block_align) {}
AT(.text.qskip)
void music_qskip(bool direct, u8 second) {}
AT(.text.qskip)
void calc_qskip_target_offset(void) {}
AT(.text.qskip)
void music_qskip_end(void) {}
AT(.text.qskip)
void music_set_jump(void *brkpt) {}
AT(.text.qskip)
void music_get_breakpiont(void *brkpt) {}
#if !WARNING_MP3_EN
int spi_mp3_dec_init(void) { return 0; }
void mp3_end(void) {}
bool mp3_dec_frame(void) {return false;}
#endif
#endif

#if !BT_MUSIC_EFFECT_EN
void msc_pcm_out_var_init(void) {}
void msc_pcm_out_start(void) {}
void msc_pcm_out_stop(void) {}
u8 codecs_pcm_is_start(void) {return 0;}
AT(.com_text.codecs.pcm)
void msc_pcm_effect_process(u8 *buf, u32 samples, u32 nch, u32 is_24bit, u32 pcm_info) {}
AT(.com_text.codecs.pcm)
void msc_pcm_out_24bit_process(u32 left, u32 right) {}
AT(.com_text.codecs.pcm)
void msc_pcm_out_16bit_process(u32 left, u32 right) {}
AT(.com_text.codecs.alg)
void alg_music_effect_process(void) {}

void mp3_pcm_out_start(void) {}
AT(.mp3dec.pcm)
void mp3_pcm_out_process(u32 samples) {}
AT(.sbcdec.pcm)
void sbc_pcm_out_start(void) {}
AT(.sbcdec.pcm)
bool sbc_pcm_out_process(u32 samples, bool is_tws) { return false; }
AT(.aacdec.pcm)
void aac_pcm_out_start(void) {}
AT(.aacdec.pcm)
void aac_pcm_out_process(u32 samples, bool is_tws) {}
AT(.aacdec.pcm.clear)
void aac_tws_obuf_clear(void) {}

AT(.wavdec.code)
void obuf_put_16bit_sample_wav(u32 left, u32 right)
{
    dac_put_sample_16bit_w(left, right);
}
AT(.wavdec.code)
void obuf_put_24bit_sample_wav(u32 left, u32 right)
{
    dac_put_sample_24bit_w(left, right);
}
AT(.apedec.code)
void obuf_put_one_sample_wma(u32 left, u32 right)
{
    dac_put_sample_16bit_w(left, right);
}
AT(.flacdec.code)
void obuf_put_one_sample_flac(u32 left, u32 right)
{
    dac_put_sample_16bit_w(left, right);
}
#endif

#if !BT_MUSIC_EFFECT_DBB_EN
u8 music_dbb_get_level_cnt(void) {return 0;}
u8 music_dbb_get_bass_level(void) {return 0;}
int music_dbb_stop(void) {return 0;}
int music_dbb_update_param(u8 vol_level, u8 bass_level) {return 0;}
int music_dbb_init(dbb_param_cb_t* p) {return 0;}
#endif

#if !BT_MUSIC_EFFECT_SPATIAL_AU_EN
void v3d_frame_process(u32* buf, u32 samples, u32 in_nch, u32 in_24bits) {}
void v3d_clear_cache(void) {}
void v3d_set_param(int mode, u16 wet, u16 dry) {}
void v3d_set_fade(u32 fade) {}
bool v3d_fade_is_done(u32 fade) {return true;}
void v3d_exit(void) {}
void v3d_init(void) {}
#else
#if BT_TWS_EN
AT(.audio_text.headset_v3d.stereo)
void v3d_process(s16 *data_l, s16 *data_r) {}
#else
AT(.audio_text.headset_v3d.tws)
void v3d_process_tws(s16 *data_l, s16 *data_r, u32 ch) {}
#endif // BT_TWS_EN
#if BT_MUSIC_EFFECT_SPATIAL_AU_L1
int v3d_get_data_ptr(u32* ptr) {return v3d_get_data_ptr_l1(ptr);}
int v3d_get_data_ptr_l0(u32* ptr) {return 0;}
#endif // BT_MUSIC_EFFECT_SPATIAL_AU_L1
#if !BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
AT(.audio_text.headset_v3d)
void v3d_delay_buf_clear_cache(void) {}
AT(.audio_text.headset_v3d)
void v3d_delay_buf_process(s16* buf_l, s16* buf_r) {}
bool v3d_delay_buf_init(void* ptr_l, u32 buf_l_size, u32 delay_l_samples, void* ptr_r, u32 buf_r_size, u32 delay_r_samples) {return false;}
#endif
#endif

#if !BT_MUSIC_EFFECT_VBASS_EN && !BT_MUSIC_EFFECT_DYEQ_VBASS_EN
s32 vbass_process(void *cb, s32 data) {return 0;}
void vbass_init(void *cb, vbass_param_cb_t *p) {}
void vbass_set_param(void *cb, u32 cutoff_frequency, u32 intensity, u8 vbass_high_frequency_set, u32 pregain) {}
#endif

#if !BT_MUSIC_EFFECT_DYEQ_EN
s32 dynamic_eq_process(void *cb, s32 data) {return 0;}
void dyeq_init(void *cb, dyeq_param_cb_t *p) {}
void dyeq_coef_update(void *cb, u8 *buf) {}
void dyeq_clear_cache(void *cb) {}
#endif

#if !BT_MUSIC_EFFECT_XDRC_EN
s32 xdrc_softeq_proc(void *cb, s32 input) {return 0;}
int xdrc_softeq_cb_init(void *cb, u32 cb_len, u32 res_addr, u32 res_len) {return -5;}
void xdrc_softeq_coef_update(void *cb, u32 cb_len, u32 *coef, u32 band, bool pregain) {}
void xdrc_softeq_clear_cache(void *cb) {}
s32 xdrc_drc_process_gain(void *cb, s32 data_in) {return 0;}
s32 xdrc_drc_get_pcm32_s(s32 data, int g) {return 0;}
s16 xdrc_drc_process_s16(void *cb, s32 data_in) {return 0;}
int xdrc_drcv1_cb_init(void *cb, u32 cb_len, u32 res_addr, u32 res_len) {return -5;}
void xdrc_drcv1_coef_update(void* cb, u32 *tbl) {}
s32 pcmdelay_mono_s32(void *cb, s32 pcm) {return 0;}
int pcmdelay_init(void *cb,void *delay_buf, u16 delay_buf_len, u16 samples_size, u16 delay_samples) {return -5;}
bool pcmdelay_coef_update(void *cb, u16 delay_samples) {return false;}
#endif

#if !ABP_EN
void alpha_beta_pink_generation_init(ABP_generation_init_cb_t *p) {}
void alpha_beta_pink_generation(s16 *samples, s32 len, u8 idx) {}
void alpha_beta_pink_generation_stereo(s16 *samples, s32 len) {}
void dac1_fade_in_real(void) {}
void dac1_fade_out_real(void) {}
void abp_kick_start(void) {}
#endif

#if !BT_MUSIC_EFFECT_HRTF_RT_EN
void hrtf_rt_init(hrtf_rt_cb_t *p) {}
void hrtf_rt_audio_frame_process(u32* buf, u32 samples, u32 in_nch, u32 in_24bits) {}
void hrtf_rt_update_angle_process(s16 az, s16 el) {}
#endif

#if !BT_MUSIC_EFFECT_HRTF_RT_RST_EN
s32 hrtf_rt_angle_filter(s32 in_angle) {return 0;}
void hrtf_rt_angle_filter_init(u32 filter_cnt_total, s16 delta_thr, s16 step) {}
#endif

//是否支持BT AAC音频
#if !BT_A2DP_AAC_AUDIO_EN
void aac_dec_init(void) {}
void aac_decode_init(void) {}
bool aac_dec_frame(void) {return false;}
bool aac_nor_dec_frame(void) {return false;};
bool aac_tws_dec_frame(void) {return false;};
void aac_cache_free_do(void) {}
size_t aac_cache_read_do(uint8_t *buf, uint max_size) {return 0;}
uint16_t tws_pack_aac(uint8_t *buf) {
    return 0;
}
AT(.com_text.aac.obuf)
void aac_fill_tws_obuf(void) {}
AT(.aacdec.text)
void aac_obuf_tws_cpy(void) {}
AT(.aacdec.text)
bool aac_decode(void) { return false; }
#else
void aac_decode_init_do(void);
bool aac_dec_frame_do(void);
void aac_cache_free_do(void);
size_t aac_cache_read_do(uint8_t *buf, uint max_size);
AT(.text.music.init.aac)
void aac_decode_init(void) {
    aac_decode_init_do();
}
AT(.aacdec.text)
bool aac_dec_frame(void) {
    return aac_dec_frame_do();
}
AT(.aacdec.text)
void aac_cache_free(void) {
#if BT_TWS_EN
    aac_cache_free_do();
#endif
}
AT(.aacdec.text)
size_t aac_cache_read(uint8_t *buf, uint max_size) {
#if BT_TWS_EN
    return aac_cache_read_do(buf, max_size);
#else
    return 0;
#endif
}
#endif

#if !BT_A2DP_LHDC_AUDIO_EN
void bt_lhdc_dec_init(void) {}
void lhdc_decode_init(u16 codec_id, u8 *value){}
bool lhdc_dec_init(u16 codec_id, u8 *value) {return false;}
bool lhdc_dec_frame(void) {return false;}
int lhdc_decode_frame(bool is_tws){return 0;}
void lhdc_dec_destory(void){}
void lhdc_dec_set_clk(void) {}

void au0_dmain_start(void){}
void au0_dmain_stop(void){}
void au0_dmain_kick(s32 *ptr, u32 samples){}
AT(.com_text.au0dma)
void au0_dmain_isr(void){}
void au0_dmain_sem_init(void){}

void spiflash_security_uid_read(void){}

void lhdc_obuf_tws_cpy(void) {}
void lhdc_fill_tws_obuf(void) {}
void lhdc_cpy_tws_obuf(void) {}
AT(.com_text.sbc.play)
void lhdc_dec_kick_isr(uint32_t ticks){}
u8 avdtp_tws_get_lhdc_spr(uint16_t codec_id, uint8_t spec_val){return 0x01;}
bool lhdc_nor_dec_frame(void){return false;}
bool lhdc_tws_dec_frame(void){return false;}

void lhdc_pcm_out_start(void);
void lhdc_pcm_out_process(u8 *buf, u32 samples, u32 nch, u32 in_24bits, bool is_tws) {}
void gpdma_lhdc_kick(u32 *ptr, u32 samples, u32 in_24bits) {}
void lhdc_kick_copy_tws_obuf(void){}
void lhdc_gpdma_done(void) {}
AT(.com_text.bt.api.codec)
bool bt_decode_is_lhdc(void) {return false;}
#else
size_t lhdc_cache_read_do(uint8_t *buf, uint max_size);
void lhdc_cache_free_do(void);

AT(.text.lhdc.dec.cache)
void lhdc_cache_free(void) {
#if BT_TWS_EN
    lhdc_cache_free_do();
#endif
}
AT(.text.lhdc.dec.cache)
size_t lhdc_cache_read(uint8_t *buf, uint max_size) {
#if BT_TWS_EN
    return lhdc_cache_read_do(buf, max_size);
#else
    return 0;
#endif
}
#endif

#if !BT_A2DP_LDAC_AUDIO_EN
void bt_ldac_dec_init(void) {}
void ldac_decode_init(u16 codec_id, u8 *value){}
bool ldac_dec_init(u16 codec_id, u8 *value) {return false;}
bool ldac_dec_frame(void) {return false;}
int ldac_decode_frame(bool is_tws){return 0;}
void ldac_dec_destory(void){}
void ldac_dec_set_clk(void) {}

void ldac_obuf_tws_cpy(void) {}
void ldac_fill_tws_obuf(void) {}
void ldac_cpy_tws_obuf(void) {}
AT(.com_text.sbc.play)
void ldac_dec_kick_isr(uint32_t ticks){}
u8 avdtp_tws_get_ldac_spr_idx(uint8_t spr_bitmap){return 0x01;}
bool ldac_nor_dec_frame(void){return false;}
bool ldac_tws_dec_frame(void){return false;}
void ldac_kick_copy_tws_obuf(void) {}
bool ldac_tws_dec_waiting(void){return true;}

void ldac_pcm_out_start(void) {}
void ldac_pcm_out_process(u8 *buf, u32 samples, u32 nch, u32 in_24bits, bool is_tws) {}
void ldac_gpdma_done(void) {}
void gpdma_ldac_kick(u32 *ptr, u32 samples, u32 in_24bits) {}
AT(.com_text.bt.api.codec)
bool bt_decode_is_ldac(void) {return false;}
#else
size_t ldac_cache_read_do(uint8_t *buf, uint max_size);
void ldac_cache_free_do(void);

AT(.text.ldac.dec.cache)
void ldac_cache_free(void) {
#if BT_TWS_EN
    ldac_cache_free_do();
#endif
}
AT(.text.ldac.dec.cache)
size_t ldac_cache_read(uint8_t *buf, uint max_size) {
#if BT_TWS_EN
    return ldac_cache_read_do(buf, max_size);
#else
    return 0;
#endif
}
#endif

#if !FMRX_REC_EN
void fmrx_rec_start(void){}
void fmrx_rec_stop(void){}
#endif // FMRX_REC_EN

#if !BT_REC_EN && !KARAOK_REC_EN
void bt_music_rec_start(void) {}
void bt_music_rec_stop(void) {}
void au0_dma_isr(void){}
#endif

#if !USB_SUPPORT_EN
void usb_isr(void){}
void usb_init(void){}
#endif

#if ((!SD_SUPPORT_EN) && (!FUNC_USBDEV_EN))
void sd_disk_init(void){}
void sdctl_isr(void){}
void sd0_insert(u8 index){}
void sd0_remove(u8 index){}
void sd_disk_switch(u8 index){}
bool sd0_stop(bool type){return false;}

bool sd0_init(void){return false;}
bool sd0_read(void *buf, u32 lba){return false;}
bool sd0_write(void* buf, u32 lba){return false;}

#endif

#if !FUNC_MUSIC_EN
u32 fs_get_file_size(void){return 0;}
void fs_save_file_info(unsigned char *buf){}
void fs_load_file_info(unsigned char *buf){}
#endif // FUNC_MUSIC_EN

#if !SYS_KARAOK_EN
void karaok_process(void){}
AT(.com_text.karaok)
void karaok_sdadc_process(u8 *ptr, u32 samples, int ch_mode){}
#endif

#if !SYS_ECHO_EN
s16 *karaok_echo_get_buf(u32 buf_num) {return NULL;}
AT(.com_text.karaok)
void karaok_sdadc_echo_process(s16 *rptr){}
void echo_reset_buf(void *cfg){}
void echo_set_delay(u16 delay){}
void echo_set_level(const u16 *vol){}
#endif

#if !SYS_MAGIC_VOICE_EN
AT(.com_text.karaok)
void karaok_sdadc_magic_process(s16 *rptr){}
void magic_voice_process(void) {}
void mav_kick_start(void) {}
#endif


#if !FUNC_SPDIF_EN
void spdif_pcm_process(void){}
bool spdif_smprate_detect(void) {    return false;}
AT(.com_text.isr.spdif)
void spdif_isr(u32 spfrx_pnd){}
#endif

#if !FUNC_SPDIF_TX_EN
AT(.com_text.isr.spdif)
void spdif_tx_isr(u32 spftx_pnd) {}
#endif


#if ((!MUSIC_UDISK_EN)&&(!MUSIC_SDCARD_EN))
FRESULT fs_open(const char *path, u8 mode){return 0;}
FRESULT fs_read (void* buff, UINT btr, UINT* br){return 0;}
FRESULT fs_lseek (DWORD ofs, u8 whence){return 0;}
#endif

#if !KARAOK_REC_EN
AT(.com_text.karaok.rec)
bool karaok_rec_fill_buf(u8 *buf, u16 len) {return false;}
#endif

#if !I2S_DMA_EN
void i2s_isr(void) {}
void i2s_process(void) {}
#endif

void voice_assistant_enc_process(void);
#if !LE_DUEROS_DMA_EN
AT(.com_text.opus)
void opus_enc_process(void) {}
#endif

#if !QTEST_EN
void qtest_var_init(void) {}
AT(.com_text.qtest)
void qtest_only_pair(void) {}
AT(.com_text.qtest)
u8 qtest_get_mode(void) {return 0;}
bool qtest_is_send_btmsg(void) {return false;}
AT(.com_text.bsp.uart.vusb)
void qtest_packet_huart_recv(u8 *rx_buf){};
AT(.com_text.bsp.uart.vusb)
void qtest_packet_uart1_recv(u8 data);
void qcheck_save_checkcode(u32 checkcode) {}
#endif

#if (CHARGE_BOX_INTF_SEL != INTF_UART1) && (TEST_INTF_SEL != INTF_UART1)
AT(.text.sys_clk)
void update_uart1baud_in_sysclk_set(u32 baudrate){}  //关闭智能充电仓，切换系统频率不重新设置uart1波特率
#endif

#if (CHARGE_BOX_INTF_SEL != INTF_UART2) && (TEST_INTF_SEL != INTF_UART2)
AT(.text.sys_clk)
void update_uart2baud_in_sysclk_set(u32 baudrate){}  //关闭智能充电仓，切换系统频率不重新设置uart2波特率
#endif


//#if (UART0_PRINTF_SEL == PRINTF_NONE)
void wdt_irq_init(void) {}
//#endif

#if SYS_SLEEP_LEVEL > 2
void sys_sleep_restore(void);
AT(.com_text.ret.bb)
void nanos_sleep_restore(void)
{
    sys_sleep_restore();
}

AT(.sleep_text.sleep.cb)
void sys_sleep_proc_cb(u8 lpclk_type)
{
    sys_sleep_proc_lv(lpclk_type, 0x84);
}
#endif

#if !USER_TKEY
AT(.com_text.tkey.isr)
void tkey_isr(void) {}
#endif

#if !ANC_DS_DMA_EN
void anc_process(u8 flag) {}
int anc_dma_start(anc_dma_cfg_cb* cfg, u8 kick_start) {return -1;}
int anc_dma_exit(void) {return -1;}
bool anc_dma_is_use(void) {return false;}
#endif

#if !ANC_ALG_EN
void alg_anc_process(void) {}
void anc_alg_sdadc_process(u8* ptr, u32 samples, int ch_mode) {}
void anc_alg_ram_clear(void) {}
int anc_alg_init(anc_alg_param_cb* p) {return -1;}
int anc_alg_exit(anc_alg_param_cb* p) {return -1;}
void anc_alg_rdft_hw_forward_512(s32 *buf) {}
void anc_alg_rdft_hw_forward_256(s32 *buf) {}
void anc_alg_rdft_hw_forward_128(s32 *buf) {}
void anc_alg_rdft_hw_init(void) {}
#endif

#if !ANC_ALG_STEREO_EN
AT(.anc_text.process.comm.stereo)
void anc_alg_sdadc_process_fffb_r(u8* ptr, u32 samples, int ch_mode) {}
AT(.anc_text.process.comm.stereo)
void alg_anc_process_fffb_r_do(void) {}
#endif

#if (!ANC_DS_DMA_EN) || (!ANC_ALG_EN)
AT(.anc_text.process.ancdma)
void anc_alg_ancdma_process(u8* ptr, u32 samples, int ch_mode) {}
AT(.anc_text.process.ancdma)
void alg_anc_process_ancdma_do(void) {}
#endif

#if !ANC_ALG_DUMP_EN
s16* alg_anc_dump_data_ptr_by_idx(int x, int idx) {return NULL;}
void alg_anc_dump_process(void) {}
void anc_alg_dump_3ch(s16* buf0, s16* buf1, s16* buf2, u32 frame_len, u8 interleave) {}
void anc_alg_dump_dmic(s16* buf_l, s16* buf_r, u32 frame_len, u8 interleave) {}
void anc_alg_dump(void* buf, u32 frame_len) {}
void anc_alg_dump_init(u32 frame_len) {}
#endif

#if !ANC_ALG_WIND_NOISE_FF_TALK_EN
void dewind_dm_init(void* cfg) {}
bool dewind_dm_set(u8* ptr, u16 size, u8 ch) {return false;}
int dewind_dm_process(s16* data_talk, s16* data_ff, int* wind_level, int* ene_level) {return -1;}
void alg_anc_windnoise_ff_talk_do(int num) {}
bool alg_anc_windnoise_ff_talk_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_windnoise_ff_talk_init(int num) {}
#endif

#if !ANC_ALG_WIND_NOISE_FF_FB_EN
void anc_wind_ff_fb_init(void* cfg, u8 ch) {}
bool anc_wind_ff_fb_set(u8* ptr, u16 size, u8 ch) {return false;}
int anc_wind_ff_fb_process(s16* ffdata, s16* fbdata, s16* txdata, int* res, u8 ch) {return -1;}
void alg_anc_windnoise_ff_fb_do(int num) {}
bool alg_anc_windnoise_ff_fb_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_windnoise_ff_fb_init(int num) {}
#endif

#if (!ANC_ALG_ASM_SIM_FF_EN) && (!ANC_ALG_ASM_FF_EN)
void asm_simple_init(void* cfg, u8 ch) {}
bool asm_simple_set(u8* ptr, u16 size, u8 ch) {return false;}
int anc_asm_simple_block_process(s16* ff, int* res, s16 asm_flag, u8 ch) {return -1;}
#endif

#if !ANC_ALG_ASM_SIM_FF_EN
void alg_anc_asm_simple_do(int num) {}
bool alg_anc_asm_simple_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_asm_simple_init(int num) {}
#endif

#if !ANC_ALG_ASM_FF_EN
void alg_anc_asm_do(int num) {}
bool alg_anc_asm_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_asm_init(int num) {}
#endif

#if (!ANC_ALG_HOWLING_FB_EN) && (!ANC_ALG_HOWLING_FF_EN)
void anc_howling_config(void* config_p) {}
bool anc_howling_set(u8* ptr, u16 size, u8 ch) {return false;}
int anc_howling_block_process(s16* ancout, s16* mic, int* res, u8 ch) {return -1;}
#endif

#if !ANC_ALG_HOWLING_FB_EN
void alg_anc_howling_fb_do(int num) {}
bool alg_anc_howling_fb_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_howling_fb_init(int num) {}
#endif

#if !ANC_ALG_HOWLING_FF_EN
void alg_anc_howling_ff_do(int num) {}
bool alg_anc_howling_ff_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_howling_ff_init(int num) {}
#endif

#if !ANC_ALG_DYVOL_FF_EN
void dyvol_process(void* buf, int* res) {}
void dyvol_init(void* cb) {}
void dyvol_set_enable(u8 en) {}
void alg_anc_dyvol_ff_do(int num) {}
void alg_anc_dyvol_ff_init(int num) {}
#endif

#if !ANC_ALG_MSC_ADP_FB_EN
void anc_msc_adp_process(s16* fbdata, s16* txdata, int* res, u8 ch) {}
bool anc_msc_adp_set(u8* ptr, u16 size, u8 ch) {return false;}
void anc_msc_adp_init(void* cb, u8 ch) {}
void alg_anc_msc_adp_fb_do(int num) {}
bool alg_anc_msc_adp_fb_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_msc_adp_fb_init(int num) {}
#endif

#if !ANC_ALG_ADP_EQ_FF_FB_EN
void adp_eq_init(void* cb, u8 ch) {}
bool adp_eq_set(u8* ptr, u16 size, u8 ch) {return false;}
void adp_eq_process(s16* ffdata, s16* fbdata, s16* txdata, int* res, u8 ch) {}
void alg_anc_adp_eq_ff_fb_do(int num) {}
bool alg_anc_adp_eq_ff_fb_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_adp_eq_ff_fb_init(int num) {}
#endif

#if !ANC_ALG_FIT_DETECT_FF_FB_EN
void anc_fd_process(s16* ffdata, s16* fbdata, s16* txdata, u8 aem_flag, int* res, u8 ch) {}
void anc_fd_init(void* cfg, u8 ch) {}
bool anc_fd_set(u8* ptr, u16 size, u8 ch) {return false;}
void alg_anc_fit_detect_ff_fb_do(int num) {}
bool alg_anc_fit_detect_ff_fb_set(void* ptr, u16 size, u8 ch) {return false;}
void alg_anc_fit_detect_ff_fb_init(int num) {}
#endif

#if !ANC_ALG_AI_WN_FF_EN
void wnn_buffer_init(void *st) {}
void wnn_npu_init(void) {}
void wnn_prepare(u8 is_2ch) {}
void wn_ai_layer_m1_npu_proc(void* st) {}
void wnn_npu_kick_start(s32* input_x, u8 ch) {}
void wnn_exit(void) {}
#endif

#if !ANC_ALG_AI_WN_DSP_FF_EN
void wnn_dsp_init(void) {}
bool wnn_set(u8* ptr, u16 size, u8 ch) {return false;}
void wn_ai_layer_m1_proc(u8 ch) {}
#endif

#if (!ANC_ALG_AI_WN_FF_EN) && (!ANC_ALG_AI_WN_DSP_FF_EN)
void anc_ai_wn_block_process(s16 *data, int* res) {}
void wnn_init(void* cfg, u8 ch) {}
void wnn_comm_init(void) {}
void alg_anc_ai_wn_ff_do(int num) {}
void alg_anc_ai_wn_ff_init(int num) {}
void alg_anc_ai_wn_ff_exit(int num) {}
bool alg_anc_ai_wn_ff_set(void* ptr, u16 size, u8 ch) {return false;}
#endif

#if !ANC_ALG_LIMITER_FF_EN
void anc_limiter_init(void) {}
void anc_limiter_config(void* config_p) {}
int anc_limiter_block_process(s16* ancout, int* res) {return -1;}
void alg_anc_limiter_ff_do(int num) {}
void alg_anc_limiter_ff_init(int num) {}
#endif

#if !ANC_ALG_AEM_RT_FF_FB_EN
void aem_rt_init(void* cb) {}
void aem_rt_process(s16* ffdata, s16* fbdata, s16* txdata, int* res) {}
void alg_anc_aem_rt_ff_fb_do(int num) {}
void alg_anc_aem_rt_ff_fb_init(int num) {}
#endif

#if !ANC_ALG_AI_WN_MINI_FF_EN
void anc_ai_wn_mini_block_process(s16 *data, int* res, u8 ch) {}
void anc_ai_wn_mini_init(void* cb, u8 ch) {}
void anc_ai_wn_mini_comm_init(void) {}
bool anc_ai_wn_mini_set(u8* ptr, u16 size, u8 ch) {return false;}
void alg_anc_ai_wn_mini_ff_do(int num) {}
void alg_anc_ai_wn_mini_ff_init(int num) {}
bool alg_anc_ai_wn_mini_ff_set(void* ptr, u16 size, u8 ch) {return false;}
#endif

#if !ANC_SNDP_SAE_SHIELD_ADAPTER_EN
void alg_anc_sndp_sae_shield_leak_do(int num) {}
void alg_anc_sndp_sae_shield_leak_init(int num) {}
void alg_anc_sndp_sae_adapter_anc_do(int num) {}
void alg_anc_sndp_sae_adapter_anc_init(int num) {}
int SAE_EL01_shield_leakagaedet_process(short* in_fb, int* leakaga_level, short* anc_mode, int* frm_cnt) {return -1;}
int SAE_EL01_adapter_anc_process(short* sMicInFrm, int* anc_mode, short* ff_total_gain, int* frm_cnt) {return -1;}
int SAE_EL01_init(int type, int* param, int param_len) {return -1;}
#endif

#if !ANC_SNDP_SAE_WIND_DETECT_EN
void alg_anc_sndp_sae_dwind_do(int num) {}
void alg_anc_sndp_sae_dwind_init(int num) {}
int SAE_EW02_dwind_process(short* sMicInFrm, int* windLevel, int* frm_cnt) {return -1;}
int SAE_EW02_init(void) {return -1;}
#endif

#if !ANC_ALG_USER_EN
AT(.anc_text.process.anc_alg_user)
void alg_anc_user_do(int num, int* res, u32* res_len) {}
AT(.anc_text.process.anc_alg_user)
void alg_anc_user_do_process(s16* ff, s16* fb, s16* talk, int* res, u32* res_len, int num) {}
void alg_anc_user_init(void* param) {}
void alg_anc_user_exit(void) {}
#endif

#if USB_DET_VER_SEL
AT(.com_text.usb.dectect)
void usbchk_switch_otg_device(void) {}
void usbchk_switch_otg_host(void) {}
void usbchk_only_host(void) {}
void usbchk_only_device(void) {}
u8 usbchk_connect(u8 mode) {return 0;}
#else
AT(.com_text.usb.dectect)
u8 usb_connect(void) {return 0;}
#endif

#if !TINY_TRANSPARENCY_EN
AT(.com_text.tiny.proc)
void ttp_sdadc_process(u8 *ptr, u32 samples, int ch_mode){}
AT(.com_text.tiny.dnr)
void alg_mic_dnr_process(void){}
AT(.com_text.thread.dnr)WEAK
void mic_dnr_kick_start(void){}
#endif

//不够flash空间时可去掉差分或VCMBUF模式
#if DAC_DIFF_DIS
void dac_diff_ang_power_on(u32 restart) { printk("diff error\n");}
void dac_diff_power_off(void) {}
#endif
#if DAC_VCMBUF_DIS
void dac_vcmbuf_ang_power_on(u32 restart) { printk("vcmbuf error\n");}
void dac_vcmbuf_power_off(void) {}
#endif

#if !FOT_EN && !AB_MATE_APP_EN
u8 fot_checksum_cal(u8 *buf) {return 0;}
#endif

#if !USER_TKEY_SLIDE
AT(.com_text.tkey.isr)
void tkey_slide_param_reint(tk_cb_t *s) {}
AT(.com_text.tkey.isr)
void tkey_slide_up_down_check(tk_cb_t *s, tk_cb_t *p) {}
AT(.com_text.tkey.isr)
void tkey_slide_process(tk_cb_t *s, tk_cb_t *p, u16 tkcnt, u32 variance) {}
AT(.com_text.tkey)
void tkey_slide_msg(u8 silde_up) {}
#endif

#if !DAC_DNC_EN
void dac_dnc_init(void) {}
void dac_dnc_start(void) {}
void dac_dnc_stop(void) {}
AT(.com_text.dac.dnc)
void dnc_detect_process(void) {}
bool dac_dnc_is_en(void) {   return false; }
#endif
#if !USB_USER_HID_OUT_EN
AT(.com_text.usbdev)
void ude_hid_get_data(void){}
#endif

#if USB_BC_EN
void usb_bc_init(u8 set){}
#endif

#if !DAC_SYNC_DET_EN
AT(.com_text.dac.det)
void dac_sync_detect_isr(void) {}
void dac_sync_detect_start(void) {}
void dac_sync_detect_stop(void) {}
#endif

#if !ASR_EN
void npu_matrix_init(void){}
void npu_matrix_exit(void){}
void matrix_hw(int32_t* sum, int8_t* x0, int8_t *y0, s16 loop){}
void matrix_hw_1(int32_t* sum, int8_t* x0, int8_t *y0, s16 loop1, s16 loop2){}
void spiflash_lock(void){}
void spiflash_unlock(void){}
bool spiflash_read_kick(void *buf, u32 addr, uint len){return false;}
bool spiflash_read_wait(void){return false;}
#endif

#if !WARNING_WSBC_EN
void load_code_btdec(void)
{
    extern u32 __nrram_dec_comm_lma, __nrram_dec_comm_size, __nrram_dec_comm_vma;
    memcpy(&__nrram_dec_comm_vma, & __nrram_dec_comm_lma, (u32)&__nrram_dec_comm_size);
}
#endif

#if !SRC_EN
void src_init(u8 in_sample_rate, u16 out_samples_expected, u8 channel, uint idx){}
void src_audio_input(u8 *buf, u32 in_samples, uint idx){}
void src_out_done_cb(uint idx){}
void src0_adjust_speed(int speed){}
void src0_audio_input(u8 *buf, u32 samples){}
void src_out_process(u8 all_flag){}
void src_isr(void){}
#endif
