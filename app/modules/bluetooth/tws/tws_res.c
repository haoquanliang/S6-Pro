#include "include.h"
#include "tws_res.h"


#if BT_TWS_EN


#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


//add res_list
//send alarm
//wait alarm
//audio bypass
//res init
//wait kick
//loop, wait end
//res exit
//if no more res, goto audio enable
//wait done
//send next alarm
//wait next alarm
//goto res init
//audio enable
//finish


static uint8_t res_lable_cnt = 0;
res_list_t res_list;
tws_res_t tws_res;



bool mp3_res_play_kick(u32 addr, u32 len, bool kick);
void wav_sync_kick(void);
bool wav_res_play_kick(u32 addr, u32 len, bool kick);
void wav_res_dec_process(void);
bool wav_res_is_play(void);
bool wav_res_stop(void);
void mp3_res_play_exit(void);

void tws_res_play_init_cb(void);
void tws_res_play_exit_cb(void);


//----------------------------------------------------------------------------
ALIGNED(128) NO_INLINE
static bool res_list_add(uint8_t res_idx, uint8_t res_lable, uint8_t res_flag)
{
    GLOBAL_INT_DISABLE();
    if(res_list.len < res_list.total) {
        if(res_list.wpos >= res_list.total) {
            res_list.wpos = 0;
        }
        res_list.item[res_list.wpos].res_idx = res_idx;
        res_list.item[res_list.wpos].res_lable = res_lable;
        res_list.item[res_list.wpos].res_flag = res_flag;
        res_list.wpos++;
        res_list.len++;
        GLOBAL_INT_RESTORE();
        return true;
    }
    GLOBAL_INT_RESTORE();

    return false;
}

ALIGNED(128) NO_INLINE
static bool res_list_peek(res_item_t *item)
{
    bool ret = false;

    GLOBAL_INT_DISABLE();
    if(res_list.len > 0) {
        if(res_list.rpos >= res_list.total) {
            res_list.rpos = 0;
        }
        *item = res_list.item[res_list.rpos];
        ret = true;
    }
    GLOBAL_INT_RESTORE();

    return ret;
}

ALIGNED(128) NO_INLINE
static bool res_list_get(res_item_t *item)
{
    bool ret = false;

    GLOBAL_INT_DISABLE();
    if(res_list.len > 0) {
        if(res_list.rpos >= res_list.total) {
            res_list.rpos = 0;
        }
        *item = res_list.item[res_list.rpos];

        res_list.len--;
        res_list.rpos++;

        ret = true;
    }
    GLOBAL_INT_RESTORE();

    return ret;
}

ALIGNED(128) NO_INLINE
static bool res_list_search(uint8_t res_idx)
{
    bool ret = false;

    GLOBAL_INT_DISABLE();
    uint8_t rpos = res_list.rpos;
    uint8_t len = res_list.len;

    while(len--) {
        if(rpos >= res_list.total) {
            rpos = 0;
        }
        if(res_list.item[rpos].res_idx == res_idx) {
            ret = true;
            break;
        }
        rpos++;
    }
    GLOBAL_INT_RESTORE();

    return ret;
}

ALIGNED(64) NO_INLINE
static void res_list_try_add(void)
{
    GLOBAL_INT_DISABLE();
    res_list.add_len ++;
    GLOBAL_INT_RESTORE();
}

//----------------------------------------------------------------------------
static void tws_res_add_do(uint16_t param)
{
    uint8_t res_idx = (uint8_t)param;
    uint8_t res_lable = (uint8_t)(param>>8) & 0xf;
    uint8_t res_flag = (uint8_t)(param>>12) & 0xf;

    TRACE("res_add(%c): %d(%d)\n", bt_tws_is_slave()? 'S': 'M', res_idx, res_lable);

    if(res_idx == TWS_RES_CALL_HANGUP) {
        tws_res.break_ring |= BIT(0);
    } else if(res_idx == TWS_RES_CALL_REJECT) {
        tws_res.break_ring |= BIT(1);
    }

    res_list_add(res_idx, res_lable, res_flag);
}

static void tws_res_done_do(uint16_t param)
{
    uint8_t res_lable = (uint8_t)(param>>8);

    if(tws_res.res_lable == res_lable) {
        tws_res.done_flag = true;
    }

#if TRACE_EN
    uint8_t res_idx = (uint8_t)param;
    TRACE("res_done: %d(%d--%d)\n", res_idx, res_lable, tws_res.res_lable);
#endif
}

ALIGNED(128) NO_INLINE
static void tws_res_alarm_start(res_item_t *item, uint8_t role)
{
    GLOBAL_INT_DISABLE();
    tws_res.res_idx = item->res_idx;
    tws_res.res_lable = item->res_lable;
    tws_res.res_flag  = item->res_flag;
    tws_res.kick_flag = false;
    tws_res.done_flag = (role == 1)? true : false;
    tws_res.play_kick_cb = NULL;
    GLOBAL_INT_RESTORE();

    tws_res.play_err = 0;
    tws_res.res_type = tws_res_get_type(item->res_idx);

    if(role == 0) {     //master send request to slave
        bt_tws_req_alarm_res((item->res_lable<<8) | item->res_idx);
    }

    TRACE("res_start: %d(%d) -- role=%d\n", tws_res.res_idx, tws_res.res_lable, role);
}

static void tws_res_play_prepare(void)
{
    uint8_t res_type = tws_res.res_type;

    if(res_type == RES_TYPE_MP3) {
        dac_src_fade_out(0, 10);
        bt_audio_bypass();
    }

#if SYS_KARAOK_EN
    bsp_karaok_exit();
#endif

    tws_res.dac_sta = dac_get_pwr_sta();
    func_bt_set_dac(1);

    tws_res.mute_bak = sys_cb.mute;
    if (sys_cb.mute) {
        bsp_loudspeaker_unmute();
    }
}

static void tws_res_play_init(void)
{
    uint8_t res_idx  = tws_res.res_idx;
    uint8_t res_type = tws_res.res_type;
    uint32_t addr, len;

    if(tws_res.alarm_lable != tws_res.res_lable) {  //暂时先规避lable不一致时，没有kick的问题
        tws_res.kick_flag = true;
        TRACE("res_alarm: lable miss ###\n");
    }

    tws_res_get_addr(res_idx, &addr, &len);
    if(res_type == RES_TYPE_MP3 || res_type == RES_TYPE_WAV || res_type == RES_TYPE_WSBC) {
        if (addr == 0 || len == 0) {
            tws_res.play_err |= PLAY_ERR_INVALID;
            return;
        }
    } else if(res_type == RES_TYPE_PIANO || res_type == RES_TYPE_TONE) {
        if (addr == 0 && len == 0) {
            tws_res.play_err |= PLAY_ERR_INVALID;
            return;
        }
    }

    if(res_type == RES_TYPE_MP3) {
#if WARNING_MP3_EN
        if(get_music_dec_sta() != MUSIC_STOP) { //避免来电响铃/报号未完成，影响get_music_dec_sta()状态
            music_control(MUSIC_MSG_STOP);
        }
#if BT_MUSIC_EFFECT_EN
        music_effect_alg_suspend(MUSIC_EFFECT_SUSPEND_FOR_RES);
#endif // BT_MUSIC_EFFECT_EN
#if APP_MP3_BYPASS_EQ
         ab_mate_bypass_eq();
#endif


        bsp_change_volume(WARNING_VOLUME);

        mp3_res_play_kick(addr, len, tws_res.kick_flag);
        tws_res.play_kick_cb = music_sync_kick;
#endif
#if WARNING_WSBC_EN
    } else if(res_type == RES_TYPE_WSBC) {
        sys_clk_req(INDEX_RES_PLAY, SYS_160M);
        tws_res.play_kick_cb = warning_play_kick;
        if(warning_play_init((u8 *)addr, len, tws_res.kick_flag) != 0) {
            tws_res.play_err |= PLAY_ERR_FORMAT;
        }
#endif
#if WARNING_WAV_EN
    } else if(res_type == RES_TYPE_WAV) {
#if  DAC_DNC_EN
        if (res_idx == TWS_RES_ANC ||               //anc生效时，在提示音播放前关闭dnc
            res_idx == TWS_RES_TRANSPARENCY) {
                dac_dnc_stop();
            }
#endif
#if TINY_TRANSPARENCY_EN
        sys_cb.ttp_sta = sys_cb.ttp_start;          //ttp使用dac1，等wav提示音播完，再重新start
        bsp_ttp_stop();
#endif
#if ABP_EN && !ABP_PLAY_DIS_WAV_EN
        if (abp_is_playing()) {
        #if BT_TWS_EN
            abp_stop_fade_out();
        #endif
            abp_stop();
            dac1_aubuf_clr();
        }
#endif
        sys_clk_req(INDEX_RES_PLAY, SYS_120M);

        tws_res.play_kick_cb = wav_sync_kick;
        if(!wav_res_play_kick(addr, len, tws_res.kick_flag)) {
            tws_res.play_err |= PLAY_ERR_FORMAT;
        }
#endif
#if WARNING_PIANO_EN
    } else if(res_type == RES_TYPE_TONE || res_type == RES_TYPE_PIANO) {
        int type = (res_type == RES_TYPE_TONE)? WARNING_TONE : WARNING_PIANO;
        tws_res.play_kick_cb = tone_sync_kick;
        tone_play_kick(type, (void *)addr, tws_res.kick_flag);
#endif
    }
}

static void tws_res_play_exit(void)
{
    uint8_t res_type = tws_res.res_type;

    if(tws_res.res_idx == TWS_RES_CALL_HANGUP) {
        tws_res.break_ring &= ~BIT(0);
    } else if(tws_res.res_idx == TWS_RES_CALL_REJECT) {
        tws_res.break_ring &= ~BIT(1);
    }
    if(tws_res.play_err & PLAY_ERR_INVALID) {
        return;
    }

    if(res_type == RES_TYPE_MP3) {
        music_control(MUSIC_MSG_STOP);
        mp3_res_play_exit();
#if APP_MP3_BYPASS_EQ
        if(sys_cb.incall_flag)
        {
            //MP3播完后，SCO算法初始化的时候会设置EQ
        }
        else
        {
            app_eq_set(); 
        }       
#endif          
#if BT_MUSIC_EFFECT_EN
        music_effect_alg_reinit();
        music_effect_alg_restart();
#endif // BT_MUSIC_EFFECT_EN
        bsp_change_volume(sys_cb.vol);
#if WARNING_WSBC_EN
    } else if(res_type == RES_TYPE_WSBC) {
        warning_play_exit();
        sys_clk_free(INDEX_RES_PLAY);
#endif
#if WARNING_WAV_EN
    } else if(res_type == RES_TYPE_WAV) {
        wav_res_stop();

        sys_clk_free(INDEX_RES_PLAY);
#if TINY_TRANSPARENCY_EN
        if (sys_cb.ttp_sta) {
            bsp_ttp_start();
        }
#endif
#if ABP_EN && !ABP_PLAY_DIS_WAV_EN
        if (sys_cb.abp_mode) {
            bsp_abp_set_mode(sys_cb.abp_mode);
        }
#endif
#endif
#if WARNING_PIANO_EN
    } else if(res_type == RES_TYPE_TONE || res_type == RES_TYPE_PIANO) {
        tone_play_end();
#endif
    }
}

static void tws_res_play_end(void)
{
    uint8_t res_type = tws_res.res_type;

    sys_cb.mute = tws_res.mute_bak;
    if (sys_cb.mute) {
        bsp_loudspeaker_mute();
    }
    func_bt_set_dac(tws_res.dac_sta);

    if(res_type == RES_TYPE_MP3) {
        bt_audio_enable();
    }
#if SYS_KARAOK_EN
    bsp_karaok_init();
#endif
}

static bool tws_res_check_break(void)
{
#if WARNING_BREAK_EN
    if( tws_res.break_ring && (tws_res.res_idx <= TWS_RES_RING && tws_res.res_idx >= TWS_RES_NUM_0) ) {
        tws_res.play_err |= PLAY_ERR_BREAK;
        return true;
    }
    if(bt_tws_is_slave()) {
        if((tws_res.res_flag&RES_FLAG_TWS) == 0 && tws_res.break_flag) {
            tws_res.play_err |= PLAY_ERR_BREAK;
            return true;
        }
    }
#endif
    return false;
}

static bool tws_res_play_proc(void)
{
    uint8_t res_type = tws_res.res_type;

    if(tws_res.play_err & PLAY_ERR_INVALID) {
        return true;
    }

    if(tws_res_check_break()) {
        return true;
    }

    if(res_type == RES_TYPE_MP3) {
#if WARNING_MP3_EN
        if(get_music_dec_sta() == MUSIC_STOP) {
            return true;
        }
#endif
#if WARNING_WSBC_EN
    } else if(res_type == RES_TYPE_WSBC) {
        if(warning_play_proc() != 0) {
            return true;
        }
#endif
#if WARNING_WAV_EN
    } else if(res_type == RES_TYPE_WAV) {
        if(!wav_res_is_play()) {
            return true;
        }

        wav_res_dec_process();
#endif
#if WARNING_PIANO_EN
    } else if(res_type == RES_TYPE_TONE || res_type == RES_TYPE_PIANO) {
	     if (!tone_is_playing()) {
            return true;
	     }
#endif
    } else {
        return true;
    }

    return false;
}

AT(.com_text.tws.res)
void tws_res_play_kick(u32 tickn)
{
    if(tws_res.play_tickn != -1L && tws_res.alarm_lable == tws_res.res_lable) {
        if(TICK_GEQ(tickn, tws_res.play_tickn)) {
            tws_res.play_tickn = -1L;
            tws_res.kick_flag = true;
            GLOBAL_INT_DISABLE();
			if (res_list.add_len) {
            	res_list.add_len--;
			}
            GLOBAL_INT_RESTORE();
            if(tws_res.play_kick_cb != NULL) {
                tws_res.play_kick_cb();
            }
        }
    }
}

static bool tws_res_list_is_empty(void)
{
    res_item_t item;

    if(res_list_peek(&item)) {
        if(tws_res_get_type(item.res_idx) == tws_res.res_type) {
            return false;
        }
    }

    return true;
}

static bool tws_res_is_busy(void)
{
    if(tws_res.state != RES_STA_IDLE) {
        return (bool)(tws_res.state <= RES_STA_PROC);
    }
    return false;
}

static bool tws_res_slave_w4_start(void)
{
    res_item_t item;
    uint retry = ITEM_MAX_NB + 4;
    uint8_t req_lable = tws_res.req_lable;
    uint8_t req_idx = tws_res.req_idx;

    while(retry--) {
        if(tws_res.rd_lable == req_lable) {
            break;
        }
        if(!res_list_peek(&item)) {
            break;
        }

        uint diff = req_lable - item.res_lable;
        if(diff == 0) {
            if(req_idx != item.res_idx) {
                res_list_get(&item);        //discard
                TRACE("res_miss: %d(%d), %d(%d) ####\n", item.res_idx, item.res_lable, req_idx, req_lable);
            } else {
                tws_res.rd_lable = req_lable;
                return true;
            }
        } else if(diff < ITEM_MAX_NB/2) {
            res_list_get(&item);            //discard
            TRACE("res_miss: %d(%d), %d(%d) ####\n", item.res_idx, item.res_lable, req_idx, req_lable);
        }
    }

    return false;
}

void tws_res_process_do(void)
{
    uint16_t delay = 200;
    uint8_t  role = 0;
    res_item_t item;

    if(!tws_res.enable) {
        if(tws_res.state >= RES_STA_DELAY && tws_res.state <= RES_STA_NEXT_RES_INIT) {
            tws_res.state = RES_STA_RES_EXIT;
        }
    }

    switch(tws_res.state) {
    case RES_STA_IDLE:
        if(!tws_res.enable) {
            break;
        }
        if(bt_tws_is_slave()) {
            if(!tws_res_slave_w4_start()) {
                break;
            }
            role = 1;
        }
        if(res_list_get(&item)) {
            tws_res_alarm_start(&item, role);
            tws_res.tickn = TICK_ADD(tws_time_tickn_get(), 500);
            tws_res.state = RES_STA_WAIT_ALARM;
        }
        break;

    case RES_STA_WAIT_ALARM:
        if(tws_res.alarm_lable == tws_res.res_lable || tws_time_tickn_expire(tws_res.tickn)) {
            tws_res_play_prepare();
            tws_res.state = RES_STA_AUDIO_BYPASS;
        }
        break;

    case RES_STA_AUDIO_BYPASS:
        tws_res_play_init();
        tws_res_play_init_cb();
        tws_res.state = RES_STA_WAIT_KICK;
        tws_res.tickn = TICK_ADD(tws_time_tickn_get(), 500);
        break;

    case RES_STA_WAIT_KICK:
        if(tws_res.kick_flag || tws_time_tickn_expire(tws_res.tickn)) {
            tws_res.state = RES_STA_PROC;
        }
        break;

    case RES_STA_PROC:
        if(tws_res_play_proc()) {
            tws_res_play_exit();
            tws_res_play_exit_cb();
            if(bt_tws_is_slave()) {
                bt_tws_res_done(tws_res.res_idx | (tws_res.res_lable<<8));
            }
            tws_res.state = RES_STA_RES_EXIT;
        }
        break;

    case RES_STA_RES_EXIT:
        if(!tws_res.enable || tws_res_list_is_empty()) {         //no more res
            tws_res_play_end();
            tws_res.state = RES_STA_IDLE;
            break;
        }

        if(bt_tws_is_connected() && !bt_tws_is_slave()) {
            delay = 500;
        }
        tws_res.tickn = TICK_ADD(tws_time_tickn_get(), delay);
        tws_res.state = RES_STA_DELAY;
        //no break;

    case RES_STA_DELAY:
        if(!bt_tws_is_connected()) {
            tws_res.done_flag = true;
        }

        tws_res_check_break();
        if(tws_res.play_err != 0) {
            tws_res.state = RES_STA_NEXT_RES_INIT;
            break;
        }
        if(tws_time_tickn_expire(tws_res.tickn)) {
            if(!tws_res.done_flag) {
                tws_res.tickn = TICK_ADD(tws_time_tickn_get(), 300);
                tws_res.state = RES_STA_DELAY_MORE;
                break;
            }
            tws_res.state = RES_STA_NEXT_RES_INIT;
        }
        break;

    case RES_STA_DELAY_MORE:
        tws_res_check_break();
        if(tws_res.done_flag || tws_res.play_err || tws_time_tickn_expire(tws_res.tickn)) {
            tws_res.state = RES_STA_NEXT_RES_INIT;
        }
        break;

    case RES_STA_NEXT_RES_INIT:
        if(bt_tws_is_slave()) {
            if(!tws_res_slave_w4_start()) {
                break;
            }
            role = 1;
        }

        if(res_list_get(&item)) {
            u8 next_res_type = tws_res_get_type(item.res_idx);
            if(next_res_type == RES_TYPE_MP3 && tws_res.res_type != RES_TYPE_MP3) {        //下一个是mp3
                bt_audio_bypass();
            } else if(next_res_type != RES_TYPE_MP3 && tws_res.res_type == RES_TYPE_MP3) { //下一个是wav
                bt_audio_enable();
            }
#if APP_MP3_BYPASS_EQ
            if(next_res_type == RES_TYPE_MP3)
            {
                //音乐的EQ会影响mp3格式的语音提示，在播放前关掉，播完后恢复正常
                //注意：各种音效也会调整EQ
                ab_mate_bypass_eq();
            }
#endif

            tws_res_alarm_start(&item, role);
            tws_res.tickn = TICK_ADD(tws_time_tickn_get(), 500);
            tws_res.state = RES_STA_NEXT_WAIT_ALARM;


        } else {
            tws_res.state = RES_STA_RES_EXIT;
        }
        break;

    case RES_STA_NEXT_WAIT_ALARM:
        if(tws_res.alarm_lable == tws_res.res_lable || tws_time_tickn_expire(tws_res.tickn)) {
            tws_res.state = RES_STA_AUDIO_BYPASS;
        }
        break;
    }
}

AT(.com_text.tws.res)
void tws_res_process(void)
{
#if TRACE_EN
    static uint8_t state = -1;
    if(state != tws_res.state) {
        state = tws_res.state;
        TRACE("res_state: %d\n", tws_res.state);
    }
#endif

    if(tws_res.state == RES_STA_IDLE && res_list_is_empty()) {
        return;
    }

   tws_res_process_do();
}

void tws_res_cleanup(void)
{
    res_item_t item;
    uint retry = ITEM_MAX_NB + 4;

    while(retry--) {
        if(!res_list_get(&item)) {
            break;
        }
    }
}

bool tws_res_search(uint8_t res_idx)
{
    if(tws_res_is_busy() && tws_res.res_idx == res_idx) {
        return true;
    } else {
        return res_list_search(res_idx);
    }
}

uint8_t tws_res_add(uint8_t res_idx, uint8_t flag)
{
    u32 addr, len;
    uint8_t res_type = tws_res_get_type(res_idx);

    if (res_type == RES_TYPE_INVALID) {
        return RES_ERR_INVALID;
    } else {
        if(tws_res.break_ring && (tws_res.res_idx <= TWS_RES_RING && tws_res.res_idx >= TWS_RES_NUM_0)) {
            return RES_ERR_NO_ERR;
        }

        tws_res_get_addr(res_idx, &addr, &len);
        if(res_type == RES_TYPE_MP3 || res_type == RES_TYPE_WAV || res_type == RES_TYPE_WSBC) {
            if (addr == 0 || len == 0) {
                return RES_ERR_INVALID;
            }
        } else if(res_type == RES_TYPE_PIANO || res_type == RES_TYPE_TONE) {
            if (addr == 0 && len == 0) {
                return RES_ERR_INVALID;
            }
        }
#if ABP_EN && ABP_PLAY_DIS_WAV_EN
        if (abp_is_playing() && (res_type == RES_TYPE_PIANO || res_type == RES_TYPE_TONE || res_type == RES_TYPE_WAV)) {
            return RES_ERR_INVALID;
        }
#endif
    }

    if(res_idx == TWS_RES_MAX_VOL) {
        if(tws_res_search(res_idx)) {
            return RES_ERR_NO_ERR;
        }
    }

    if(res_list_is_full() || (res_list.add_len >= ITEM_MAX_NB)) {
        TRACE("res_full: %d(%d) ####\n", res_idx);
        return RES_ERR_LIST_FULL;
    }
	res_list_try_add();
    res_lable_cnt++;
    res_lable_cnt &= 0xf;

    TRACE("==>res_play: %d(%d)\n", res_idx, res_lable_cnt);
    bt_tws_res_add(res_idx | (res_lable_cnt<<8) | ((flag&0xf)<<12));
    return RES_ERR_NO_ERR;
}

void tws_res_var_init(void)
{
    memset(&tws_res, 0x00, sizeof(tws_res));
    memset(&res_list, 0x00, sizeof(res_list));
    res_list.total = ITEM_MAX_NB;
    tws_res.rd_lable = -1;
    tws_res.req_lable = -1;
    tws_res.alarm_lable = -1;
    tws_res.play_tickn = -1L;
    tws_res.enable = true;
}

//----------------------------------------------------------------------------
//callback
void tws_local_res_add_cb(uint16_t param)
{
    tws_res_add_do(param);
}

void tws_remote_res_add_cb(uint16_t param)
{
    tws_res_add_do(param | BIT(12));
}

void tws_remote_res_done_cb(uint16_t param)
{
    tws_res_done_do(param);
}

//副耳发起提示音请求，由主耳控制同步播报
void tws_slave_res_add_req_cb(uint16_t param)
{
    uint8_t res_idx = (uint8_t)param;

    if(!bt_tws_is_slave()) {
        switch(res_idx) {
        case TWS_RES_MAX_VOL:
        case TWS_RES_LOW_BATTERY:
        case TWS_RES_LANGUAGE_EN:
        case TWS_RES_LANGUAGE_ZH:
        case TWS_RES_CALL_HANGUP:
        case TWS_RES_CALL_REJECT:
        case TWS_RES_REDIALING:
        case TWS_RES_MUSIC_MODE:
        case TWS_RES_GAME_MODE:
        case TWS_RES_NR_DISABLE:
        case TWS_RES_ANC:
        case TWS_RES_TRANSPARENCY:
#if ASR_EN
        case TWS_RES_ASR_ON:
        case TWS_RES_ASR_OFF:
#endif
            bsp_res_play(res_idx);
            break;
        }
    }
}

//同步提示音播报前，发控制消息到手机
void tws_res_play_init_cb(void)
{
    uint8_t res_idx = tws_res.res_idx;

    if(!bt_tws_is_slave()) {
        if(res_idx == TWS_RES_CALL_HANGUP) {
            bt_call_answer_incoming();  //接听
        } else if(res_idx == TWS_RES_CALL_REJECT) {
            bt_call_terminate();        //拒接
        } else if(res_idx == TWS_RES_REDIALING) {
            bt_call_redial_last_number(); //回拨电话
        }
    }
}

//同步提示音播报完毕，设置状态
void tws_res_play_exit_cb(void)
{
    uint8_t res_idx = tws_res.res_idx;

    bsp_res_play_exit_cb(res_idx);
}
#endif // BT_TWS_EN

////循环播放提示音demo
////cir_kick: true,播放的mp3\wav提示音会循环播放;false,提示音播放一遍就结束;
//bool cir_kick = false;
//u16 res_start_offset = 512*2; //循环播放起点
//u16 res_end_offset = 512*2;   //循环播放终点
//bool res_msc_cycle_hook(void)
//{
//    return cir_kick;
//}

