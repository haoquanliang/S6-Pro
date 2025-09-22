#ifndef __BSP_OPUS_H
#define __BSP_OPUS_H

#define OPUS_ENC_START      0x80
#define OPUS_ENC_STOP       0x81

enum{
    NR_TYPE_NO = 0,
    NR_TYPE_SMIC,
    NR_TYPE_DMIC,
};
#if OPUS_ENC_EN && !LE_DUEROS_DMA_EN
typedef struct {
    u8 spr;                 //采样率
    u8 nch;                 //声道数
    u8 packet_size;         //编码后一帧opus packet大小
    bool is_init;
    u16 bitrate;			//压缩比特率
} opus_cb_t;
#endif
uint8_t sys_clk_get(void);
extern void bsp_opus_encode_start(void);
extern void bsp_opus_encode_stop(void);
extern u8 bsp_opus_get_enc_frame(u8 *buff, u16 len);
extern bool bsp_opus_is_encode(void);
u16 opus_enc_data_len_get(void);

extern bool opus_start;

void bsp_opus_rec_mic_start(void);
void bsp_opus_rec_mic_stop(void);
bool bsp_opus_rec_is_working(void);
u8 bsp_get_opus_param_packetsize(void);
u8 bsp_get_opus_param_nchannel(void);
u16 bsp_get_opus_data(u8* buf, u16 len);

bool bsp_opus_dec_init(u32 spr, u32 nch);
void bsp_opus_dec_exit(void);
void bsp_opus_dec_kick(void);
void bsp_opus_enc_dec_test(void);
#endif
