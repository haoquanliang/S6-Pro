#include "include.h"

func_cb_t func_cb AT(.buf.func_cb);



#define TRACE_R(...)            print_r(__VA_ARGS__)


#if VBAT_DETECT_EN

void lowpower_warning_do(void)
{
    printf("lowpower_warning_do\r\n");
    if (sys_cb.lpwr_warning_times) {        //低电语音提示次数
        if (RLED_LOWBAT_FOLLOW_EN) {
            led_lowbat_follow_warning();
        }

        sys_cb.lowbat_flag = 1;
#if SWETZ_WARNING_TONE
#if APP_SWITCH_TONE_TYPE
        user_low_bat_tone_play();
#else

        wav_res_play(RES_BUF_ZH_BAT_LOW_WAV, RES_LEN_ZH_BAT_LOW_WAV);
#endif        
#else 
//            bsp_res_play(TWS_RES_LOW_BATTERY);
        bsp_piano_warning_play(WARNING_TONE, TONE_LOW_BATTERY);
#endif

        plugin_lowbat_vol_reduce();         //低电降低音乐音量

        #if 0
        if (RLED_LOWBAT_FOLLOW_EN) {
            while (get_led_sta(1)) {        //等待红灯闪完
                delay_5ms(2);
            }
            led_lowbat_recover();
        }
        #endif

        if (sys_cb.lpwr_warning_times != 0xff) {
            sys_cb.lpwr_warning_times--;
        }
    }
}

void lowpower_switch_to_normal_do(void)
{
    sys_cb.vbat_nor_cnt++;

    if (sys_cb.vbat_nor_cnt > 40) {
        sys_cb.lowbat_flag = 0;
        sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
        plugin_lowbat_vol_recover();    //离开低电, 恢复音乐音量
    }
}

void lowpower_poweroff_do(void)
{
    bsp_piano_warning_play(WARNING_TONE, TONE_LOW_BATTERY);
    sys_cb.pwrdwn_tone_en = 1;     
#if SWETZ_LOWVBAT_OUT_SYNC_OFF
    sys_cb.local_bat_level = 0;//关机前把电量归0
    sys_cb.discon_reason = 0;
#endif
    func_cb.sta = FUNC_PWROFF;     //低电，进入关机或省电模式
}

AT(.text.func.msg)
void lowpower_vbat_process(void)
{
    int lpwr_vbat_sta = is_lowpower_vbat_warning();

    if (lpwr_vbat_sta == 1) {
//        bsp_res_play(TWS_RES_LOW_BATTERY);
        lowpower_poweroff_do();
        return;
    }

    if ((func_cb.mp3_res_play == NULL) || (lpwr_vbat_sta != 2)) {
        if ((sys_cb.lowbat_flag) && (sys_cb.vbat > 3800)) {
            lowpower_switch_to_normal_do();
        }
        return;
    }

    //低电提示音播放
    sys_cb.vbat_nor_cnt = 0;
#if SWETZ_LOW_WARNING_TIME
if (sys_cb.lpwr_warning_cnt > LOW_WARNING_TIME) {
#else 
if (sys_cb.lpwr_warning_cnt > xcfg_cb.lpwr_warning_period) {
#endif
        sys_cb.lpwr_warning_cnt = 0;
        lowpower_warning_do();
    }
}
#endif // VBAT_DETECT_EN

void func_volume_up(void)
{
    if (func_cb.sta == FUNC_BT) {
        if (sys_cb.incall_flag) {
            bsp_bt_call_volume_msg(KU_VOL_UP);
        } else {
#if BT_HID_VOL_CTRL_EN
            if(bsp_bt_hid_vol_change(HID_KEY_VOL_UP)) {
                return;
            }
#endif

            bt_music_vol_up();

#if WARNING_MAX_VOLUME
            if (sys_cb.vol == VOL_MAX) {
                maxvol_tone_play();
            }
#endif
        }
    } else {
        bsp_set_volume(bsp_volume_inc(sys_cb.vol));
    }

    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(1);
    }
}

void func_volume_down(void)
{
    if (func_cb.sta == FUNC_BT) {
        if (sys_cb.incall_flag) {
            bsp_bt_call_volume_msg(KU_VOL_DOWN);
        } else {
#if BT_HID_VOL_CTRL_EN
            if(bsp_bt_hid_vol_change(HID_KEY_VOL_DOWN)) {
                return;
            }
#endif

            bt_music_vol_down();

#if WARNING_MIN_VOLUME
            if (sys_cb.vol == 0) {
                minvol_tone_play();
            }
#endif
        }
    } else {
        bsp_set_volume(bsp_volume_dec(sys_cb.vol));
    }

    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(0);
    }
}

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
    vusb_reset_clr();

#if VBAT_DETECT_EN
    lowpower_vbat_process();
#endif // VBAT_DETECT_EN

#if QTEST_EN
    if(QTEST_IS_ENABLE()) {
        qtest_process();
    }
#endif

#if BT_BACKSTAGE_EN
    if (func_cb.sta != FUNC_BT) {
        func_bt_warning();
        uint status = bt_get_status();
#if BT_BACKSTAGE_PLAY_DETECT_EN
        if (status >= BT_STA_PLAYING) {
#else
        if (status > BT_STA_PLAYING) {
#endif
            func_cb.sta_break = func_cb.sta;
            func_cb.sta = FUNC_BT;
        }
    }
#endif

#if PWRKEY_2_HW_PWRON
    //PWRKEY模拟硬开关关机处理
    if (sys_cb.pwrdwn_hw_flag) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        sys_cb.pwrdwn_hw_flag = 0;
    }
#endif

#if USER_NTC
    if (sys_cb.ntc_2_pwrdwn_flag) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        sys_cb.ntc_2_pwrdwn_flag = 0;
    }
#endif

#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_process();
    }
#endif // CHARGE_EN

#if SYS_KARAOK_EN
    karaok_process();
#endif

#if ANC_ALG_EN
    anc_alg_process();
#endif // ANC_ALG_EN

#if ANC_TOOL_EN
    anc_tool_process();
#endif // ANC_TOOL_EN

#if BT_MUSIC_EFFECT_EN
    music_effect_func_process();
#endif // BT_MUSIC_EFFECT_EN

#if ANC_MAX_VOL_DIS_FB_EN
    bsp_anc_max_vol_dis_fb_process();
#endif // ANC_MAX_VOL_DIS_FB_EN

    if(f_bt.bt_is_inited) {
        bt_thread_check_trigger();
        bsp_res_process();
        app_process();
        le_popup_process();
#if APP_MESSAGE_RUN
        message_runnable();
#endif

    }

#if OFFLINE_LOG_EN
    offline_log_process();
#endif

    gsensor_process();

    sys_run_loop();
}

#if SWETZ_SPP_CMD
AT(.text.func.msg)
static void spp_at_cmd_process(void)
{
    char spp_tx_buffer[80];
    u8 *ptr = eq_rx_buf;
    memset(spp_tx_buffer,0,sizeof(spp_tx_buffer));
    if (memcmp((char *)ptr, "version", strlen("version")) == 0)
    {
		sprintf(spp_tx_buffer, "+version:V0.0.%d",SW_VERSION);
		bt_spp_tx(SPP_SERVICE_CH0, (uint8_t *)spp_tx_buffer, strlen(spp_tx_buffer));
    }

#if BT_MUSIC_EFFECT_DBB_EN
    else if (memcmp((char *)ptr, "dbb_on", strlen("dbb_on")) == 0)
    {
        if (bt_tws_is_connected())
        {
            sprintf(spp_tx_buffer, "+dbb_on:ok");    
            message_send(MSG_ID_DBB_SWITCH, 1, 0);
        }
        else 
        {
            sprintf(spp_tx_buffer, "+dbb_on:err"); 
        }

        bt_spp_tx(SPP_SERVICE_CH0, (uint8_t *)spp_tx_buffer, strlen(spp_tx_buffer));        
    }
    else if (memcmp((char *)ptr, "dbb_off", strlen("dbb_off")) == 0)
    {
        if (bt_tws_is_connected())
        {
            sprintf(spp_tx_buffer, "+dbb_off:ok");   
            message_send(MSG_ID_DBB_SWITCH, 0, 0);
        }
        else 
        {
            sprintf(spp_tx_buffer, "+dbb_off:err"); 
        }
        bt_spp_tx(SPP_SERVICE_CH0, (uint8_t *)spp_tx_buffer, strlen(spp_tx_buffer));        
    } 
#endif 

}
#endif


#if SWETZ_INCASE_MUTE
AT(.text.func.msg)
void app_check_mute(void)
{
    bool current_state = vhouse_cb.inbox_sta;////CHARGE_INBOX();
    if (current_state)
    {   
        if (!sys_cb.mute)
        {
            bsp_sys_mute();
        }
    }
    
    else
    {
        if (sys_cb.mute)
        {
            bsp_sys_unmute();
        }
        
    }



}
#endif

void lr_notify_in_case_info(void)
{
    u8 in_case = sys_cb.flag_local_in_case;

    app_lr_send_notification(LR_NOTIFY_IN_CASE_STATUS, 1, &in_case);
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
#if SWETZ_NTC
    static u8 ntc_cnt;
#endif

    switch (msg) {
#if BT_TWS_EN
        case EVT_BT_UPDATE_STA:
            f_bt.disp_update = 1;    //刷新显示
            break;
#endif
        case KL_NEXT_VOL_UP:
        case KH_NEXT_VOL_UP:
        case KL_PREV_VOL_UP:
        case KH_PREV_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
        case KU_VOL_UP_NEXT:
        case KU_VOL_UP_PREV:
        case KU_VOL_UP:
        case KU_VOL_UP_DOWN:
            func_volume_up();
            break;

        case KLU_VOL_UP:
        case KLU_NEXT_VOL_UP:
        case KL_PREV_VOL_DOWN:
        case KH_PREV_VOL_DOWN:
        case KL_NEXT_VOL_DOWN:
        case KH_NEXT_VOL_DOWN:
        case KU_VOL_DOWN_PREV:
        case KU_VOL_DOWN_NEXT:
        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
        case KL_VOL_UP_DOWN:
        case KH_VOL_UP_DOWN:
            func_volume_down();
            break;

        //长按PP/POWER软关机(通过PWROFF_PRESS_TIME控制长按时间)
        case KLH_POWER:
        case KLH_MODE_PWR:
        case KLH_PLAY_PWR_USER_DEF:
            if(xcfg_cb.bt_tswi_kpwr_en) {       //按键关机是否主从切换
                if(sys_cb.discon_reason == 0xff) {
                    sys_cb.discon_reason = 0;   //不同步关机
                }
            }
            sys_cb.pwrdwn_tone_en = 1;
            func_cb.sta = FUNC_PWROFF;
            break;
        case KU_MODE:
        case KU_MODE_PWR:
            func_cb.sta = FUNC_NULL;
            break;

#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case KU_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            break;

#if SWETZ_CHECK_INBOX
        case EVT_IN_CASE:
                printf("EVT_IN_CASE\r\n");
                  lr_notify_in_case_info();  
#if APP_USER_NOTIFY
#if APP_INBOX_STA_1S_AFTER_UPDATE
                    message_cancel_all(MSG_ID_UPDATE_1S_AFTER_INCASE_STA);
                    message_send(MSG_ID_UPDATE_1S_AFTER_INCASE_STA, 0, 500);
#else
                     msg_enqueue(EVT_UPDATE_INCASE_STA);
#endif
#endif


#if SWETZ_ROLE_SWITCH_BY_INBOX
                 msg_enqueue(EVT_INBOX_CHANGED);
#endif


            break;

#if BT_MUSIC_EFFECT_DBB_EN
        case EVT_USER_SYNC_DBB_ON:
        music_dbb_audio_start();
        break;

        case EVT_USER_SYNC_DBB_OFF:
        music_dbb_audio_stop();    
        break;
#endif 
            
#if APP_USER_FIND_EAR    
        case EVT_FIND_ME_LEFT_START:
                
                if(sys_cb.tws_left_channel){
                    printf("EVT_FIND_ME_LEFT_START\r\n");
                     
                     sys_cb.find_me_count = 0;
                     message_send(MSG_ID_FIND_ME_LEFT, 0, 0);
                }else if(bt_tws_is_connected()){
                    
                    app_lr_send_msg(EVT_FIND_ME_LEFT_START);
                }
               sys_cb.find_left_ear_going = true;
               app_lr_send_notification(LR_NOTIFY_SYNC_L_FIND_STA, 1, &sys_cb.find_left_ear_going);
               ab_mate_notify_find_me_left();
            break;

         case EVT_FIND_ME_LEFT_STOP:
                    printf("sys_cb.tws_left_channel:%d\r\n",sys_cb.tws_left_channel);
                    if(sys_cb.tws_left_channel){
                        printf("EVT_FIND_ME_LEFT_STOP\r\n");
                        
                        message_cancel_all(MSG_ID_FIND_ME_LEFT);
                    }else if(bt_tws_is_connected()){

                        app_lr_send_msg(EVT_FIND_ME_LEFT_STOP);
                    }
                    sys_cb.find_left_ear_going = false;
                    app_lr_send_notification(LR_NOTIFY_SYNC_L_FIND_STA, 1, &sys_cb.find_left_ear_going);
                    ab_mate_notify_find_me_right();
                break;

        case EVT_FIND_ME_RIGHT_START:
                if(!sys_cb.tws_left_channel){
                    printf("EVT_FIND_ME_RIGHT_START\r\n");
                    sys_cb.find_me_count = 0;
                    
                     sys_cb.find_ear_count = 0;
                     message_send(MSG_ID_FIND_ME_RIGHT, 0, 0);
                }else if(bt_tws_is_connected()){
                    
                    app_lr_send_msg(EVT_FIND_ME_RIGHT_START);
                }
                sys_cb.find_right_ear_going = true;
                app_lr_send_notification(LR_NOTIFY_SYNC_R_FIND_STA, 1, &sys_cb.find_right_ear_going);
                    break;

         case EVT_FIND_ME_RIGHT_STOP:
                    
                    if(!sys_cb.tws_left_channel){
                        printf("EVT_FIND_ME_RIGHT_STOP\r\n");
                        sys_cb.find_ear_count = 0;
                        message_cancel_all(MSG_ID_FIND_ME_RIGHT);
                       
                    }else if(bt_tws_is_connected()){

                        app_lr_send_msg(EVT_FIND_ME_RIGHT_STOP);
                    }
                    sys_cb.find_right_ear_going = false;
                    app_lr_send_notification(LR_NOTIFY_SYNC_R_FIND_STA, 1, &sys_cb.find_right_ear_going);
                break;


#endif

        case EVT_OUT_CASE:
                printf("EVT_OUT_CASE\r\n");
                lr_notify_in_case_info();  
                 
               
#if APP_USER_NOTIFY
               
#if APP_INBOX_STA_1S_AFTER_UPDATE

                    message_send(MSG_ID_UPDATE_1S_AFTER_INCASE_STA, 0, 500);
#else
                     msg_enqueue(EVT_UPDATE_INCASE_STA);
#endif
                
#endif
#if SWETZ_OUTCASE_AFTER_NOT_KEY
             sys_cb.flag_outcase_5s_kye_null = true;
             message_cancel_all(MSG_ID_TIMEOUT_5S_AFTER_OUT_BOX);
             message_send(MSG_ID_TIMEOUT_5S_AFTER_OUT_BOX, 0, 5000);
#endif                 
#if SWETZ_ROLE_SWITCH_BY_INBOX
                 msg_enqueue(EVT_OUTBOX_CHANGED);
#endif

            break;
#endif

#if APP_USER_NOTIFY
            case EVT_UPDATE_INCASE_STA:
                    if(!bt_tws_is_slave()){
                        printf("EVT_UPDATE_INCASE_STA\r\n");
                            ab_mate_user_incase_sta_notify();
                    }else {
                            app_lr_send_msg(EVT_UPDATE_INCASE_STA);
                    }
                break;
#endif

#if SWETZ_ROLE_SWITCH_BY_INBOX
        case EVT_INBOX_CHANGED:
            app_role_switch_by_inbox();
            break;

        case EVT_OUTBOX_CHANGED:
            app_role_switch_by_outbox();
            break;
#endif

#ifdef SWETZ_KEY_TONE
        case EVT_KEY_PRESS:
                
              //  wav_res_play(RES_BUF_KEY_TONE_WAV, RES_LEN_KEY_TONE_WAV);
              //mp3_res_play(RES_BUF_EVEN_TONE_MP3,RES_LEN_EVEN_TONE_MP3);
              if(bt_tws_is_slave()){//主机收到直接执行，从机收到发送主机执行
                app_lr_send_msg(EVT_KEY_PRESS);    
              }else {
                printf("EVT_KEY_PRESS\r\n");
                bsp_res_play(TWS_RES_EVEN);
              }
               
            break;
#endif

#if SWETZ_KEY_RECON
        case EVT_RECONN_AG:
        if (bt_tws_is_connected() && bt_tws_is_slave())
        {
            app_lr_send_msg(EVT_RECONN_AG);
        }
        else
        {
            if (bt_nor_get_link_info(NULL) && (!bt_nor_is_connected()))
            {
                printf("reconn ag\n");
                bt_connect();
#if SWETZ_RECON_TONE                
                sys_cb.recon_tone = 1;
#endif
            }
        }
            break;

#endif

#if SYS_KARAOK_EN
        case KU_VOICE_RM:
            karaok_voice_rm_switch();
            break;
#if SYS_MAGIC_VOICE_EN || HIFI4_PITCH_SHIFT_EN
        case KL_VOICE_RM:
            magic_voice_switch();
            break;
#endif
#endif

#if ANC_EN
        case KU_ANC:
        case KD_ANC:
        case KL_ANC:
            sys_cb.anc_user_mode++;
            if (sys_cb.anc_user_mode > 2) {
                sys_cb.anc_user_mode = 0;
            }
            bsp_anc_set_mode(sys_cb.anc_user_mode);
            break;
#endif

        case MSG_SYS_500MS:

            break;

#if SWETZ_EVT_5S
        case EVT_SYS_5S:
            print_bd_addr_sw();
#if SWETZ_ROLE_SWITCH_BY_BAT
            app_role_switch_by_bat();
#endif
        {
            u8 bat_level = bsp_get_bat_level();
          //  if(sys_cb.local_bat_level != bat_level){
                    sys_cb.local_bat_level = bat_level;
                    app_lr_send_notification(LR_NOTIFY_BATTERY_LEVEL, 1, &sys_cb.local_bat_level);
                    
         //   }
        }
        printf("vbat:%d adc_cb.vbat_val:%d sys_cb.local_bat_level:%d  sys_cb.peer_bat_level:%d  box_vbat:%d\r\n",sys_cb.vbat,adc_cb.vbat_val,sys_cb.local_bat_level,sys_cb.peer_bat_level,charge_box_get_charge_box_bat_level());

      // printf("sys_cb.SWETZ_tick:%d\r\n",sys_cb.SWETZ_tick);
        printf("disp status %d,tws conn %d,tws slave %d, nor conn %d, scan %d, sys_cb.vol:%d  sys_cb.hfp_vol:%d sys_cb.lang_id:%d low_latency:%d sys_cb.eq_mode:%d dev.en:%d\n\r",
        f_bt.disp_status,bt_tws_is_connected(),bt_tws_is_slave(),bt_nor_is_connected(), bt_get_scan(),
        sys_cb.vol,sys_cb.hfp_vol,sys_cb.lang_id,bt_is_low_latency(),ab_mate_app.eq_info.mode,ab_mate_app.mult_dev.en);
       printf("in_case:%d  peer_in_case:%d  mult_dev.en:%d \r\n",sys_cb.flag_local_in_case,sys_cb.flag_peer_in_case,ab_mate_app.mult_dev.en);
        
             printf("local_in_case:%d peer_in_case:%d\r\n",sys_cb.flag_local_in_case,sys_cb.flag_peer_in_case);
           //  app_lr_send_notification(LR_NOTIFY_IN_CASE_STATUS, 1, &sys_cb.flag_local_in_case);
        //printf("ab_mate_app.con_sta:%d\r\n",ab_mate_app.con_sta);
        //  printf("sys_cb.sleep_delay:%d sys_cb.pwroff_delay:%d sys_cb.sleep_en:%d port_2led_is_sleep_en:%d  bt_is_allow_sleep:%d\r\n",sys_cb.sleep_delay,sys_cb.pwroff_delay,sys_cb.sleep_en,port_2led_is_sleep_en(),bt_is_allow_sleep());
        //  u8 bt_tws_addr[6];
        // printf("get_link:%d\r\n",bt_tws_get_link_info(bt_tws_addr));
        // printf("bt_tws_addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
        // bt_tws_addr[0], bt_tws_addr[1], bt_tws_addr[2],
        // bt_tws_addr[3], bt_tws_addr[4], bt_tws_addr[5]);
        // char spp_tx_buffer[80];
        // sprintf(spp_tx_buffer, "+ntc:%d", saradc_get_value8(ADCCH_NTC));
        // bt_spp_tx(SPP_SERVICE_CH0, (uint8_t *)spp_tx_buffer, strlen(spp_tx_buffer));
       // printf("ab_mate_app.box_vbat:%d\r\n",ab_mate_app.box_vbat);
        // printf("a2dp_index:%d ring_index:%d",bt_get_cur_a2dp_media_index(),bt_call_get_ring_index());
         //printf("local_vbat:%d remote_vbat:%d\r\n",ab_mate_app.local_vbat,ab_mate_app.remote_vbat);
     //   printf("find_leftgoing:%d find_rightgoing:%d\r\n",sys_cb.find_left_ear_going,sys_cb.find_right_ear_going);
   //printf("music_effect_get_state:%d %d\r\n",music_effect_get_state(MUSIC_EFFECT_DBB),music_effect_get_state_real(MUSIC_EFFECT_DBB));  
   //printf("ab_mate_app.v3d_audio_en:%d\r\n",ab_mate_app.v3d_audio_en);
//    printf("find_left:%d find_right:%d",sys_cb.find_left_ear_going,sys_cb.find_right_ear_going);
//             printf("sys_cb.sw_rst_flag:%d\r\n",sys_cb.sw_rst_flag);
                param_lang_id_read();
                printf("read land:%d\r\n",sys_cb.lang_id);
        break;
            
#endif

#if SWETZ_EVT_1S
        case EVT_SYS_1S:
                app_check_mute();
#if SWETZ_NTC
            ntc_cnt++;
            if(ntc_cnt == 4){
                ntc_gpio_power_supply();
            }
            if(ntc_cnt >= 7){
                ntc_cnt = 0;
                user_ntc_check();
                ntc_gpio_power_down();
            }

#endif                
            break;
#endif

#if SWETZ_SPP_CMD
        case EVT_SPP_AT_CMD:
            spp_at_cmd_process();

            break;
#endif
#if APP_SPATIAL_AUDIO_TONE
            case EVT_SPATIAL_AUDIO_ON:
                    
            if (!music_effect_get_state(MUSIC_EFFECT_SPATIAL_AUDIO))
            {
                printf("EVT_SPATIAL_AUDIO_ON\r\n");
                f_bt.warning_status |= BT_WARN_SPATIAL_AUDIO_SWITCH;
            }
                break;
            case EVT_SPATIAL_AUDIO_OFF:
            if (music_effect_get_state(MUSIC_EFFECT_SPATIAL_AUDIO))
            {
                printf("EVT_SPATIAL_AUDIO_OFF\r\n");
                f_bt.warning_status |= BT_WARN_SPATIAL_AUDIO_SWITCH;
            }
                break;

#endif
#if APP_SPATIAL_AUDIO_TONE
            case EVT_EQ_PARA_DEFAULT:
                if (ab_mate_app.eq_info.mode != 0)
                {
                    ab_mate_app.eq_info.mode = 0;
                    ab_mate_tws_eq_info_sync();          
                    ab_mate_cm_write(&ab_mate_app.eq_info.mode, AB_MATE_CM_EQ_DATA, 1+AB_MATE_EQ_BAND_CNT, 2); 
                    ab_mate_notify_eq();  
                }
                break;
            case EVT_SP_AUDIO_DEFAULT:
                    ab_mate_notify_audio();  
                break;

#endif

#if SWETZ_BT_TO_PAIR    

// 在断开设备时，根据不同的状态选择断开哪个设备：
// • 如果正在播放音乐（BT_STA_PLAYING）：断开非当前A2DP媒体索引的设备。
// • 如果正在来电（BT_STA_INCOMING）：断开非当前响铃索引的设备。
// • 如果正在去电或通话中（BT_STA_OUTGOING或BT_STA_INCALL）：断开非当前HFP索引的设备。
// • 如果以上都不是，但当前活动地址已连接：断开非活动地址的设备。
// • 否则，断开非当前HFP索引的设备。
        case EVT_BOX_CMD_AG_PAIR:
            printf("EVT_BOX_CMD_AG_PAIR\r\n");
            if(!bt_tws_is_slave()){
                    if(ab_mate_app.mult_dev.en){
                            if(bt_get_connected_num() == 2){
                            u8 bt_status = bt_get_status();
                            u8 disc_index = 0;
                            u8 bt_addr[6];
                            char name[32];

                        if(bt_status == BT_STA_PLAYING){
                                if(bt_get_cur_a2dp_media_index()){
                                         disc_index = 0;
                                }else {
                                         disc_index = 1;
                                }
                        }else if(bt_status == BT_STA_INCOMING){
                                if (bt_call_get_ring_index())
                                {
                                    disc_index = 0;
                                }
                                else 
                                {
                                    disc_index = 1;
                                }      
                        }else if ((bt_status == BT_STA_OUTGOING) || (bt_status == BT_STA_INCALL))
                    {
                        if (hfp_index)
                        {
                            disc_index = 0;
                        }
                        else 
                        {
                            disc_index = 1;
                        }
                    }

                        

                        
                        bt_get_link_btaddr(disc_index, bt_addr);//拿到没活动设备的连接地址
                        bt_nor_get_link_info_name(bt_addr, name, 32);//获取配对信息中的蓝牙名
                        printf("disc device:%s  disc_index:%d \n",name,disc_index);
                        TRACE_R(bt_addr,6);
                        memcpy(ab_mate_app.mult_dev.discon_addr, bt_addr, 6);//copy需要断开的设备地址到ab_mate_app.mult_dev.discon_addr
                        bt_disconnect_address();//蓝牙设备断开地址
                         bt_set_scan(0x03);
                        f_bt.warning_status |= BT_WARN_PAIRING;
                        }else {
                                    f_bt.warning_status |= BT_WARN_PAIRING;
                                    bt_set_scan(0x03); //没连接或者连接一个设备就进可发现可连接
                            }
                    }else {
                        msg_enqueue(EVT_PAIRING);
                    }

            }else {
                app_lr_send_msg(EVT_BOX_CMD_AG_PAIR);
            }
            break;
#endif


        case EVT_PAIRING:
        if (bt_tws_is_connected() &&  bt_tws_is_slave())
        {
            app_lr_send_msg(EVT_PAIRING);
        }
        else
        {

            printf("enter pairing bt_scan:%x\n",bt_get_scan());
            if(bt_get_scan() != 0x03)
            {
                f_bt.warning_status |= BT_WARN_PAIRING;
                f_bt.need_pairing = 0;
                bt_set_scan(0x03);
            }
            if(bt_nor_is_connected())
            {
                bt_nor_disconnect();
            }            
        }
            break;

#if MUSIC_UDISK_EN
        case EVT_UDISK_INSERT:
            if (dev_is_online(DEV_UDISK)) {
                if (dev_udisk_activation_try(0)) {
                    sys_cb.cur_dev = DEV_UDISK;
                    func_cb.sta = FUNC_MUSIC;
                }
            }
            break;
#endif // MUSIC_UDISK_EN

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD)) {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if FUNC_USBDEV_EN
        case EVT_PC_INSERT:
            if (dev_is_online(DEV_USBPC)) {
                func_cb.sta = FUNC_USBDEV;
            }
            break;

        case EVT_UDE_SET_VOL:
            printf("ude set db: %d\n", sys_cb.db_level);
            bsp_change_volume_db(sys_cb.db_level);
            break;
#endif // FUNC_USBDEV_EN

#if LINEIN_DETECT_EN
        case EVT_LINEIN_INSERT:
            if (dev_is_online(DEV_LINEIN)) {
#if LINEIN_2_PWRDOWN_EN
                sys_cb.pwrdwn_tone_en = LINEIN_2_PWRDOWN_TONE_EN;
                func_cb.sta = FUNC_PWROFF;
#else
                func_cb.sta = FUNC_AUX;
#endif // LINEIN_2_PWRDOWN_EN
            }
            break;
#endif // LINEIN_DETECT_EN

        case EVT_A2DP_SET_VOL:
        case EVT_TWS_INIT_VOL:
            if((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0) {
                if(!bsp_res_is_vol_busy()) {
                    bsp_change_volume(sys_cb.vol);
                }
            }
            //no break
        case EVT_TWS_SET_VOL:
            if (sys_cb.incall_flag == 0) {
                printf("VOL: %d\n", sys_cb.vol);
                if(msg != EVT_TWS_INIT_VOL) {   //TWS同步音量，不需要显示
                    gui_box_show_vol();
                }
            }
            //no break
        case EVT_DEV_SAVE_VOL:
            sys_cb.cm_times = 0;
            sys_cb.cm_vol_change = 1;
            break;

#if BT_MUSIC_EFFECT_SOFT_VOL_EN
        case EVT_SOFT_VOL_SET:
            bsp_change_volume(sys_cb.vol);
            break;
#endif // BT_MUSIC_EFFECT_SOFT_VOL_EN
#if ABP_EN
        case EVT_ABP_SET:
            if (sys_cb.abp_mode == 0) {
                abp_stop();
            } else {
                abp_start(sys_cb.abp_mode - 1);
            }
            break;
#endif // ABP_EN

        case EVT_BT_SCAN_START:
            if (bt_get_status() < BT_STA_SCANNING) {
                bt_scan_enable();
            }
            break;
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
        case EVT_ONLINE_SET_EQ:
            bsp_eq_parse_cmd();
            break;
#endif
#if ANC_EN
        case EVT_ONLINE_SET_ANC:
            bsp_anc_parse_cmd();
            break;
#endif

#if ENC_DBG_EN
        case EVT_ONLINE_SET_ENC:
            bsp_enc_parse_cmd();
            break;
#endif

#if BT_MUSIC_EFFECT_DBG_EN
        case EVT_ONLINE_SET_EFFECT:
            bsp_effect_parse_cmd();
            break;
#endif

#if ANC_ALG_DBG_EN
        case EVT_ONLINE_SET_ANC_ALG:
            bsp_anc_alg_parse_cmd();
            break;
#endif // ANC_ALG_DBG_EN

#if ANC_TOOL_EN
        case EVT_ONLINE_SET_ANC_TOOL:
            bsp_anc_tool_parse_cmd();
            break;
#endif // ANC_TOOL_EN

#if SYS_KARAOK_EN
        case EVT_ECHO_LEVEL:
//            printf("echo level:%x\n", sys_cb.echo_level);
    #if SYS_ECHO_EN
            bsp_echo_set_level();
    #endif
            break;

        case EVT_MIC_VOL:
//            printf("mic vol:%x\n", sys_cb.mic_vol);
            bsp_karaok_set_mic_volume();
            break;

        case EVT_MUSIC_VOL:
//            printf("music vol:%x\n", sys_cb.music_vol);
            bsp_karaok_set_music_volume();
            break;
#endif
#if LANG_SELECT == LANG_EN_ZH
        case EVT_BT_SET_LANG_ID:
            param_lang_id_write();
            param_sync();
            break;
#endif

#if EQ_MODE_EN
        case EVT_BT_SET_EQ:
            music_set_eq_by_num(sys_cb.eq_mode);
            break;
#endif

#if CHARGE_BOX_EN && (UART0_PRINTF_SEL != PRINTF_VUSB)
        //耳机入仓关机
        case EVT_CHARGE_INBOX:
            if(sys_cb.discon_reason == 0xff) {
                sys_cb.discon_reason = 0;   //不同步关机
            }
            sys_cb.pwrdwn_tone_en = 0;
            charge_box_inbox_wakeup_enable();
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

#if QTEST_EN
        case EVT_QTEST_PICKUP_PWROFF:
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

        case EVT_HFP_SET_VOL:
            if(sys_cb.incall_flag & INCALL_FLAG_SCO){
                bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
                sys_cb.cm_times = 0;
                sys_cb.cm_vol_change = 1;
            }
            break;

#if DAC_DNC_EN
        case EVT_DNC_START:
            if (bsp_res_is_playing()) {
                msg_enqueue(EVT_DNC_START);
            } else {
                #if ANC_EN
                if (sys_cb.anc_user_mode == 0)
                #endif
                {
                    dac_dnc_start();
                }
            }
            break;

        case EVT_DNC_STOP:
            if (bsp_res_is_playing()) {
                msg_enqueue(EVT_DNC_STOP);
            } else {
                dac_dnc_stop();
            }
            break;
#endif

#if IODM_TEST_EN
        case EVT_IODM_TEST:
            iodm_reveice_data_deal();
            break;
#endif

#if SWETZ_SYNC_EVT
        case EVT_USER_SYNC_EVT:
            printf("=====================EVT_IODM_TEST\r\n");
            break;
#endif

#if APP_REST_FACTORY
        case EVT_USER_REST:
                printf("EVT_USER_REST\r\n");
                sw_reset_kick(SW_RST_FLAG);
            break;
#endif
#if ASR_EN
        case EVT_ASR_START:
            if (sys_cb.asr_enable) {
                bsp_asr_start();
            }
            break;

        case EVT_ASR_STOP:
            bsp_asr_stop();
            break;
#endif
#if OPUS_ENC_EN && LE_DUEROS_DMA_EN
       case EVT_ENCODE_STOP:
            bsp_opus_encode_stop();
            break;
#endif
#if BT_HID_DOUYIN_EN
        case EVT_HID_SLIDE_UP:
            printf("user_finger_up\n");
            user_finger_up();
            break;
        case EVT_HID_SLIDE_DOWN:
            printf("user_finger_down\n");
            user_finger_down();
            break;
        case EVT_HID_P:
            printf("user_finger_p\n");
            user_finger_p();
            break;
        case EVT_HID_PP:
            printf("user_finger_pp\n");
            user_finger_pp();
            break;
        case EVT_HID_TAKE_PIC:
            printf("bsp_bt_hid_photo\n");
            bsp_bt_hid_photo(HID_KEY_VOL_UP);
            break;
#endif

#if (BT_MAP_EN || BT_HFP_TIME_EN)
        case EVT_BT_MAP_DONE:
        case EVT_HFP_TIME_DONE:
            ab_mate_time_info_notify(CMD_TIME_LOCALTIME);
            break;
#endif
    }

    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6)) {
        sys_cb.cm_vol_change = 0;
        param_hfp_vol_write();
        param_sys_vol_write();
        cm_sync();
    }

#if SD_SOFT_DETECT_EN
    sd_soft_cmd_detect(120);
#endif
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    if (sys_cb.cm_vol_change) {
        sys_cb.cm_vol_change = 0;
        param_sys_vol_write();
    }
    param_sync();
    gui_box_clear();
    reset_sleep_delay();
    reset_pwroff_delay();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
    bsp_clr_mute_sta();
    sys_cb.voice_evt_brk_en = 1;    //播放提示音时，快速响应事件。
#if SYS_KARAOK_EN
    karaok_voice_rm_disable();
#endif
}

AT(.text.func)
void func_exit(void)
{
#if UPD_UART_EN
    upd_uart_check_update();
#endif
    u8 func_num;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }
    func_num++;                                     //切换到下一个任务
    if (func_num >= funcs_total) {
        func_num = 0;
    }
    func_cb.sta = func_sort_table[func_num];        //新的任务
#if SYS_MODE_BREAKPOINT_EN
    param_sys_mode_write(func_cb.sta);
#endif // SYS_MODE_BREAKPOINT_EN
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    func_bt_chk_off();
    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_MUSIC_EN
        case FUNC_MUSIC:
            func_music();
            break;
#endif // FUNC_MUSIC_EN

#if FUNC_CLOCK_EN
        case FUNC_CLOCK:
            func_clock();
            break;
#endif // FUNC_CLOCK_EN

#if FUNC_BT_EN
        case FUNC_BT:
            func_bt();
            break;
#endif

#if FUNC_BTHID_EN
        case FUNC_BTHID:
            func_bthid();
            break;
#endif // FUNC_BTHID_EN

#if FUNC_AUX_EN
        case FUNC_AUX:
            func_aux();
            break;
#endif // FUNC_AUX_EN

#if FUNC_USBDEV_EN
        case FUNC_USBDEV:
            func_usbdev();
            break;
#endif

#if FUNC_SPEAKER_EN
        case FUNC_SPEAKER:
            func_speaker();
            break;
#endif // FUNC_SPEAKER_EN

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif // FUNC_IDLE_EN

#if FUNC_BT_DUT_EN
        case FUNC_BT_DUT:
            func_bt_dut();
            break;
#endif
#if FUNC_BT_FCC_EN
        case FUNC_BT_FCC:
            func_bt_fcc();
            break;
#endif
#if IODM_TEST_EN
        case FUNC_BT_IODM:
            func_bt_iodm();
            break;
#endif

        case FUNC_CHARGE:
            func_charge();
            break;

        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwrdwn_tone_en);
            break;

        default:
            func_exit();
            break;
        }
    }
}
