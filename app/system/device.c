#include "include.h"

#if SD_SOFT_DETECT_EN

AT(.text.bsp.sys.init)
void sd_soft_detect_poweron_check(void)  //开机检测
{
    if (!SD_IS_SOFT_DETECT()) {  //配置工具中是否配置sd检测.
        return;
    }
    dev_delay_times(DEV_SDCARD, 1);  //检测到1次成功, 则认为SD在线.
    u8 i = 5;
    while(i--) {
        sd_soft_cmd_detect(0);
        if (dev_is_online(DEV_SDCARD)) {
            break;
        }
        delay_ms(10);
    }
    dev_delay_times(DEV_SDCARD, 3);
}

AT(.com_text.detect)
void sd_soft_cmd_detect(u32 check_ms) //check_ms 时间间隔检测一次.  //主循环中执行检测.
{
    static u32 check_ticks = 0;
    if (!SD_IS_SOFT_DETECT()) {  //配置工具中是否配置sd检测.
        return;
    }
    if (tick_check_expire(check_ticks, check_ms) || (0 == check_ticks)) {  //每隔100ms才检测一次.
        check_ticks = tick_get();
    }else {
        return;
    }
    if (sd_soft_detect()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
//            printf("sd soft insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
        }
    }
}

#endif

#if MUSIC_SDCARD_EN
AT(.com_text.detect)
void sd_detect(void)
{
    if ((!is_sd_support()) || (SD_DETECT_IS_BUSY())) {
        return;
    }

#if SD_SOFT_DETECT_EN
    if (SD_IS_SOFT_DETECT()) {
        return;
    }
#endif

    if (SD_IS_ONLINE()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
//            printf("sd insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
//            printf("sd remove\n");
        }
    }
}
#endif // MUSIC_SDCARD_EN

AT(.com_text.detect)
u8 get_usbtf_muxio(void)
{
#if SD_USB_MUX_IO_EN
	return 1;
#else
	return 0;
#endif
}

//AT(.com_text.const)
//const char usb_detect_str[] = "USB STA:%d\r\n";
//AT(.com_text.const)
//const char usb_insert_str[] = "udisk insert\n";
//AT(.com_text.const)
//const char usb_remove_str[] = "udisk remove\n";

#if USB_SUPPORT_EN
AT(.com_text.detect)
void usb_detect(void)
{
    if (!is_usb_support()) {
        return;
    }
    u8 usb_sta;
#if USB_DET_VER_SEL
    usb_sta = usb_connect();
#else
#if FUNC_USBDEV_EN
    usb_sta = usbchk_connect(USBCHK_OTG);
#else
    usb_sta = usbchk_connect(USBCHK_ONLY_HOST);
#endif
#endif

    if (usb_sta == USB_UDISK_CONNECTED) {
        if (dev_online_filter(DEV_UDISK)) {
            udisk_insert();
            msg_enqueue(EVT_UDISK_INSERT);
//            printf(usb_insert_str);
        }
    } else {
        if (dev_offline_filter(DEV_UDISK)) {
            udisk_remove();
            msg_enqueue(EVT_UDISK_REMOVE);
//            printf(usb_remove_str);
        }
    }

#if FUNC_USBDEV_EN
    if (usb_sta == USB_PC_CONNECTED) {
        if (dev_online_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_INSERT);
//            printf("pc insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_USBPC)) {
            msg_enqueue(EVT_PC_REMOVE);
            pc_remove();
//            printf("pc remove\n");
        }
    }
#endif
}
#endif // USB_SUPPORT_EN

#if LINEIN_DETECT_EN
AT(.com_text.detect)
void linein_detect(void)
{
    if (LINEIN_DETECT_IS_BUSY()) {
        return;
    }
    if (LINEIN_IS_ONLINE()) {
        if (dev_online_filter(DEV_LINEIN)) {
            msg_enqueue(EVT_LINEIN_INSERT);
//            printf("linein insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_LINEIN)) {
            msg_enqueue(EVT_LINEIN_REMOVE);
//            printf("linein remove\n");
        }
    }
}
#endif // LINEIN_DETECT_EN

#if USER_INEAR_DETECT_EN
AT(.com_text.detect)
void earin_detect(void)
{
    if (INEAR_IS_ONLINE()) {
        if (dev_online_filter(DEV_EARIN)) {
        }
    } else {
        if (dev_offline_filter(DEV_EARIN)) {
        }
    }
}
#endif

