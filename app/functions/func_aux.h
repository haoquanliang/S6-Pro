#ifndef _FUNC_AUX_H
#define _FUNC_AUX_H

typedef struct {
    u8  pause       :   1,
        rec_en      :   1,
        disp_update :   1;
    u8  skip_frame_cnt;      //aux启动时需要skip的帧数，等待模拟稳定
}func_aux_t;
extern func_aux_t f_aux;

void func_aux_pause_play(void);
void func_aux_mp3_res_play(u32 addr, u32 len);
void func_aux_message(u16 msg);
void func_aux_stop(void);
void func_aux_start(void);
void bsp_aux_mute(u8 ch);
void bsp_aux_unmute(u8 ch);

#if (GUI_SELECT != GUI_NO)
void func_aux_display(void);
void func_aux_exit_display(void);
void func_aux_enter_display(void);
#else
#define func_aux_display()
#define func_aux_exit_display()
#define func_aux_enter_display()
#endif


#endif // _FUNC_AUX_H
