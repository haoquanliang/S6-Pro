#ifndef _API_DAC_H_
#define _API_DAC_H_

#define PCM_OUT_24BITS              BIT(0)
#define PCM_OUT_MONO                BIT(1)

void adpll_init(u8 out48k_spr);
void adpll_spr_set(u8 out48k_spr);

//dac
void dac_cb_init(u32 dac_cfg);
void dac_power_on(void);
void dac_restart(void);
void dac_power_off(void);
bool dac_get_pwr_sta(void);
void dac_mono_init(bool dual_en, bool lr_sel);              //when dual_en=1, select select left and right mixed
                                                            //                lr_sel is invalid
                                                            //when dual_en=0, lr_sel=1 select left channel
                                                            //                lr_sel=0 select right channel
void dac_channel_enable(void);                              //enable dac output channel
void dac_channel_disable(void);                             //disable dac output channel, for power save
void dac_balance_set(u16 l_vol, u16 r_vol);                 //left & right volume balance(0~0x7fff)
void dac_ang_gain_set(u8 gain);
void dac_clk_source_sel(u32 val);
void dac_fade_process(void);
void dac_unmute_set_delay(u16 delay);
void dac_fifo_detect(void);
u32 dac_pcm_pow_calc(void);

void dac_dump_vol(void);                                    //dump all volume regs, for debug
void dac_src_vol_set(uint src_idx, u32 vol);
bool dac_src_fade_in(uint src_idx, uint fade_step);
bool dac_src_fade_out(uint src_idx, uint fade_step);
void dac_src_fade_wait(uint8_t src_idx);
void dac_src_w4_empty(int src_idx);

bool aubuf_dma_is_busy(u8 index);

//dac0
#define dac_vol_set(vol)        dac_src_vol_set(0, vol)     //dac0 set volume(0~0x7fff)
#define dac_fade_out()          dac_src_fade_out(0, 2)      //dac0 fade out
#define dac_fade_in()           dac_src_fade_in(0, 2)       //dac0 fade in
#define dac_fade_wait()         dac_src_fade_wait(0)        //dac0 fade wait
bool dac_is_fade_in(void);                                  //dac0 get fade in state
void dac_vol_set_reduce(u16 percent);

void dac_spr_set(uint spr);                                 //dac0 set sample rate
u8 dac_spr_get(void);                                       //dac0 get sample rate
void dac_aubuf_init(void);                                  //dac0 aubuf init
void dac_aubuf_clr(void);                                   //dac0 clear aubuf
void dac_put_zero(uint samples);                            //dac0 put some samples to aubuf, value is zero
void dac_put_sample_16bit(s16 left, s16 right);             //dac0 put one sample(16bit) to aubuf, value is left & right
void dac_put_sample_24bit(s32 left, s32 right);             //dac0 put one sample(24bit) to aubuf, value is left & right
void dac_put_sample_16bit_w(s16 left, s16 right);           //dac0 put one sample(16bit) to aubuf, wait if aubuf is full
void dac_put_sample_24bit_w(u32 left, u32 right);           //dac0 put one sample(16bit) to aubuf, wait if aubuf is full
void aubuf0_dma_init(void);
void aubuf0_dma_exit(void);
void aubuf0_dma_kick(void *ptr, u32 samples, uint nch, bool is_24bit);
void aubuf0_dma_w4_done(void);

//dac1(dac1混合到dac0输出，可独立控制音量)
#define dac1_vol_set(vol)       dac_src_vol_set(1, vol)     //dac1 set volume(0~0x7fff)
#define dac1_fade_out()         dac_src_fade_out(1, 4)      //dac1 fade out
#define dac1_fade_in()          dac_src_fade_in(1, 4)       //dac1 fade in
#define dac1_fade_wait()        dac_src_fade_wait(1)        //dac1 fade wait
#define dac1_aubuf_w4_empty()   dac_src_w4_empty(1)
void dac1_spr_set(uint spr);                                //dac1 set sample rate
void dac1_aubuf_init(void);                                 //dac1 aubuf init
void dac1_aubuf_clr(void);                                  //dac1 clear aubuf
void dac1_put_zero(uint samples);                           //dac1 put some samples to aubuf, value is zero
void dac1_put_sample_16bit(s16 left, s16 right);            //dac1 put one sample(16bit) to aubuf, value is left & right
void dac1_put_sample_24bit(s32 left, s32 right);            //dac1 put one sample(24bit) to aubuf, value is left & right
void aubuf1_dma_init(void);
void aubuf1_dma_exit(void);
void aubuf1_dma_kick(void *ptr, u32 samples, uint nch, bool is_24bit);
void aubuf1_dma_w4_done(void);
#define aubuf1_dma_is_busy()    aubuf_dma_is_busy(1)

//dnc
void dac_dnc_init(void);
void dac_dnc_start(void);
void dac_dnc_stop(void);

#endif
