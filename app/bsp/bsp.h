#ifndef __BSP_H_
#define __BSP_H_

#include "bsp_i2c.h"
#include "bsp_timer.h"
#include "bsp_huart.h"
#include "bsp_uart.h"
#include "bsp_saradc.h"
#include "bsp_port_int.h"
#include "bsp_vad.h"
#include "bsp_iis.h"
#include "bsp_opus.h"
#include "bsp_asr.h"

#if BT_TWS_EN
    #define TICK_MAX_DELAY                  TWS_TICK_MAX_DELAY
    #define TICKN_GET()                     tws_time_tickn_get()
    #define TICKN_IS_EXPIRE(tickn)          tws_time_tickn_expire(tickn)

    #define TICK_TIME_GET(t, d)             tws_time_get(t, d)
    #define TICK_ADD(a, b)                  TWS_TICK_ADD(a, b)
    #define TICK_SUB(a, b)                  TWS_TICK_SUB(a, b)
    #define TICK_GEQ(a, b)                  TWS_TICK_GEQ(a, b)
    #define TICK_2_TICKN(tick, tickoff)     TWS_TICK_2_TICKN(tick, tickoff)
    #define TICKN_2_TICK(tickn, tickoff)    TWS_TICKN_2_TICK(tickn, tickoff)
#else
    #define TICK_MAX_DELAY                  0x7fffffff
    #define TICKN_GET()                     tick_get()
    #define TICKN_IS_EXPIRE(tickn)          (tickn - tick_get() >= TICK_MAX_DELAY)

    #define TICK_TIME_GET(t, d)             (t)->tickn = tick_get() + d
    #define TICK_ADD(a, b)                  (a + b)
    #define TICK_SUB(a, b)                  (a - b)
    #define TICK_GEQ(a, b)                  ((a - b) < TICK_MAX_DELAY)
    #define TICK_2_TICKN(tick, tickoff)     tick        //非TWS时tick和tickn相等
    #define TICKN_2_TICK(tickn, tickoff)    tickn       //非TWS时tick和tickn相等
#endif // BT_TWS_EN

#endif  // __BSP_H_

