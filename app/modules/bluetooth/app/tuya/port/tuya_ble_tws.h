#ifndef __TUYA_BLE_TWS_H
#define __TUYA_BLE_TWS_H

typedef enum
{
    TWS_INFO_ALL = 0,
    TWS_INFO_ALL_RSP,
    TWS_INFO_EQ,
    TWS_INFO_VBAT,
    TWS_INFO_OTA,
    TWS_INFO_TUYA_SYS_SETTING,
    TWS_INFO_RST_FACTORY,
    TWS_INFO_BT_NAME,
    TWS_INFO_PLAY_STA,
    TWS_INFO_DEVICE_UNBOND,
    TWS_INFO_BOND_STA,
    TWS_INFO_KEY,
    TWS_INFO_KEY_EN,
    TWS_INFO_BT_RESET,
    TWS_INFO_KEY_RST,
}tuya_tws_info_id_t;

#if BT_TWS_EN

extern u8 tuya_ble_tws_data[250];
extern u16 tuya_ble_tws_data_len;

void tuya_ble_tws_data_sync(void);
void tuya_ble_tws_recv_proc(uint8_t *data_ptr, u16 size);
void tuya_ble_tws_eq_sync(void);
void tuya_ble_tws_info_all_sync(void);
void tuya_ble_tws_sys_settings_sync(void);
void tuya_ble_tws_vbat_exchange(void);
void tuya_ble_tws_reset_sync(void);
void tuya_ble_tws_bt_name_sync(void);
void tuya_ble_tws_play_sta_sync(void);
bool tuya_ble_tws_is_auth_master(void);
void tuya_ble_tws_device_unbond(void);
void tuya_ble_tws_bond_sta_sync(void);
void tuya_ble_tws_key_info_sync(void);
void tuya_ble_tws_key_en_sync(void);
void tuya_ble_tws_bt_reset_sync(void);
void tuya_ble_tws_key_reset_sync(void);

#else
#define tuya_ble_tws_data_sync()
#define tuya_ble_tws_recv_proc(data_ptr,size)
#define tuya_ble_tws_eq_sync()
#define tuya_ble_tws_info_all_sync()
#define tuya_ble_tws_sys_settings_sync()
#define tuya_ble_tws_vbat_exchange()
#define tuya_ble_tws_reset_sync()
#define tuya_ble_tws_bt_name_sync()
#define tuya_ble_tws_play_sta_sync()
#define tuya_ble_tws_is_auth_master()
#define tuya_ble_tws_device_unbond()
#define tuya_ble_tws_bond_sta_sync()
#define tuya_ble_tws_key_info_sync()
#define tuya_ble_tws_key_en_sync()
#define tuya_ble_tws_bt_reset_sync()
#define tuya_ble_tws_key_reset_sync()

#endif

#endif
