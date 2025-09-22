#include "include.h"
#include "bsp_vad.h"
#include "asr.h"
#if VAD_EN

#define TRACE_EN                        0
#if TRACE_EN
#define TRACE(...)                      printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

void asr_kick_start(void);
u32 getcfg_mic_bias_method(u8 mic_ch);
static void vad_reset(void);

#if VAD_MODE_SEL == VAD_MODE_1
AT(.vad.mic_buf.adc)
u16 vad_sdadc_buf[VAD_DMA_SIZE];
#else
struct asr_buf_t {
    volatile s16 wptr;
    volatile s16 rptr;
    short pcm_buf[ASR_PCM_BUFF_NUM][ASR_PCM_NUM];
} asr_buf AT(.vad.mic_buf.data);
#endif

typedef enum {
    VAD_NORMAL = 0,
    VAD_SLEEP,
    VAD_W4_WAKE,
} VAD_SLEEP_STA;

typedef enum {
    VAD_IDLE = 0,
    VAD_START,
    VAD_CONTINUE,
} VAD_STA;

typedef struct {
    volatile u8 sta;            //vad唤醒状态
    volatile u8 sleep_flag;
    volatile u16 offset;        //VADDMACON0的高16位, +1:u8
    u8 start;                   //vad功能是否启动

    u16 vad_reboot_cnt;         //vad reboot 计数
    u16 *speech_start;          //语音在adcdma上对应的起始地址
    u32  start_tick;            //唤醒时的tick
    u32  dma_len;               //DMA总长度
    u8  *dma_ptr;               //DMA 中断触发模式，half or all done
    u8  *dma_ptr_t;
    u32  dma_addr_end;
    u32 samples;                //vad的样点数

    u16 neg_flag_cnt;
    u8 voice_flag;
    bool init_flag;
} vad_cb_t;
static vad_cb_t vad_cb;


#if VAD_MODE_SEL == VAD_MODE_1
//计算预取前面frames_num帧的offset
AT(.vad_text.vad)
static void vad_prefetch_cal(u16 frames_num)
{
    u32 vad_len = frames_num;
    //VAD要取KWS_SAMPLES个u16的整数倍，offset要往前推成整数
    if(vad_cb.offset < KWS_SAMPLES) {
        vad_cb.offset  = 0;
    } else {
        vad_cb.offset -= ((vad_cb.offset)%KWS_SAMPLES);
    }
	//计算往前推frames_num帧之后，VAD在DMA上的起始地址
    if(vad_cb.offset >= vad_len) {
        vad_cb.speech_start  = (u16 *)(asr_get_adc_buff_addr() + ((vad_cb.offset - vad_len)<<1));
    } else {
        vad_cb.speech_start  = (u16 *)(asr_get_adc_buff_addr() + ((vad_cb.dma_len - (vad_len - vad_cb.offset))<<1));
    }
}

//取出前面(512点/帧)的数据
AT(.vad_text.vad)
static u32 vad_prefetch_read(void)
{
    u32 p = (u32)vad_cb.speech_start;
    vad_cb.speech_start += KWS_SAMPLES;
    if ((u32)vad_cb.speech_start >= vad_cb.dma_addr_end) {
        vad_cb.speech_start = (u16 *)asr_get_adc_buff_addr();
    }
    return p;
}


AT(.vad_text.vad)
static u32 vad_dma_audio_read(u16 offset)
{
    u32 rptr;
    if (vad_cb.offset > 0) {
        rptr = (u8*)vad_sdadc_buf + vad_cb.offset * 2;
    } else {
        rptr = (u32)vad_cb.dma_ptr_t;
    }
    u32 p = (u32)(rptr + (offset << 1));

    if (p >= vad_cb.dma_addr_end) {
        u32 mod = p - vad_cb.dma_addr_end;
        p = asr_get_adc_buff_addr() + mod;
    }
    return p;
}

AT(.vad_text.vad)
static void vad_continue_process(void)
{
    s16 num, num2;
    u8 i;
    int16_t *rptr;
    u16 offset, dma_half_size;
    num2 = 0;
    offset = vad_cb.offset;

    vad_cb.dma_ptr_t = vad_cb.dma_ptr;
    dma_half_size = vad_cb.dma_len / 2;
    num = dma_half_size / KWS_SAMPLES;          //除以KWS_SAMPLES, 计算DMA中有num个KWS_SAMPLES帧

    if (offset > 0) {                           //刚取完前面帧，不需要取DMA所有数据
        if ((u32)vad_cb.dma_ptr_t == asr_get_adc_buff_addr()) {
            if (offset <= dma_half_size) {      //VAD起来的offset位于half done前 //offset==dma_half_size, num-num2=0
                num2 = offset / KWS_SAMPLES;
            } else {                            //dma half done, 但是VAD起来的offset位于all done前
                offset = (dma_half_size<<1) - offset;
                num2 = -(offset / KWS_SAMPLES);
            }
        } else {
            if (offset > dma_half_size) {       //VAD起来的offset位于all done前  //offset==dma_half_size, num-num2=0
                offset = offset - dma_half_size;
                num2 = offset / KWS_SAMPLES;
            } else {                            //dma all done, 但是VAD起来的offset位于half done前
                offset = dma_half_size - offset;
                num2 = -(offset / KWS_SAMPLES);
            }
        }
    }
    if (num2 > num) {
        TRACE("------------vad offset error!--------\n");
        vad_reset();                            //VAD强制清零
        return;
    }
    num -= num2;
#if DEBUG_EN
    GPIOASET = BIT(14);
#endif
    TRACE("num %d %d\n", num, num2);
    for (i = 0; i < num; i++) {
        rptr = (int16_t *)vad_dma_audio_read(KWS_SAMPLES * i);
        asr_callback_process(rptr);
//        if (i != 0 && i % 5 == 0) delay_5ms(1); //主动释放线程5ms
    }
#if DEBUG_EN
    GPIOACLR = BIT(14);
#endif
    vad_cb.offset = 0;
}

#else
AT(.com_text.alg)
static s16 alg_ptr_to_next(s16 ptr)
{
    s16 res = ptr + 1;
    if (res >= ASR_PCM_BUFF_NUM) {
        res = 0;
    }
    return res;
}

void vad_force_asr_start(void)
{
    vad_cb.start_tick = tick_get();
    struct asr_buf_t *p = &asr_buf;
    p->rptr = ASR_PCM_BUFF_NUM - 2;
    vad_cb.sta = VAD_START;
    if (vad_cb.sleep_flag == VAD_SLEEP) {
        vad_cb.sleep_flag = VAD_W4_WAKE;
    } else if (vad_cb.sleep_flag == VAD_NORMAL) {
        asr_kick_start();
    }
}

#endif

AT(.com_text.vad)
static void vad_reboot(void)
{
    if (vad_cb.init_flag) {
        VADCON0 &= ~BIT(0);                 //VAD disable
        vad_cb.vad_reboot_cnt = 0;
        VADCON0 |= BIT(0);                  //VAD enable
    }
}

AT(.vad_text.vad)
static void vad_reset(void)
{
    if (vad_cb.init_flag) {
        TRACE("%s\n",__func__);
        VADCON0 &= ~BIT(0);                 //VAD disable
        vad_cb.sta = VAD_IDLE;
        vad_cb.offset = 0;
        vad_cb.speech_start = 0;
        vad_cb.start_tick = 0;
        VADCON0 |= BIT(0);                  //VAD enable
        SDADC0DMACON |= BIT(9);             //VAD interrupt enable
        sys_clk_free(INDEX_KARAOK);
    }
}

AT(.com_text.vad.sta)
bool vad_is_start(void)
{
    return vad_cb.start;
}

///返回VAD是否已经唤醒
AT(.vad_text.vad)
bool vad_is_wake(void)
{
    return (bool)(vad_cb.sta >= VAD_START);
}

///VAD启动检测
void vad_start(void)
{
    if (vad_cb.init_flag) {
        vad_reset();
        SDADC0DMACON &= ~BIT(9);            //VAD interrupt disable
        SDADCDMACLR |= BIT(24);             //clear VAD interrupt flag
        SDADC0DMACON |= BIT(9);             //VAD interrupt enable

        VADCON0 |= BIT(6);                  // tick
        VADCON0 |= BIT(0);                  // VAD enable

        vad_cb.start = 1;
        TRACE("VADCON0 %08x\n",VADCON0);
        TRACE("VADCON1 %08x\n",VADCON1);
        TRACE("VADCON2 %08x\n",VADCON2);
        TRACE("SDADCDMACON:%08x\n", SDADCDMACON);
    }
}

void vad_stop(void)
{
    if (vad_cb.init_flag) {
        SDADC0DMACON &= ~BIT(9);             //VAD interrupt disable
        SDADCDMACLR |= BIT(24);
        VADCON0 &= ~BIT(0);                  //VAD disable
        VADCON0 &= ~BIT(6);                  // tick
        vad_cb.start = 0;
        vad_cb.sta = VAD_IDLE;
        vad_cb.init_flag = false;
    }
}

///进出休眠模式的配置
AT(.vad_text.vad)
void vad_sleep_config(bool enter)
{
    if (enter) {
        vad_cb.sleep_flag = VAD_SLEEP;
        vad_reset();
    } else {
        vad_cb.sleep_flag = VAD_NORMAL;
        if (vad_cb.sta == VAD_START && vad_cb.sleep_flag == VAD_NORMAL) {
            asr_kick_start();
        }
    }
}

AT(.vad_text.vad.proc)
void vad_kws_process(void)
{
    if ((KWS_RUNTIME_LIMIT != 0) && (vad_cb.start_tick > 0)) {
        if (tick_check_expire(vad_cb.start_tick, KWS_RUNTIME_LIMIT)) { //超过KWS限制的时间
            vad_reset();
            return;
        }
    }
#if VAD_MODE_SEL == VAD_MODE_1
    if (vad_cb.sta == VAD_START) {
        sys_clk_req(INDEX_KARAOK,SYS_160M);                             //借用INDEX_KARAOK
        u8 vad_pre_num = VAD_PREFETCH_NUM + VAD_PRE_GET;
        vad_prefetch_cal(vad_pre_num * KWS_SAMPLES);
        int16_t *rptr;
        for (u8 i = 0; i < vad_pre_num; i++) {
            //先把当前DMA的数据取出来扔到asr alg
            rptr = (int16_t *)vad_prefetch_read();
            asr_callback_process(rptr);
//            if (i != 0 && i % 5 == 0) delay_5ms(1);                     //主动释放线程5ms
        }
        TRACE("VAD_START\n");
        TRACE("vad_pre_num: %d\n", vad_pre_num);
        TRACE("vad offset: %x\n", vad_cb.offset);
        vad_cb.sta = VAD_CONTINUE;
    } else if (vad_cb.sta == VAD_CONTINUE){
        vad_continue_process();
    }
#else
    struct asr_buf_t *p = &asr_buf;
    if (vad_cb.sta == VAD_START) {
        sys_clk_req(INDEX_KARAOK,SYS_160M);
        asr_callback_process(p->pcm_buf[p->rptr]);
        p->rptr = alg_ptr_to_next(p->rptr);
        vad_cb.sta = VAD_CONTINUE;
    } else if (vad_cb.sta == VAD_CONTINUE){
        asr_callback_process(p->pcm_buf[p->rptr]);
        p->rptr = alg_ptr_to_next(p->rptr);
    }
#endif
}

AT(.com_text.vad)
void vad_isr(void)
{
    if ((SDADCDMAFLAG & BIT(24))) {
        SDADCDMACLR |= BIT(24);
        SDADC0DMACON &= ~BIT(9);                        //VAD interrupt disable
        vad_cb.offset = VADDMACON>> 16;                 //获取VAD中断时，VAD DMA BUFF的偏移地址
        vad_cb.start_tick = tick_get();
        vad_cb.vad_reboot_cnt = 0;
        vad_cb.voice_flag = 1;
#if VAD_MODE_SEL == VAD_MODE_2
        struct asr_buf_t *p = &asr_buf;
        p->rptr = p->wptr - (ASR_PRE_READ + 1);
        if (p->rptr < 0) {
            p->rptr += ASR_PCM_BUFF_NUM;
        }
#endif
        vad_cb.sta = VAD_START;
        if (vad_cb.sleep_flag == VAD_SLEEP) {
            vad_cb.sleep_flag = VAD_W4_WAKE;
        } else if (vad_cb.sleep_flag == VAD_NORMAL) {
            asr_kick_start();
        }
    }
}

///SDADC DMA中断
AT(.com_text.vad)
void vad_sdadc_isr(void)
{
    if (SDADCDMAFLAG & BIT(24)) {   //VAD voice
        SDADCDMACLR |= BIT(24);
        if (vad_cb.sta == VAD_CONTINUE) {
            if (vad_cb.neg_flag_cnt) {
                vad_cb.neg_flag_cnt--;
                if (vad_cb.neg_flag_cnt == (VAD_NEG_FLAG_THRESHOLD - 1)) {
                    vad_cb.neg_flag_cnt = VAD_NEG_FLAG_THRESHOLD / 2;
                    vad_cb.voice_flag = 1;
                }
            }
        }
    } else {                        //VAD not voice
        if (vad_cb.sta == VAD_CONTINUE) {
            if (vad_cb.neg_flag_cnt >= VAD_NEG_FLAG_THRESHOLD) {
                vad_cb.neg_flag_cnt = VAD_NEG_FLAG_THRESHOLD;
                vad_cb.voice_flag = 0;
            } else {
                vad_cb.neg_flag_cnt++;
            }
        }
    }
}

AT(.com_text.vad.proc)
void vad_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    vad_cb.vad_reboot_cnt++;
    if (vad_cb.vad_reboot_cnt >= VAD_REBOOT_THRESOLD ) {
        vad_reboot();
    }
#if VAD_MODE_SEL == VAD_MODE_1
    vad_cb.dma_ptr = ptr;
#else
    struct asr_buf_t *p = &asr_buf;
    memcpy(p->pcm_buf[p->wptr], ptr, ASR_PCM_NUM * 2);
    p->wptr = alg_ptr_to_next(p->wptr);
#endif
    if (((vad_cb.sta == VAD_CONTINUE) || (vad_cb.sta == VAD_START)) && vad_cb.sleep_flag == VAD_NORMAL) {
        asr_kick_start();
    }
}

u8 vad_init(void)
{
#if VAD_MODE_SEL == VAD_MODE_2
    memset(&asr_buf, 0, sizeof(struct asr_buf_t));
#endif
	memset(&vad_cb,0,sizeof(vad_cb_t));
    vad_cb.dma_len = asr_get_adc_buff_len();
    vad_cb.dma_addr_end = asr_get_adc_buff_addr() + vad_cb.dma_len * 2;
    vad_cb.sta = VAD_IDLE;
    vad_cb.start = 0;
    vad_cb.samples = (1 << VAD_SAMPLES) * 128;

    CLKGAT3 |= BIT(12);         //vad_clken
    delay_ms(1);
    VADCON0 = 0;
    u8 mic_index = xcfg_cb.bt_mmic_cfg;
    VADCON0 |= (mic_index << 1);
    VADCON0 |= (VAD_SAMPLES << 4);    //sample
	VADCON1 = (VAD_NEG_CNT | (VAD_POS_CNT << 16));
	VADCON2 = 0x00FF000F;
	vad_cb.init_flag = true;
	return 1;
}

u8 vad_mic_pwr_sel_io(void)
{
    u8 mic_index = xcfg_cb.bt_mmic_cfg;
    u8 io = (getcfg_mic_bias_method(mic_index+1)& 0xf0) >> 4;
    return io;
}

void bsp_vad_start(void)
{
    vad_init();
    vad_start();
#if VAD_MODE_SEL == VAD_MODE_2
    vad_force_asr_start();
#endif
}

void bsp_vad_stop(void)
{
    vad_stop();
}


#endif

