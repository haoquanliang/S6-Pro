#include "include.h"
#include "asr.h"
#if !ASR_EN
///自定义asr mic buff，如果不使用asr功能，但需要改动mic的samples和地址的话，
///只能在这两个函数修改，bsp_audio.c那边修改无效
//u32 asr_get_adc_buff_addr(void)
//{
//    return DMA_ADR(); //返回地址自定义地址
//}
//
//u32 asr_get_adc_buff_len(void)
//{
//    return 0;        //返回样点数，即上面自定义buf的总长度/2
//}
#else
#define ASR_DUMP_EN                     0       //是否dump mic数据
#define TRACE_EN                        1
#if TRACE_EN
#define TRACE(...)                      printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#define SDADC_DMA_BUF_IDX       3
#define SDADC_MIC_NUM_MAX       5
#define SDADC_DMA_SIZE          256

void huart_wait_txdone(void);
void load_asr_comm_code(void);
void asr_kick_start(void);

extern u8 sdadc_buf[SDADC_DMA_BUF_IDX][SDADC_DMA_SIZE * 4];
extern struct asr_buf_t asr_buf;
static volatile u8 asr_sta = 0;
#if !VAD_EN
AT(.vad.mic_buf)
short sdadc_buf_asr[3*640];
static u8 asr_wprt, asr_rptr;
#endif

#if ASR_DUMP_EN
static u8 asr_dump_header[8] AT(.vad.dump.header);
static s16 asr_dump_buffer[640] AT(.vad.dump.buffer);
void asr_huart_init(void)
{
    asr_dump_header[0] = 0x36;
    asr_dump_header[1] = 0xAD;
    asr_dump_header[2] = 0xf9;
    asr_dump_header[3] = 0x54;
}

AT(.com_text.asr_dump)
static bool asr_huart_putcs_do(const void *buf, uint len)
{
    memcpy(asr_dump_buffer, buf, len);
    huart_tx(asr_dump_buffer, len);
    return true;
}

AT(.com_text.asr_dump)
bool asr_huart_putcs(const void *buf, uint len)
{
    static u8 frame_num = 0;
    bool ret;
    asr_dump_header[4] = 2;                 //type
    asr_dump_header[5] = (u8)(len >> 8);
    asr_dump_header[6] = (u8)(len >> 0);
    asr_dump_header[7] = frame_num++;
    ret = asr_huart_putcs_do(asr_dump_header, 8);
    huart_wait_txdone();
    if (!ret) {
        return false;
    }
    ret = asr_huart_putcs_do(buf, len);
    return ret;
}
#endif

///语音识别回调函数
void asr_callback_process(short *ptr)
{
#if ASR_DUMP_EN
    #if !VAD_EN
    asr_huart_putcs(ptr,640*2);
    #else
    #if VAD_MODE_SEL == VAD_MODE_1
    asr_huart_putcs(ptr,512*2);
    #else
    asr_huart_putcs(ptr,ASR_PCM_NUM*2);
    #endif
    #endif
#endif
    asr_alg_process(ptr);
}

uint8_t sysclk_get_spiflash_clkdiv(u32 sys_clk)
{
    if (sys_clk == SYS_160M) {
        return 1;
    } else {
        return 0;
    }
}

///自定义asr mic buff
u32 asr_get_adc_buff_addr(void)
{
#if VAD_MODE_SEL == VAD_MODE_1 && VAD_EN
    extern u16 vad_sdadc_buf[VAD_DMA_SIZE];
    return DMA_ADR(vad_sdadc_buf);
#else
    return DMA_ADR(sdadc_buf);
#endif
}

#if VAD_MODE_SEL == VAD_MODE_1 && VAD_EN
u32 asr_get_adc_buff_len(void)
{
    return VAD_DMA_SIZE;
}
#endif

void asr_kws_process(void)
{
    if (bsp_asr_get_statue() == 0) return;
#if VAD_EN
    vad_kws_process();
#else
    asr_callback_process(&sdadc_buf_asr[asr_rptr * 640]);
    asr_rptr++;
    if (asr_rptr == 3) asr_rptr = 0;
#endif
}

AT(.com_text.vad.proc)
void asr_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if VAD_EN
    vad_sdadc_process(ptr, samples, ch_mode);
#else
    memcpy(&sdadc_buf_asr[asr_wprt * 640],ptr,640 * 2);
    asr_wprt++;
    asr_kick_start();
	if (asr_wprt == 3) asr_wprt = 0;
#endif
}

void bsp_asr_start(void)
{
    if (asr_sta == 1) {
        return;
    }
#if BT_MUSIC_EFFECT_EN
    music_effect_alg_suspend(MUSIC_EFFECT_SUSPEND_FOR_ASR);
#endif
    npu_matrix_init();
#if ASR_DUMP_EN
    asr_huart_init();
#endif
    asr_alg_start();
    audio_path_init(AUDIO_PATH_ASR);
    audio_path_start(AUDIO_PATH_ASR);

#if VAD_EN
    bsp_vad_start();
#else
    asr_wprt = 0;
    asr_rptr = 0;
    sys_clk_req(INDEX_KARAOK,SYS_160M);
#endif
    asr_sta = 1;
    TRACE("bsp_asr_start\n");
}

void bsp_asr_stop(void)
{
    if (asr_sta == 0) {
        return;
    }
    TRACE("bsp_asr_stop\n");
    asr_sta = 0;
#if VAD_EN
    bsp_vad_stop();
#endif
    audio_path_exit(AUDIO_PATH_ASR);
    asr_alg_stop();
    delay_ms(5);           //确保npu计算结束
    npu_matrix_exit();
    sys_clk_free(INDEX_KARAOK);
}

AT(.com_text.asr.sta)
u8 bsp_asr_get_statue(void)
{
    return asr_sta;
}

void asr_bt_evt_notice(uint evt, void *params)
{
    switch (evt) {
    case BT_NOTICE_TWS_CONNECTED:
        if (bt_tws_is_slave()) {
            msg_enqueue(EVT_ASR_STOP);
        }
        break;

    case BT_NOTICE_TWS_ROLE_CHANGE:
        if (bt_tws_is_slave()) {
            msg_enqueue(EVT_ASR_STOP);
        } else {
            msg_enqueue(EVT_ASR_START);
        }
        break;
    }
}
#endif
