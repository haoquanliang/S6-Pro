#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_USBDEV_EN
AT(.text.func.usbdev.msg)
void func_usbdev_message(u16 msg)
{
    switch (msg) {
        //按下PP
        case K_PLAY:
            #if USB_IPHONE_PREMOTE_EN
            if(!xcfg_cb.user_key_multi_press_en){
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 0);
            }
            #endif
        break;

        //播放/暂停，接听
        case KU_PLAY:
            #if USB_IPHONE_PREMOTE_EN
                if(!xcfg_cb.user_key_multi_press_en){
                    usb_device_hid_send(REPORT_ID1, 0, 0);
                }else{
                    usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
                }
            #else
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
            #endif
            break;
        
        //拒接电话, 语音助手
        case KL_PLAY:
            usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 0);
            usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 0);
            break;

        //长按抬键
        case KLU_PLAY:
            usb_device_hid_send(REPORT_ID1, 0, 0);
            break;

        case KU_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            usb_device_hid_send(REPORT_ID1, UDE_HID_MUTE, 1);
            break;

        //windows上一曲
        case KTH_PLAY:
        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
            if(!get_mobile_sta()){
                usb_device_hid_send(REPORT_ID1, UDE_HID_PREVFILE, 1);
            }else{
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
                delay_5ms(1);
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
                delay_5ms(1);
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
            }
            break;

        //windows下一曲
        case KD_PLAY:
        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
            if(!get_mobile_sta()){
                usb_device_hid_send(REPORT_ID1, UDE_HID_NEXTFILE, 1);
            }else{
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
                delay_5ms(1);
                usb_device_hid_send(REPORT_ID1, UDE_HID_PLAYPAUSE, 1);
            }
            break;

        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
            usb_device_hid_send(REPORT_ID1, UDE_HID_VOLDOWN, 5);
            break;

        case KU_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
            usb_device_hid_send(REPORT_ID1, UDE_HID_VOLUP, 5);
            break;

        case KU_STOP:
            // usb_device_hid_send(REPORT_ID1, UDE_HID_STOP, 1);
            break;

#if UDE_STORAGE_EN
        case EVT_SD_INSERT:
            ude_sdcard_switch(DEV_SDCARD);
            break;

        case EVT_SD_REMOVE:
            break;
#endif // UDE_STORAGE_EN

        case EVT_PC_REMOVE:
            func_cb.sta = func_cb.last;
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif
