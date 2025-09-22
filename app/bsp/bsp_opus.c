#include "include.h"
#include "api.h"

#if OPUS_ENC_EN && LE_DUEROS_DMA_EN

#define OPUS_BIT_RATE				16000       //32kbps: 压缩比1:8, 每帧输出80字节;  16kbps: 压缩比1:16, 每帧输出40字节

u16 opus_pcm_len(void);
int opus_enc_frame(s16 *pcm, u8 *packet);
void opus_kick_start(void);
int sco_set_mic_gain_after_aec(void);
void mic_post_gain_process_s(s16 *ptr, int gain, int samples);
bool opus_enc_init(u32 spr, u32 nch, u32 bitrate);
void opus_enc_exit(void);

static u8 opus_sysclk, opus_init = 0;
bool opus_start;
static uint8_t opus_skip_frame = 0;

//---------------------------------------------------------------------
//
#define MAX_PCM_BUF_SIZE     	1280            //50ms
typedef struct {
    s16 pcm[MAX_PCM_BUF_SIZE];
    u32 len;
    u16 widx;
    u16 ridx;
} opus_cb_t;

static opus_cb_t opus_pcm_cb AT(.opus.buf.bsp);
static volatile int opus_enc_sta;
static u8 opus_channel;
static u8 opus_packet[40] AT(.opus.buf.bsp);

//---------------------------------------------------------------------
//缓存压缩数据
#define OPUS_CACHE_LEN       480

struct opus_mcb_t {
    u16 wptr;
    u16 rptr;
    volatile u16 len;
    u8 output[OPUS_CACHE_LEN];

    u8 nr_type;
    int gain;
} bsp_opus_mcb AT(.opus.buf.bsp);

//--------------------------------------------------------------------------------
AT(.com_text.opus)
void opus_skip_frame_do(s16 *ptr, u32 samples, int ch_mode)
{
    opus_skip_frame--;
    memset(ptr, 0, samples << (1+ch_mode));
}

//--------------------------------------------------------------------------------
//128点输入一次
AT(.com_text.sndp)
WEAK void opus_nr_process(s16 *buf)
{

}

//---------------------------------------------------------------------
AT(.com_text.opus)
void bsp_opus_put_pcm(u8 *buf, u16 len, int ch_mode)
{
    opus_channel = ch_mode + 1;
    opus_cb_t *p = &opus_pcm_cb;

    if (p->len <= MAX_PCM_BUF_SIZE) {
        memcpy(&p->pcm[p->widx], buf, len * 2 * opus_channel);
        p->widx += (len * opus_channel);
        if (p->widx >= MAX_PCM_BUF_SIZE) {
            p->widx = 0;
        }
        GLOBAL_INT_DISABLE();
        p->len += (len * opus_channel);
        GLOBAL_INT_RESTORE();
    }else{
        uart_putchar('!');
    }
}

//---------------------------------------------------------------------
AT(.com_text.opus)
static s16* opus_get_pcm(void)
{
    s16 *buf = NULL;
    u32 frame_size = opus_pcm_len();
    frame_size = opus_channel * frame_size;
    opus_cb_t *p = &opus_pcm_cb;

    if (p->len >= frame_size) {
        buf = &p->pcm[p->ridx];
        p->ridx += frame_size;
        if (p->ridx >= MAX_PCM_BUF_SIZE) {
            p->ridx = 0;
        }
        GLOBAL_INT_DISABLE();
        p->len -= frame_size;
        GLOBAL_INT_RESTORE();
    }

    return buf;
}

//---------------------------------------------------------------------
//缓存压缩好的数据
AT(.com_text.opus)
void bsp_opus_put_frame(u8 *buf, u16 len)
{
    struct opus_mcb_t *p = &bsp_opus_mcb;

    if (p->len <= OPUS_CACHE_LEN) {
        memcpy(&p->output[p->wptr], buf, len);
        p->wptr += len;
        if (p->wptr >= OPUS_CACHE_LEN) {
            p->wptr = 0;
        }
        GLOBAL_INT_DISABLE();
        p->len += len;
        GLOBAL_INT_RESTORE();
    }else{
        uart_putchar('*');
    }
}

//---------------------------------------------------------------------
//
AT(.com_text.opus)
u8 opus_get_frame(u8 *rx_buff, u16 opus_frame_len)
{
	struct opus_mcb_t * p = &bsp_opus_mcb;

	if (p->len < opus_frame_len) {	   //1帧
		return 0;
	}

	memcpy(rx_buff, &p->output[p->rptr], opus_frame_len);

	p->rptr += opus_frame_len;
	if (p->rptr >= OPUS_CACHE_LEN) {
		p->rptr = 0;
	}

	GLOBAL_INT_DISABLE();
	p->len -= opus_frame_len;
	GLOBAL_INT_RESTORE();

	return opus_frame_len;
}


//---------------------------------------------------------------------
//
void bsp_opus_start_init(u8 nr_type)
{
    memset(&bsp_opus_mcb, 0, sizeof(struct opus_mcb_t));
    memset(&opus_pcm_cb, 0, sizeof(opus_pcm_cb));

    opus_enc_sta = 0;
    bsp_opus_mcb.nr_type = nr_type;
    bsp_opus_mcb.gain = sco_set_mic_gain_after_aec();
}

//---------------------------------------------------------------------
//
void bsp_opus_stop_end(void)
{
    if (bsp_opus_mcb.nr_type) {
        bsp_opus_mcb.nr_type = 0;
    }
}

//---------------------------------------------------------------------
//opus_sdadc_process
AT(.com_text.opus)
WEAK void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    if (opus_skip_frame) {
        opus_skip_frame_do((s16*)ptr, samples, ch_mode);
        return;
    }

    if(bsp_opus_mcb.nr_type){
        opus_nr_process((void *)ptr);
        if(bsp_opus_mcb.gain){
            mic_post_gain_process_s((s16 *)ptr, bsp_opus_mcb.gain, samples);   //数字后增益
        }
        if(bsp_opus_mcb.nr_type == 2){
            ch_mode = 0;
        }
    }

    bsp_opus_put_pcm(ptr, samples, ch_mode);

    if (!opus_enc_sta && opus_pcm_cb.len >= (opus_pcm_len()<< ch_mode)) {
        opus_kick_start();
    }

#if TME_APP_EN
    tme_opus_frame_energy_cal(ptr,samples);
#endif
}

//---------------------------------------------------------------------
//

//---------------------------------------------------------------------
//
AT(.com_text.opus)
void opus_enc_process(void)
{
    int rlen;
    s16 *in = opus_get_pcm();
    if (in == NULL) {
        return;
    }

    opus_enc_sta = 1;
    rlen = opus_enc_frame(in, opus_packet);
    if(rlen < 0 || rlen > (128*opus_channel)) {
        uart_putchar('&');
        //printf("opus encode failed: %d\n",rlen);
    } else {
        //printf("opus:%d\n", rlen);
        bsp_opus_put_frame(opus_packet, rlen);
    }
    opus_enc_sta = 0;
}

AT(.text.opus.bsp)
bool bsp_opus_is_encode(void)
{
    return (opus_init == 1);
}

u16 opus_enc_data_len_get(void)
{
    return bsp_opus_mcb.len;
}

AT(.text.opus.bsp)
void bsp_opus_encode_start(void)
{
    if(opus_init == 0 && f_bt.disp_status < BT_STA_INCOMING){
        printf("--->bsp_opus_encode_start\n");
        bt_tws_user_key(OPUS_ENC_START);
        bt_audio_bypass();
        delay_5ms(10);      //wait audio bypass
        if(opus_enc_init(SPR_16000, 1, OPUS_BIT_RATE)){
            opus_sysclk = sys_clk_get();
            sys_clk_set(SYS_160M);
            opus_skip_frame = 125; //丢掉MIC刚启动时不稳定的数据
            bsp_opus_start_init(NR_TYPE_SMIC);
            audio_path_init(AUDIO_PATH_OPUS);
            audio_path_start(AUDIO_PATH_OPUS);
            opus_init = 1;
        }
     }
}

AT(.text.opus.bsp)
void bsp_opus_encode_stop(void)
{
    if(opus_init){
        printf("--->bsp_opus_encode_stop\n");
        opus_init = 0;
        opus_skip_frame = 0;
        bt_tws_user_key(OPUS_ENC_STOP);
        audio_path_exit(AUDIO_PATH_OPUS);
        opus_enc_exit();
        bsp_opus_stop_end();
        sys_clk_set(opus_sysclk);
        bt_audio_enable();
        delay_5ms(10);      //wait audio bypass enable
    }
}

AT(.text.opus.bsp)
u8 bsp_opus_get_enc_frame(u8 *buff, u16 len)
{
    if(opus_init && (buff != NULL)){
        return opus_get_frame(buff,len);
    }

    return 0;
}

#elif OPUS_ENC_EN
void asr_kick_start(void);
void opus_enc_exit(void);
bool opus_enc_init(u32 spr, u32 nch, u32 bitrate);
int opus_enc_frame(s16 *pcm, u8 *packet);
u16 opus_pcm_len(void);
void opus_mic_tx(u8 type, u8* addr, u16 data_len);

#define REC_OBUF_SIZE                1024                //ADC PCM缓存BUF SIZE
#define REC_ENC_SIZE                 0xA00               //录音压缩数据缓存BUF SIZE

u8 rec_obuf[REC_OBUF_SIZE] AT(.opus.rec.obuf);
u8 rec_encbuf[REC_ENC_SIZE] AT(.opus.rec.enc);
au_stm_t rec_enc_stm AT(.opus.rec.buf);
au_stm_t rec_pcm_stm AT(.opus.rec.buf);
peri_nr_cfg_t opus_mic_nr_cfg AT(.opus.rec.nr);

static opus_cb_t opus_enc_cb;
u8 opus_pack[40] AT(.opus.rec.buf);
u8 mic_data[640] AT(.opus.rec.buf);

void rec_stm_buff_init(void)
{
    memset(&rec_pcm_stm, 0, sizeof(rec_pcm_stm));
    memset(&rec_enc_stm, 0, sizeof(rec_enc_stm));

    rec_pcm_stm.buf = rec_pcm_stm.rptr = rec_pcm_stm.wptr = rec_obuf;
    rec_pcm_stm.size = REC_OBUF_SIZE;

    rec_enc_stm.buf = rec_enc_stm.rptr = rec_enc_stm.wptr = rec_encbuf;
    rec_enc_stm.size = REC_ENC_SIZE;
}

u8 bsp_get_opus_param_packetsize(void)
{
    return opus_enc_cb.packet_size;
}

u8 bsp_get_opus_param_nchannel(void)
{
    return opus_enc_cb.nch;
}

void bsp_opus_rec_mic_start(void)
{
    if (opus_enc_cb.is_init == true) return;
    printf("%s\n",__func__);
    opus_enc_cb.spr = SPR_16000;
    opus_enc_cb.nch = 1;
    opus_enc_cb.is_init = false;
    opus_enc_cb.packet_size = 40;
    opus_enc_cb.bitrate = 16000;

    if (opus_enc_init(opus_enc_cb.spr, opus_enc_cb.nch, opus_enc_cb.bitrate)) {
        sys_clk_req(INDEX_ENCODE, SYS_160M);
        rec_stm_buff_init();
        audio_path_init(AUDIO_PATH_OPUS);
        audio_path_start(AUDIO_PATH_OPUS);
        opus_enc_cb.is_init = true;
        printf("opus_enc_init ok\n");
    } else {
        printf("opus_enc_init fail\n");
    }

}

void bsp_opus_rec_mic_stop(void)
{
    if (opus_enc_cb.is_init == false) return;
    printf("%s\n",__func__);
    audio_path_exit(AUDIO_PATH_OPUS);
    opus_enc_exit();
    opus_enc_cb.is_init = false;
    sys_clk_free(INDEX_ENCODE);
}

AT(.com_text.opus.sta)
bool bsp_opus_rec_is_working(void)
{
    return opus_enc_cb.is_init;
}

AT(.com_text.opus.rec)
void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if REC_OPUS_MIC_DUMP_EN
    opus_mic_tx(1, ptr, samples<<1); //dump降噪前
#endif
    puts_stm_buf(&rec_pcm_stm, ptr, samples<<1);
    if (rec_pcm_stm.len >= opus_pcm_len() * 2) {
        asr_kick_start();
    }
#if OPUS_DEC_EN
    else {
        bsp_opus_dec_kick(); //由mic callback 定时检查是否能解码
    }
#endif
}

u16 bsp_get_opus_data(u8* buf, u16 len)
{
    if (gets_stm_buf(&rec_enc_stm, buf, len)) {
        return len;
    } else {
        return 0;
    }
}
#endif

#if OPUS_DEC_EN
#define OPUS_ENC_DEC_TEST               0                     //测试编解码功能，需要打开OPUS_DEC_EN，OPUS_ENC_EN.声音路径：mic-->enc-->dec-->dump
#define OPUS_PACKETBUF_SIZE             (240*4)               //OPUS PACKET BUF SIZE
bool opus_dec_init(u32 spr, u32 nch);
int opus_dec_frame(s16 *pcm_obuf, u8 *packet, u32 packet_len);
void opus_mic_tx(u8 type, u8* addr, u16 data_len);

static opus_cb_t opus_dec_cb;
u8 opus_packebuf[OPUS_PACKETBUF_SIZE] AT(.opus.rec.dec);
au_stm_t opus_pack_stm AT(.opus.rec.buf);
u8 opus_dec_pack[80] AT(.opus.rec.buf);
u8 opus_dec_pcm[640] AT(.opus.rec.buf);

bool bsp_opus_dec_init(u32 spr, u32 nch)
{
    memset(&opus_dec_cb, 0, sizeof(opus_cb_t));
    opus_dec_cb.spr = spr;
    opus_dec_cb.nch = nch;
    if (spr == SPR_32000) {
        opus_dec_cb.packet_size = 80;
    } else {
        opus_dec_cb.packet_size = 40;
    }
    opus_dec_cb.packet_size *= nch;
    opus_dec_cb.is_init = false;

    memset(&opus_pack_stm, 0, sizeof(opus_pack_stm));
    opus_pack_stm.buf = opus_pack_stm.rptr = opus_pack_stm.wptr = opus_packebuf;
    opus_pack_stm.size = OPUS_PACKETBUF_SIZE;

    if (opus_dec_init(spr, nch)) {
        opus_dec_cb.is_init = true;
        sys_clk_req(INDEX_ENCODE, SYS_160M);
        printf("%s, ok\n",__func__);
        return true;
    } else {
        printf("%s, fail\n",__func__);
        return false;
    }
}

void bsp_opus_dec_exit(void)
{
    opus_dec_cb.is_init = false;
#if OPUS_ENC_EN
    if (!opus_enc_cb.is_init)  //由enc函数释放时钟
#endif
    {
        sys_clk_free(INDEX_ENCODE);
    }
}

AT(.com_text.opus.dec)
bool bsp_opus_dec_put_pack(u8* buf, u32 len)
{
    return puts_stm_buf(&opus_pack_stm, buf, len);
}

AT(.com_text.opus.dec)
void bsp_opus_dec_kick(void)
{
    if (opus_dec_cb.is_init && opus_pack_stm.len >= opus_dec_cb.packet_size) {
        asr_kick_start();
    }
}

#if OPUS_ENC_DEC_TEST
void bsp_opus_enc_dec_test(void)
{
    bsp_opus_dec_init(SPR_16000, 1);
    bsp_opus_rec_mic_start();
}
#endif

#endif

#if OPUS_DEC_EN ||OPUS_ENC_EN
#define REC_OPUS_MIC_DUMP_EN         0      //dump opus链路数据

#if REC_OPUS_MIC_DUMP_EN
void huart_wait_txdone(void);

u8 dump_header[7];
AT(.opus_dump_buf)
u8 huart_buf[640 + 7];
AT(.com_text.opus_dump)
void opus_mic_tx(u8 type, u8* addr, u16 data_len)
{
    //特别说明：type1--5只能是pcm数据，type6必须是opus编码后的数据
    huart_wait_txdone();
    u16 len = 0;
    dump_header[0] = 0x36;
    dump_header[1] = 0xAD;
    dump_header[2] = 0xf9;
    dump_header[3] = 0x54;
    dump_header[4] = (u8)(data_len >> 8);
    dump_header[5] = (u8)(data_len >> 0);
    dump_header[6] = type;
    memcpy(&huart_buf[len],dump_header,7);
    len += 7;
    memcpy(&huart_buf[len],addr,data_len);
    len += data_len;
    huart_tx(huart_buf,len);
}
#endif

//借用asr线程
void asr_kws_process(void)
{

#if OPUS_ENC_EN && !LE_DUEROS_DMA_EN
    u16 len = 0;
    if (opus_enc_cb.is_init && gets_stm_buf(&rec_pcm_stm, mic_data, opus_pcm_len() * 2)) {
        len = opus_enc_frame((s16*)mic_data,opus_pack);
        puts_stm_buf(&rec_enc_stm, opus_pack, len);
        #if REC_OPUS_MIC_DUMP_EN
        opus_mic_tx(6, opus_pack, len); //dump opus编码后的
        #endif

    }
#endif

#if OPUS_DEC_EN && !LE_DUEROS_DMA_EN
    #if OPUS_ENC_DEC_TEST
    if (len) {
        int samples = opus_dec_frame((s16*)opus_dec_pcm, opus_pack, len);
        #if REC_OPUS_MIC_DUMP_EN
        opus_mic_tx(2, opus_dec_pcm, samples << 1); //dump opus 解码后的
        #endif
    }
    #else
    if (opus_dec_cb.is_init && gets_stm_buf(&opus_pack_stm, opus_dec_pack, opus_dec_cb.packet_size)) {
        int samples = opus_dec_frame((s16*)opus_dec_pcm, opus_dec_pack, len);
        #if REC_OPUS_MIC_DUMP_EN
        opus_mic_tx(2, opus_dec_pcm, samples << 1); //dump opus 解码后的
        #endif
    }
    #endif
#endif
}
#endif
