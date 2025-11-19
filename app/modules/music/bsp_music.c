#include "include.h"

#define FS_CRC_SEED         0xffff

uint calc_crc(void *buf, uint len, uint seed);
bool mp3_res_play_kick(u32 addr, u32 len, bool kick);
bool wav_res_play_kick(u32 addr, u32 len, bool kick);
void wav_res_dec_process(void);
bool wav_res_is_play(void);
bool wav_res_stop(void);
void mp3_res_play_exit(void);

#if FUNC_MUSIC_EN

#if BT_BACKSTAGE_EN
const u32 *res_addr[] = {
#if WARNING_FUNC_MUSIC
    &RES_BUF_MUSIC_MODE_MP3,
#endif
#if WARNING_USB_SD
    &RES_BUF_SDCARD_MODE_MP3,
    &RES_BUF_USB_MODE_MP3,
#endif
#if WARNING_FUNC_BT
    &RES_BUF_BT_MODE_MP3,
#endif
#if WARNING_FUNC_FMRX
    &RES_BUF_FM_MODE_MP3,
#endif
#if WARNING_FUNC_AUX
    &RES_BUF_AUX_MODE_MP3,
#endif
};
#endif

//扫描全盘文件
bool pf_scan_music(u8 new_dev)
{
    if (new_dev) {
#if USB_SD_UPDATE_EN
        func_update();                                  //尝试升级
#endif // USB_SD_UPDATE_EN
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = BIT(0);
    sys_cb.rec_num = 0;
    sys_cb.ftime = 0;
#endif // REC_FAST_PLAY

    f_msc.file_total = fs_get_total_files();
    if (!f_msc.file_total) {
        f_msc.dir_total = 0;
        return false;
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = 0;
#endif // REC_FAST_PLAY

    if (new_dev) {
#if MUSIC_PLAYDEV_BOX_EN
        gui_box_show_playdev();
#endif // MUSIC_PLAYDEV_BOX_EN

#if WARNING_USB_SD
        if (sys_cb.cur_dev <= DEV_SDCARD1) {
            mp3_res_play(RES_BUF_SDCARD_MODE_MP3, RES_LEN_SDCARD_MODE_MP3);
        } else {
            mp3_res_play(RES_BUF_USB_MODE_MP3, RES_LEN_USB_MODE_MP3);
        }
#endif // WARNING_USB_SD
    }

    f_msc.dir_total = fs_get_dirs_count();          //获取文件夹总数
    return true;
}

#if MUSIC_PLAYMODE_NUM
//music播放模式切换
void music_playmode_next(void)
{
    sys_cb.play_mode++;
    if (sys_cb.play_mode >= MUSIC_PLAYMODE_NUM) {
        sys_cb.play_mode = NORMAL_MODE;
    }
    gui_box_show_playmode();
    plugin_playmode_warning();
}
#endif // MUSIC_PLAYMODE_NUM

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_clr(void)
{
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    param_msc_breakpoint_write();
    param_sync();
    //printf("%s\n", __func__);
}

void bsp_music_breakpoint_init(void)
{
    int clr_flag = 0;
    if (f_msc.brkpt_flag) {
        f_msc.brkpt_flag = 0;
        param_msc_breakpoint_read();
        //printf("restore: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
        if (calc_crc(f_msc.fname, 8, FS_CRC_SEED) == f_msc.brkpt.fname_crc) {
            music_set_jump(&f_msc.brkpt);
        } else {
            clr_flag = 1;
        }
    }
    f_msc.brkpt.fname_crc = calc_crc(f_msc.fname, 8, FS_CRC_SEED);
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    if (clr_flag) {
        param_msc_breakpoint_write();
    }
}

void bsp_music_breakpoint_save(void)
{
    music_get_breakpiont(&f_msc.brkpt);
    param_msc_breakpoint_write();
    param_sync();
    //printf("save: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
}
#endif // MUSIC_BREAKPOINT_EN
#endif // FUNC_MUSIC_EN


#if APP_MP3_BYPASS_EQ
void ab_mate_bypass_eq(void)
{  
    for(u8 i=0; i< ab_mate_app.eq_info.band_cnt; i++){
        WDT_CLR();
        music_set_eq_for_index(i,0);
    }
    music_set_eq_overall_gain(0);

    while (!tick_check_expire(ab_mate_app.tick, 30)) {     //连续两次EQ设置相隔30ms，连续短时间设置容易失败
        delay_5ms(1);
    }
    
#if EQ_APP_EN
    eq_msc_index_init(0);
#endif
      
    music_set_eq_by_res(RES_BUF_EQ_FLAT_EQ, RES_LEN_EQ_FLAT_EQ);
    ab_mate_app.tick = tick_get();   
    printf("bypass eq\n");
}
#endif


#if WARNING_MP3_EN
void mp3_res_play(u32 addr, u32 len)
{
#if QTEST_EN
    if(qtest_get_mode()){
        return;
    }
#endif

    u16 msg;
    u8 mute_bak;
//    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }

#if BT_BACKSTAGE_EN
    if (func_cb.sta_break != FUNC_NULL) {
        for (u32 i = 0; i < sizeof(res_addr) / sizeof(u32); i++) {
            if (*res_addr[i] == addr) {
                if (func_cb.sta != FUNC_BT) {
                    func_cb.sta_break = FUNC_NULL;
                }
                return;
            }
        }
    }
#endif
    u8 dac_sta = dac_get_pwr_sta();
    func_bt_set_dac(1);

#if SYS_KARAOK_EN
    u8 voice_bak = 0, func_sta = func_cb.sta;
    if (karaok_get_voice_rm_sta()) {
        voice_bak = 1;
        karaok_voice_rm_disable();
    }
    bsp_karaok_exit();
#endif

    mute_bak = sys_cb.mute;
    if (sys_cb.mute) {
        bsp_loudspeaker_unmute();
    }
    if(get_music_dec_sta() != MUSIC_STOP) { //避免来电响铃/报号未完成，影响get_music_dec_sta()状态
        music_control(MUSIC_MSG_STOP);
    }
#if BT_MUSIC_EFFECT_EN
    music_effect_alg_suspend(MUSIC_EFFECT_SUSPEND_FOR_RES);
#endif // BT_MUSIC_EFFECT_EN
    bsp_change_volume(WARNING_VOLUME);
#if APP_MP3_BYPASS_EQ
    //音乐的EQ会影响mp3格式的语音提示，在播放前关掉，播完后恢复正常
    //注意：各种音效也会调整EQ 
    ab_mate_bypass_eq();
#endif


    mp3_res_play_kick(addr, len, true);

    while (get_music_dec_sta() != MUSIC_STOP) {
        bt_thread_check_trigger();
        sys_run_loop();
        WDT_CLR();
        msg = msg_dequeue();
        if (sys_cb.voice_evt_brk_en) {
            if (((msg == EVT_SD_INSERT) || (msg == EVT_UDISK_INSERT)) && (func_cb.sta != FUNC_MUSIC)) {
                func_message(msg);
                break;
            }
#if BT_HFP_INBAND_RING_EN
        //    if (ring_mp3_msg_work()) {   //mp3本地铃声事件处理
        //        func_message(msg);
        //        break;
        //    }
#endif

        }
#if LINEIN_DETECT_EN
        if ((msg == EVT_LINEIN_INSERT) && ((sys_cb.voice_evt_brk_en) || (LINEIN_2_PWRDOWN_EN))) {
            func_message(msg);
            break;
        }
#endif // LINEIN_DETECT_EN
        if (msg != NO_MSG) {
            msg_enqueue(msg);       //还原未处理的消息
        }
#if (CHARGE_BOX_TYPE == CBOX_SSW)
        if(bt_get_status() >= BT_STA_IDLE) {
            charge_box_ssw_process(0);
        }
#endif
#if BT_TWS_EN && WARNING_BREAK_EN
        if(sys_cb.tws_res_brk) {
            sys_cb.tws_res_brk = 0;
            break;
        }
#endif
    }
    music_control(MUSIC_MSG_STOP);
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
    mp3_res_play_exit();
    sys_cb.mute = mute_bak;
    if (sys_cb.mute) {
        bsp_loudspeaker_mute();
    }
    func_bt_set_dac(dac_sta);

#if SYS_KARAOK_EN
    if (voice_bak) {
        karaok_voice_rm_enable();
    }
    bsp_karaok_init();
#endif
}
#else
void mp3_res_play(u32 addr, u32 len) {}
#endif

#if WARNING_SYSVOL_ADJ_EN
void bsp_res_sysvol_adjust(void)
{
    if (sys_cb.vol == VOL_MAX) {
#if BT_MUSIC_EFFECT_DYEQ_EN
        if (music_effect_get_state_real(MUSIC_EFFECT_DYEQ)) {
            music_dyeq_audio_set_vol_by_dvol(WARNING_SYSVOL_ADJ_LEVEL);
            return;
        }
#endif // BT_MUSIC_EFFECT_DYEQ_EN
#if BT_MUSIC_EFFECT_XDRC_EN
        if (music_effect_get_state_real(MUSIC_EFFECT_XDRC)) {
            music_xdrc_audio_set_vol_by_dvol(WARNING_SYSVOL_ADJ_LEVEL);
            return;
        }
#endif // BT_MUSIC_EFFECT_XDRC_EN
        dac_vol_set(WARNING_SYSVOL_ADJ_LEVEL);
    }
}

void bsp_res_sysvol_resume(void)
{
    bsp_change_volume(sys_cb.vol);
}
#endif

#if WARNING_WAV_EN
void wav_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if ABP_EN
    if (abp_is_playing()) {
#if ABP_PLAY_DIS_WAV_EN
        return;
#else
    #if BT_TWS_EN
        abp_stop_fade_out();
    #endif
        abp_stop();
        dac1_aubuf_clr();
#endif // ABP_PLAY_DIS_WAV_EN
    }
#endif // ABP_EN
#if APP_FIND_TONE_MAX 
    if (addr == RES_BUF_FIND_EAR_WAV)
    {
        sys_cb.flag_playing_find_ear = true;
    }
#endif 
    u8 dac_sta = dac_get_pwr_sta();
    func_bt_set_dac(1);

#if TINY_TRANSPARENCY_EN
    sys_cb.ttp_sta = sys_cb.ttp_start;
    bsp_ttp_stop();
#endif

#if WARNING_SYSVOL_ADJ_EN
    bsp_res_sysvol_adjust();
#endif

    sys_clk_req(INDEX_RES_PLAY, SYS_120M);

    wav_res_play_kick(addr, len, 1);
    while (wav_res_is_play()) {
        bt_thread_check_trigger();
        wav_res_dec_process();
        WDT_CLR();
#if BT_TWS_EN && WARNING_BREAK_EN
        if(sys_cb.tws_res_brk) {
            sys_cb.tws_res_brk = 0;
            break;
        }
#endif
    }

    wav_res_stop();
    sys_clk_free(INDEX_RES_PLAY);

#if WARNING_SYSVOL_ADJ_EN
    bsp_res_sysvol_resume();
#endif

#if ABP_EN && !ABP_PLAY_DIS_WAV_EN
    if (sys_cb.abp_mode) {
        abp_start(sys_cb.abp_mode - 1);
    #if BT_TWS_EN
        abp_start_fade_in();
    #endif
    }
#endif

    func_bt_set_dac(dac_sta);
#if APP_FIND_TONE_MAX 
    if (addr == RES_BUF_FIND_EAR_WAV)
    {
        sys_cb.flag_playing_find_ear = false;
    }
#endif
#if TINY_TRANSPARENCY_EN
    if (sys_cb.ttp_sta) {
        bsp_ttp_start();
    }
#endif
}
#endif

void piano_res_play(u8 type, void *res)
{
#if ABP_EN
    if (abp_is_playing()) {
#if ABP_PLAY_DIS_WAV_EN
        return;
#else
        abp_stop();
        dac1_aubuf_clr();
#endif // ABP_PLAY_DIS_WAV_EN
    }
#endif // ABP_EN

    u8 dac_sta = dac_get_pwr_sta();
    func_bt_set_dac(1);

#if WARNING_SYSVOL_ADJ_EN
    bsp_res_sysvol_adjust();
#endif

    tone_play_kick(type, res, true);
    while (tone_is_playing()) {
        bt_thread_check_trigger();
        piano_play_process();
    }
    tone_play_end();

#if WARNING_SYSVOL_ADJ_EN
    bsp_res_sysvol_resume();
#endif

    func_bt_set_dac(dac_sta);

#if ABP_EN && !ABP_PLAY_DIS_WAV_EN
    if (sys_cb.abp_mode) {
        bsp_abp_set_mode(sys_cb.abp_mode);
    }
#endif
}

#if WARNING_WSBC_EN
void wsbc_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if ABP_EN
    if (abp_is_playing()) {
#if ABP_PLAY_DIS_WAV_EN
        return;
#else
        abp_stop();
        dac1_aubuf_clr();
#endif // ABP_PLAY_DIS_WAV_EN
    }
#endif // ABP_EN

    u8 dac_sta = dac_get_pwr_sta();
    func_bt_set_dac(1);

#if TINY_TRANSPARENCY_EN
    sys_cb.ttp_sta = sys_cb.ttp_start;
    bsp_ttp_stop();
#endif

#if WARNING_SYSVOL_ADJ_EN
    bsp_res_sysvol_adjust();
#endif

    sys_clk_req(INDEX_RES_PLAY, SYS_160M);

    warning_play_init((u8*)addr, len, 1);

    while (!warning_play_proc()) {
        bt_thread_check_trigger();
        WDT_CLR();
#if BT_TWS_EN && WARNING_BREAK_EN
        if(sys_cb.tws_res_brk) {
            sys_cb.tws_res_brk = 0;
            break;
        }
#endif
    }

    warning_play_exit();
    sys_clk_free(INDEX_RES_PLAY);

#if WARNING_SYSVOL_ADJ_EN
    bsp_res_sysvol_resume();
#endif

#if ABP_EN && !ABP_PLAY_DIS_WAV_EN
    if (sys_cb.abp_mode) {
        abp_start(sys_cb.abp_mode - 1);
    }
#endif

    func_bt_set_dac(dac_sta);

#if TINY_TRANSPARENCY_EN
    if (sys_cb.ttp_sta) {
        bsp_ttp_start();
    }
#endif
}

#endif

//提示音播报完毕，设置状态
void bsp_res_play_exit_cb(uint8_t res_idx)
{
#if BT_LOW_LATENCY_EN
#if APP_SWITCH_TONE_TYPE
    if (TWS_RES_MUSIC_MODE == res_idx || TWS_RES_TN_MUSIC_MODE == res_idx) {
        printf("music mode\n");
        bt_low_latency_disable();
    } else if (TWS_RES_GAME_MODE == res_idx || TWS_RES_TN_GAME_MODE == res_idx) {
        printf("game mode\n");
        bt_low_latency_enable();
    }
#else
    if (TWS_RES_MUSIC_MODE == res_idx) {
        printf("music mode\n");
        bt_low_latency_disable();
    } else if (TWS_RES_GAME_MODE == res_idx) {
        printf("game mode\n");
        bt_low_latency_enable();
    }

#endif

#endif

#if ANC_EN
    uint8_t anc_mode = 0;
    if (TWS_RES_NR_DISABLE == res_idx) {
        anc_mode = 1;
    } else if (TWS_RES_ANC == res_idx) {
        anc_mode = 2;
    } else if (TWS_RES_TRANSPARENCY == res_idx) {
        anc_mode = 3;
    }
    if (anc_mode) {
        anc_mode--;
        bsp_anc_set_mode(anc_mode);

        bsp_param_write(&anc_mode, PARAM_ANC_NR_STA, 1);
        bsp_param_sync();
    }
#endif
#if ASR_EN
    if (res_idx == TWS_RES_ASR_ON) {
        sys_cb.asr_enable = 1;
        if (!bt_tws_is_slave()) {
            bsp_asr_start();
        }
    } else if (res_idx == TWS_RES_ASR_OFF) {
        sys_cb.asr_enable = 0;
        bsp_asr_stop();
    }
#endif
}

#if !BT_TWS_EN
uint8_t bsp_res_play(uint8_t res_idx)
{
    uint8_t res_type = tws_res_get_type(res_idx);

    if (res_type != RES_TYPE_INVALID) {
        u32 addr, len;

        tws_res_get_addr(res_idx, &addr, &len);
        if (len != 0) {
            if(res_type == RES_TYPE_MP3) {
#if WARNING_MP3_EN
                bt_audio_bypass();
                mp3_res_play(addr, len);
                bsp_res_play_exit_cb(res_idx);
                bt_audio_enable();
#endif
#if WARNING_WSBC_EN
            } else if(res_type == RES_TYPE_WSBC) {
                wsbc_res_play(addr, len);
#endif
#if WARNING_WAV_EN
            } else if(res_type == RES_TYPE_WAV) {
                wav_res_play(addr, len);
#endif
#if WARNING_PIANO_EN
            } else if(res_type == RES_TYPE_PIANO || res_type == RES_TYPE_TONE) {
                int type = (res_type == RES_TYPE_TONE)? WARNING_TONE : WARNING_PIANO;
                piano_res_play(type, (void *)addr);
#endif
            }
        }
    }

    return RES_ERR_INVALID;
}
#endif

AT(.com_text.pcm.out)
void pcm_out_process(bool is_24bit, void *buffer, u16 sample)
{
#if EQ_DAC_DIV_EN
    if (is_24bit) {
        s32 *ptr = buffer;
        if (sys_cb.tws_left_channel) {
            for (int i = 0; i < sample; i++) {
                ptr[2 * i + 1] = ptr[2 * i];
            }
        } else {
            for (int i = 0; i < sample; i++) {
                ptr[2 * i] = ptr[2 * i + 1];
            }
        }
    } else {
        s16 *ptr = buffer;
        if (sys_cb.tws_left_channel) {
            for (int i = 0; i < sample; i++) {
                ptr[2 * i + 1] = ptr[2 * i];
            }
        } else {
            for (int i = 0; i < sample; i++) {
                ptr[2 * i] = ptr[2 * i + 1];
            }
        }
    }
#endif
}
