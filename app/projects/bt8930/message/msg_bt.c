#include "include.h"
#include "func.h"
#include "func_bt.h"

u16 get_user_def_vol_msg(u8 func_sel)
{
    u16 msg = NO_MSG;

    if(func_sel == UDK_VOL_UP) {            //VOL+
#if BT_TWS_EN
        if(xcfg_cb.user_def_lr_en && xcfg_cb.bt_tws_en) {
            msg = func_bt_tws_get_channel()? KU_VOL_UP : KU_VOL_DOWN;
        } else
#endif
        {
            msg = KU_VOL_UP;
        }
    } else if(func_sel == UDK_VOL_DOWN) {   //VOL-
#if BT_TWS_EN
        if(xcfg_cb.user_def_lr_en && xcfg_cb.bt_tws_en) {
            msg = func_bt_tws_get_channel()? KU_VOL_DOWN : KU_VOL_UP;
        } else
#endif
        {
            msg = KU_VOL_DOWN;
        }
    }
    return msg;
}

void user_def_track_msg(u16 msg)
{
    if (msg == KU_PREV) {
        bt_music_prev();
    } else {
        bt_music_next();
    }
}

///检查USER_DEF按键配置功能
bool user_def_func_is_ready(u8 func_sel)
{
    if (func_sel == UDK_NONE) {
        return false;
    }
    if ((func_sel <= UDK_NEXT) || (func_sel == UDK_PLAY_PAUSE)) {
        if ((f_bt.disp_status != BT_STA_CONNECTED) && (f_bt.disp_status != BT_STA_PLAYING)) {
            return false;
        }
    }
    return true;
}

///检查USER_DEF按键消息处理
bool user_def_key_msg(u8 func_sel)
{
    u16 msg = NO_MSG;
    printf("func_sel:%d\r\n",func_sel);
    if (!user_def_func_is_ready(func_sel)) {
        return false;
    }

#if SWETZ_KEY_TONE
    if(func_sel == UDK_PLAY_PAUSE || func_sel == UDK_PREV || func_sel == UDK_NEXT){
            msg_enqueue(EVT_KEY_PRESS);
    }   
#endif
    if (func_sel == UDK_REDIALING) {
        if(bsp_res_play(TWS_RES_REDIALING) == RES_ERR_INVALID) {
            bt_call_redial_last_number();               //回拨电话
        }
    } else if (func_sel == UDK_SIRI) {                  //SIRI
        bt_hfp_siri_switch();
        printf("bt_hfp_siri_switch\r\n");
    } else if (func_sel == UDK_NR) {                    //NR
        bt_ctl_nr_sta_change();                         //发消息通知手机
    } else if (func_sel == UDK_PREV) {                  //PREV
#if BT_TWS_EN
        if(xcfg_cb.user_def_lr_en && xcfg_cb.bt_tws_en) {
            msg = func_bt_tws_get_channel()? KU_PREV : KU_NEXT;
        } else
#endif
        {
            msg = KU_PREV;
        }
        user_def_track_msg(msg);
    } else if (func_sel == UDK_NEXT) {                  //NEXT
#if BT_TWS_EN
        if(xcfg_cb.user_def_lr_en && xcfg_cb.bt_tws_en) {
            msg = func_bt_tws_get_channel()? KU_NEXT : KU_PREV;
        } else
#endif
        {
            msg = KU_NEXT;
        }
        user_def_track_msg(msg);
    } else if (func_sel == UDK_MODE) {                  //MODE
        func_message(KU_MODE);
    } else if (func_sel == UDK_PHOTO) {
        return bsp_bt_hid_photo(HID_KEY_VOL_UP);        //拍照
    } else if (func_sel == UDK_HOME) {
        return bt_hid_consumer(HID_KEY_IOS_HOME);       //IOS Home按键功能
    } else if (func_sel == UDK_LANG) {
        func_bt_switch_voice_lang();                    //中英文切换
    } else if (func_sel == UDK_PLAY_PAUSE) {
        bt_music_play_pause();
    } else if (func_sel == UDK_DUT) {                  //CBT 测试模式
        if(func_cb.sta != FUNC_BT_DUT){
            func_cb.sta = FUNC_BT_DUT;
            sys_cb.discon_reason = 0;
        }
    } else if (func_sel == UDK_LOW_LATENCY) {

#if SWETZ_L_THREE_P
        if(func_bt_tws_get_channel()){
             bt_hfp_siri_switch();

        }else {
        bool low_latency = bt_is_low_latency();
        if (low_latency) {
            bsp_res_play(TWS_RES_MUSIC_MODE);
        } else {
            bsp_res_play(TWS_RES_GAME_MODE);
        }

        }
#else 
        bool low_latency = bt_is_low_latency();
        if (low_latency) {
#if APP_SWITCH_TONE_TYPE
            user_music_mode_tone_play();
#else
            bsp_res_play(TWS_RES_MUSIC_MODE);
#endif
        } else {
#if APP_SWITCH_TONE_TYPE
            user_game_mode_tone_play();
#else
            bsp_res_play(TWS_RES_GAME_MODE);
#endif            
            
        }

#endif

    }else {                                            //VOL+, VOL-
        func_message(get_user_def_vol_msg(func_sel));
    }
    return true;
}

bool user_def_lkey_tone_is_enable(u8 func_sel)
{
    if (!xcfg_cb.user_def_kl_tone_en) {
        return false;
    }
    return user_def_func_is_ready(func_sel);
}

#if BT_HID_MANU_EN
//双击VOL-, VOL+的功能处理
void bt_hid_vol_msg(u8 sel)
{
    if (sel == 1) {
        bsp_bt_hid_photo(HID_KEY_VOL_UP);
    } else if (sel == 2) {
        bsp_bt_hid_photo(HID_KEY_VOL_DOWN);
    } else if (sel == 3) {
        bsp_bt_hid_tog_conn();
    }
}
#endif

bool bt_tws_pair_mode(u8 method)
{
#if BT_TWS_EN
    if ((xcfg_cb.bt_tws_en) && (xcfg_cb.bt_tws_pair_mode == method) && (!bt_nor_is_connected())) {
        if(bt_tws_is_connected()) {
            bt_tws_disconnect();
        } else {
            bt_tws_search_slave(15000);
        }
        return true;
    }
#endif
    return false;
}

AT(.text.func.bt.msg)
void func_bt_message_do(u16 msg)
{
    int klu_flag = 0;

#if SWETZ_OUTCASE_AFTER_NOT_KEY
     if(sys_cb.flag_outcase_5s_kye_null == true && (msg != EVT_SYS_1S && msg != EVT_SYS_5S && msg != EVT_UPDATE_INCASE_STA)
        && (msg != EVT_FIND_ME_LEFT_START)
        && (msg != EVT_FIND_ME_LEFT_STOP)
        && (msg != EVT_FIND_ME_RIGHT_START)
        && (msg != EVT_FIND_ME_RIGHT_STOP)
        && (msg != EVT_UPDATE_INCASE_STA)
    ){
        return;
     }   
   
#endif



    switch (msg) {
    case KU_PLAY:
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
        printf("33KU_PLAY_PWR_USER_DEF\r\n");
#if SWETZ_SYNC_EVT
    //bt_tws_req_alarm_user(1);//双耳同步执行
#endif
#if SWETZ_TEST
    // message_send(MSG_ID_TIMEOUT_3S_AFTER_POWER_ON, 0, 5000);//单边触发事件延时执行
   // app_lr_send_msg(EVT_SYS_5S);//发给对耳执行
#endif

#if SWETZ_KEY_RECON
    if(!bt_nor_is_connected()){
                printf("112215\r\n");
                msg_enqueue(EVT_RECONN_AG);//-触发蓝牙重连逻辑     
        }
        user_def_key_msg(xcfg_cb.user_def_ks_sel);
#else 
        if (!bt_nor_is_connected()) {
            bt_tws_pair_mode(3);           //单击PLAY按键手动配对
        } else {
        	user_def_key_msg(xcfg_cb.user_def_ks_sel);
        }

#endif


        break;

    case KL_PLAY_PWR_USER_DEF:
        klu_flag = 1;                                                       //长按抬键的时候呼SIRI
    case KL_PLAY_USER_DEF:
        f_bt.user_kl_flag = 0;
        if (!bt_tws_pair_mode(4)) {                                         //是否长按配对功能
            if (user_def_lkey_tone_is_enable(xcfg_cb.user_def_kl_sel)) {
                bsp_piano_warning_play(WARNING_TONE, 2);                    //长按“滴”一声
            }
            if (klu_flag) {
                f_bt.user_kl_flag = user_def_func_is_ready(xcfg_cb.user_def_kl_sel);     //长按抬键的时候再处理
            } else {
                user_def_key_msg(xcfg_cb.user_def_kl_sel);
            }
        }
        break;

        //SIRI, NEXT, PREV在长按抬键的时候响应,避免关机前切歌或呼SIRI了
    case KLU_PLAY_PWR_USER_DEF:
        if (f_bt.user_kl_flag) {
            user_def_key_msg(xcfg_cb.user_def_kl_sel);
            f_bt.user_kl_flag = 0;
        }
        break;

    case KH_PLAY_USER_DEF:
        func_message(get_user_def_vol_msg(xcfg_cb.user_def_kl_sel));
        break;

    case KD_PLAY_USER_DEF:
    case KD_PLAY_PWR_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kd_sel)) {
#if BT_TWS_EN
        } else if(bt_tws_pair_mode(2)) {
#endif
        } else if (xcfg_cb.user_def_kd_lang_en) {
            func_bt_switch_voice_lang();
        }
        break;

    ///三击按键处理
    case KTH_PLAY_USER_DEF:
    case KTH_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kt_sel);
        break;

    ///四击按键处理
    case KFO_PLAY_USER_DEF:
    case KFO_PLAY_PWR_USER_DEF:
        #if ASR_EN
        if(sys_cb.asr_enable) {
            bsp_res_play(TWS_RES_ASR_OFF);
        } else {
            bsp_res_play(TWS_RES_ASR_ON);
        }
        #else
        user_def_key_msg(xcfg_cb.user_def_kfour_sel);
        #endif
        break;

    ///五击按键处理
    case KFI_PLAY_USER_DEF:
    case KFI_PLAY_PWR_USER_DEF:
        if (xcfg_cb.user_def_kfive_sel) {
            user_def_key_msg(xcfg_cb.user_def_kfive_sel);
        }
        break;

    case KU_SIRI:
    case KL_SIRI:
    case KL_HOME:
        if (bt_nor_is_connected()) {
            bt_hfp_siri_switch();
        }
        break;

    case KU_HOME:
        bt_hid_consumer(HID_KEY_IOS_HOME);
        break;

//    case KL_PLAY:
//        if (xcfg_cb.bt_key_discon_en) {
//            bt_disconnect(0);
//            break;
//        }

    case KU_PREV_VOL_DOWN:
    case KU_PREV_VOL_UP:
    case KL_VOL_DOWN_PREV:
    case KL_VOL_UP_PREV:
    case KU_PREV:
        bt_music_prev();
        sys_cb.key2unmute_cnt = 15 * sys_cb.mute;
        break;

    case KU_NEXT:
    case KU_NEXT_VOL_UP:
    case KU_NEXT_VOL_DOWN:
    case KL_VOL_UP_NEXT:
    case KL_VOL_DOWN_NEXT:
        bt_music_next();
        sys_cb.key2unmute_cnt = 15 * sys_cb.mute;
        break;
    case KL_PREV:
        bt_music_rewind();
        break;
    case KLU_PREV:
        bsp_clr_mute_sta();
        bt_music_rewind_end();
        break;
    case KL_NEXT:
        bt_music_fast_forward();
        break;
    case KLU_NEXT:
        bsp_clr_mute_sta();
        bt_music_fast_forward_end();
        break;

    case KD_MODE:
    case KD_MODE_PWR:
        bt_tws_pair_mode(5);
        break;

    case KD_HSF:
        if (bt_nor_is_connected()) {
            if(bsp_res_play(TWS_RES_REDIALING) == RES_ERR_INVALID) {
                bt_call_redial_last_number();       //回拨电话
            }
        }
        break;

#if BT_HID_MANU_EN
    case KD_NEXT_VOL_UP:
    case KD_PREV_VOL_UP:
    case KD_VOL_UP_NEXT:
    case KD_VOL_UP_PREV:
    case KD_VOL_UP:
        bt_hid_vol_msg(3);
        break;

    case KD_PREV_VOL_DOWN:
    case KD_NEXT_VOL_DOWN:
    case KD_VOL_DOWN_PREV:
    case KD_VOL_DOWN_NEXT:
    case KD_VOL_DOWN:
        bt_hid_vol_msg(1);
        break;
#endif

    case MSG_SYS_1S:
//        sys_dump_vbat();

        bt_hfp_report_bat();
#if BT_TWS_DBG_EN
        bt_tws_report_dgb();
#endif
        break;

    case EVT_A2DP_MUSIC_PLAY:
        if (!sbc_is_bypass()) {
            dac_fade_in();
        }
        if (f_bt.pp_2_unmute) {
            f_bt.pp_2_unmute = 0;
            bsp_clr_mute_sta();
        }
        break;

    case EVT_A2DP_MUSIC_STOP:
    case EVT_A2DP_SYNC_CLOSE:
        if (!sbc_is_bypass() && !bsp_res_is_playing()) {
            dac_fade_out();
        }
        printf("EVT_A2DP_MUSIC\r\n");
        break;
            
    case EVT_KEY_2_UNMUTE:
        bsp_clr_mute_sta();
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.bt.msg)
void func_bt_message(u16 msg)
{
    if(msg){
        func_bt_message_do(msg);
    }
}

AT(.text.func.btring.msg)
void sfunc_bt_ring_message_do(u16 msg)
{

#if SWETZ_OUTCASE_AFTER_NOT_KEY
     if(sys_cb.flag_outcase_5s_kye_null == true && (msg != EVT_SYS_1S && msg != EVT_SYS_5S && msg != EVT_UPDATE_INCASE_STA)){
        return;
     }   
#endif

    switch (msg) {
    case KU_HSF:                //接听
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
#if !SWETZ_DEL_RING_SHORT_FUC
        if(bsp_res_play(TWS_RES_CALL_HANGUP) == RES_ERR_INVALID) {
            bt_call_answer_incoming();
        }
#endif        
        break;

    case KL_PLAY_PWR_USER_DEF:
        //PWRKEY松开前不产生KHL_PLAY_PWR消息。按键松开自动清此标志。
        sys_cb.poweron_flag = 1;
    case KL_PLAY_USER_DEF:
        if (user_def_key_msg(xcfg_cb.user_def_kl_sel)) {
            break;
        }
    case KL_HSF:
        if(bsp_res_play(TWS_RES_CALL_REJECT) == RES_ERR_INVALID) {
            bt_call_terminate();    //拒接
        }
        break;

    case KLH_PLAY_PWR_USER_DEF:
        //ring不响应关机消息，解决关机时间1.5时长按拒接偶尔触发关机的问题。
        break;

    case KD_PLAY_USER_DEF:
    case KD_PLAY_PWR_USER_DEF:
#if SWETZ_RING_DOUBLE
        if(bsp_res_play(TWS_RES_CALL_HANGUP) == RES_ERR_INVALID) {
            bt_call_answer_incoming();
        }
#else 
        if (user_def_key_msg(xcfg_cb.user_def_kd_sel)) {
            break;
        }
        if(bsp_res_play(TWS_RES_CALL_REJECT) == RES_ERR_INVALID) {
            bt_call_terminate();    //拒接
        }

#endif
        break;

    ///三击按键处理
    case KTH_PLAY_USER_DEF:
    case KTH_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kt_sel);
        break;

	    ///四击按键处理
    case KFO_PLAY_USER_DEF:
    case KFO_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfour_sel);
        break;

    ///五击按键处理
    case KFI_PLAY_USER_DEF:
    case KFI_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfive_sel);
        break;

    case MSG_SYS_1S:
        bt_hfp_report_bat();
        break;

        //屏蔽来电响铃调音量
    case KL_PREV_VOL_UP:
    case KH_PREV_VOL_UP:
    case KL_NEXT_VOL_UP:
    case KH_NEXT_VOL_UP:
    case KLU_NEXT_VOL_UP:
    case KU_VOL_UP_PREV:
    case KU_VOL_UP_NEXT:
    case KU_VOL_UP:
    case KL_VOL_UP:
    case KH_VOL_UP:
    case KLU_VOL_UP:

    case KL_PREV_VOL_DOWN:
    case KH_PREV_VOL_DOWN:
    case KL_NEXT_VOL_DOWN:
    case KH_NEXT_VOL_DOWN:
    case KU_VOL_DOWN_PREV:
    case KU_VOL_DOWN_NEXT:
    case KU_VOL_DOWN:
    case KL_VOL_DOWN:
    case KH_VOL_DOWN:

    case KU_VOL_UP_DOWN:
    case KL_VOL_UP_DOWN:
    case KH_VOL_UP_DOWN:
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.btring.msg)
void sfunc_bt_ring_message(u16 msg)
{
    if(msg){
        sfunc_bt_ring_message_do(msg);
    }
}

void app_2nd_ringtone_check(void)
{
#if SWETZ_RING_SEC_ROUTE
    if ((!bt_tws_is_slave())
        && (ab_mate_app.mult_dev.en)
        )
    {
        uint call_0 = btstack_get_call_indicate_for_index(0);
        uint call_1 = btstack_get_call_indicate_for_index(1);

        printf("call: %d, %d\n", call_0, call_1);

        if ((call_0 == BT_CALL_INCOMING)
            || (call_1 == BT_CALL_INCOMING)
            )
        {
            if (!sys_cb.flag_2nd_ringtone_ongoing)
            {
                sys_cb.flag_2nd_ringtone_ongoing = true;
                msg_enqueue(EVT_2ND_RINGTONE);               
            }           
        }
        if ((call_0 != BT_CALL_INCOMING)
            && (call_1 != BT_CALL_INCOMING)
            )
        {
            if (sys_cb.flag_2nd_ringtone_ongoing)
            {
                sys_cb.flag_2nd_ringtone_ongoing = false;
                message_cancel_all(MSG_ID_2ND_RINGTONE);
            }            
        }
    }   
#endif    
}



void sfunc_bt_call_message_do(u16 msg)
{
    u8 call_status;

#if SWETZ_OUTCASE_AFTER_NOT_KEY
     if(sys_cb.flag_outcase_5s_kye_null == true && (msg != EVT_SYS_1S && msg != EVT_SYS_5S && (msg != EVT_UPDATE_INCASE_STA))){
        return;
     }       
#endif

#if SWETZ_RING_SEC_ROUTE
     if(msg == EVT_SYS_1S){
           // printf("sfunc_bt_call_message_do\r\n");
            app_2nd_ringtone_check();
     }

#endif


    switch (msg) {

    case EVT_2ND_RINGTONE:
        if (sys_cb.flag_2nd_ringtone_ongoing)    
            {
                message_send(MSG_ID_2ND_RINGTONE, 0, 5000);
                bsp_res_play(TWS_RES_UGRING); 
            }
        break;

    case KU_HOME:
    case KL_HOME:
        if (bt_get_siri_status()) {
            bt_call_terminate();                        //结束SIRI
        }
        break;

    ///挂断当前通话，或者结束当前通话并接听第2路通话
    case KU_HSF:
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
#if !SWETZ_DEL_CALL_SHORT_FUC    
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_answer_incoming();                  //接听第2路通话
        } else {
            bt_call_terminate();                        //挂断当前通话
        }
#endif        
        break;

    ///拒接第2路通话, 或私密接听切换
    case KL_PLAY_PWR_USER_DEF:
        sys_cb.poweron_flag = 1;                        //PWRKEY松开前不产生KHL_PLAY_PWR消息。按键松开自动清此标志。
    case KL_PLAY_USER_DEF:
        printf("KL_PLAY_USER_DEF\r\n");
        if (user_def_key_msg(xcfg_cb.user_def_kl_sel)) {
            break;
        }
    case KL_HSF:
        printf("KL_HSF\r\n");
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_terminate();                        //拒接第2路通话
        }
        break;

    ///保持当前通话并接通第2路通话，或者2路通话切换
    case KD_PLAY_PWR_USER_DEF:
    case KD_PLAY_USER_DEF:
        printf("KD_PLAY_USER_DEF\r\n");
        if (user_def_key_msg(xcfg_cb.user_def_kd_sel)) {
            break;
        }
    case KD_HSF:
         printf("KD_HSF\r\n");
#if SWETZ_CALL_DOUBLE
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_answer_incoming();                  //接听第2路通话
        } else {
            bt_call_terminate();                        //挂断当前通话
        }
#else 
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_answer_incoming();                  //接听第2路通话
        } else if(call_status >= BT_CALL_ACTIVE) {
            bt_call_swap();                             //切换两路通话或切换私密通话
        }
#endif
        break;

    ///三击按键处理
    case KTH_PLAY_USER_DEF:
    case KTH_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kt_sel);
        break;

    ///四击按键处理
    case KFO_PLAY_USER_DEF:
    case KFO_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfour_sel);
        break;

    ///五击按键处理
    case KFI_PLAY_USER_DEF:
    case KFI_PLAY_PWR_USER_DEF:
        user_def_key_msg(xcfg_cb.user_def_kfive_sel);
        break;

    case EVT_HFP_SET_VOL:
        if(sys_cb.incall_flag & INCALL_FLAG_SCO) {
            bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
            sys_cb.cm_times = 0;
            sys_cb.cm_vol_change = 1;
        }
        break;

    case EVT_A2DP_MUSIC_PLAY:
        dac_fade_in();
        break;

    case MSG_SYS_1S:
        bt_hfp_report_bat();
        break;

    default:
        func_message(msg);
        break;
    }
}

void sfunc_bt_call_message(u16 msg)
{
    if(msg){
        sfunc_bt_call_message_do(msg);
    }
}

#if FUNC_BTHID_EN

AT(.text.func.bt.msg)
void func_bthid_message_do(u16 msg)
{
    switch (msg) {
    case KU_PLAY:
    case KU_PLAY_USER_DEF:
    case KU_PLAY_PWR_USER_DEF:
        if (bt_get_status() < BT_STA_DISCONNECTING) {
            bt_connect();
        } else {
            bsp_bt_hid_photo(HID_KEY_VOL_UP);
            mp3_res_play(RES_BUF_TAKE_PHOTO_MP3,RES_LEN_TAKE_PHOTO_MP3);
        }
        break;

    case KL_PLAY:
        bt_disconnect(0);
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.bt.msg)
void func_bthid_message(u16 msg)
{
    if(msg){
        func_bthid_message_do(msg);
    }
}
#endif
