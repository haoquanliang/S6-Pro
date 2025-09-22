#ifndef _FUNC_USBDEV_H
#define _FUNC_USBDEV_H

typedef struct {
    u8 vol;
    u8 cur_dev;
    u8 dev_change;
    u8 rw_sta;
    volatile u8 uda_bypass;
} f_ude_t;
extern f_ude_t f_ude;

void func_usbdev_message(u16 msg);
void ude_sdcard_switch(u8 dev);
void ude_sdcard_change_process(void);
void src_var_init(void);
void uda_mic_alg_init(void);
void uda_mic_alg_exit(void);

#if (GUI_SELECT != GUI_NO)
void func_usbdev_display(void);
void func_usbdev_enter_display(void);
void func_usbdev_exit_display(void);
#else
#define func_usbdev_display()
#define func_usbdev_enter_display()
#define func_usbdev_exit_display()
#endif

#endif // _FUNC_USBDEV_H
