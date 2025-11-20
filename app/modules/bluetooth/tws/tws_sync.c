/*
 *  tws_sync.c
 *
 *  Created by zoro on 2029-9-20.
 */
#include "include.h"


#if BT_TWS_EN


#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printk(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


void dev_vol_alarm_cb(uint16_t params);
uint16_t dev_vol_req_cb(uint8_t a2dp_vol, uint8_t feat, bool remote);


AT(.com_text.tws.alarm)
void tws_time_proc_cb(u32 tickn)
{
    tws_res_play_kick(tickn);
}

#if SWETZ_SYNC_EVT
AT(.com_text.tws.alarm)
void user_alarm_cb(uint16_t params)
{
#if SWETZ_ARM_PRINTF
    printf(str,params);
#endif    
    // if(params == 1){
    //     msg_enqueue(EVT_USER_SYNC_EVT);
    // }
    // if(params == 2){
    //     msg_enqueue(EVT_USER_RING_EXIT);
    // }
}
#endif

AT(.com_rodata.alarm.str)
const char str_alarm[] = "alarm_cb: %d(%04x)\n";

AT(.com_text.tws.alarm)
void tws_time_alarm_cb(uint32_t params, uint32_t alarm_tickn)
{
    uint32_t cmd = (params >> 28);
    params = (params & 0xffff);

    if(cmd == ALARM_RES) {
        tws_res_set_alarm_lable(params, alarm_tickn);
    } else if(cmd == ALARM_VOL) {
        dev_vol_alarm_cb(params);
    }
#if ABP_EN
    else if (cmd == ALARM_ABP) {
    #if BT_TWS_EN
        if (params == 0x10) {
            abp_start_fade_in();
        } else
    #endif // BT_TWS_EN
        {
            abp_stop_fade_out();
            sys_cb.abp_mode = params;
            msg_enqueue(EVT_ABP_SET);
        }
    }
#endif // ABP_EN
#if SWETZ_SYNC_EVT
    else if (cmd == ALARM_USER)
    {
            //user_alarm_cb(params);
        if (params == USER_SYNC_EVT_DBB_ON)
        {
                msg_enqueue(EVT_USER_SYNC_DBB_ON);
        }
        else if (params == USER_SYNC_EVT_DBB_OFF)
        {
                msg_enqueue(EVT_USER_SYNC_DBB_OFF);
        } 
        else if(params == 1){
                msg_enqueue(EVT_USER_SYNC_EVT);
        }
        else if(params == 2){
                msg_enqueue(EVT_USER_RING_EXIT);
        }              
    }
 
#endif

    TRACE(str_alarm, cmd, params);
}

void tws_local_alarm_req_cb(uint32_t tickn, uint16_t param)
{
    uint32_t cmd    = (param >> 12);
    uint32_t params = (param & 0xfff);

    if(cmd == ALARM_VOL) {
        params = dev_vol_req_cb(params&0xff, (params>>8), false);
    }

    tws_alarm_add(tickn, (cmd<<28) | params);

//    tws_time_t time;
//    tws_time_get(&time, 0);
//    TRACE("alarm_local  %d(%04x): %08x[%08x,%08x]\n", cmd, params, TICK_ADD(tickn, time.tickoff), tickn, time.tickn);
}

void tws_remote_alarm_req_cb(uint32_t tick, uint16_t param)
{
    tws_time_t time;
    uint32_t inst_tickn;
    uint32_t cmd    = (param >> 12);
    uint32_t params = (param & 0xfff);

    if(cmd == ALARM_VOL) {
        params = dev_vol_req_cb(params&0xff, (params>>8), true);
    }

    tws_time_get(&time, 0);
    inst_tickn = TICK_SUB(tick, time.tickoff);

    tws_alarm_add(inst_tickn, (cmd<<28) | params);

    if(bt_tws_is_slave()) {
        if(cmd == ALARM_RES) {
            tws_res_set_req_lable(params);
        }
    }

//    TRACE("alarm_remote %d(%04x): %08x[%08x,%08x]\n", cmd, params, tick, inst_tickn, time.tickn);
}

//----------------------------------------------------------------------------
////同步提示音播放
//bool bsp_tws_res_play(uint8_t res_idx)
//{
//    return tws_res_add(res_idx, RES_FLAG_TWS);
//}
//
//同步音量调节
//void bsp_tws_set_vol(u8 vol)
//{
//}

//同步LED显示
void bsp_tws_set_led(const void *cfg)
{
    if(bt_tws_is_connected()) {
        if(!bt_tws_is_slave()) {
            led_cal_sync_info(cfg);         //设置显示缓存，通过回调函数显示
            bt_tws_sync_led();              //更新到副耳
        } else {
            if(!bt_nor_is_connected()) {
                led_set_sta_normal(cfg);
            }
        }
    } else {
        led_set_sta_normal(cfg);
    }
}

#endif // BT_TWS_EN
