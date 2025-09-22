#ifndef _BSP_VAD_H
#define _BSP_VAD_H

enum {
    VAD_128 = 0,
    VAD_256,
    VAD_512,
    VAD_1024,
};

bool vad_is_start(void);
bool vad_is_wake(void);
void vad_sleep_config(bool enter);
void vad_start(void);
void vad_stop(void);
void vad_kws_process(void);
void vad_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
u8 vad_mic_pwr_sel_io(void);
#define 	VAD_MODE_1                  1
#define 	VAD_MODE_2                  2
#define 	VAD_MODE_SEL                VAD_MODE_1

#define     KWS_SAMPLES                 640             //KWS一帧的样点数，固定值
#define     VAD_NEG_CNT                 0X0F            //非人声阈值，范围0x0000到0X3FFF
#define     VAD_POS_CNT                 0X20            //人声阈值，越小对声音越灵敏，范围0x0000到0X3FFF
#define     KWS_RUNTIME_LIMIT           5000            //语音识别算法运行时间限制，0为不限制，单位 ms
#define     VAD_NEG_FLAG_THRESHOLD      6               //voice_flag转为0的neg_flag_cnt阈值
#define     VAD_REBOOT_THRESOLD         200             //vad reboot的cnt阈值
#define     VAD_SAMPLES                 VAD_128         //VAD一帧的样点数，固定值

#if VAD_MODE_SEL == VAD_MODE_1
#define     VAD_DMA_NUM_MAX             10              //VAD DMA 共保存VAD_DMA_NUM_MAX帧mic数据
#define     VAD_PREFETCH_NUM            5               //VAD内部往前取的帧数
#define     VAD_PRE_GET                 2               //取VAD起来之前的数据帧数
#define     VAD_DMA_SIZE                512* VAD_DMA_NUM_MAX
#else
#define     ASR_PCM_BUFF_NUM            20				//保存多少帧mic数据
#define     ASR_PRE_READ                18				//往前读取多少帧数据
#define     ASR_PCM_NUM                 400
#define     VAD_DMA_SIZE                1280			//mic buff size
#endif

#endif
