#ifndef _FUNC_H
#define _FUNC_H

#include "func_bt.h"
#include "func_bt_hid.h"
#include "func_bt_dut.h"
#include "func_idle.h"
#include "func_music.h"
#include "func_usbdev.h"
#include "func_aux.h"
#include "func_speaker.h"
#include "func_lowpwr.h"
#include "func_update.h"

//task number
enum {
    FUNC_NULL = 0,
    FUNC_MUSIC,
    FUNC_FMRX,
    FUNC_CLOCK,
    FUNC_BT,
    FUNC_BTHID,
    FUNC_USBDEV,
    FUNC_AUX,
    FUNC_SPDIF,
    FUNC_SPEAKER,
    FUNC_PWROFF,
    FUNC_CHARGE,
    FUNC_BT_DUT,                        //CBT测试模式
    FUNC_BT_FCC,                        //FCC测试模式
    FUNC_BT_IODM,                       //IODM测试模式
    FUNC_IDLE,
    FUNC_I2S,
};

//task control block
typedef struct {
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
#if BT_BACKSTAGE_EN
    u8 sta_break;                                   //被中断的任务
#endif
    void (*mp3_res_play)(u32 addr, u32 len);        //各任务的语音播报函数接口
    void (*set_vol_callback)(u8 dir);               //设置音量的回调函数，用于各任务的音量事件处理。
} func_cb_t;
extern func_cb_t func_cb;

extern const u8 func_sort_table[];     //任务切换排序table

u8 get_funcs_total(void);
void func_process(void);
void func_message(u16 msg);

void func_run(void);
void func_music(void);
void func_idle(void);
void func_clock(void);
void func_fmrx(void);
void func_bt(void);
void func_bthid(void);
void func_usbdev(void);
void func_aux(void);
void func_charge(void);

#if FUNC_SPEAKER_EN
void func_speaker(void);
#endif // FUNC_SPEAKER_EN

void func_exspifalsh_music(void);
void func_bt_warning(void);
#if SWETZ_INCASE_MUTE
void app_check_mute(void);
#endif


#endif // _FUNC_H
