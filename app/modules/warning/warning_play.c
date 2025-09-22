#include "include.h"
#include "warning_play.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#define TRACE_ERR(...)          printf(__VA_ARGS__)


u32 waring_get_digvol_cb(void);
void wsbc_kick_start(void);
void peri_nr_init(void);

#if WARNING_WSBC_EN

enum {
    PLAY_STA_IDLE,
    PLAY_STA_INITING,
    PLAY_STA_STOP,
    PLAY_STA_STOPING,
    PLAY_STA_DEC_1ST,
    PLAY_STA_W4_KICK,
    PLAY_STA_PROC,
};

#define HEADER_LEN              8

warning_cb_t warning_cb;
volatile u8 warning_play_state = PLAY_STA_IDLE;

AT(.text.warning)
static u8 *warning_ptr_get(void)
{
    return warning_cb.res_ptr+warning_cb.rd_pos;
}

static ALWAYS_INLINE u32 warning_remain_len_get(void)
{
    return (warning_cb.res_size - warning_cb.rd_pos);
}

AT(.text.warning.proc)
void warning_fill_stream(void)
{
    u32 cbuf_reamin_len = CACHE_BUF_SIZE - warning_cb.res_ring_buf.total;
    u8 *warning_ptr = warning_ptr_get();
    u32 warning_remain_len = warning_remain_len_get();

    while (cbuf_reamin_len >= (CACHE_BUF_SIZE/2) && warning_remain_len > 0) {
        if (warning_remain_len >= cbuf_reamin_len) {
            ring_buf_put((void *)&warning_cb.res_ring_buf, warning_ptr, cbuf_reamin_len);
            warning_cb.rd_pos += cbuf_reamin_len;
        } else {
            ring_buf_put((void *)&warning_cb.res_ring_buf, warning_ptr, warning_remain_len);
            warning_cb.rd_pos += warning_remain_len;
        }
        cbuf_reamin_len = CACHE_BUF_SIZE - warning_cb.res_ring_buf.total;
        warning_ptr = warning_ptr_get();
        warning_remain_len = warning_remain_len_get();

        if (!warning_remain_len) {
            return;
        }
    }
}

AT(.text.warning)
static bool warning_init(u8 *ptr, u32 size, u8 *spr_idx)
{
    u16 sample_rate;
    memset((u8 *)&warning_cb, 0, sizeof(warning_cb));
    warning_cb.res_ptr  = ptr + HEADER_LEN;
    warning_cb.res_size = size - HEADER_LEN;

    ring_buf_init(&warning_cb.res_ring_buf, warning_cb.ibuf, CACHE_BUF_SIZE, 0);
    warning_fill_stream();

    if(!wsbc_play_init(ptr, &sample_rate, &warning_cb.frame_size)) {
        return false;
    }

    if(sample_rate == 8000) {
        *spr_idx = SPR_8000;
    } else if(sample_rate == 16000) {
        *spr_idx = SPR_16000;
    } else {
        return false;
    }
    return true;
}

AT(.text.warning)
static void warning_start(u8 spr_idx)
{
    dac1_aubuf_init();
    dac1_vol_set(waring_get_digvol_cb());
    dac1_spr_set(spr_idx);

    dac1_fade_in();
    dac1_fade_wait();

    warning_play_state = PLAY_STA_DEC_1ST;
    TRACE("res_play_start\n");
}

AT(.text.warning)
static void warning_stop(void)
{
    aubuf1_dma_w4_done();
    dac1_fade_out();
    dac1_put_zero(128);                 //使OBUF播放完
    dac1_aubuf_w4_empty();
    dac1_fade_wait();
    dac1_vol_set(0);
    AUBUF1CON |= BIT(0);                //Reset audio Buffer
    DACDIGCON0 &= ~BIT(8);              //SRC1 disable

    TRACE("res_play_stop\n");
    warning_play_state = PLAY_STA_IDLE;
}

AT(.com_text.warning)
bool warning_is_playing(void)
{
    return warning_play_state != PLAY_STA_IDLE;
}

AT(.com_text.warning)
void warning_dec_check_kick(void)
{
    u8 play_state = warning_play_state;
    if(play_state >= PLAY_STA_DEC_1ST) {        //解码状态
        if(!aubuf1_dma_is_busy()) {             //上一帧DMA是否完成
            wsbc_kick_start();
        }
    }
}

AT(.com_text.warning)
void warning_kick_dac_dma(void)
{
    uint samples = warning_cb.samples;

    if(samples > 0) {
        ///注意obuf放eram时不支持dma方式推dac
        aubuf1_dma_kick(warning_cb.obuf, samples, 1, 0);
//        print_r((u8 *)serial, frame_len);
    }
}

static void warning_dec_callback(uint8_t *buf, uint16_t len)
{
    memcpy(warning_cb.serial, buf, len);
}

AT(.text.wsbc.proc)
void warning_dec_proc(void)
{
    u16 frame_len = warning_cb.frame_size;
    u8 play_state = warning_play_state;

    if(play_state == PLAY_STA_PROC || play_state == PLAY_STA_DEC_1ST) {
        if (ring_buf_get((void *)&warning_cb.res_ring_buf, warning_dec_callback, frame_len)) {
            aubuf1_dma_w4_done();
            warning_cb.samples = wsbc_play_proc(warning_cb.serial, warning_cb.obuf, frame_len);
            if(play_state == PLAY_STA_DEC_1ST) {
                warning_play_state = PLAY_STA_W4_KICK;
            } else {
                warning_kick_dac_dma();
            }
        } else {
            if(warning_remain_len_get() < frame_len) {
                warning_play_state = PLAY_STA_STOP;
            }
        }
    } else if(play_state == PLAY_STA_STOPING) {
        warning_play_state = PLAY_STA_STOP;
    }
}

AT(.text.wsbc.proc)
void alg_wsbc_dec_process(void)
{
    warning_dec_proc();
}

AT(.com_text.wsbc.warning)
void warning_play_kick(void)
{
    GLOBAL_INT_DISABLE();
    if(warning_play_state == PLAY_STA_W4_KICK) {
        warning_play_state = PLAY_STA_PROC;
        warning_kick_dac_dma();
        GLOBAL_INT_RESTORE();
    } else {
        GLOBAL_INT_RESTORE();
    }
}

AT(.text.warning)
int warning_play_init(u8 *ptr, u32 size, bool kick_start)
{
    u8 spr_idx = SPR_16000;
    TRACE("res_play_init: %x, %x\n", ptr, size);
    if ((size == 0) || (ptr == 0)) {
        return -1;
    }
    warning_play_state = PLAY_STA_INITING;
#if BT_SCO_FAR_NR_EN
    if (sco_is_connected()) {
        bt_peri_nr_wait_done();
        bt_peri_nr_exit();
    }
#endif
    if(!warning_init(ptr, size, &spr_idx)) {
#if BT_SCO_FAR_NR_EN
        if (sco_is_connected()) {
            peri_nr_init();
        }
#endif
        warning_play_state = PLAY_STA_IDLE;
        return -2;
    }

    warning_start(spr_idx);
    return 0;
}

AT(.text.warning.proc)
int warning_play_proc(void)
{
    //避免错过kick时间
    warning_play_kick();

    //循环填充位流
    u8 play_state = warning_play_state;
    if(play_state == PLAY_STA_STOPING) {
        return 0;
    } else if(play_state > PLAY_STA_STOP) {
        u32 res_len = warning_remain_len_get();
        u32 cbuf_len = warning_cb.res_ring_buf.total;
        if(cbuf_len < warning_cb.frame_size && res_len > 0) {
            TRACE_ERR("\n====res_less: (%d,%d)\n", cbuf_len, res_len);
        }
        warning_fill_stream();
        return 0;
    }
    return 1;
}

ALIGNED(64)
void warning_play_exit(void)
{
    u8 tout = 8;

    GLOBAL_INT_DISABLE();
    if(warning_play_state > PLAY_STA_STOPING) {
        warning_play_state = PLAY_STA_STOPING;
    }
    GLOBAL_INT_RESTORE();

    TRACE("res_play_exit\n");
    while(warning_play_state == PLAY_STA_STOPING && tout--) {
        delay_5ms(1);
    }

    warning_stop();

#if BT_SCO_FAR_NR_EN
    if (sco_is_connected()) {
        peri_nr_init();
    }
#endif
}
#endif // WARNING_WSBC_EN

