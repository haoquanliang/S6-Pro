/*
 *  popup.h
 *
 *  Created by zoro on 30/8/2022.
 */
#ifndef __POPUP_H
#define __POPUP_H

//弹窗配对相关接口
#if GFPS_EN
#include "gfps.h"
    #define le_popup_init()                     gfps_init()
    #define le_popup_process()                  gfps_process()
    #define le_popup_evt_notice(a,b)            gfps_evt_notice(a,b)
    #define le_popup_vhouse_cmd_notice(a)       gfps_vhouse_cmd_notice(a)
    #define le_popup_need_wakeup()              gfps_need_wakeup()
#else
    #define le_popup_init()
    #define le_popup_process()
    #define le_popup_evt_notice(a,b)
    #define le_popup_vhouse_cmd_notice(a)
    #define le_popup_need_wakeup()              0
#endif

#endif // __POPUP_H
