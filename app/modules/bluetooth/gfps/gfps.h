#ifndef __GFPS_H
#define __GFPS_H

#include "include.h"

//CM Param
#define PARAM_PERSONALIZED_NAME_LEN     0           // 1byte
#define PARAM_PERSONALIZED_NAME         1           // 64 Bytes
#define PARAM_ACCOUNT_KEY               0x41        // 92 bytes, sizeof(struct account_key_list), 5 keys

//Define in libs, Modify the library file synchronously
//*******************************************************************************************/
#define ACCOUNT_KEY_LENGTH      16
#define ACCOUNT_KEY_MAX_COUNT   5 // 至少5个

typedef struct account_key_list {
    uint8_t keys[ACCOUNT_KEY_LENGTH * ACCOUNT_KEY_MAX_COUNT]; // each account key has 16 bytes
    uint16_t use_freq[ACCOUNT_KEY_MAX_COUNT]; // 用来记录使用频次
    uint8_t count;
} *account_key_list_t;

enum{
    GFPS_MODE_NONE = 0,
    GFPS_MODE_DISCOVER,
    GFPS_MODE_NON_DISCOVER,
};
//*******************************************************************************************/


void *gfps_get_account_key_info();   // 获取账户秘钥结构体指针，第一个字节会放置结构体大小，开机调用后要读取flash信息覆盖

void gfps_adv_to_discoverable(void);
void gfps_adv_to_non_discoverable(void);
u8 gfps_get_cur_adv_mode(void);

void gfps_spp_connected_callback(void);
void gfps_spp_connected_proc(void);
void gfps_spp_recv_callback(uint8_t *data, uint16_t data_len);
void gfps_spp_receive_proc(uint8_t *data, uint16_t data_len);

void gfps_ble_disconnected_callback(void);
void gfps_ble_disconnected_proc(void);
void gfps_ble_connected_callback(void);
void gfps_ble_connected_proc(void);

void gfps_bt_disconnected_callback(void);
void gfps_bt_connected_callback(void);
void gfps_bt_disconnected_proc(void);
void gfps_bt_connected_proc(void);

void gfps_init();
void gfps_bt_init(void);
bool gfps_need_wakeup(void);
void gfps_evt_notice(u8 evt, void *params);
void gfps_process(void);
uint16_t gfps_tws_get_data(uint8_t *buf);
void gfps_tws_set_data(uint8_t *data, uint16_t len);
void gfps_vhouse_cmd_notice(u8 cmd);
bool gfps_is_enable(void);

/// 消息流
void gfps_send_msg_bluetooth_silence_mode(bool silent); // 启用/禁用静音模式后调用，发送状态给手机。使用场景，耳机从头上拿下/戴上

// Device Information Event消息，电池电量更新，电量变化时发送，根据每个组件状态传参
void gfps_send_msg_dev_info_battery(uint8_t left_value, bool left_charging,uint8_t right_value, bool right_charging,uint8_t case_value, bool case_charging);
void gfps_send_msg_dev_info_battery_updated2(uint8_t *battery_data,uint8_t battery_data_len);

// Device Action Event消息，响铃，同步响铃状态到手机使用
void gfps_send_msg_device_action_ring(bool ring);                        // 单个组件
void gfps_send_msg_tws_device_action_ring(bool left_ring, bool right_ring); // 两个组件（对耳）

#endif // __GFPS_H

