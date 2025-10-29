#include "include.h"
#include "bt_sco.h"


u8 cfg_bt_near_frame = 128;
static u8 ring_fade_flag AT(.sco_cache);
static uint32_t call_setup_tick AT(.sco_cache);

static s16 agc_sco_buf[240] AT(.agc_buf.cb);

bool bt_sco_is_msbc(void);
extern bool smic_test_mic_en;
extern u8 dmic_test_mic_ch;
extern call_cfg_t *call_cfg;

void unlock_dcode(void);
void load_code_bt_voice(void);
void unlock_code_bt_voice(void);
void bt_sco_tx_process_do(u32 *ptr, u32 samples);
void dac_aubuf_set_size(u8 flag);


//------------------------------------------------------------------------------------------
#if BT_SCO_DUMP_EN || BT_AEC_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_EQ_DUMP_EN
static struct {
    u8 frame_cnt[DUMP_SCO_MAX_NB];
} sco_dump;

s16 bt_sco_dump_buf[4][240];

AT(.bt_voice.sco.dump)
void bt_sco_dump_cb(uint type, void *ptr, uint size)
{
    int idx = -1;

    switch(type) {
        //idx的值按照算法链路处理的顺序由小到大排列，最后处理的放最前面，idx值越大
#if BT_SCO_DUMP_EN || BT_AEC_DUMP_EN
    #if BT_DUMP_6M_EN
        case DUMP_MIC_NR:           idx++;
        case DUMP_AEC_FAR:          idx++;
        #if BT_SCO_DMIC_EN
        case DUMP_MIC_FF:           idx++;
        #endif
        case DUMP_MIC_TALK:         idx++;
    #else
        case DUMP_MIC_NR:           idx++;
        #if BT_AEC_DUMP_EN || BT_SNDP_FBDM_EN
        case DUMP_AEC_FAR:          idx++;
        #endif
        #if BT_SCO_DMIC_EN
        case DUMP_MIC_FF:           idx++;
        #endif
        case DUMP_MIC_TALK:         idx++;
    #endif

#elif BT_EQ_DUMP_EN
        case DUMP_EQ_OUTPUT:        idx++;
        case DUMP_MIC_NR:           idx++;
        #if BT_SCO_DMIC_EN && BT_DUMP_6M_EN
        case DUMP_MIC_FF:           idx++;
        #endif
        case DUMP_MIC_TALK:         idx++;

#elif BT_SCO_FAR_DUMP_EN
        case DUMP_FAR_NR_OUTPUT:    idx++;
        case DUMP_FAR_NR_INPUT:     idx++;
#endif
            //需要打印的case
            if ((0 <= idx) && (idx < DUMP_SCO_MAX_NB)) {
                void *buf = bt_sco_dump_buf[idx];
                memcpy(buf, ptr, size);
                sco_huart_putcs(idx, buf, size, sco_dump.frame_cnt[idx]++);
            }
            break;

        default :
            //不需要的走default
            break;
    }
}

    #define bt_sco_dump(a, b, c)            bt_sco_dump_cb(a, b, c)
#else
    #define bt_sco_dump(a, b, c)
#endif

#define RING_FADE_IN                40
#define RING_FADE_IDLE              (RING_FADE_IN + 1)
#define RING_FADE_DISCARD           (RING_FADE_IN + 50)
#define RING_FADE_OUT               1           //不可修改
#define RING_FADE_KEEP_SILENCE      2           //不可修改
#define RING_FADE_STOP              0XFF        //不可修改
#define RING_FADE_STA_CLEAR         0           //不可修改
#define RING_FADE_SOFT_EN           0           //是否使用软件淡出，否时直接淡出dac
AT(.bt_voice.sco)
void sco_fade_process(bool fade_out)
{
    if (bt_is_calling() && ring_fade_flag == RING_FADE_STA_CLEAR) {
        if (!tick_check_expire(call_setup_tick, 1500)) {
            ring_fade_flag = RING_FADE_IDLE;
        } else {
            ring_fade_flag = RING_FADE_OUT;                     //从铃声切到通话，开始淡出消除铃声结束po音
        }
    } else if(!bt_is_calling()){                                //铃声阶段，清空标志
        ring_fade_flag = RING_FADE_STA_CLEAR;
        if (!call_setup_tick) {
            call_setup_tick = tick_get();
        }
    }
    if (fade_out || ring_fade_flag == RING_FADE_OUT) {
        if(!fade_out)ring_fade_flag = RING_FADE_KEEP_SILENCE;   //为true不切换状态一直淡出等待sco结束
        dac_fade_out();
    } else if (ring_fade_flag < RING_FADE_IN && ring_fade_flag >= RING_FADE_KEEP_SILENCE) {
        ring_fade_flag++;                                       //铃声淡出完成后，剩余的铃声数据清空
        if(dac_is_fade_in()) {
            dac_fade_out();
        }
    } else if (ring_fade_flag >= RING_FADE_IN && ring_fade_flag != 0xff) {      //淡入通话声音
        if (ring_fade_flag >= RING_FADE_IDLE && ring_fade_flag < RING_FADE_DISCARD) {
            ring_fade_flag++;                                   //sco直接进入通话阶段，没有经历铃声阶段
            if(dac_is_fade_in()) {
                dac_fade_out();
            }
        } else {
            dac_fade_in();
        }
    }
}


//TODO:增加淡出流程，修复来电接听、挂断导致的po音
//AT(.bt_voice.sco)
//void sco_fade_process(bool fade_out)
//{
//}

AT(.bt_voice.sco.trumpet) WEAK
void bt_sco_trumpet_nr(s32 *fft_in)
{
//    if (bt_sco_dbg_get_alg_status(SCO_DBG_TRUMPET)) {
        trumpet_denoise((int*)fft_in);
//    }
}

//------------------------------------------------------------------------------------------
AT(.bt_voice.dac.drc) WEAK
void bt_sco_dac_drc_proc(s16 *ptr, u32 samples)
{
    for(int i = 0; i < samples; ++i) {
        ptr[i] = bt_sco_dac_drc_calc((s32)ptr[i]);
    }
}

AT(.bt_voice.dac.dnr) WEAK
void bt_sco_dac_dnr_proc(s16 *ptr, u32 samples, int dnr_thr)
{
	u16 pwr = dnr_buf_maxpow(ptr, samples);
    //printf("%d,", pwr);
    if (pwr <= dnr_thr) {
        memset(ptr, 0, samples*2);
    }
}

AT(.bt_voice.far_nr_do) WEAK
void bt_sco_far_nr_process(s16 *ptr, u16 samples)
{
	bt_sco_dump(DUMP_FAR_NR_INPUT, ptr, samples * 2);

#if BT_SCO_FAR_NR_EN            //通话下行————降噪
    bt_sco_far_peri_nr_process(ptr, samples);
#endif

#if BT_SCO_DAC_DRC_EN           //通话下行————DRC
    bt_sco_dac_drc_proc(ptr, samples);
#endif

#if BT_SCO_DAC_DNR_EN           //通话下行————DNR
    nr_cb_t *nr = &call_cfg->nr;
    bt_sco_dac_dnr_proc(ptr, samples, nr->dac_dnr_thr);
#endif

	bt_sco_dump(DUMP_FAR_NR_OUTPUT, ptr, samples * 2);
}

AT(.bt_voice.sco)
void bt_sco_rx_algo_proc(u8 position, u16 *ptr, u8 len)
{
    if ((call_cfg == NULL) || (position == 0)) {
        return;
    }
//    nr_cb_t *nr = &call_cfg->nr;
//
//    if(!bt_is_calling()){
//        u16 data_len = len/2;
//        u16 maxpow = dnr_voice_maxpow_calling((u32*)ptr,data_len);
//        if(maxpow > nr->calling_voice_pow){
//            nr->calling_voice_temp_cnt++;
//        }else if(nr->calling_voice_temp_cnt < nr->calling_voice_cnt){   //过滤一些突然很大的一包数据，需要连续联测5包数据
//            nr->calling_voice_temp_cnt = 0;
//        }
//        if(nr->calling_voice_temp_cnt < nr->calling_voice_cnt){
//            data_len = len*2;
//            memset((u8*)ptr,0x00,data_len);
//        }else{
//            nr->calling_voice_temp_cnt = nr->calling_voice_cnt;
//        }
//    }else{
//        nr->calling_voice_temp_cnt = 0;
//    }

    bt_sco_far_nr_process((s16*)ptr, len/2);
}

//------------------------------------------------------------------------------------------
AT(.bt_voice.agc.proc) WEAK
void bt_sco_agc_proc_do(s16 *ptr, int samples)
{
    int proc_cnt = (bt_sco_is_msbc()) ? 2 : 1;
    memcpy(agc_sco_buf, ptr, samples * 2);

    bt_sco_agc_proc((s16 *)agc_sco_buf);
    if(proc_cnt == 2) {
        bt_sco_agc_proc((s16 *)&agc_sco_buf[120]);
    }
    memcpy(ptr, agc_sco_buf, samples * 2);
}

///实测系统26M时, 每次EQ处理需要300us左右
AT(.bt_voice.mic)
u16 *bt_sco_tx_algo_proc(u16 *ptr, u16 samples)
{
    if (smic_test_mic_en || dmic_test_mic_ch) {     //mic产测流程不过算法
        return ptr;
    }
    if (!call_cfg) {
        return ptr;
    }
    if (call_cfg->mic_eq_en) {
        mic_eq_proc(ptr, samples);
    } else {    //走产测流程时，可能不过mic eq，此时硬件后置增益不起效，跑一个软件后置增益
        mic_post_gain_process_s((s16 *)ptr, call_cfg->post_gain, samples);
    }
	if (call_cfg->nr.agc_cb.agc_en) {
        bt_sco_agc_proc_do((s16 *)ptr, samples);
    }

    bt_sco_dump(DUMP_EQ_OUTPUT, ptr, samples * 2);
    return ptr;
}

AT(.bt_voice.bb.sco_tx)
void bt_sco_tx_process(u8 *ptr, u32 samples, int ch_mode)
{
    s16 *rptr = (s16 *)ptr;
    if (!bt_sco_is_msbc() && bt_sco_dnn_en()) {     //DNN单麦降噪走窄带通话时，需要把16k转为8k，DNN需要16k因此要转换
        for(int i = 0; i<(samples/2); ++i) {
            rptr[i] = rptr[2*i];
        }
        samples /= 2;
    }

    ptr = (u8 *)bt_sco_tx_algo_proc((u16 *)ptr, samples);         		//MIC EQ
    bt_sco_tx_process_do((u32*)ptr, samples);
}

#if BT_SCO_AGC_EN
void bt_sco_agc_init(u8 *sysclk, agc_cb_t *agc)
{
    agc->agc_en   = BT_SCO_AGC_EN;
    if (agc->agc_en) {
        memset(agc_sco_buf, 0, sizeof(agc_sco_buf));
    }

    if (bt_sco_is_msbc()) {
        agc->sampleHzIn = 16000;
    } else {
        agc->sampleHzIn = 8000;
    }

    agc->bit      		  = 16;
    agc->compress_agcDb   = 12;
    agc->target_agcDbfs   = 3;
    agc->low_signal_en    = 0;
	agc->max_gain         = 10;

    bt_agc_init();
}
#endif

void peri_nr_init(void)
{
#if WARNING_WSBC_EN
    if (warning_is_playing() || call_cfg == NULL) {
        return;
    }
#endif
    call_cfg_t *p = call_cfg;
    nr_cb_t *nr = &p->nr;

    bt_peri_nr_init();
    bt_sco_far_nr_int(nr->far_nr);
}

void bt_call_init(call_cfg_t *p)
{
    if (p == NULL) {
        return;
    }
    if (!bt_sco_is_msbc()){
        unlock_dcode();
    }
    call_cfg = p;
    aec_cfg_t *aec = &p->aec;

    load_code_bt_voice();

    //回声消除算法初始化
    if (aec->type == AEC_TYPE_DEFAULT) {
        aec_init(aec);
//    } else if (alc->alc_en) {
//        alc_init();
    }

#if BT_SCO_FAR_NR_EN
    peri_nr_init();
#endif

#if BT_SCO_AGC_EN
	///通话AGC算法初始化
	bt_sco_agc_init(NULL, &call_cfg->nr.agc_cb);
#endif

    //设置bt_aec_far_buf_cov取远端参考信号时的样点数
    cfg_bt_near_frame = 240;

    dac_aubuf_set_size(2);

    ///算法数据dump初始化
#if BT_SCO_DUMP_EN || BT_AEC_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_EQ_DUMP_EN
    if (!xcfg_cb.huart_en) {
        printf("dump huart xcfg init err!\n");
        while(1);
    }
    memset(bt_sco_dump_buf, 0, sizeof(bt_sco_dump_buf));
    sco_huart_init();
#endif

    bt_sco_pcm_buf_init();
    ring_fade_flag = 0;
    call_setup_tick = 0;

}

void bt_call_exit(void)
{
    aec_cfg_t *aec = &call_cfg->aec;
//    alc_cb_t *alc = &call_cfg->alc;

    if (aec->type == AEC_TYPE_DEFAULT) {
        aec_exit();
//    } else if(alc->alc_en) {
//        alc_exit();
    }

    bt_peri_nr_exit();
    call_cfg = NULL;
    unlock_code_bt_voice();
}
