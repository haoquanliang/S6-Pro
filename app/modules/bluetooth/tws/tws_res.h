/*
 *  tws_res.h
 *
 *  Created by zoro on 2029-9-20.
 */
#ifndef __TWS_RES_H
#define __TWS_RES_H


#include "tws_res_tbl.h"

#define ITEM_MAX_NB         8

//res_flag
enum {
    RES_FLAG_TWS        = BIT(0),
};

typedef struct {
    uint8_t res_idx;
    uint8_t res_lable   : 4;
    uint8_t res_flag    : 4;
} res_item_t;

typedef struct {
    res_item_t item[ITEM_MAX_NB];
    uint8_t wpos;
    uint8_t rpos;
    uint8_t len;
    uint8_t total;
	uint8_t add_len;
} res_list_t;

//state
enum {
    RES_STA_IDLE,
    RES_STA_WAIT_ALARM,
    RES_STA_AUDIO_BYPASS,
    RES_STA_WAIT_KICK,
    RES_STA_PROC,
    RES_STA_RES_EXIT,
    RES_STA_DELAY,
    RES_STA_DELAY_MORE,
    RES_STA_NEXT_RES_INIT,
    RES_STA_NEXT_WAIT_ALARM,
};

//play_err
enum {
    PLAY_ERR_BREAK      = BIT(0),
    PLAY_ERR_INVALID    = BIT(1),
    PLAY_ERR_FORMAT     = BIT(2),
};

typedef struct {
    uint8_t state;
    uint8_t res_idx;
    uint8_t res_type;
    uint8_t res_lable;
    uint8_t res_flag;
    uint8_t rd_lable;
    uint8_t req_idx;
    uint8_t req_lable;
    uint8_t alarm_lable;
    uint8_t play_err;
    uint8_t break_ring;
    bool kick_flag;
    bool done_flag;
    bool break_flag;
    bool enable;
    uint32_t tickn;

    uint32_t play_tickn;
    void (*play_kick_cb)(void);

    uint8_t dnr_sta;
    uint8_t dac_sta;
    uint8_t mute_bak;
} tws_res_t;

extern tws_res_t tws_res;
extern res_list_t res_list;


enum {
    RES_ERR_NO_ERR,
    RES_ERR_LIST_FULL,
    RES_ERR_INVALID,
};

#define res_list_is_empty()                     (bool)(res_list.len == 0)
#define res_list_is_full()                      (bool)(res_list.len >= ITEM_MAX_NB)

#define tws_res_set_alarm_lable(param, tickn)   tws_res.alarm_lable = ((param)>>8);\
                                                tws_res.play_tickn = TICK_ADD(tickn, 300)
#define tws_res_set_req_lable(param)            tws_res.req_idx = ((param) & 0xff);\
                                                tws_res.req_lable = ((param)>>8)
#define tws_res_set_enable(en)                  tws_res.enable = en
#define tws_res_set_break(en)                   tws_res.break_flag = en;\
                                                sys_cb.tws_res_brk = en
#define tws_res_reset_lable()                   tws_res.req_lable = 255;\
                                                tws_res.rd_lable = 255
#define tws_res_is_playing()                    (bool)(tws_res.state != RES_STA_IDLE)
#define tws_res_is_vol_busy()                   (bool)(tws_res.state != RES_STA_IDLE && tws_res.res_type == RES_TYPE_MP3)
#define tws_res_is_empty()                      res_list_is_empty()

void tws_res_var_init(void);                    //提示音初始化
void tws_res_process(void);                     //提示音处理循环，需要在主循环调用
void tws_res_play_kick(u32 tickn);              //提示音开始播放处理，需要tws_time_proc_cb调用
void tws_res_cleanup(void);                     //清除提示音播放列表
bool tws_res_search(uint8_t res_idx);           //查找列表是否存在某个提示音
uint8_t tws_res_add(uint8_t res_idx, uint8_t flag);           //添加一个提示音到播放列表，返回值：RES_ERR_NO_ERR/RES_ERR_LIST_FULL/RES_ERR_INVALID

#endif // __TWS_RES_H

