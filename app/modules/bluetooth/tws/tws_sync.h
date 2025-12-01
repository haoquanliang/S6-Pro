/*
 *  tws_sync.h
 *
 *  Created by zoro on 2029-9-20.
 */
#ifndef __TWS_SYNC_H
#define __TWS_SYNC_H


#include "tws_res.h"

#if SWETZ 
enum 
{
    USER_SYNC_EVT_SPATIAL_AUDIO_ON = 1,
    USER_SYNC_EVT_SPATIAL_AUDIO_OFF,
    USER_SYNC_EVT_LOW_LATENCY_OFF,
    USER_SYNC_EVT_DBB_ON,
    USER_SYNC_EVT_DBB_OFF,
    USER_SYNC_EVT_ANC_OFF,
    USER_SYNC_EVT_ANC_RESUME,
    USER_SYNC_EVT_RESET,//同步执行软复位
};
#endif 

#if BT_TWS_EN

void bsp_tws_set_led(const void *cfg);      //设置同步LED显示

//提示音API函数
#define bsp_res_init()                      tws_res_var_init()          //提示音初始化
#define bsp_res_process()                   tws_res_process()           //提示音处理循环，需要在主循环调用
#define bsp_res_set_enable(en)              tws_res_set_enable(en)      //是否使能提示音播放，关闭后可以加播放列表但不播放
#define bsp_res_set_break(en)               tws_res_set_break(en)       //设置打断副耳独立的提示音
#define bsp_res_is_full()                   res_list_is_full()          //提示音列表是否存满
#define bsp_res_is_empty()                  res_list_is_empty()         //提示音列表是否播空
#define bsp_res_is_playing()                tws_res_is_playing()        //是否有提示音正在播放
#define bsp_res_is_vol_busy()               tws_res_is_vol_busy()       //提示音是否正在控制音量
#define bsp_res_cleanup()                   tws_res_cleanup()           //清除提示音播放列表
#define bsp_res_search(idx)                 tws_res_search(idx)         //查找列表是否存在某个提示音
#define bsp_res_play(idx)                   tws_res_add(idx, RES_FLAG_TWS)    //添加一个提示音到播放列表，返回值：RES_ERR_NO_ERR/RES_ERR_LIST_FULL/RES_ERR_INVALID

#define bsp_res_tone_play(n)                bt_tws_req_alarm_tone(n)

#else // BT_TWS_EN

//提示音API函数
#define bsp_res_init()
#define bsp_res_process()
#define bsp_res_set_enable(en)
#define bsp_res_set_break(en)
#define bsp_res_is_full()                    false                      //提示音列表是否存满
#define bsp_res_is_empty()                   true                       //提示音列表是否播空
#define bsp_res_is_playing()                 false                      //是否有提示音正在播放
#define bsp_res_is_vol_busy()                false                      //提示音是否正在控制音量
#define bsp_res_cleanup()
#define bsp_res_search(idx)                  false                      //查找列表是否存在某个提示音
uint8_t bsp_res_play(uint8_t res_idx);                                  //播放一个提示音

#define bsp_res_tone_play(n)

#endif // BT_TWS_EN


#endif // __TWS_SYNC_H
