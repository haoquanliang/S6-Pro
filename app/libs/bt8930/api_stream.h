#ifndef _API_STREAM_H
#define _API_STREAM_H

///通用AUDIO数据流Buffer管理
typedef struct {
    u32 size;                       //stream buf size
volatile u32 len;                   //stream buf valid data bytes
    u8 *buf;                        //stream buf start address
    u8 *wptr;                       //stream buf write point
    u8 *rptr;                       //stream buf read point
} au_stm_t;
bool puts_stm_buf(au_stm_t *stm, u8 *buf, u32 len);
bool gets_stm_buf(au_stm_t *stm, u8 *buf, u32 len);
void pcm_soft_vol_process(s16 *ptr, int gain, int samples);     //通用软件调音量函数

#endif // _API_STREAM_H
