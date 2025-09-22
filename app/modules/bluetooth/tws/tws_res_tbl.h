/*
 *  tws_res.h
 *
 *  Created by zoro on 2029-9-20.
 */
#ifndef __TWS_RES_TBL_H
#define __TWS_RES_TBL_H


enum {
    //一般由主耳发起的提示音
    TWS_RES_NUM_0,
    TWS_RES_NUM_1,
    TWS_RES_NUM_2,
    TWS_RES_NUM_3,
    TWS_RES_NUM_4,
    TWS_RES_NUM_5,
    TWS_RES_NUM_6,
    TWS_RES_NUM_7,
    TWS_RES_NUM_8,
    TWS_RES_NUM_9,
    TWS_RES_RING,
    TWS_RES_CONNECTED,          //STA
    TWS_RES_DISCONNECT,         //STA
    TWS_RES_PAIRING,            //STA

    //副耳也可以请求的提示音
    TWS_RES_LOW_BATTERY,        //STA
    TWS_RES_MAX_VOL,            //STA
    TWS_RES_LANGUAGE_EN,        //STA
    TWS_RES_LANGUAGE_ZH,        //STA

    TWS_RES_CALL_HANGUP,        //hangup
    TWS_RES_CALL_REJECT,        //reject
    TWS_RES_REDIALING,          //redial
    TWS_RES_MUSIC_MODE,         //music_mode
    TWS_RES_GAME_MODE,          //game_mode
    TWS_RES_NR_DISABLE,         //nr disable
    TWS_RES_ANC,                //anc
    TWS_RES_TRANSPARENCY,       //transparency
#if ASR_EN
    TWS_RES_ASR_ON,
    TWS_RES_ASR_OFF,
#endif
    TWS_RES_MAX,
};

u8 tws_res_get_type(u32 index);
void tws_res_get_addr(u32 index, u32 *addr, u32 *len);

#endif // __TWS_RES_TBL_H
