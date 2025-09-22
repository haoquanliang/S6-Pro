#ifndef _API_USB_H
#define _API_USB_H

enum {
    USBCHK_ONLY_HOST,
    USBCHK_ONLY_DEVICE,
    USBCHK_OTG,
};

enum {
    USB_DISCONNECT,
    USB_UDISK_CONNECTED,
    USB_PC_CONNECTED,
};


//USB Device HID Report id
#define REPORT_ID1              0x01
#define REPORT_ID2              0x02
//USB Device HID Comsumer usage id
#define UDE_HID_PLAYPAUSE       BIT(0)                          //0xcd
#define UDE_HID_VOLUP           BIT(1)                          //0xe9
#define UDE_HID_VOLDOWN         BIT(2)                          //0xea
#define UDE_HID_RESV            BIT(3)                          //0xcf
#define UDE_HID_HOME            BIT(4)                          //0x40
#define UDE_HID_NEXTFILE        BIT(5)                          //0xb5
#define UDE_HID_PREVFILE        BIT(6)                          //0xb6
#define UDE_HID_MUTE            BIT(7)                          //0xe2

void usb_init(void);                                            //初始化使能
void usb_disable(void);                                         //关闭
u8 usbchk_connect(u8 mode);                             		//usb旧检测方式
u8 usb_connect(void);                                   		//usb新检测方式
void udisk_remove(void);                                        //移除USB设备
void udisk_insert(void);                                        //插入USB设备使能
void udisk_invalid(void);                                       //usb状态还原
void usb_bc_init(u8 set);                                       //usb bc控制
void udh_dset_valid(u32 set_val);                               //控制usb hid使能,set_val:单位秒

u8 get_device_addr(void);                                       //获取主机分配的接口地址编号
void udisk_suspend(void);                                       //挂起USB设备
void udisk_resume(void);                                        //恢复挂起的设备

void usb_device_enter(u8 enum_type);                            //USB_DEV功能使能 enum_type：UDE_STORAGE_EN...
void usb_device_exit(void);                                     //退出USB_DEV
void usb_device_process(void);                                  //USB设备主循环
void ude_tmr_isr(void);                                         //USB中断
void pc_remove(void);                                           //USB从机退出

bool usb_device_hid_send(u8 report_id, u16 hid_val, u16 rls_time);  //usb hid发送, rls_time: 0-disable, 1~0xffff-自动释放时间,单位5ms
bool ude_hid_buf_send(u8 *buf, u8 len);					        //发送hid buff

u8 ude_cfgval_get(void);                                        //获取USB枚举开始标志
u8 get_mobile_sta(void);                                        //获取主机状态,1-安卓手机

void uda_get_spk_mute(u8 *buf);                                 //获取speaker mute
void uda_set_spk_mute(u8 val);                                  //设置speaker mute
u16 uda_get_spk_volume(u8 ch);                                  //获取speaker 0-L,1-R,2-LR音量值 
u8 uda_get_spk_spr(void);                                       //获取speaker当前采样率
u8 uad_get_spk_bits(void);                                      //获取speaker当前bits长度

void uda_get_mic_mute(u8 *buf);                                 //获取mic mute
void uda_set_mic_mute(u8 val);                                  //设置mic mute
u8 uda_get_mic_spr(void);                                       //获取mic当前采样率
u16 uda_get_mic_volume(u8 ch);                                  //获取mic音量
u8 uad_get_mic_bits(void);                                      //获取mic当前bits长度
void set_mic_disframe(u16 discard_time);                        //设置mic开启时丢弃数据的时长
#endif // _API_USB_H
