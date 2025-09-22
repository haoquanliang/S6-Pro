/*
 *  api_tws.h
 *
 *  Created by zoro on 2022-9-14.
 */
#ifndef __API_TWS_H
#define __API_TWS_H


//--------------------------------------------------------------------------------------------
// TWS同步TICK用法
// a)tickn为本地时钟产生的1ms计数器，tick为tws同步后的1ms计数器
// b)tick是在tws连接上后同步得到的计数值，仅在tws连接状态有效
// c)tick用于tws之间发命令进行具有时间同步的操作
// d)作为发送方，用tws_time_get获取tickn和tickoff，计算出tick发送给对方，其中tickn用于本地执行
// e)作为接收方，收到tick后，用tws_time_get获取tickoff，计算出tickn，用于本地执行


//--------------------------------------------------------------------------------------------
#define TWS_TICK_PR                             (655360000/1000)    //TICK周期655360ms
#define TWS_TICK_MAX_DELAY                      (TWS_TICK_PR/2)         //TICK最大超时时间

#define TWS_TICK_ADD(tick_a, tick_b)            ((uint32_t)(((tick_b) + (tick_a)) % TWS_TICK_PR))               //tick_a + tick_b
#define TWS_TICK_SUB(tick_a, tick_b)            ((uint32_t)(((tick_a) + TWS_TICK_PR - (tick_b)) % TWS_TICK_PR)) //tick_a - tick_b
#define TWS_TICK_GEQ(tick_a, tick_b)            (TWS_TICK_SUB(tick_a, tick_b) < TWS_TICK_MAX_DELAY)             //tick_a >= tick_b

#define TWS_TICK_2_TICKN(tick, tickoff)         TWS_TICK_SUB(tick, tickoff)
#define TWS_TICKN_2_TICK(tickn, tickoff)        TWS_TICK_ADD(tickn, tickoff)

typedef struct {
    uint32_t tickn;             //本地tick
    uint32_t tickoff;           //偏移量，tickoff = TICK_SUB(sync_time, tickn)
} tws_time_t;

uint32_t tws_time_tickn_get(void);                          //获取本地tickn
bool tws_time_tickn_expire(uint32_t tickn);                 //查询tickn时刻是否到达

uint32_t tws_time_get(tws_time_t *time, uint32_t delay);    //获取tws时间，同时获取tickoff
                                                            //*time返回time->clkn = curr.clkn + delay, 返回值为time->clkn转换的tick
                                                            //注意：delay大于TICK_MAX_DELAY会影响tws_time_tickn_expire，尽量小于TICK_MAX_DELAY
uint32_t tws_time_tick2n(uint32_t tick);                    //将tws同步tick转换为本地tickn
uint32_t tws_time_n2tick(uint32_t tickn);                   //将tws本地tickn转换为同步tick


bool tws_alarm_add(uint32_t tickn, uint32_t params);
bool tws_alarm_is_full(void);

#endif // __API_TWS_H
