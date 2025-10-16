#include "include.h"
#include "func.h"
#include "func_bt.h"


func_bt_t f_bt;
void uart_cmd_process(void);


ALIGNED(128)
u16 func_bt_chkclr_warning(u16 bits)
{
    u16 value;
    GLOBAL_INT_DISABLE();
    value = f_bt.warning_status & bits;
    if(value != 0) {
        f_bt.warning_status &= ~value;
        GLOBAL_INT_RESTORE();
        return value;
    }
    GLOBAL_INT_RESTORE();
    return value;
}


#if FUNC_BT_EN
void func_bt_set_dac(u8 enable)
{
    if (bsp_dac_off_for_bt_conn()) {
        if (enable) {
            if (!dac_get_pwr_sta()) {
                dac_restart();
            }
        } else {
            if (dac_get_pwr_sta()) {
                dac_power_off();
            }
        }
    }
}

void func_bt_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

    bt_audio_bypass();
    mp3_res_play(addr, len);
    bt_audio_enable();
}

//切换提示音语言
void func_bt_switch_voice_lang(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    if (xcfg_cb.lang_id >= LANG_EN_ZH) {
        sys_cb.lang_id = (sys_cb.lang_id) ? 0 : 1;
        multi_lang_init(sys_cb.lang_id);
        param_lang_id_write();
        param_sync();
        if (xcfg_cb.bt_tws_en) {
            bt_tws_sync_setting();                                              //同步语言
            bsp_res_play(TWS_RES_LANGUAGE_EN + sys_cb.lang_id);                 //同步播放语言提示音
        } else {
            func_bt_mp3_res_play(RES_BUF_LANGUAGE, RES_LEN_LANGUAGE);
        }
    }
#endif
}

#if BT_TWS_EN
static void func_bt_tws_set_channel(void)
{
    if (DAC_CH_SEL == DAC_DUAL || DAC_CH_SEL == DAC_VCMBUF_DUAL || DAC_CH_SEL == DAC_DIFF_DUAL) {       //dac选为双声道
        return;
    }
    if(f_bt.tws_status & FEAT_TWS_FLAG) {   //对箱状态.
        tws_get_lr_channel();
        dac_mono_init(0, sys_cb.tws_left_channel);
    } else {
        dac_mono_init(1, 0);
    }
}
#endif

u8 func_bt_tws_get_channel(void)
{
#if BT_TWS_EN
    return sys_cb.tws_left_channel;
#else
    return false;
#endif
}

void func_bt_warning_do(void)
{
#if QTEST_EN
    if(qtest_get_mode()){
        func_bt_chkclr_warning(0xffff);
        return;
    }
#endif

    if(func_bt_chkclr_warning(BT_WARN_TWS_DISCON | BT_WARN_TWS_CON)) {
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en) {
            if(xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
        }
#endif
    }

    if(func_bt_chkclr_warning(BT_WARN_DISCON)) {
#if BT_HID_DOUYIN_EN
        cfg_bt_hid_android_param = 0;
        bt_hid_finger_select_ios();
#endif
#if WARNING_BT_DISCONNECT
#if SWETZ_MULT_DEL_TONE
 if(bt_get_connected_num() == 0){
        if(!bt_tws_is_slave()) {
            bsp_res_play(TWS_RES_DISCONNECT);
            return;
        }  
 }
#else
        if(!bt_tws_is_slave()) {
            bsp_res_play(TWS_RES_DISCONNECT);
            return;
        }  
#endif
#endif // WARNING_BT_DISCONNECT
    }

    if(func_bt_chkclr_warning(BT_WARN_PAIRING)) {
        if(!bt_tws_is_slave()) {
            bsp_res_play(TWS_RES_PAIRING);
            DEBUG_SW("TWS_RES_PAIRING\r\n");
            return;
        }
    }

#if BT_TWS_EN
    if(xcfg_cb.bt_tws_en) {
        u16 tws_warning = func_bt_chkclr_warning(BT_WARN_TWS_SCON | BT_WARN_TWS_MCON );
        if(tws_warning != 0) {
            f_bt.tws_had_pair = 1;
            if (xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
            ///固定声道方案，TWS连接后异步播放声音提示音。否则同步播放连接提示音

            if (xcfg_cb.bt_tws_lr_mode >= 8) {
                func_bt_tws_set_channel();
                tws_get_lr_channel();
#if !SWETZ_DEL_TWS_CON_TONE
                if (!bsp_res_is_playing()) {
                    #if WARNING_WSBC_EN
                    if(sys_cb.tws_left_channel) {
                        wsbc_res_play(RES_BUF_LEFT_CH, RES_LEN_LEFT_CH);
                    } else {
                        wsbc_res_play(RES_BUF_RIGHT_CH, RES_LEN_RIGHT_CH);
                    }
                    #else
                    if(sys_cb.tws_left_channel) {
                        func_cb.mp3_res_play(RES_BUF_LEFT_CH, RES_LEN_LEFT_CH);
                    } else {
                        bt_audio_bypass();
                        u8 timer_cnt = 100;
                        while (timer_cnt--) {
                            bt_thread_check_trigger();
                            bsp_res_process();
                            delay_5ms(2);
                            WDT_CLR();
                        }
                        func_cb.mp3_res_play(RES_BUF_RIGHT_CH, RES_LEN_RIGHT_CH);
                        bt_audio_enable();
                    }
                    #endif
                }
#endif                
            } else {
                if (tws_warning & BT_WARN_TWS_MCON) {
                    bsp_res_play(TWS_RES_CONNECTED);
                    return;
                }
            }
        }
    }
#endif

	if(func_bt_chkclr_warning(BT_WARN_CON)) {
#if WARNING_BT_CONNECT
        if(!bt_tws_is_slave()) {
#if SWETZ_MULT_DEL_TONE
            if(bt_get_connected_num() <= 1){
                bsp_res_play(TWS_RES_CONNECTED);
                DEBUG_SW("TWS_RES_CONNECTED\r\n");
            }
#else 


#endif

            return;
        }
#endif
    }

#if BT_HID_MANU_EN
    //按键手动断开HID Profile的提示音
    if (xcfg_cb.bt_hid_manu_en) {
    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_CON)) {
            func_cb.mp3_res_play(RES_BUF_CAMERA_ON_MP3, RES_LEN_CAMERA_ON_MP3);
        }
    #endif

    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_DISCON)) {
            func_cb.mp3_res_play(RES_BUF_CAMERA_OFF_MP3, RES_LEN_CAMERA_OFF_MP3);
        }
    #endif

    #if BT_HID_DISCON_DEFAULT_EN
        if (f_bt.hid_discon_flag) {
            if (bt_hid_is_ready_to_discon()) {
                f_bt.hid_discon_flag = 0;
                bt_hid_disconnect();
            }
        }
    #endif
    }
#endif // BT_HID_MANU_EN
}

AT(.text.func.bt.process)
void func_bt_warning(void)
{
    if(f_bt.warning_status != 0 && !bsp_res_is_full()) {
        func_bt_warning_do();
    }
}

void func_bt_disp_status_do(void)
{
#if SWETZ
    if(!bt_nor_is_connected()) {
#else 
    if(!bt_is_connected()) {
#endif
        en_auto_pwroff();
        sys_cb.sleep_en = BT_PAIR_SLEEP_EN;
    } else {
        dis_auto_pwroff();
        sys_cb.sleep_en = 1;
    }

    switch (f_bt.disp_status) {
    case BT_STA_CONNECTING:
        if (BT_RECONN_LED_EN) {
            led_bt_reconnect();
            break;
        }
    case BT_STA_INITING:
    case BT_STA_IDLE:
        led_bt_idle();
#if WARNING_BT_PAIR
            if(f_bt.need_pairing && f_bt.disp_status == BT_STA_IDLE) {
                f_bt.need_pairing = 0;
#if SWETZ_TONE
        if(xcfg_cb.warning_bt_pair && xcfg_cb.bt_tws_en && (!bt_nor_get_link_info(NULL))) {
#else
        if(xcfg_cb.warning_bt_pair && xcfg_cb.bt_tws_en) {
#endif
                
                    f_bt.warning_status |= BT_WARN_PAIRING;
                }
            }
#endif
        break;
    case BT_STA_SCANNING:
        led_bt_scan();
        break;

    case BT_STA_DISCONNECTING:
        led_bt_connected();
        break;

    case BT_STA_CONNECTED:
        led_bt_connected();
#if ABP_EN && ABP_MUSIC_DIS_PINK_EN
        if (sys_cb.abp_mode == ABP_MODE_PINK) {
            bsp_abp_set_mode(sys_cb.abp_mode);
        }
#endif
#if ANC_MAX_VOL_DIS_FB_EN
        bsp_anc_max_vol_dac_det_stop();
#endif // ANC_MAX_VOL_DIS_FB_EN
        break;
    case BT_STA_INCOMING:
        led_bt_ring();
        break;
    case BT_STA_PLAYING:
        led_bt_play();
#if ABP_EN && ABP_MUSIC_DIS_PINK_EN
        if (sys_cb.abp_mode == ABP_MODE_PINK) {
            abp_stop();
        }
#endif
#if ANC_MAX_VOL_DIS_FB_EN
        bsp_anc_max_vol_dac_det_start();
#endif // ANC_MAX_VOL_DIS_FB_EN
        break;
    case BT_STA_OUTGOING:
    case BT_STA_INCALL:
        led_bt_call();
        break;
    }

    if(f_bt.disp_status >= BT_STA_CONNECTED) {
        f_bt.need_pairing = 1;
        sys_cb.dac_sta_bck = 1;
        func_bt_set_dac(1);
        dac_fade_in();
    } else {
        sys_cb.dac_sta_bck = 0;
        func_bt_set_dac(0);
    }

#if BT_BACKSTAGE_EN
    if (f_bt.disp_status < BT_STA_PLAYING && func_cb.sta_break != FUNC_NULL) {
        func_cb.sta = func_cb.sta_break;
    }
#endif
}

AT(.text.func.bt.process) ALIGNED(128)
void func_bt_disp_status(void)
{
    uint status = bt_get_disp_status();

    GLOBAL_INT_DISABLE();
    if(f_bt.disp_status != status || f_bt.disp_update) {
        f_bt.disp_status = status;
        f_bt.disp_update = 0;
        GLOBAL_INT_RESTORE();

        func_bt_disp_status_do();
    } else {
        GLOBAL_INT_RESTORE();
    }
}

AT(.text.func.bt.process)
void func_bt_status(void)
{
    func_bt_disp_status();

#if FUNC_BTHID_EN
    if(is_bthid_mode()) {
        func_bt_hid_warning();
    } else
#endif
    {
        func_bt_warning();
    }
}

#if USER_INEAR_DETECT_EN
AT(.text.func.bt.process)
void func_bt_inear_process(void)
{
    if (dev_is_online(DEV_EARIN)) {
        if (sys_cb.loc_ear_sta) {
            if (sys_cb.rem_ear_sta) {     //检测到对耳已经入耳，不用播放入耳提示音
                func_cb.mp3_res_play(RES_BUF_INEAR_DU_MP3, RES_LEN_INEAR_DU_MP3);
            }
            bt_set_ear_sta(0);          //入耳
//            bt_music_play();          //播放音乐，需要时打开
        }
    } else {
        if (!sys_cb.loc_ear_sta) {
            bt_set_ear_sta(1);          //摘下
//            bt_music_pause();         //暂停播放
        }
    }
}
#endif // USER_TKEY_INEAR
#if BT_2ACL_AUTO_SWITCH
struct {
    uint32_t check_tick;
    uint16_t play_timer_cnt;
    uint16_t clear_timer_cnt;
    uint8_t protect_timer_cnt;
} bt_silence;

AT(.text.func.bt.process)
bool bt_play_data_check_do(void)
{
    bool ret = false;

    if (bt_silence.protect_timer_cnt) {
        bt_silence.protect_timer_cnt--;
        return false;
    }
    //消抖
    if (!bt_is_silence()) {
        bt_silence.clear_timer_cnt = 0;
        bt_silence.play_timer_cnt++;
        if (bt_silence.play_timer_cnt > 100) {
            ret = true;
            bt_silence.play_timer_cnt = 0;
            bt_silence.protect_timer_cnt = 200;
        }
    } else {
        bt_silence.clear_timer_cnt++;
        if (bt_silence.clear_timer_cnt > 100) {
            bt_silence.play_timer_cnt = 0;
        }
    }
    return ret;
}

AT(.text.func.bt.process)
bool bt_play_data_check(void)
{
#if BT_TWS_EN
    if(bt_tws_is_slave()){
        return false;
    }
#endif

    if (tick_check_expire(bt_silence.check_tick, 10)) {
        bt_silence.check_tick = tick_get();
    } else {
        return false;
    }

    return bt_play_data_check_do();
}

AT(.text.func.bt)
void bt_play_data_init(void)
{
    memset(&bt_silence, 0, sizeof(bt_silence));
}

void bt_play_switch_device()
{
    bt_silence.play_timer_cnt = 0;
    bt_silence.clear_timer_cnt = 0;
    bt_silence.protect_timer_cnt = 200;
    bt_music_play_switch();
}
#endif

//借用读参数区做load flash的动作，使flash一直busy
static void func_bt_load_flash(void)
{
    static u32 tick = 0;
    u8 load_buf[32];
    if (tick_check_expire(tick, 1000)) {
        printf("load flash test\n");
        tick = tick_get();
    }
    cm_read(load_buf, PAGE0(0), 32);
    cm_read(load_buf, PAGE1(0), 32);
    cm_read(load_buf, PAGE2(0), 32);
}

AT(.text.func.bt.process)
void func_bt_sub_process(void)
{
    func_bt_status();
#if USER_INEAR_DETECT_EN
    func_bt_inear_process();
#endif
#if USER_TKEY_DEBUG_EN
    bsp_tkey_spp_tx();
#endif
#if BT_2ACL_AUTO_SWITCH
    if (bt_play_data_check()) {
        bt_music_play_switch();
    }
#endif
#if BT_BQB_RF_EN
    if (func_cb.sta != FUNC_BT_DUT) {
        func_cb.sta = FUNC_BT_DUT;
    }
#endif

    if (func_cb.sta == FUNC_BT_DUT || BT_DUT_MODE_EN) {
        func_bt_load_flash();       //如果在DUT模式就让flash动起来，测试flash对rf的影响
    }
}

AT(.text.func.bt.process)
void func_bt_process(void)
{
    func_process();
    func_bt_sub_process();
#if BT_TWS_MS_SWITCH_EN
    if ((xcfg_cb.bt_tswi_msc_en) && bt_tws_need_switch(0) && !bsp_res_is_playing()) {
        printf("AUDIO SWITCH\n");
        bt_tws_switch();
    }
#endif

    if(f_bt.disp_status == BT_STA_INCOMING) {
        sfunc_bt_ring();
        reset_sleep_delay();
        reset_pwroff_delay();
        f_bt.siri_kl_flag = 0;
        f_bt.user_kl_flag = 0;
    } else if(f_bt.disp_status == BT_STA_OTA) {
        sfunc_bt_ota();
        reset_sleep_delay();
        reset_pwroff_delay();
    } else if(f_bt.disp_status >= BT_STA_OUTGOING) {
        sfunc_bt_call();
        reset_sleep_delay();
        reset_pwroff_delay();
        f_bt.siri_kl_flag = 0;
        f_bt.user_kl_flag = 0;
    }

    if(sys_cb.pwroff_delay == 0) {
        sys_cb.pwrdwn_tone_en = 1;      //连接超时关主从切换,同步关机
#if CHARGE_BOX_EN
        if (CHARGE_INBOX()) {           //仓内超时不要同步关机，另一个耳机可能在仓外
            sys_cb.discon_reason = 0;
        }
#endif
        func_cb.sta = FUNC_PWROFF;
        return;
    }
    if(sleep_process(bt_is_allow_sleep)) {
        f_bt.disp_status = 0xff;
    }
}

AT(.text.func.bt)
void func_bt_init(void)
{
    if (!f_bt.bt_is_inited) {
        msg_queue_clear();
        func_bt_set_dac(0);
        bsp_bt_init();
        f_bt.bt_is_inited = 1;
    }
}

AT(.text.func.bt)
void func_bt_chk_off(void)
{
    if ((func_cb.sta != FUNC_BT) && (f_bt.bt_is_inited)) {
#if BT_PWRKEY_5S_DISCOVER_EN
        bsp_bt_pwrkey5s_clr();
#endif
        bt_disconnect(0);
        bt_off();
        func_bt_set_dac(1);
        f_bt.bt_is_inited = 0;
    }
}

bool func_bt_charge_dcin(void)
{
#if QTEST_EN
    u32 qtest_5v_tick = tick_get();
    if(qtest_get_mode()){
        return false;
    }

    if(qtest_cb.sta) {
        while(!tick_check_expire(qtest_5v_tick, 1000)) {
            if(!CHARGE_DC_IN()) {
                return false;
            }
            if(func_cb.sta == FUNC_BT) {
                bt_thread_check_trigger();
                bsp_res_process();
            }
        }
        qtest_cb.sta = 0;
    }
#endif

    if ((xcfg_cb.bt_tswi_charge_rst_en) || (func_cb.sta != FUNC_BT)) {
		sys_cb.discon_reason = 0;

		//开启UART2检测VUSB KEY，避免func_bt_exit过程太久，无法进入VUSB升级
		sys_clk_set(SYS_24M);
        #if !UART1_EN && !UART2_EN
		uart2_key_mode(9600);
        #endif
        func_bt_exit();
        sw_reset_kick(SW_RST_DC_IN);                    //直接复位进入充电
        while(1);
    }
    if(sys_cb.discon_reason == 0xff) {
        sys_cb.discon_reason = 0;   //不同步关机
    }

    return true;
}

AT(.text.func.bt)
void func_bt_enter(void)
{
    if (func_cb.last != FUNC_NULL) {    //开机进入不清res
        bsp_res_cleanup();
    }
    bsp_res_set_enable(true);
    func_cb.mp3_res_play = func_bt_mp3_res_play;
    func_bt_enter_display();
    led_bt_init();
    func_bt_init();
    //en_auto_pwroff();

#if WARNING_FUNC_BT
    mp3_res_play(RES_BUF_BT_MODE, RES_LEN_BT_MODE);
#endif // WARNING_FUNC_BT

#if WARNING_BT_WAIT_CONNECT
    mp3_res_play(RES_BUF_WAIT4CONN, RES_LEN_WAIT4CONN);
#endif // WARNING_BT_WAIT_CONNECT

    f_bt.disp_status = 0xfe;
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;

    bt_redial_init();
    bt_audio_enable();

#if BT_PWRKEY_5S_DISCOVER_EN
    if(bsp_bt_pwrkey5s_check()) {
        f_bt.need_pairing = 0;  //已经播报了
        func_bt_disp_status();
        func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
    } else {
        func_bt_disp_status();
#if WARNING_BT_PAIR
        if (xcfg_cb.warning_bt_pair && !xcfg_cb.bt_tws_en) {
            func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
        }
#endif // WARNING_BT_PAIR
    }
#endif

#if SWETZ_TONE
    f_bt.warning_status |= BT_WARN_PAIRING;
#endif

#if BT_2ACL_AUTO_SWITCH
    bt_play_data_init();
#endif
#if LE_PRIV_EN
    ble_priv_adv_en(1);
#endif

#if SWETZ_CHECK_INBOX
   // charge_set_leakage(1, 0);//这样CHARGE_INBOX()才能检测到是否在仓中,仓检测漏电流需要关掉
#endif

}

AT(.text.func.bt)
void func_bt_exit(void)
{
    bsp_res_set_enable(false);
    while(bsp_res_is_playing()) {
        bt_thread_check_trigger();
        bsp_res_process();
    }
    bsp_res_cleanup();
    bsp_res_set_break(false);

    if(sys_cb.discon_reason == 0xff && func_cb.sta == FUNC_PWROFF) {
        sys_cb.discon_reason = 1;   //默认同步关机
    }

    dac_fade_out();
#if ASR_EN
    if (sys_cb.asr_enable) {
        bsp_asr_stop();
        sys_cb.asr_enable = 0;
    }
#endif
#if BT_PWRKEY_5S_DISCOVER_EN
    bsp_bt_pwrkey5s_clr();
#endif
    func_bt_exit_display();
    bt_audio_bypass();
#if BT_TWS_EN
    dac_mono_init(1, 0);
    u16 timeout = 350;
    while (bt_nor_acl_is_connected() && bt_tws_is_connected() && !bt_tws_is_slave() && timeout--) {     //如果没跑完回连流程，在这里等一下回连完HFP和A2DP
        if (hfp_is_connected() && a2dp_is_avctp_connect()) {
            break;
        }
        vusb4s_reset_clr_cnt();
        bt_thread_check_trigger();
        delay_5ms(2);
        WDT_CLR();
    }
#endif

#if !BT_BACKSTAGE_EN
    bt_disconnect(sys_cb.discon_reason);
    bt_off();
    f_bt.bt_is_inited = 0;
#else
    if (bt_get_status() == BT_STA_PLAYING && !bt_is_testmode()) {        //蓝牙退出停掉音乐
        delay_5ms(10);
        if(bt_get_status() == BT_STA_PLAYING) {     //再次确认play状态
            u32 timeout = 850; //8.5s
            bt_music_pause();
            while (bt_get_status() == BT_STA_PLAYING && timeout > 0) {
                timeout--;
                delay_5ms(2);
            }
        }
    }
#endif
#if ABP_EN
    if (abp_is_playing()) {
        sys_cb.abp_mode = 0;
        abp_stop();
    }
#endif // ABP_EN
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;
    func_bt_set_dac(1);
    func_cb.last = FUNC_BT;

#if APP_MESSAGE_RUN
    message_init();
#endif

}

AT(.text.func.bt)
void func_bt(void)
{
    printf("%s\n", __func__);

    func_bt_enter();

    while (func_cb.sta == FUNC_BT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
        func_bt_display();
    }

    func_bt_exit();
}

#endif //FUNC_BT_EN
