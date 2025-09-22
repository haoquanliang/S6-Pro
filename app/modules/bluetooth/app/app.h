/*
 *  bt_app.h
 *
 *  Created by shenpei on 2021-3-21.
 */
#ifndef __APP_H
#define __APP_H


//手机APP 控制协议相关接口
#if AB_MATE_APP_EN
#include "ab_mate/ab_mate_app.h"
    #define app_ctrl_ble_init()                          ab_mate_ble_app_init()
    #define app_ctrl_ble_disconnect_callback()           ab_mate_ble_disconnect_callback()
    #define app_ctrl_ble_connect_callback()              ab_mate_ble_connect_callback()
    #define app_ctrl_process()                           ab_mate_process()
    #define app_ctrl_var_init()                          ab_mate_var_init()
    #define app_ctrl_init_do()                           ab_mate_init_do()
    #define app_ctrl_eq_set()                            ab_mate_eq_set_do()
    #define app_ctrl_enter_sleep()                       ab_mate_enter_sleep()
    #define app_ctrl_exit_sleep()                        ab_mate_exit_sleep()
    #define app_ctrl_need_wakeup()                       ab_mate_system_need_wakeup()
    #define app_ctrl_bt_evt_notice(a,b)                  ab_mate_bt_evt_notice(a,b)
    #define app_ctrl_ble_role_switch_get_data(a)         ab_mate_ble_role_switch_get_data(a)
    #define app_ctrl_ble_role_switch_set_data(a,b)       ab_mate_ble_role_switch_set_data(a,b)
    #define app_ctrl_sleep_mode_process()
    #define app_ctrl_tws_user_key_process(a)
    #define app_ctrl_id3_tag_process(a,b,c)
#elif APP_DEMO_EN
#include "app_demo/app_demo.h"
    #define app_ctrl_ble_init()                        app_demo_gatt_app_init()
    #define app_ctrl_ble_disconnect_callback()
    #define app_ctrl_ble_connect_callback()
    #define app_ctrl_process()
    #define app_ctrl_var_init()
    #define app_ctrl_init_do()
    #define app_ctrl_eq_set()                            0
    #define app_ctrl_enter_sleep()
    #define app_ctrl_exit_sleep()
    #define app_ctrl_need_wakeup()                       0
    #define app_ctrl_bt_evt_notice(a,b)
    #define app_ctrl_ble_role_switch_get_data(a)         0
    #define app_ctrl_ble_role_switch_set_data(a,b)       0
    #define app_ctrl_sleep_mode_process()
    #define app_ctrl_tws_user_key_process(a)
    #define app_ctrl_id3_tag_process(a,b,c)
#elif LE_DUEROS_DMA_EN
#include "dueros_dma/dueros_dma_app.h"
    #define app_ctrl_ble_init()                          dueros_dma_ble_app_init()
    #define app_ctrl_ble_disconnect_callback()           dueros_dma_app_ble_disconnect_callback()
    #define app_ctrl_ble_connect_callback()              dueros_dma_app_ble_connect_callback()
    #define app_ctrl_process()                           dueros_dma_app_process()
    #define app_ctrl_var_init()                          dueros_dma_var_init()
    #define app_ctrl_init_do()                           dueros_dma_app_init()
    #define app_ctrl_eq_set()                            0
    #define app_ctrl_enter_sleep()                       dueros_dma_app_enter_sleep()
    #define app_ctrl_exit_sleep()                        dueros_dma_app_exit_sleep()
    #define app_ctrl_need_wakeup()                       dueros_dma_app_is_need_wakeup()
    #define app_ctrl_bt_evt_notice(a,b)                  dueros_dma_app_bt_evt_notice(a,b)
    #define app_ctrl_ble_role_switch_get_data(a)         0
    #define app_ctrl_ble_role_switch_set_data(a,b)       0
    #define app_ctrl_sleep_mode_process()
    #define app_ctrl_tws_user_key_process(a)             dueros_dma_tws_user_key_process(a)
#elif LE_TUYA_EN
#include "tuya_ble_profile.h"
#include "tuya_ble_app.h"
    #define app_ctrl_ble_init()                          tuya_ble_app_ctrl_init()
    #define app_ctrl_ble_disconnect_callback()           tuya_ble_disconnected_handler()
    #define app_ctrl_ble_connect_callback()              tuya_ble_connected_handler()
    #define app_ctrl_process()                           tuya_ble_process()
    #define app_ctrl_var_init()                          tuya_ble_var_init()
    #define app_ctrl_init_do()                           tuya_ble_app_init()
    #define app_ctrl_eq_set()                            tuya_ble_eq_set()
    #define app_ctrl_enter_sleep()                       tuya_ble_enter_sleep()
    #define app_ctrl_exit_sleep()                        tuya_ble_exit_sleep()
    #define app_ctrl_need_wakeup()                       tuya_ble_data_receive_flag_get()
    #define app_ctrl_bt_evt_notice(a,b)                  tuya_bt_evt_notice(a,b)
    #define app_ctrl_ble_role_switch_get_data(a)         tuya_ble_role_switch_get_data(a)
    #define app_ctrl_ble_role_switch_set_data(a,b)       tuya_ble_role_switch_set_data(a,b)
    #define app_ctrl_sleep_mode_process()                tuya_ble_sleep_mode_process()
    #define app_ctrl_tws_user_key_process(a)
    #define app_ctrl_id3_tag_process(a,b,c)
#else
    #define app_ctrl_ble_init()
    #define app_ctrl_ble_disconnect_callback()
    #define app_ctrl_ble_connect_callback()
    #define app_ctrl_process()
    #define app_ctrl_var_init()
    #define app_ctrl_init_do()
    #define app_ctrl_eq_set()                            0
    #define app_ctrl_enter_sleep()
    #define app_ctrl_exit_sleep()
    #define app_ctrl_need_wakeup()                       0
    #define app_ctrl_bt_evt_notice(a,b)
    #define app_ctrl_ble_role_switch_get_data(a)         0
    #define app_ctrl_ble_role_switch_set_data(a,b)       0
    #define app_ctrl_sleep_mode_process()
    #define app_ctrl_tws_user_key_process(a)
    #define app_ctrl_id3_tag_process(a,b,c)
#endif


#if AB_MATE_APP_EN
#include "ab_mate/ab_mate_app.h"
#ifdef AB_MATE_SPP_UUID
    #define app_ctrl_spp_disconnect_callback()
    #define app_ctrl_spp_connect_callback()
    #define app_ctrl_spp_rx_callback(a,b)                0

    #define app_ctrl_spp2_disconnect_callback()          ab_mate_spp_disconnect_callback()
    #define app_ctrl_spp2_connect_callback()             ab_mate_spp_connect_callback()
    #define app_ctrl_spp2_rx_callback(a,b)               ab_mate_receive_proc(a,b,AB_MATE_CON_SPP)
#else
    #define app_ctrl_spp_disconnect_callback()           ab_mate_spp_disconnect_callback()
    #define app_ctrl_spp_connect_callback()              ab_mate_spp_connect_callback()
    #define app_ctrl_spp_rx_callback(a,b)                ab_mate_receive_proc(a,b,AB_MATE_CON_SPP)

    #define app_ctrl_spp2_disconnect_callback()
    #define app_ctrl_spp2_connect_callback()
    #define app_ctrl_spp2_rx_callback(a,b)               0
#endif

    #define app_ctrl_spp1_disconnect_callback()
    #define app_ctrl_spp1_connect_callback()
    #define app_ctrl_spp1_rx_callback(a,b)               0
#elif LE_DUEROS_DMA_EN
#include "dueros_dma/dueros_dma_app.h"
    #define app_ctrl_spp_disconnect_callback()
    #define app_ctrl_spp_connect_callback()
    #define app_ctrl_spp_rx_callback(a,b)               0

    #define app_ctrl_spp1_disconnect_callback()         dueros_dma_spp_disconnect_callback()
    #define app_ctrl_spp1_connect_callback()            dueros_dma_spp_connect_callback()
    #define app_ctrl_spp1_rx_callback(a,b)              dueros_dma_recv_proc(a,b,DUEROS_DMA_CON_SPP)

    #define app_ctrl_spp2_disconnect_callback()
    #define app_ctrl_spp2_connect_callback()
    #define app_ctrl_spp2_rx_callback(a,b)              0
#elif APP_DEMO_EN
    #define app_ctrl_spp_disconnect_callback(a)
    #define app_ctrl_spp_connect_callback(a)
    #define app_ctrl_spp_rx_callback(a,b)              app_demo_spp_rx_callback(a,b)

    #define app_ctrl_spp1_disconnect_callback(a)
    #define app_ctrl_spp1_connect_callback(a)
    #define app_ctrl_spp1_rx_callback(a,b)             0

    #define app_ctrl_spp2_disconnect_callback(a)
    #define app_ctrl_spp2_connect_callback(a)
    #define app_ctrl_spp2_rx_callback(a,b)             0
#else
    #define app_ctrl_spp_disconnect_callback()
    #define app_ctrl_spp_connect_callback()
    #define app_ctrl_spp_rx_callback(a,b)                0

    #define app_ctrl_spp1_disconnect_callback()
    #define app_ctrl_spp1_connect_callback()
    #define app_ctrl_spp1_rx_callback(a,b)               0

    #define app_ctrl_spp2_disconnect_callback()
    #define app_ctrl_spp2_connect_callback()
    #define app_ctrl_spp2_rx_callback(a,b)               0
#endif

u16 get_spp_mtu_size(void);
bool app_test_cmd_process(u8 *ptr, u16 size, u8 type);
bool ab_fot_ble_send_packet(u8 *buf, u8 len);
bool ble_send_packet(u8 *buf, u8 len);

#include "app_fota/app_fota.h"
#include "app_fota/app_fota_profile.h"

void app_ble_connect_callback(void);
void app_ble_disconnect_callback(void);
void app_process(void);
void app_var_init(void);
void app_ble_init(void);
void app_init_do(void);
u8 app_eq_set(void);
void app_enter_sleep(void);
void app_exit_sleep(void);
bool app_need_wakeup(void);
void app_bt_evt_notice(uint evt, void *params);
uint16_t app_ble_role_switch_get_data(uint8_t *data_ptr);
uint16_t app_ble_role_switch_set_data(uint8_t *data_ptr, uint16_t len);
void app_sleep_mode_process(void);
void app_tws_user_key_process(uint32_t *opcode);
void app_tws_connect_callback(void);
void app_tws_disconnect_callback(void);
void app_spp_connect_callback(uint8_t ch);
void app_spp_disconnect_callback(uint8_t ch);
bool app_spp_rx_callback(uint8_t ch, u8 *data,u16 len);
#endif // __APP_H
