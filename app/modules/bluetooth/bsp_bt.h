#ifndef _BSP_BT_H
#define _BSP_BT_H

//标准HID键
#define HID_KEY_ENTER       0x28

//自定义HID键
#define HID_KEY_VOL_DOWN    0x00EA
#define HID_KEY_VOL_UP      0x00E9
#define HID_KEY_IOS_HOME    0x0040      //ios home
#define HID_KEY_IOS_POWER   0x0030      //ios 锁屏
#define HID_KEY_IOS_LAYOUT  0x01AE      //ios Keyboard Layout
typedef enum
{
    IMG_POINT_UP = 0,           //上滑
    IMG_POINT_DOWN,             //下滑
    IMG_POINT_LEFT,             //左滑
    IMG_POINT_RIGHT,            //右滑
    IMG_POINT_DD,
}img_point_type;

void bsp_bt_init(void);
void bsp_bt_close(void);
void bsp_bt_vol_change(void);
void bsp_bt_vol_set(uint8_t vol);
bool bsp_bt_hid_photo(u16 keycode);
bool bsp_bt_hid_vol_change(u16 keycode);
bool bsp_bt_pwrkey5s_check(void);
void bsp_bt_pwrkey5s_clr(void);
void bsp_bt_hid_tog_conn(void);
void bsp_bt_hid_screen_left(void);
void bsp_bt_hid_screen_right(void);
void bsp_bt_msg_nr_sta_change(void);
uint bsp_bt_get_hfp_vol(uint hfp_vol);
void bsp_bt_call_volume_msg(u16 msg);

void bt_redial_init(void);
void bt_reset_redial_number(uint8_t index);
void bt_update_redial_number(uint8_t index, char *buf, u32 len);
const char *bt_get_last_call_number(uint8_t index);
bool bt_tws_get_channel_cfg(uint8_t *channel);
bool bt_tws_get_channel(uint8_t *channel);
void bt_tws_clr_link_info(void);
void bt_clr_all_link_info(void);
void bt_clr_master_addr(void);
const char *bt_get_local_name(void);
//bt hid
void user_finger_down(void);                    //向下滑
void user_finger_up(void);                      //向上滑
void user_finger_p(void);                       //单击
void user_finger_pp(void);                      //双击
void bt_hid_point_user(img_point_type type);    //视频APP，翻页等功能, img_point_type 选择动作

extern uint8_t  cfg_bt_tws_tick_freq;

#if SWETZ_UART_CMD
void bt_get_fixed_bd_addr(u8 *addr);
#endif 

#endif //_BSP_BT_H
