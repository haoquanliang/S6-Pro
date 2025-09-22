#include "dma_stream.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
#include "dma_utils.h"
#include "include.h"

#if LE_DUEROS_DMA_EN

DUEROS_DMA     g_dma_data AT(.dueros_dma_data);
DUER_DMA_OPER* g_dma_operation_ptr = NULL;

inline bool dma_get_device_capability(DUER_DMA_CAPABILITY* device_capability)
{
    DMA_OPER_UNINITED_RETURN(get_device_capability, NULL);
    return g_dma_operation_ptr->get_device_capability(device_capability);
}

inline bool dma_get_firmeware_version(uint8_t* fw_rev_0, uint8_t* fw_rev_1,
                                      uint8_t* fw_rev_2, uint8_t* fw_rev_3)
{
    DMA_OPER_UNINITED_RETURN(get_firmeware_version, NULL);
    return g_dma_operation_ptr->get_firmeware_version(fw_rev_0, fw_rev_1, fw_rev_2, fw_rev_3);
}

void* dueros_dma_heap_malloc(size_t size)
{
    DMA_OPER_UNINITED_RETURN(dma_heap_malloc, NULL);
    return g_dma_operation_ptr->dma_heap_malloc(size);
}

void dueros_dma_heap_free(void* ptr)
{
    DMA_OPER_UNINITED_RETURN_VOID(dma_heap_free);
    return g_dma_operation_ptr->dma_heap_free(ptr);
}

inline bool dma_sem_wait(uint32_t timeout_ms)
{
    DMA_OPER_UNINITED_RETURN(dma_sem_wait, false);
    return g_dma_operation_ptr->dma_sem_wait(timeout_ms);
}

inline bool dma_sem_signal(void)
{
    DMA_OPER_UNINITED_RETURN(dma_sem_signal, false);
    return g_dma_operation_ptr->dma_sem_signal();
}

inline bool dma_mutex_lock(DMA_MUTEX_ID mutex_id)
{
    DMA_OPER_UNINITED_RETURN(dma_mutex_lock, false);
    return g_dma_operation_ptr->dma_mutex_lock(mutex_id);
}

inline bool dma_mutex_unlock(uint32_t mutex_id)
{
    DMA_OPER_UNINITED_RETURN(dma_mutex_unlock, false);
    return g_dma_operation_ptr->dma_mutex_unlock(mutex_id);
}

inline bool dma_get_userdata_config(DMA_USER_DATA_CONFIG* dma_userdata_config)
{
    DMA_OPER_UNINITED_RETURN(dma_get_userdata_config, false);
    return g_dma_operation_ptr->dma_get_userdata_config(dma_userdata_config);
}

inline bool dma_set_userdata_config(const DMA_USER_DATA_CONFIG* dma_userdata_config)
{
    DMA_OPER_UNINITED_RETURN(dma_set_userdata_config, false);
    return g_dma_operation_ptr->dma_set_userdata_config(dma_userdata_config);
}

uint32_t dma_rand(void)
{
    DMA_OPER_UNINITED_RETURN(dma_rand, 0);
    return g_dma_operation_ptr->dma_rand();
}

inline bool dma_get_ota_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_ota_state, false);
    return g_dma_operation_ptr->get_ota_state();
}

inline uint8_t dma_get_upgrade_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_upgrade_state, 0);
    return g_dma_operation_ptr->get_upgrade_state();
}

inline int32_t dma_get_mobile_connect_type(void)
{
    DMA_OPER_UNINITED_RETURN(get_mobile_connect_type, 0);
    return g_dma_operation_ptr->get_mobile_connect_type();
}

inline bool dma_set_ble_advertise_data(const char* adv_data, uint8_t adv_data_len,
                                       const char* scan_response, uint8_t scan_response_len,
                                       const char* ibeacon_adv_data, uint8_t ibeacon_adv_data_len)
{
    DMA_OPER_UNINITED_RETURN(set_ble_advertise_data, false);
    return g_dma_operation_ptr->set_ble_advertise_data(adv_data, adv_data_len, scan_response, scan_response_len,
            ibeacon_adv_data, ibeacon_adv_data_len);
}

inline bool dma_set_ble_advertise_enable(bool on)
{
    DMA_OPER_UNINITED_RETURN(set_ble_advertise_enable, false);
    return g_dma_operation_ptr->set_ble_advertise_enable(on);
}

inline bool dma_get_bt_address(DMA_BT_ADDR_TYPE addr_type, uint8_t* bt_address)
{
    DMA_OPER_UNINITED_RETURN(get_bt_address, false);
    return g_dma_operation_ptr->get_bt_address(addr_type, bt_address);
}

inline bool dma_get_bt_local_name(char* bt_local_name)
{
    DMA_OPER_UNINITED_RETURN(get_bt_local_name, false);
    return g_dma_operation_ptr->get_bt_local_name(bt_local_name);
}

inline bool dma_get_ble_local_name(char* ble_local_name)
{
    DMA_OPER_UNINITED_RETURN(get_ble_local_name, false);
    return g_dma_operation_ptr->get_ble_local_name(ble_local_name);
}

inline bool dma_get_mobile_bt_address(uint8_t* bt_address)
{
    DMA_OPER_UNINITED_RETURN(get_mobile_bt_address, false);
    return g_dma_operation_ptr->get_mobile_bt_address(bt_address);
}

inline bool dma_get_linkkey_exist_state(const uint8_t* bt_address)
{
    DMA_OPER_UNINITED_RETURN(get_linkkey_exist_state, false);
    return g_dma_operation_ptr->get_linkkey_exist_state(bt_address);
}

inline bool dma_get_serial_number(DMA_SN_TYPE sn_type, uint8_t* serial_number)
{
    DMA_OPER_UNINITED_RETURN(get_serial_number, false);
    return g_dma_operation_ptr->get_serial_number(sn_type, serial_number);
}

inline bool dma_get_sco_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_sco_state, false);
    return g_dma_operation_ptr->get_sco_state();
}

inline bool dma_get_reconnect_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_reconnect_state, false);
    return g_dma_operation_ptr->get_reconnect_state();
}

inline bool dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_CTRL_TYPE cmd, DMA_VOICE_STREAM_CODEC_TYPE codec_type)
{
    DMA_OPER_UNINITED_RETURN(set_voice_mic_stream_state, false);
    return g_dma_operation_ptr->set_voice_mic_stream_state(cmd, codec_type);
}

inline bool dma_set_stream_upload_enable(bool on)
{
    DMA_OPER_UNINITED_RETURN(set_stream_upload_enable, false);
    return g_dma_operation_ptr->set_stream_upload_enable(on);
}

inline bool dma_get_stream_upload_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_stream_upload_state, false);
    return g_dma_operation_ptr->get_stream_upload_state();
}

inline bool dma_get_mobile_mtu(uint32_t* mtu)
{
    DMA_OPER_UNINITED_RETURN(get_mobile_mtu, false);
    return g_dma_operation_ptr->get_mobile_mtu(mtu);
}

inline bool dma_get_peer_mtu(uint32_t* mtu)
{
    DMA_OPER_UNINITED_RETURN(get_peer_mtu, false);
    return g_dma_operation_ptr->get_peer_mtu(mtu);
}

inline bool dma_set_wakeup_enable(bool on)
{
    DMA_OPER_UNINITED_RETURN(set_wakeup_enable, false);
    return g_dma_operation_ptr->set_wakeup_enable(on);
}

inline bool dma_get_check_summary(const void* input_data, uint32_t len, uint8_t* output_string)
{
    DMA_OPER_UNINITED_RETURN(get_check_summary, false);
    return g_dma_operation_ptr->get_check_summary(input_data, len, output_string);
}

inline bool dma_get_prepare_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_prepare_state, false);
    return g_dma_operation_ptr->get_prepare_state();
}

inline bool dma_process_cmd(DMA_OPERATION_CMD cmd, void* param_buf, uint32_t  param_size)
{
    DMA_OPER_UNINITED_RETURN(dma_process_cmd, false);
    return g_dma_operation_ptr->dma_process_cmd(cmd, param_buf, param_size);
}

inline bool dma_get_peer_connect_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_peer_connect_state, false);
    return g_dma_operation_ptr->get_peer_connect_state();
}

inline bool dma_send_custom_info_to_peer(uint8_t* param_buf, uint16_t param_size)
{
    DMA_OPER_UNINITED_RETURN(send_custom_info_to_peer, false);
    return g_dma_operation_ptr->send_custom_info_to_peer(param_buf, param_size);
}

inline bool  dma_get_tws_role(DMA_TWS_ROLE_TYPE* role_type)
{
    DMA_OPER_UNINITED_RETURN(get_tws_role, false);
    return g_dma_operation_ptr->get_tws_role(role_type);
}

inline bool dma_get_tws_side(DMA_TWS_SIDE_TYPE* side_type)
{
    DMA_OPER_UNINITED_RETURN(get_tws_side, false);
    return g_dma_operation_ptr->get_tws_side(side_type);
}

inline bool dma_set_role_switch_enable(bool on)
{
    DMA_OPER_UNINITED_RETURN(set_role_switch_enable, false);
    return g_dma_operation_ptr->set_role_switch_enable(on);
}

inline bool dma_get_box_state(DMA_BOX_STATE_TYPE* box_state)
{
    DMA_OPER_UNINITED_RETURN(get_box_state, false);
    return g_dma_operation_ptr->get_box_state(box_state);
}

inline bool dma_get_wearing_state(void)
{
    DMA_OPER_UNINITED_RETURN(get_wearing_state, false);
    return g_dma_operation_ptr->get_wearing_state();
}

inline uint8_t dma_get_battery_level(void)
{
    DMA_OPER_UNINITED_RETURN(get_battery_level, false);
    return g_dma_operation_ptr->get_battery_level();
}

inline uint8_t dma_get_box_battery_level(void)
{
    DMA_OPER_UNINITED_RETURN(get_box_battery_level, false);
    return g_dma_operation_ptr->get_box_battery_level();
}

inline bool dma_get_triad_info(char* triad_id, char* triad_secret)
{
    DMA_OPER_UNINITED_RETURN(get_triad_info, false);
    return g_dma_operation_ptr->get_triad_info(triad_id, triad_secret);
}

inline bool dma_play_local_tts(DMA_TTS_ID tts_id, bool both_side)
{
    DMA_OPER_UNINITED_RETURN(play_local_tts, false);
    return g_dma_operation_ptr->play_local_tts(tts_id, both_side);
}

bool dma_get_all_config(void)
{
    bool ret = dma_get_userdata_config((DMA_USER_DATA_CONFIG*)&g_dma_data.dma_userdata_config);

    if (!ret) {
        return ret;
    }

    DMA_TRACE("magic_number in Flash is: 0x%x\n", g_dma_data.dma_userdata_config.dma_config.magic_number);

    if (CONFIGURATION_MAGIC_NUMBER != g_dma_data.dma_userdata_config.dma_config.magic_number) {
        memset(&g_dma_data.dma_userdata_config, 0, sizeof(g_dma_data.dma_userdata_config));
        g_dma_data.dma_userdata_config.dma_config.magic_number = CONFIGURATION_MAGIC_NUMBER;
        g_dma_data.dma_userdata_config.dma_config.left_tap_setting = CUSTOM_TAP_TYPE_PULLUP;
        g_dma_data.dma_userdata_config.dma_config.right_tap_setting = CUSTOM_TAP_TYPE_PULLUP;
        DMA_TRACE("write magic_number 0x% in Flash\n", g_dma_data.dma_userdata_config.dma_config.magic_number);
        dma_set_userdata_config((DMA_USER_DATA_CONFIG*)&g_dma_data.dma_userdata_config);
    }

    g_dma_data.wakeup_enable = g_dma_data.dma_userdata_config.dma_config.wakeup_enable;
    g_dma_data.enable_wearing_detect = g_dma_data.dma_userdata_config.dma_config.enable_wearing_detect;
    g_dma_data.left_tap_setting = g_dma_data.dma_userdata_config.dma_config.left_tap_setting;
    g_dma_data.right_tap_setting = g_dma_data.dma_userdata_config.dma_config.right_tap_setting;
    g_dma_data.left_tap_setting_new_style = g_dma_data.dma_userdata_config.dma_config.left_tap_setting_new_style;
    g_dma_data.right_tap_setting_new_style = g_dma_data.dma_userdata_config.dma_config.right_tap_setting_new_style;
    g_dma_data.anc_cycle_list = g_dma_data.dma_userdata_config.dma_config.anc_cycle_list;

    memcpy(g_dma_data.rand, g_dma_data.dma_userdata_config.dma_config.rand, sizeof(g_dma_data.rand));
    return true;
}

bool dma_get_config(DMA_NVREC_CMD cmd, uint8_t* param, uint8_t len)
{
    bool ret = dma_get_userdata_config((DMA_USER_DATA_CONFIG*)&g_dma_data.dma_userdata_config);

    if (!ret) {
        return ret;
    }

    switch (cmd) {
    case DMA_NVREC_GET_RAND: {
        memcpy(param, g_dma_data.dma_userdata_config.dma_config.rand, len);
        break;
    }

    case DMA_NVREC_GET_PEER_SN: {
        memcpy(param, g_dma_data.dma_userdata_config.dma_config.peer_sn, len);
        break;
    }

    case DMA_NVREC_GET_PULLUP_FLAG: {
        uint8_t bdAddr[6];
        ret = dma_get_mobile_bt_address(bdAddr);

        DMA_TRACE("DMA_NVREC_GET_PULLUP_FLAG : %02X %02X %02X %02X %02X %02X \n",
                  bdAddr[0], bdAddr[1], bdAddr[2], bdAddr[3], bdAddr[4], bdAddr[5]);

        if (!ret) {
            *(bool*)param = false;
            break;
        }

        for (int16_t i = 0; i < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum; ++i) {
            DMA_TRACE("i = %d MAC = %02X %02X %02X %02X %02X %02X \n", i,
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[0],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[1],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[2],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[3],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[4],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[5]);

            if (!memcmp(bdAddr, g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr, 6)) {
                *(bool*)param = g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].enable_pullup;
                return true;
            }
        }

        *(bool*)param = false;
        break;
    }

    case DMA_NVREC_GET_PULLUP_OFF_STATE: {
        uint8_t bdAddr[6];
        ret = dma_get_mobile_bt_address(bdAddr);

        if (!ret) {
            *(bool*)param = false;
            break;
        }

        for (int16_t i = 0; i < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum; ++i) {
            if (!memcmp(bdAddr, g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr, 6)) {
                *(bool*)param = g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].enable_pullup_timeout_off;
                return true;
            }
        }

        *(bool*)param = false;
        break;
    }

    case DMA_NVREC_GET_WAKEUP_ENABLE: {
        *(bool*)param = g_dma_data.dma_userdata_config.dma_config.wakeup_enable;
        break;
    }

    case DMA_NVREC_GET_UPGRADE_FLAG: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.upgrade_succeed;
        break;
    }

    case DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING: {
        DMA_TRACE("%s %d value = %d", __func__, DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING,
                  g_dma_data.dma_userdata_config.dma_config.left_tap_setting);
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.left_tap_setting;
        break;
    }

    case DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING: {
        DMA_TRACE("%s %d value = %d", __func__, DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING,
                  g_dma_data.dma_userdata_config.dma_config.right_tap_setting);
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.right_tap_setting;
        break;
    }

    case DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING_NEW_STYLE: {
        DMA_TRACE("%s %d value = %d", __func__, DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING_NEW_STYLE,
                  g_dma_data.dma_userdata_config.dma_config.left_tap_setting_new_style);
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.left_tap_setting_new_style;
        break;
    }

    case DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING_NEW_STYLE: {
        DMA_TRACE("%s %d value = %d", __func__, DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING_NEW_STYLE,
                  g_dma_data.dma_userdata_config.dma_config.right_tap_setting_new_style);
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.right_tap_setting_new_style;
        break;
    }

    case DMA_NVREC_GET_DEBUG_INFO_TIME: {
        break;
    }

    case DMA_NVREC_GET_WEARING_DETECT_ENABLE: {
        *(bool*)param = g_dma_data.dma_userdata_config.dma_config.enable_wearing_detect;
        break;
    }

    case DMA_NVREC_GET_DMA_CONNECT_INFO: {
        MAC_ADDR_TO_CONNECT_TIMES mac_addr_to_times;
        ret = dma_get_mobile_bt_address(mac_addr_to_times.bdAddr);

        if (!ret) {
            break;
        }

        for (int16_t i = 0; i < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum; ++i) {
            if (!memcmp(mac_addr_to_times.bdAddr, g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr,
                        6)) {
                g_dma_data.on_link_hash_id = i;
                memcpy(param, &g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[g_dma_data.on_link_hash_id], len);
                return true;
            }
        }

        mac_addr_to_times.dma_connected_before = false;
        mac_addr_to_times.dma_tts_report_onoff = false;
        mac_addr_to_times.dma_connect_retry_times = 0;
        mac_addr_to_times.enable_pullup = false;
        mac_addr_to_times.enable_pullup_timeout_off = false;
        memcpy(param, &mac_addr_to_times, len);
        g_dma_data.on_link_hash_id = 0xFF;
        break;
    }

    case DMA_NVREC_GET_BOX_BATTERY_LEVEL: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.box_battery_level;
        break;
    }

    case DMA_NVREC_GET_ONE_EAR_ANC_MODE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.one_ear_noise_reduction_mode;
        break;
    }

    case DMA_NVREC_GET_TWO_EAR_ANC_MODE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.two_ear_noise_reduction_mode;
        break;
    }

    case DMA_NVREC_GET_TWO_EAR_ANC_CYCLE_LIST: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.anc_cycle_list;
        break;
    }

    case DMA_NVREC_GET_HEALTH_REMINDER_ENABLE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.health_reminder_enable;
        break;
    }

    case DMA_NVREC_GET_GAME_MODE_ENABLE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.game_mode_enable;
        break;
    }

    case DMA_NVREC_GET_WIND_NOISE_DETECTION_ENABLE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.wind_noise_detection_enable;
        break;
    }

    case DMA_NVREC_GET_CUSTOM_EQ_ENABLE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.custom_eq_enable;
        break;
    }

    case DMA_NVREC_GET_HEARING_PROTECTION_ENABLE: {
        *(uint8_t*)param = g_dma_data.dma_userdata_config.dma_config.hearing_protection_enable;
        break;
    }

    default:
        break;
    }

    return true;
}

bool dma_set_config(DMA_NVREC_CMD cmd, const uint8_t* param, uint8_t len)
{
    bool ret = dma_get_userdata_config((DMA_USER_DATA_CONFIG*)&g_dma_data.dma_userdata_config);

    if (!ret) {
        return ret;
    }

    switch (cmd) {
    case DMA_NVREC_SET_RAND: {
        memcpy(g_dma_data.dma_userdata_config.dma_config.rand, param, len);
        break;
    }

    case DMA_NVREC_SET_PEER_SN: {
        memcpy(g_dma_data.dma_userdata_config.dma_config.peer_sn, param, len);
        break;
    }

    case DMA_NVREC_SET_PULLUP_FLAG: {
        uint8_t bdAddr[6];
        ret = dma_get_mobile_bt_address(bdAddr);
        DMA_TRACE("DMA_NVREC_SET_PULLUP_FLAG : %02X %02X %02X %02X %02X %02X \n",
                  bdAddr[0], bdAddr[1], bdAddr[2], bdAddr[3], bdAddr[4], bdAddr[5]);

        if (!ret) {
            break;
        }

        for (int16_t i = 0; i < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum; ++i) {
            DMA_TRACE("i = %d MAC = %02X %02X %02X %02X %02X %02X \n", i,
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[0],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[1],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[2],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[3],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[4],
                      g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr[5]);

            if (!memcmp(bdAddr, g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr, 6)) {
                g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].enable_pullup = *(bool*)param;
                break;
            }
        }

        break;
    }

    case DMA_NVREC_SET_PULLUP_OFF_STATE: {
        uint8_t bdAddr[6];
        ret = dma_get_mobile_bt_address(bdAddr);

        if (!ret) {
            break;
        }

        for (int16_t i = 0; i < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum; ++i) {
            if (!memcmp(bdAddr, g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr, 6)) {
                *(bool*)param = g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].enable_pullup_timeout_off;
                break;
            }
        }

        break;
    }

    case DMA_NVREC_SET_WAKEUP_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.wakeup_enable = *(bool*)param;
        break;
    }

    case DMA_NVREC_SET_UPGRADE_FLAG: {
        g_dma_data.dma_userdata_config.dma_config.upgrade_succeed = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_LEFT_CUSTOM_TAP_SETTING: {
        g_dma_data.dma_userdata_config.dma_config.left_tap_setting = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_RIGHT_CUSTOM_TAP_SETTING: {
        g_dma_data.dma_userdata_config.dma_config.right_tap_setting = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_LEFT_CUSTOM_TAP_SETTING_NEW_STYLE: {
        g_dma_data.dma_userdata_config.dma_config.left_tap_setting_new_style = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_RIGHT_CUSTOM_TAP_SETTING_NEW_STYLE: {
        g_dma_data.dma_userdata_config.dma_config.right_tap_setting_new_style = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_DEBUG_INFO_TIME: {
        break;
    }

    case DMA_NVREC_SET_WEARING_DETECT_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.enable_wearing_detect = *(bool*)param;
        break;
    }

    case DMA_NVREC_SET_DMA_CONNECT_INFO: {
        MAC_ADDR_TO_CONNECT_TIMES* mac_addr_to_times = (MAC_ADDR_TO_CONNECT_TIMES*)param;

        if (g_dma_data.on_link_hash_id < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum) {
            if (!memcmp(mac_addr_to_times->bdAddr,
                        g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[g_dma_data.on_link_hash_id].bdAddr, 6)) {
                memcpy(&g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[g_dma_data.on_link_hash_id], param, len);
                break;
            }
        }

        int16_t i = 0;

        for (i = 0; i < g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum; ++i) {
            if (!dma_get_linkkey_exist_state(g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i].bdAddr)) {
                g_dma_data.on_link_hash_id = i;
                memcpy(&g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[g_dma_data.on_link_hash_id], param, len);
                break;
            }
        }

        if (i < MAX_BT_PAIRED_DEVICE_COUNT - 1) {
            ++g_dma_data.dma_userdata_config.dma_connected_info.pairedDevNum;
            memcpy(&g_dma_data.dma_userdata_config.dma_connected_info.mac_addr_to_times[i],
                   mac_addr_to_times, sizeof(MAC_ADDR_TO_CONNECT_TIMES));
            g_dma_data.on_link_hash_id = i;
            break;
        }

        DMA_ASSERT(false, "%s : %d can't save connect info", __func__, __LINE__);
        break;
    }

    case DMA_NVREC_SET_BOX_BATTERY_LEVEL: {
        g_dma_data.dma_userdata_config.dma_config.box_battery_level = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_ONE_EAR_ANC_MODE: {
        g_dma_data.dma_userdata_config.dma_config.one_ear_noise_reduction_mode = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_TWO_EAR_ANC_MODE: {
        g_dma_data.dma_userdata_config.dma_config.two_ear_noise_reduction_mode = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_TWO_EAR_ANC_CYCLE_LIST: {
        g_dma_data.dma_userdata_config.dma_config.anc_cycle_list = *(uint8_t*)param;
        break;
    }

    case DMA_NVREC_SET_HEALTH_REMINDER_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.health_reminder_enable = *(bool*)param;
        break;
    }

    case DMA_NVREC_SET_GAME_MODE_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.game_mode_enable = *(bool*)param;
        break;
    }

    case DMA_NVREC_SET_WIND_NOISE_DETECTION_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.wind_noise_detection_enable = *(bool*)param;
        break;
    }

    case DMA_NVREC_SET_CUSTOM_EQ_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.custom_eq_enable = *(bool*)param;
        break;
    }

    case DMA_NVREC_SET_HEARING_PROTECTION_ENABLE: {
        g_dma_data.dma_userdata_config.dma_config.hearing_protection_enable = *(bool*)param;
        break;
    }

    default:
        return false;
    }

    return dma_set_userdata_config((DMA_USER_DATA_CONFIG*)&g_dma_data.dma_userdata_config);
}

void dma_construct_ble_advertise_data(void)
{
    uint8_t avail_space = BLE_ADV_DATA_MAX_LEN - 3 - 4 - 2;
    uint8_t ble_adv_name_len = 0;

    g_dma_data.ble_adv_data_len = 0;
    g_dma_data.ble_scan_rsp_len = 0;
    memset(&g_dma_data.ble_adv_data, 0, sizeof(g_dma_data.ble_adv_data));
    memset(&g_dma_data.ble_scan_rsp, 0, sizeof(g_dma_data.ble_scan_rsp));

    /* 广播Flag，LE普通发现，不支持BR/EDR */
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x02;
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x01;
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x1A;

    /* 设备BLE名称 */
    memset(g_dma_data.get_device_info, 0, sizeof(g_dma_data.get_device_info));
    dma_get_ble_local_name((char*)g_dma_data.get_device_info);
    ble_adv_name_len = strlen((char*)g_dma_data.get_device_info) + 1;

    if (avail_space < ble_adv_name_len) {
        ble_adv_name_len = avail_space;
        g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = ble_adv_name_len + 1;
        g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x9;
    } else {
        g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = ble_adv_name_len + 1;
        g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x8;
    }

    memcpy(&g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len],
           g_dma_data.get_device_info, ble_adv_name_len);
    g_dma_data.ble_adv_data_len += ble_adv_name_len;

    /* UUID */
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x03;
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0x03;
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0xC2;
    g_dma_data.ble_adv_data[g_dma_data.ble_adv_data_len++] = 0xFD;

    g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = 0x0E;
    g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = 0xFF;
    memcpy(&g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len], g_dma_data.vendor_id, 2);
    g_dma_data.ble_scan_rsp_len += 2;
    g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = 0xFD;
    g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = 0xC2;

    /* BT MAC */
    memset(g_dma_data.get_device_info, 0, sizeof(g_dma_data.get_device_info));
    dma_get_bt_address(DMA_BT_ADDR_DEFAULT, g_dma_data.get_device_info);

    memcpy(&g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len], g_dma_data.get_device_info, 6);
    g_dma_data.ble_scan_rsp_len += 6;

    /* 开关盖状态 */
    DMA_BOX_STATE_TYPE box_state = DMA_BOX_STATE_OPEN;
    dma_get_box_state(&box_state);

    if ((DMA_BOX_STATE_OPEN == box_state) || (DMA_BOX_STATE_UNKNOWN == box_state)) {
        g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = 0xFF;
    } else {
        g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = 0xF0;
    }

    /* 设备类型和版本 */
    g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = g_dma_data.device_type;
    g_dma_data.ble_scan_rsp[g_dma_data.ble_scan_rsp_len++] = g_dma_data.device_version;
}

void dma_start_ble_adv(bool enable_ibeacon)
{
    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    dma_get_tws_role(&role_type);

    if (DMA_TWS_MASTER == role_type && !g_dma_data.dma_linked) {
        dma_set_ble_advertise_enable(false);

        dma_construct_ble_advertise_data();

        if ((1 != dma_get_mobile_connect_type()) || !enable_ibeacon) {
            g_dma_data.use_ibeacon = false;
            dma_set_ble_advertise_data((char*)g_dma_data.ble_adv_data, g_dma_data.ble_adv_data_len,
                                       (char*)g_dma_data.ble_scan_rsp, g_dma_data.ble_scan_rsp_len,
                                       NULL, 0);
        } else {
            uint8_t ibeacon_adv[] = {0x1A, 0xFF, 0x4C, 0x00, 0x02, 0x15, 0x00, 0x00, 0xFD, 0xC2, 0x00, 0x00, 0x10, 0x00,
                                     0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB, 0x00, 0x7B, 0x01, 0xC8, 0xC5
                                    };
            g_dma_data.use_ibeacon = true;
            dma_set_ble_advertise_data((char*)g_dma_data.ble_adv_data, g_dma_data.ble_adv_data_len,
                                       (char*)g_dma_data.ble_scan_rsp, g_dma_data.ble_scan_rsp_len,
                                       (char*)ibeacon_adv, sizeof(ibeacon_adv));
        }

        dma_set_ble_advertise_enable(true);
    }
}

void dma_reset_all_queue(void)
{
    dma_mutex_lock(DMA_DATA_MUTEX_ID);
    dma_reset_queue(&g_dma_data.data_queue);
    dma_mutex_unlock(DMA_DATA_MUTEX_ID);
    dma_mutex_lock(DMA_CMD_INPUT_MUTEX_ID);
    dma_reset_queue(&g_dma_data.cmd_queue_input);
    dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);
    dma_mutex_lock(DMA_CMD_OUTPUT_MUTEX_ID);
    dma_reset_queue(&g_dma_data.cmd_queue_output);
    dma_mutex_unlock(DMA_CMD_OUTPUT_MUTEX_ID);
}

void dma_voice_connected(void)
{
    dma_set_ble_advertise_enable(false);

    g_dma_data.dma_linked  = true;
    g_dma_data.dma_started = false;

    g_dma_data.use_dual_record = false;
    g_dma_data.upstream_started  = false;
    g_dma_data.left_version_sent = false;
    g_dma_data.right_version_sent = false;
    g_dma_data.stream_packet_index = 1;
    g_dma_data.silent_ota = true;
    g_dma_data.wait_start_speech_ack = false;
    g_dma_data.audio_format = AUDIO_FORMAT__OPUS_16KHZ_16KBPS_CBR_0_20MS;

    dma_reset_all_queue();

    dma_get_config(DMA_NVREC_GET_DMA_CONNECT_INFO, (uint8_t*)&g_dma_data.on_link_addr_hash,
                   sizeof(g_dma_data.on_link_addr_hash));
    g_dma_data.on_link_addr_hash.dma_connected_before = true;
    dma_set_config(DMA_NVREC_SET_DMA_CONNECT_INFO, (uint8_t*)&g_dma_data.on_link_addr_hash,
                   sizeof(g_dma_data.on_link_addr_hash));

    uint32_t mtu = 0;

    if (g_dma_data.is_tws_device) {
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);

        if (DMA_TWS_MASTER == role_type) {
            dma_get_mobile_mtu(&mtu);
        } else if (DMA_TWS_SLAVE == role_type) {
            dma_get_peer_mtu(&mtu);
        } else {
            DMA_ASSERT(false, "%s : %d can't get tws role", __func__, __LINE__);
        }
    } else {
        dma_get_mobile_mtu(&mtu);
    }

    if (mtu >= 512) {
        g_dma_data.trans_size = 320;
    } else if (mtu >= 180) {
        g_dma_data.trans_size = 160;
    }

    dma_sent_version();
}

void dma_voice_disconnected(void)
{
    g_dma_data.speech_state = SPEECH_STATE__IDLE;
    g_dma_data.upstream_started = false;
    g_dma_data.dma_linked  = false;
    g_dma_data.dma_started = false;
    g_dma_data.left_version_sent  = false;
    g_dma_data.right_version_sent = false;
    g_dma_data.wait_start_speech_ack = false;
    g_dma_data.use_dual_record = false;
    g_dma_data.stream_packet_index = 1;
    g_dma_data.audio_format = AUDIO_FORMAT__OPUS_16KHZ_16KBPS_CBR_0_20MS;

    dma_reset_all_queue();

    dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_STOP, DMA_VOICE_STREAM_NONE);
    dma_set_stream_upload_enable(false);

    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    dma_get_tws_role(&role_type);

    if ((g_dma_data.wakeup_enable) && ((DMA_TWS_MASTER == role_type) || (!g_dma_data.is_tws_device))
            && (g_dma_data.mobile_connected)) {
        dma_set_wakeup_enable(true);
    }

    dma_start_ble_adv(true);
}

void dma_mobile_connected(void)
{
    g_dma_data.mobile_connected = true;

    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    dma_get_tws_role(&role_type);

    if ((g_dma_data.wakeup_enable) && ((DMA_TWS_MASTER == role_type) || (!g_dma_data.is_tws_device))) {
        dma_set_wakeup_enable(true);
    }

    /* 当iOS手机连接上BT，但尚未连接BLE，且当前未开启iBeacon广播时，需要开启iBeacon广播 */
    dma_start_ble_adv(true);

    dma_get_config(DMA_NVREC_GET_DMA_CONNECT_INFO, (uint8_t*)&g_dma_data.on_link_addr_hash,
                   sizeof(g_dma_data.on_link_addr_hash));
#ifdef USE_XIAODU_OOBE_TTS
    DMA_TTS_ID tts_id = DMA_TTS_NUM;
    ++g_dma_data.on_link_addr_hash.dma_connect_retry_times;

    if (!g_dma_data.on_link_addr_hash.dma_connected_before) {
        switch (g_dma_data.on_link_addr_hash.dma_connect_retry_times) {
        case 1:
            tts_id = DMA_TTS_WAITING_DMA_CONNECTING_0_TIMES_NEVER_CONNECTED;
            break;

        case 6:
            tts_id = DMA_TTS_WAITING_DMA_CONNECTING_6_TIMES_NEVER_CONNECTED;
            break;

        case 12:
            tts_id = DMA_TTS_WAITING_DMA_CONNECTING_12_TIMES_NEVER_CONNECTED;
            break;

        case 18:
        case 24:
        case 50:
            tts_id = DMA_TTS_WAITING_DMA_CONNECTING_18or24or50n_TIMES_NEVER_CONNECTED;
            break;

        default:
            if ((g_dma_data.on_link_addr_hash.dma_connect_retry_times >= 50) &&
                    (0 == g_dma_data.on_link_addr_hash.dma_connect_retry_times % 50)) {
                tts_id = DMA_TTS_WAITING_DMA_CONNECTING_18or24or50n_TIMES_NEVER_CONNECTED;
            }

            break;
        }
    }    else {
        if ((g_dma_data.on_link_addr_hash.dma_connect_retry_times >= 50) &&
                (0 == g_dma_data.on_link_addr_hash.dma_connect_retry_times % 50)) {
            tts_id = DMA_TTS_WAITING_DMA_CONNECTING_MORETHAN_50_TIMES_AFTER_LASTCONNECT;
        }
    }

    if (DMA_TTS_NUM != tts_id) {
        dma_play_local_tts(tts_id, true);
    }

#endif
    dma_set_config(DMA_NVREC_SET_DMA_CONNECT_INFO, (uint8_t*)&g_dma_data.on_link_addr_hash,
                   sizeof(g_dma_data.on_link_addr_hash));

    if (g_dma_data.on_link_addr_hash.enable_pullup) {
        dma_process_cmd(DMA_SEND_VOICE_COMMAND, NULL, 0);
    }
}

void dma_mobile_disconnected(void)
{
    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    dma_get_tws_role(&role_type);

    /* 手机断连时关闭iBeacon广播 */
    dma_start_ble_adv(false);

    /* 手机断连时应关闭本地唤醒 */
    dma_set_wakeup_enable(false);
}

bool dma_sent_version(void)
{
    uint32_t  offset = 0;
    uint32_t  len = 0;
    uint16_t* data_len_p = NULL;
    uint8_t*  pBuf = NULL;

    if (g_dma_data.inited != DUEROS_DMA_INITED) {
        DMA_TRACE("%s DUEROS_DMA NOT INITED\n", __func__);
        return false;
    }

    pBuf = g_dma_data.send_buffer;
    data_len_p = (uint16_t*)pBuf;
    memset(pBuf, 0, sizeof(g_dma_data.send_buffer));
    memcpy(pBuf + AI_CMD_CODE_SIZE, (uint8_t*)&g_dma_data.protocol_version, sizeof(g_dma_data.protocol_version));

    offset = 0 + AI_CMD_CODE_SIZE;
    DUEROS_StoreBE16(pBuf + offset, g_dma_data.protocol_version.magic);
    offset += sizeof(g_dma_data.protocol_version.magic);
    DUEROS_StoreBE8(pBuf + offset, g_dma_data.protocol_version.hVersion);
    offset += sizeof(g_dma_data.protocol_version.hVersion);
    DUEROS_StoreBE8(pBuf + offset, g_dma_data.protocol_version.lVersion);
    offset += sizeof(g_dma_data.protocol_version.lVersion);
    (void)memcpy(pBuf + offset, &g_dma_data.protocol_version.reserve, sizeof(g_dma_data.protocol_version.reserve));

    len  = sizeof(PROTOCOL_VER);

    *data_len_p = (uint16_t)len;

    if (dma_get_prepare_state()) {
        dma_process_cmd(DMA_SEND_CMD, pBuf + AI_CMD_CODE_SIZE, len);
    } else {
        dma_mutex_lock(DMA_CMD_OUTPUT_MUTEX_ID);
        dma_enqueue(&g_dma_data.cmd_queue_output, (DMA_ITEM_TYPE*)g_dma_data.send_buffer, len + AI_CMD_CODE_SIZE);
        dma_mutex_unlock(DMA_CMD_OUTPUT_MUTEX_ID);
    }

    return 0;

}

void dma_get_setting(SpeechSettings* setting)
{
    if (setting == NULL) {
        return ;
    }

    setting->audio_format  = (AudioFormat)g_dma_data.audio_format;
    setting->audio_source  = AUDIO_SOURCE__STREAM;
    setting->audio_profile = AUDIO_PROFILE__FAR_FIELD;
    DMA_TRACE("dma_get_setting audio_format = %d\n", setting->audio_format);
}

Transport dma_transport[] = {TRANSPORT__BLUETOOTH_LOW_ENERGY, TRANSPORT__BLUETOOTH_RFCOMM};
AudioFormat dma_audioformat[] = {AUDIO_FORMAT__PCM_L16_16KHZ_MONO, AUDIO_FORMAT__OPUS_16KHZ_32KBPS_CBR_0_20MS};
static char bt_address[18] = {0,};

void  bt_address_convert_to_char(uint8_t* bt_addr)
{
    int i, j = 0;

    for (i = 0 ; i < 6; i++) {
        bt_address[ j++] = dma_hex_to_char((bt_addr[i] >> 4) & 0xf);
        bt_address[ j++] = dma_hex_to_char(bt_addr[i] & 0xf);
        bt_address[ j++] = ':';
    }

    bt_address[17] = '\0';
}

void dma_get_dev_info(DeviceInformation* device_info)
{
    if (NULL == device_info) {
        return;
    }

    DMA_TWS_SIDE_TYPE side_type = DMA_SIDE_UNKNOWN;
    bool ret = dma_get_tws_side(&side_type);

    if ((!ret || (DMA_SIDE_UNKNOWN == side_type)) && (g_dma_data.is_tws_device)) {
        return;
    } else if (!g_dma_data.is_tws_device) {
        side_type = DMA_SIDE_RIGHT;
    }

    if (DMA_SIDE_RIGHT == side_type) {
        dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);
        device_info->serial_number = (char*)g_dma_data.current_sn;
    } else {
        if (dma_get_config(DMA_NVREC_GET_PEER_SN, g_dma_data.peer_sn, sizeof(g_dma_data.peer_sn))) {
            device_info->serial_number = (char*)g_dma_data.peer_sn;
        }

        if (strlen((char*)g_dma_data.peer_sn) < 6) {
            dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);
            device_info->serial_number = (char*)g_dma_data.current_sn;
        }
    }

    dma_get_device_capability(&g_dma_data.dma_capability);

    uint8_t fw_rev[4] = {0, 1, 0, 0};
    dma_get_firmeware_version(&fw_rev[0], &fw_rev[1], &fw_rev[2], &fw_rev[3]);
    sprintf((char*)g_dma_data.software_version, "%d.%d.%d.%d", fw_rev[0], fw_rev[1], fw_rev[2], fw_rev[3]);

    device_info->name = "EARPHONE";
    device_info->device_type  = (char*)g_dma_data.dma_capability.device_type;
    device_info->manufacturer = (char*)g_dma_data.dma_capability.manufacturer;

    device_info->firmware_version = (char*)g_dma_data.software_version;

    if (g_dma_data.dma_capability.support_tap_wakeup) {
        device_info->initiator_type  = INITIATOR_TYPE__TAP;
    } else {
        device_info->initiator_type  = INITIATOR_TYPE__PHONE_WAKEUP;
    }

    device_info->product_id = (char*)g_dma_data.client_id;
    device_info->n_supported_transports = (sizeof(dma_transport) / sizeof(Transport));
    device_info->supported_transports = dma_transport;
    device_info->n_supported_audio_formats = (sizeof(dma_audioformat) / sizeof(dma_audioformat));
    device_info->supported_audio_formats = dma_audioformat;
    device_info->no_a2dp = !g_dma_data.dma_capability.support_a2dp;
    device_info->no_at_command = !g_dma_data.dma_capability.support_at_command;
    device_info->support_media  = g_dma_data.dma_capability.support_media;
    device_info->support_sota = g_dma_data.dma_capability.support_sota;
    device_info->is_earphone = g_dma_data.dma_capability.is_earphone;
    device_info->battery_structure = g_dma_data.dma_capability.support_box_battery ? 3 : 2;
    device_info->touch_setting_type = 1;
    device_info->disable_heart_beat = true;
    dma_get_bt_address(DMA_BT_ADDR_IN_USED, g_dma_data.master_mac);

    bt_address_convert_to_char(g_dma_data.master_mac);
    device_info->classic_bluetooth_mac = bt_address;
    DMA_TRACE("product_id:%s\n", device_info->product_id);
    DMA_TRACE("BT_ADDRES:%s\n", bt_address);
#ifdef USE_TRIAD_ID
    ret = dma_get_triad_info(g_dma_data.triad_id, g_dma_data.triad_secret);
    DMA_TRACE("===>triad_id:%s\n", g_dma_data.triad_id);
    DMA_TRACE("===>triad_secret:%s\n", g_dma_data.triad_secret);

    if (ret) {
        memset(g_dma_data.send_buffer, 0, sizeof(g_dma_data.send_buffer));
        uint8_t* send_message = (uint8_t*)g_dma_data.send_buffer;
        uint8_t* send_secret  = (uint8_t*)&g_dma_data.send_buffer[256];
        sprintf((char*)send_message,
                "{\"product_id\":\"%s\",\"triad_id\":\"%s\",\"fingerprint\":\"&%s&%s\"}",
                device_info->product_id, g_dma_data.triad_id, device_info->serial_number, bt_address);
        device_info->triad_id = g_dma_data.triad_id;

        AES_KEY aes_key;
        dma_aes_set_encrypt_key((uint8_t*)g_dma_data.triad_secret, 128, &aes_key);

        int len = strlen((const char*)send_message);
        unsigned char remain = 16 - (len % 16);

        for (int i = 0; i < remain; ++i) {
            send_message[len + i] = remain;
        }

        for (int i = 0; i < len; i += 16) {
            dma_aes_encrypt(send_message + i, send_secret + i, &aes_key);
        }

        dma_base64_encode((uint8_t*)send_secret, (char*)g_dma_data.triad_token, len + remain);
        DMA_TRACE("base64 = %s\n", g_dma_data.triad_token);
        device_info->token = (char*)g_dma_data.triad_token;
    }

#endif
}

void dma_get_dev_config(DeviceConfiguration* device_config)
{
    if (device_config == NULL) {
        return ;
    }

    device_config->needs_assistant_override  = false;
    device_config->needs_setup  = false;
}

void dma_set_state(State* p_state)
{
    if (p_state == NULL) {
        return;
    }

    switch (p_state->feature) {
    /* DMA的签名验证, 收到setState(0x02, false)指令的时候，设备需要清空签名信息 */
    case 0x2: {
        if (!p_state->boolean) {
            memset(g_dma_data.rand, 0, sizeof(g_dma_data.rand));
            dma_set_config(DMA_NVREC_SET_RAND, g_dma_data.rand, sizeof(g_dma_data.rand));
        }

        break;
    }

    /* DMA设备主动回连上次连接的手机 */
    case 0x40: {
        dma_process_cmd(DMA_CONNECT_MOBILE, NULL, 0);
        break;
    }

    /* DMA设备主动断开和手机的BT连接 */
    case 0x41: {
        dma_process_cmd(DMA_DISCONNECT_MOBILE, NULL, 0);
        break;
    }

    /* 设置拉起状态开关，目前仅小度自有产品使用 */
    case 0xf00d: {
        if (p_state->integer == 1) {
            g_dma_data.pullup_enable = false;
            dma_set_config(DMA_NVREC_SET_PULLUP_FLAG, (uint8_t*)&g_dma_data.pullup_enable, sizeof(g_dma_data.pullup_enable));
            dma_set_config(DMA_NVREC_SET_PULLUP_OFF_STATE, (uint8_t*)&g_dma_data.pullup_enable, sizeof(g_dma_data.pullup_enable));

        } else if (p_state->integer == 2) {
            g_dma_data.pullup_enable = true;
            dma_set_config(DMA_NVREC_SET_PULLUP_FLAG, (uint8_t*)&g_dma_data.pullup_enable, sizeof(g_dma_data.pullup_enable));
        }

        break;
    }

    /* 通知设备发起voice command */
    case 0xf00e: {
        if (1 == p_state->integer) {
            dma_process_cmd(DMA_SEND_VOICE_COMMAND, NULL, 0);
        }

        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n",  __func__, p_state->feature, p_state->integer);
        break;
    }

    /* 耳机入耳检测开关，供测试使用 */
    case 0xd000: {
        g_dma_data.enable_wearing_detect = p_state->boolean;
        dma_set_config(DMA_NVREC_SET_WEARING_DETECT_ENABLE, (uint8_t*)&p_state->boolean, sizeof(p_state->boolean));
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n",  __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 左耳敲击设置 */
    case 0xe006: {
        uint8_t tap_setting = (uint8_t)p_state->integer;
        dma_set_config(DMA_NVREC_SET_LEFT_CUSTOM_TAP_SETTING, &tap_setting, sizeof(tap_setting));
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n",  __func__, p_state->feature, p_state->integer);
        g_dma_data.left_tap_setting = tap_setting;
        break;
    }

    /* 右耳敲击设置 */
    case 0xe007: {
        uint8_t tap_setting = (uint8_t)p_state->integer;
        dma_set_config(DMA_NVREC_SET_RIGHT_CUSTOM_TAP_SETTING, &tap_setting, sizeof(tap_setting));
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n",  __func__, p_state->feature, p_state->integer);
        g_dma_data.right_tap_setting = tap_setting;
        break;
    }

    /* 语音唤醒设置 */
    case 0xe009: {
        dma_set_config(DMA_NVREC_SET_WAKEUP_ENABLE, (uint8_t*)&p_state->boolean, sizeof(p_state->boolean));
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n",  __func__, p_state->feature, p_state->boolean);

        if (g_dma_data.wakeup_enable != p_state->boolean) {
            dma_set_wakeup_enable(p_state->boolean);
        }

        g_dma_data.wakeup_enable = p_state->boolean;
        break;
    }

    /* 新功能播报设置 */
    case 0xe00a: {
        g_dma_data.on_link_addr_hash.dma_tts_report_onoff = p_state->boolean;
        dma_set_config(DMA_NVREC_SET_DMA_CONNECT_INFO, (uint8_t*)&g_dma_data.on_link_addr_hash,
                       sizeof(g_dma_data.on_link_addr_hash));
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n",  __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 用户主动OTA升级时，app下发通知给固件 */
    case 0xe011: {
        g_dma_data.silent_ota = !p_state->boolean;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n",  __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* APP通知设备端是否播放查找耳机音频 */
    case 0xe013: {
        break;
    }

    /* 设置左耳快捷操作 */
    case 0xe018: {
        uint8_t type = (p_state->integer & 0xff00) >> 8;
        uint8_t cycle_list = p_state->integer & 0xff;
        g_dma_data.left_tap_setting_new_style = type;

        dma_set_config(DMA_NVREC_SET_LEFT_CUSTOM_TAP_SETTING_NEW_STYLE,
                       &g_dma_data.left_tap_setting_new_style,
                       sizeof(g_dma_data.left_tap_setting_new_style));

        if ((CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL == type) && (0 == cycle_list)) {
            /* tap若是降噪切换，并且cycle_list为空，上报本地存储的cycle list */
            dma_send_sync_state(0xe018);
        } else if (CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL == type) {
            g_dma_data.anc_cycle_list = cycle_list;
            dma_set_config(DMA_NVREC_SET_TWO_EAR_ANC_CYCLE_LIST,
                           &g_dma_data.anc_cycle_list,
                           sizeof(g_dma_data.anc_cycle_list));
        }

        break;
    }

    /* 设置右耳快捷操作 */
    case 0xe019: {
        uint8_t type = (p_state->integer & 0xff00) >> 8;
        uint8_t cycle_list = p_state->integer & 0xff;
        g_dma_data.right_tap_setting_new_style = type;

        dma_set_config(DMA_NVREC_SET_RIGHT_CUSTOM_TAP_SETTING_NEW_STYLE,
                       &g_dma_data.right_tap_setting_new_style,
                       sizeof(g_dma_data.right_tap_setting_new_style));

        if ((CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL == type) && (0 == cycle_list)) {
            /* tap若是降噪切换，并且cycle_list为空，上报本地存储的cycle list */
            dma_send_sync_state(0xe019);
        } else if (CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL == type) {
            g_dma_data.anc_cycle_list = cycle_list;
            dma_set_config(DMA_NVREC_SET_TWO_EAR_ANC_CYCLE_LIST,
                           &g_dma_data.anc_cycle_list,
                           sizeof(g_dma_data.anc_cycle_list));
        }

        break;
    }

    }
}

void dma_get_state(uint32_t profile, State* p_state)
{
    DMA_TRACE("%s feature = 0x%x\n", __func__, p_state->feature);

    switch (profile) {
    /* DMA外设的探活 */
    case 0x1: {
        p_state->feature = 1;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean =  true;
        break;
    }

    /* DMA的签名验证，此处始终返回true */
    case 0x2 : {
        p_state->feature = 0x2;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean  = true;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n",  __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 设备是否连接了A2DP */
    case 0x132 : {
        p_state->feature = 0x132;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean  = true;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 检测设备OTA状态 */
    case 0xf00b: {
        p_state->feature = 0xf00b;
        p_state->value_case = STATE__VALUE_INTEGER;
        uint8_t upgrade_success = 0;
        dma_get_config(DMA_NVREC_GET_UPGRADE_FLAG, &upgrade_success, sizeof(upgrade_success));
        p_state->integer = upgrade_success;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature, p_state->integer);
        break;
    }

    /* 检测设备拉起状态开关，目前仅小度自有产品使用 */
    case 0xf00d: {
        p_state->feature = 0xf00d;
        p_state->value_case = STATE__VALUE_INTEGER;
        bool pullup = false;
        dma_get_config(DMA_NVREC_GET_PULLUP_FLAG, (uint8_t*)&pullup, sizeof(pullup));

        if (pullup) {
            p_state->integer = 2;
        } else {
            p_state->integer = 1;
        }

        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature,  p_state->integer);
        break;
    }

    /* 检测设备是否支持voice command*/
    case 0xf00e: {
        p_state->feature = 0xf00e;
        p_state->value_case = STATE__VALUE_INTEGER;
        p_state->integer = 1;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature, p_state->integer);
        break;
    }

    /* 耳机入耳检测开关，供测试使用 */
    case 0xd000: {
        p_state->feature = 0xd000;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = g_dma_data.enable_wearing_detect;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 获取耳机与电池盒的剩余电量 */
    case 0xe001: {
        p_state->feature = 0xe001;
        p_state->value_case = STATE__VALUE_INTEGER;

        DMA_TWS_SIDE_TYPE side_type = DMA_SIDE_UNKNOWN;
        bool ret = dma_get_tws_side(&side_type);

        if (!ret || (DMA_SIDE_UNKNOWN == side_type)) {
            break;
        }

        uint8_t left_battery_level  = 0;
        uint8_t right_battery_level = 0;
        uint8_t box_battery_level   = dma_get_box_battery_level();

        if (g_dma_data.is_left_side) {
            left_battery_level  = dma_get_battery_level();

            if (dma_get_peer_connect_state() && g_dma_data.recv_peer_battery_level) {
                DMA_TRACE("right battery level = %d\n", g_dma_data.right_battery_level);
                right_battery_level = g_dma_data.right_battery_level;
            } else if (dma_get_peer_connect_state()) {
                DMA_TRACE("not recv battery level\n");
                right_battery_level = 0xFF;
            }
        } else {
            if (dma_get_peer_connect_state() && g_dma_data.recv_peer_battery_level) {
                DMA_TRACE("left volt = %d", g_dma_data.left_battery_level);
                left_battery_level  = g_dma_data.left_battery_level;
            } else if (dma_get_peer_connect_state()) {
                DMA_TRACE("not left battery level\n");
                left_battery_level = 0xFF;
            }

            right_battery_level = dma_get_battery_level();
        }

        p_state->integer = (box_battery_level << 16) | (left_battery_level << 8) | right_battery_level;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature, p_state->integer);
        break;
    }

    /* 左耳是否在线 */
    case 0xe004: {
        p_state->feature = 0xe004;
        p_state->value_case = STATE__VALUE_BOOLEAN;

        if (g_dma_data.is_left_side || dma_get_peer_connect_state()) {
            p_state->boolean = true;
        } else {
            p_state->boolean = false;
        }

        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 右耳是否在线 */
    case 0xe005: {
        p_state->feature = 0xe005;
        p_state->value_case = STATE__VALUE_BOOLEAN;

        if (!g_dma_data.is_left_side || dma_get_peer_connect_state()) {
            p_state->boolean = true;
        } else {
            p_state->boolean = false;
        }

        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 左耳敲击设置 */
    case 0xe006: {
        p_state->feature = 0xe006;
        p_state->value_case = STATE__VALUE_INTEGER;
        uint8_t tap_setting = 0;
        dma_get_config(DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING, &tap_setting, sizeof(tap_setting));
        p_state->integer = tap_setting;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d read value = %d\n",
                  __func__, p_state->feature, p_state->integer, tap_setting);
        break;
    }

    /* 右耳敲击设置 */
    case 0xe007: {
        p_state->feature = 0xe007;
        p_state->value_case = STATE__VALUE_INTEGER;
        uint8_t tap_setting = 0;
        dma_get_config(DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING, &tap_setting, sizeof(tap_setting));
        p_state->integer = tap_setting;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d read value = %d\n",
                  __func__, p_state->feature, p_state->integer, tap_setting);
        break;
    }

    /* 检查耳机端语音唤醒是否开启 */
    case 0xe009: {
        p_state->feature = 0xe009;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = g_dma_data.wakeup_enable;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 检查耳机端新功能播报是否开启 */
    case 0xe00a: {
        p_state->feature = 0xe00a;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = g_dma_data.on_link_addr_hash.dma_tts_report_onoff;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature,  p_state->integer);
        break;
    }

    /* 主从切换请求，APP允许后可切换 */
    case 0xe00b: {
        p_state->feature = 0xe00b;
        p_state->value_case = STATE__VALUE_INTEGER;
        p_state->integer = 1;
        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature,   p_state->integer);
        break;
    }

    /* 连接前是否为音乐播放状态 */
    case 0xe00c: {
        p_state->feature = 0xe00c;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = g_dma_data.audio_play;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature,  p_state->boolean);
        break;
    }

    /* 主从切换通知，此时必须切换 */
    case 0xe00d: {
        p_state->feature = 0xe00d;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = true;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 根据当前所处的场景，耳机端通知APP端是否需要停止/开始OTA升级 */
    case 0xe00e: {
        p_state->feature = 0xe00e;
        p_state->value_case = STATE__VALUE_BOOLEAN;

        if (dma_get_sco_state() || ((1 == dma_get_mobile_connect_type()) && (dma_get_stream_upload_state()))) {
            p_state->boolean = true;
        } else {
            p_state->boolean = false;
        }

        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature, p_state->boolean);
        break;
    }

    /* 是否需要衔接语音助手服务，通过语音或敲击发送voice command的方式拉起小度APP时，应返回true */
    case 0xe012: {
        p_state->feature = 0xe012;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = g_dma_data.wait_for_dma_connect;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d type = %s\n", __func__, p_state->feature, p_state->boolean, p_state->ext);
        break;
    }

    /* 左耳是否在播放查找耳机音频 */
    case 0xe014: {
        p_state->feature = 0xe014;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = false;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature,  p_state->boolean);
        break;
    }

    /* 右耳是否在播放查找耳机音频 */
    case 0xe015: {
        p_state->feature = 0xe015;
        p_state->value_case = STATE__VALUE_BOOLEAN;
        p_state->boolean = false;
        DMA_TRACE("[%s:0x%x] p_state->boolean = %d\n", __func__, p_state->feature,  p_state->boolean);
        break;
    }

    /* 左耳固件版本 */
    case 0xe016: {
        p_state->feature = 0xe016;
        p_state->value_case = STATE__VALUE_STR;

        if (g_dma_data.is_left_side) {
            p_state->str = (char *)g_dma_data.software_version;
        } else {
            p_state->str = (char *)g_dma_data.peer_version;
        }

        DMA_TRACE("[%s:0x%x] p_state->str = %s\n", __func__, p_state->feature, p_state->str);
        break;
    }

    /* 右耳固件版本 */
    case 0xe017: {
        p_state->feature = 0xe017;
        p_state->value_case = STATE__VALUE_STR;

        if (!g_dma_data.is_left_side) {
            p_state->str = (char *)g_dma_data.software_version;
        } else {
            p_state->str = (char *)g_dma_data.peer_version;
        }

        DMA_TRACE("[%s:0x%x] p_state->str = %s\n", __func__, p_state->feature, p_state->str);
        break;
    }

    /* 左耳快捷操作 */
    case 0xe018: {
        p_state->feature = 0xe018;
        p_state->value_case = STATE__VALUE_INTEGER;

        p_state->integer = ((uint32_t)g_dma_data.left_tap_setting_new_style) << 8;

        if (CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL == p_state->integer) {
            p_state->integer |= g_dma_data.anc_cycle_list;
        }

        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature,   p_state->integer);
        break;
    }

    /* 右耳快捷操作 */
    case 0xe019: {
        p_state->feature = 0xe019;
        p_state->value_case = STATE__VALUE_INTEGER;

        p_state->integer = ((uint32_t)g_dma_data.right_tap_setting_new_style) << 8;

        if (CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL == p_state->integer) {
            p_state->integer |= g_dma_data.anc_cycle_list;
        }

        DMA_TRACE("[%s:0x%x] p_state->integer = %d\n", __func__, p_state->feature,   p_state->integer);
        break;
    }

    default: {
        DMA_TRACE("%s:unimplemented profile 0x%x\n", __func__, profile);
        break;
    }
    }

}

void dma_sync_state(bool send_online_state)
{
    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    dma_get_tws_role(&role_type);

    if (DMA_TWS_MASTER == role_type) {
        if (send_online_state) {
            dma_send_sync_state(0xe004);
            dma_send_sync_state(0xe005);
        }

        if (g_dma_data.is_left_side) {
            if (!g_dma_data.left_version_sent) {
                dma_send_sync_state(0xe016);
                g_dma_data.left_version_sent = true;
            }

            if (g_dma_data.recv_peer_dma_version && !g_dma_data.right_version_sent) {
                dma_send_sync_state(0xe017);
                g_dma_data.right_version_sent = true;
            }
        } else {
            if (g_dma_data.recv_peer_dma_version && !g_dma_data.left_version_sent) {
                dma_send_sync_state(0xe016);
                g_dma_data.left_version_sent = true;
            }

            if (!g_dma_data.right_version_sent) {
                dma_send_sync_state(0xe017);
                g_dma_data.right_version_sent = true;
            }
        }
    }
}

void dma_send_sync_state(uint32_t feature)
{
    DMA_TRACE("***** IN dueros_dma_send_sync_state feature=%x, g_dma_data.dma_started:%d\n", feature, g_dma_data.dma_started);

    if (!g_dma_data.dma_started) {
        return;
    }

    SynchronizeState synchronize_state;
    State   state;
    uint32_t request_id_nu = 0;
    char char_request_id[10] = {0,};
    uint32_t size  = 0;

    state__init(&state);
    synchronize_state__init(&synchronize_state);
    control_envelope__init(&g_dma_data.envelope);

    dma_get_state(feature, &state);

    synchronize_state.state =  &state;

    g_dma_data.envelope.command = COMMAND__SYNCHRONIZE_STATE;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_SYNCHRONIZE_STATE;
    g_dma_data.envelope.synchronizestate = &synchronize_state;

    request_id_nu = dma_rand();
    sprintf(char_request_id, "%lx", request_id_nu);
    g_dma_data.envelope.request_id = char_request_id;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, false);
}

void dma_notify_speech_state(SpeechState speech_state)
{
    switch (speech_state) {
    case SPEECH_STATE__IDLE: {
        if (SPEECH_STATE__LISTENING == g_dma_data.speech_state) {
            /*here  ASR end*/
        }

        break;
    }

    case SPEECH_STATE__LISTENING: {
        /*here ASR start*/
        g_dma_data.dma_started = true;

        if (dma_get_sco_state()) {
            break;
        }

        g_dma_data.speech_state = SPEECH_STATE__LISTENING ;
        g_dma_data.audio_format = AUDIO_FORMAT__OPUS_16KHZ_16KBPS_CBR_0_20MS;
        g_dma_data.upstream_started = true;
        dma_send_sync_state(0xe00e);
        break;
    }

    case SPEECH_STATE__PROCESSING: {
        if (SPEECH_STATE__LISTENING == g_dma_data.speech_state) {
            /*here  ASR end*/
            g_dma_data.upstream_started = false;
            g_dma_data.speech_state = SPEECH_STATE__PROCESSING;
            dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_STOP, DMA_VOICE_STREAM_NONE);
        }

        break;
    }

    case SPEECH_STATE__SPEAKING: {
        /*not care*/
        g_dma_data.speech_state = SPEECH_STATE__SPEAKING;
        break;
    }

    default: {
        DMA_TRACE("no support command\n");
        break;
    }
    }
}

InitiatorType dma_send_get_dev_info_ack(char* request_id, bool sync)
{
    Response   response;
    DeviceInformation deviceinformation;
    uint32_t size = 0;

    device_information__init(&deviceinformation);
    response__init(&response);
    control_envelope__init(&g_dma_data.envelope);

    dma_get_dev_info(&deviceinformation);

    response.payload_case = RESPONSE__PAYLOAD_DEVICE_INFORMATION ;
    response.error_code =  ERROR_CODE__SUCCESS;
    response.deviceinformation = &deviceinformation;

    g_dma_data.envelope.command = COMMAND__GET_DEVICE_INFORMATION_ACK;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;
    g_dma_data.envelope.request_id = request_id;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, sync);

    return deviceinformation.initiator_type;

}

void dma_paired_ack(char* request_id, bool sync)
{
    DMA_TRACE("dma_paired_ack\n");
    char sha256_char[65] =  {0};
    uint8_t sha256_result[33] =  {0};
    uint32_t size = 0;
    char new_string[256];

    Response  response;
    PairInformation pairInfo;

    control_envelope__init(&g_dma_data.envelope);
    response__init(&response);
    pair_information__init(&pairInfo);

    dma_get_random_string((char*)g_dma_data.rand, sizeof(g_dma_data.rand));

    dma_set_config(DMA_NVREC_SET_RAND, g_dma_data.rand, sizeof(g_dma_data.rand));

    char* sn = NULL;

    if (!g_dma_data.is_left_side) {
        dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);
        sn = (char*)g_dma_data.current_sn;
    } else {
        if (dma_get_config(DMA_NVREC_GET_PEER_SN, g_dma_data.peer_sn, sizeof(g_dma_data.peer_sn))) {
            sn = (char*)g_dma_data.peer_sn;
        }

        if (strlen((char*)g_dma_data.peer_sn) < 6) {
            dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);
            sn = (char*)g_dma_data.current_sn;
        }

    }

    DMA_TRACE("%s --%s-%s-%s-%s\n", __func__, g_dma_data.rand, g_dma_data.client_secret,
              g_dma_data.client_id, sn);
    dma_merge_4strings(new_string, (char*)g_dma_data.rand, (char*)g_dma_data.client_secret, (char*)g_dma_data.client_id,
                       sn);

    dma_get_check_summary(new_string, strlen(new_string), sha256_result);
    sha256_result[32] = '\0';
    dma_convert_byte_to_char(sha256_result, sha256_char, 32);

    pairInfo.rand = (char*)g_dma_data.rand;
    pairInfo.sign =  sha256_char;
    DMA_TRACE("-qwer---%s-\n", sha256_char);
    pairInfo.signmethod = g_dma_data.signMode;

    response.error_code = ERROR_CODE__SUCCESS;
    response.pairinformation = &pairInfo;
    response.payload_case = RESPONSE__PAYLOAD_PAIR_INFORMATION;


    g_dma_data.envelope.command = COMMAND__PAIR_ACK;
    g_dma_data.envelope.request_id = request_id;
    g_dma_data.envelope.payload_case = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, true);
}

bool dma_check_summary(ControlEnvelope* envelop)
{
    DMA_TRACE("dma_check_summary\n");
    char sha256_char[65] =  {0,};
    uint8_t sha256_result[33] =  {0,};

    char new_string[256];
    const char* key = (char*)g_dma_data.client_secret;
    DeviceInformation devInfo;
    dma_get_dev_info(&devInfo);

    char* sn = NULL;

    if (!g_dma_data.is_left_side) {
        dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);
        sn = (char*)g_dma_data.current_sn;
    } else {
        if (dma_get_config(DMA_NVREC_GET_PEER_SN, g_dma_data.peer_sn, sizeof(g_dma_data.peer_sn))) {
            sn = (char*)g_dma_data.peer_sn;
        }

        if (strlen((char*)g_dma_data.peer_sn) < 6) {
            dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);
            sn = (char*)g_dma_data.current_sn;
        }

    }

    dma_merge_5strings(new_string, envelop->rand2, (char*)g_dma_data.rand, key, (char*)g_dma_data.client_id,
                       sn);

    dma_get_check_summary(new_string, strlen(new_string), sha256_result);

    sha256_result[32] = '\0';
    dma_convert_byte_to_char(sha256_result, sha256_char, 32);

    if (strcmp(envelop->sign2, sha256_char) == 0) {
        DMA_TRACE("[dma_check_summary] success\n");
        return true;
    } else {
        DMA_TRACE("[dma_check_summary] fail\n");
        return false;
    }
}

void dma_send_get_state_check_sum_fail(char* request_id, bool sync)
{
    Response response;
    uint32_t size = 0;
    State  state;

    state__init(&state);
    control_envelope__init(&g_dma_data.envelope);
    response__init(&response);

    state.feature = 0x2;
    state.value_case  = STATE__VALUE_BOOLEAN;
    state.boolean    = false;

    response.payload_case = RESPONSE__PAYLOAD_STATE;
    response.error_code = ERROR_CODE__SIGN_VERIFY_FAIL;
    response.state = &state;

    g_dma_data.envelope.command = COMMAND__GET_STATE_ACK;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;
    g_dma_data.envelope.request_id = request_id;  //todo how to fill request

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, sync);
}

void dma_send_provide_speech_ack(uint32_t dialog_id, char* request_id, bool sync)
{
    Response   response;
    SpeechProvider provider;
    SpeechSettings setting;
    Dialog  dialog ;
    uint32_t size  = 0;

    dialog__init(&dialog);
    speech_settings__init(&setting);
    speech_provider__init(&provider);
    response__init(&response);
    control_envelope__init(&g_dma_data.envelope);

    dialog.id = dialog_id;
    dma_get_setting(&setting);
    g_dma_data.dialog = dialog_id;

    provider.dialog = &dialog;
    provider.settings = &setting;

    if (!dma_get_sco_state()) {
        response.error_code = ERROR_CODE__SUCCESS;
    } else {
        response.error_code = ERROR_CODE__UNSUPPORTED;
    }

    response.payload_case = RESPONSE__PAYLOAD_SPEECH_PROVIDER ;
    response.speechprovider = &provider;

    g_dma_data.envelope.command = COMMAND__PROVIDE_SPEECH_ACK;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;
    g_dma_data.envelope.request_id = request_id;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, sync);
}

void  dma_sent_get_device_configuration_ack(char* request_id, bool sync)
{
    Response response;
    DeviceConfiguration deviceinformation;
    uint32_t size  = 0;

    device_configuration__init(&deviceinformation);
    response__init(&response);
    control_envelope__init(&g_dma_data.envelope);

    dma_get_dev_config(&deviceinformation);

    response.payload_case = RESPONSE__PAYLOAD_DEVICE_CONFIGURATION ;
    response.error_code =  ERROR_CODE__SUCCESS;
    response.deviceconfiguration = &deviceinformation;

    g_dma_data.envelope.command = COMMAND__GET_DEVICE_CONFIGURATION_ACK;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;
    g_dma_data.envelope.request_id = request_id;
    DMA_TRACE("---%s request_id = %s \n", __func__, g_dma_data.envelope.request_id);

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, sync);
}

bool dma_send_get_state_ack(uint32_t feature, char* request_id, bool sync)
{
    Response response;
    State state;
    uint32_t size = 0;

    state__init(&state);
    response__init(&response);
    control_envelope__init(&g_dma_data.envelope);

    dma_get_state(feature, &state);

    response.payload_case = RESPONSE__PAYLOAD_STATE ;
    response.error_code =  ERROR_CODE__SUCCESS;//todo errorcode
    response.state = &state;

    g_dma_data.envelope.command = COMMAND__GET_STATE_ACK;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;
    g_dma_data.envelope.request_id = request_id;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    return dma_control_stream_send(&g_dma_data.envelope, size, true);
}

void dma_send_set_state_ack(SetState* set_state, char* request_id, bool sync)
{
    Response response;
    State state;
    uint32_t size = 0;

    state__init(&state);
    response__init(&response);
    control_envelope__init(&g_dma_data.envelope);

    state.feature    = set_state->state->feature;
    state.value_case = set_state->state->value_case;
    state.integer    = set_state->state->integer;

    dma_set_state(set_state->state);

    response.payload_case = RESPONSE__PAYLOAD_STATE ;
    response.error_code =  ERROR_CODE__SUCCESS;//todo errorcode
    response.state = &state;

    g_dma_data.envelope.command = COMMAND__SET_STATE_ACK;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;
    g_dma_data.envelope.request_id = request_id;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);

    if (0x40 != state.feature) {
        dma_control_stream_send(&g_dma_data.envelope, size, sync);
    }
}

char inside_status[8];

int dma_start_speech(uint32_t param)
{
    StartSpeech  start_speech;
    SpeechSettings speech_setting;
    SpeechInitiator speech_initiator;
    Dialog  dialog;
    SpeechInitiator__WakeWord speech_wake_word;

    unsigned int request_id_nu = 0;
    char char_request_id[10] = {0,};

    uint32_t len  = 0;
    speech_initiator__wake_word__init(&speech_wake_word);
    speech_settings__init(&speech_setting);
    speech_initiator__init(&speech_initiator);
    dialog__init(&dialog);
    start_speech__init(&start_speech);
    control_envelope__init(&g_dma_data.envelope);

    speech_wake_word.start_index_in_samples = 0 ;
    speech_wake_word.end_index_in_samples  = 10;

    speech_initiator.wake_word = &speech_wake_word;

    if (0 == param) {
        speech_initiator.type = SPEECH_INITIATOR__TYPE__WAKEWORD;
    } else {
        speech_initiator.type = SPEECH_INITIATOR__TYPE__TAP;
    }

    memset(inside_status, 0, sizeof(inside_status));
    sprintf(inside_status, "%d_%d", 1, 1);
    speech_initiator.in_out_ear = inside_status;
    speech_initiator.play_prompt_tone = false;
    DMA_TRACE("%s type = %d, inside = %s play_prompt_tone = %d\n", __func__, speech_initiator.type,
              speech_initiator.in_out_ear, speech_initiator.play_prompt_tone);

    dma_get_setting(&speech_setting);

    dialog.id = (dma_rand() % 100 + 1);
    g_dma_data.dialog = dialog.id;


    start_speech.initiator = &speech_initiator;
    start_speech.settings = &speech_setting;
    start_speech.dialog = &dialog;

    g_dma_data.envelope.command = COMMAND__START_SPEECH;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_START_SPEECH;
    g_dma_data.envelope.startspeech = &start_speech;

    request_id_nu = dma_rand();
    sprintf(char_request_id, "%x", request_id_nu);
    g_dma_data.envelope.request_id = char_request_id;
    DMA_TRACE("%s dialog %d request_id : %s\n",
              __func__, g_dma_data.dialog, char_request_id);

    g_dma_data.wait_start_speech_ack = true;
    len = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, len, true);

    return 0;
}

void dma_custom_double_click_handler(bool is_left)
{
    CUSTOM_TAP_TYPE tap_type = CUSTOM_TAP_TYPE_INVALID;

    if (is_left) {
        tap_type = g_dma_data.left_tap_setting;
    } else {
        tap_type = g_dma_data.right_tap_setting;
    }

    switch (tap_type) {
    case CUSTOM_TAP_TYPE_PULLUP:
        if (g_dma_data.dma_started) {
            dma_set_stream_upload_enable(true);
            g_dma_data.upstream_started = true;
            g_dma_data.stream_packet_index = 0;
            dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_START, DMA_VOICE_STREAM_OPUS);
            dma_start_speech(1);
            dma_play_local_tts(DMA_TTS_KEYWORD_DETECTED, true);
        } else {
            dma_play_local_tts(DMA_TTS_USE_XIAODU_APP, true);
        }

        break;

    case CUSTOM_TAP_TYPE_BACKWARD:
        dma_process_cmd(DMA_AUDIO_PLAY_BACKWARD, NULL, 0);
        break;

    case CUSTOM_TAP_TYPE_FORWARD:
        dma_process_cmd(DMA_AUDIO_PLAY_FORWARD, NULL, 0);
        break;

    case CUSTOM_TAP_TYPE_PLAY: {
        bool playing = false;
        dma_process_cmd(DMA_AUDIO_GET_PLAY_PAUSE, &playing, sizeof(playing));

        if (playing) {
            dma_process_cmd(DMA_AUDIO_PAUSE, NULL, 0);
        } else {
            dma_process_cmd(DMA_AUDIO_PLAY, NULL, 0);
        }

        break;
    }
    case CUSTOM_TAP_TYPE_INVALID: {
        break;
    }
    }


}

void dma_send_ack_common(char* requeset_id, Command command, bool sync)
{
    Response   response ;
    uint32_t size  = 0;

    response__init(&response);
    control_envelope__init(&g_dma_data.envelope);

    response.payload_case = RESPONSE__PAYLOAD__NOT_SET ;
    response.error_code =  ERROR_CODE__SUCCESS;//todo errorcode

    g_dma_data.envelope.command = command;
    g_dma_data.envelope.payload_case  = CONTROL_ENVELOPE__PAYLOAD_RESPONSE;
    g_dma_data.envelope.response = &response;

    g_dma_data.envelope.request_id = requeset_id;

    size = control_envelope__get_packed_size(&g_dma_data.envelope);
    dma_control_stream_send(&g_dma_data.envelope, size, sync);
    return;
}

bool dma_control_stream_send(const ControlEnvelope* message, uint32_t length, bool sync)
{
    DUEROS_DMA_HEADER cmd_header = {0};
    uint8_t*  output_buf_p = NULL;
    uint8_t*  data_len_p = NULL;
    uint16_t* output_size_p = NULL;
    int data_len = 0;
    uint32_t send_len = 0;
    memset(g_dma_data.send_buffer, 0, sizeof(g_dma_data.send_buffer));
    data_len_p = g_dma_data.send_buffer + AI_CMD_CODE_SIZE + DUEROS_DMA_HEADER_SIZE;

    if (length < 0xff) {
        data_len = 1;
        *(uint8_t*)data_len_p = length;
    } else if (length < 0xffff) {
        *(uint8_t*)data_len_p = (length & 0xff00) >> 8;
        *((uint8_t*)data_len_p + 1) = length & 0xff;
        data_len = 2;
    } else {
        DMA_TRACE("%s error len %d\n", __func__, length);
        return false;
    }

    cmd_header.length    = data_len - 1;
    cmd_header.reserve   = 0;
    cmd_header.streamID  = DUEROS_DMA_CMD_STREAM_ID;
    cmd_header.version   = 1;

    output_buf_p = &g_dma_data.send_buffer[AI_CMD_CODE_SIZE];
    output_buf_p[0] = ((uint8_t*)&cmd_header)[1];
    output_buf_p[1] = ((uint8_t*)&cmd_header)[0];

    control_envelope__pack(message, (g_dma_data.send_buffer + AI_CMD_CODE_SIZE + DUEROS_DMA_HEADER_SIZE + data_len));
    send_len = length + DUEROS_DMA_HEADER_SIZE + data_len;
    output_size_p = (uint16_t*)g_dma_data.send_buffer;
    *output_size_p = (uint16_t)send_len;

    if (dma_get_prepare_state() && sync) {
        dma_process_cmd(DMA_SEND_CMD, output_buf_p, send_len);
    } else {
        dma_mutex_lock(DMA_CMD_OUTPUT_MUTEX_ID);
        dma_enqueue(&g_dma_data.cmd_queue_output, (DMA_ITEM_TYPE*)g_dma_data.send_buffer, send_len + AI_CMD_CODE_SIZE);
        dma_mutex_unlock(DMA_CMD_OUTPUT_MUTEX_ID);
    }

    return true;
}

bool dma_sync_peer_state(uint32_t cmd)
{
    dma_save_peer_data();

    if (0 == cmd) {
        return false;
    }

    DMA_TWS_SIDE_TYPE side_type = DMA_SIDE_UNKNOWN;
    bool ret = dma_get_tws_side(&side_type);

    if (!ret || (DMA_SIDE_UNKNOWN == side_type)) {
        return false;
    }

    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    ret = dma_get_tws_role(&role_type);

    if (!ret || (DMA_TWS_UNKNOWN == role_type)) {
        return false;
    }

    memset(&g_dma_data.share_info_send, 0, sizeof(g_dma_data.share_info_send));
    DMA_SHARE_INFO* share_info = &g_dma_data.share_info_send;
    share_info->magic_number = DMA_INFO_MAGIC_NUMBER;

    if ((cmd & (1 << DMA_SYNC_LEFT_TAP_SETTING)) && (DMA_SIDE_LEFT == side_type)) {
        share_info->update_left_tap_setting = true;
        share_info->left_tap_setting = g_dma_data.left_tap_setting;
    }

    if ((cmd & (1 << DMA_SYNC_RIGHT_TAP_SETTING)) && (DMA_SIDE_RIGHT == side_type)) {
        share_info->update_right_tap_setting = true;
        share_info->right_tap_setting = g_dma_data.right_tap_setting;
    }

    if ((cmd & (1 << DMA_SYNC_LEFT_TAP_SETTING_NEW_STYLE)) && (DMA_SIDE_LEFT == side_type)) {
        share_info->update_left_tap_setting_new_style = true;
        share_info->left_tap_setting_new_style = g_dma_data.left_tap_setting_new_style;
    }

    if ((cmd & (1 << DMA_SYNC_RIGHT_TAP_SETTING_NEW_STYLE)) && (DMA_SIDE_RIGHT == side_type)) {
        share_info->update_right_tap_setting_new_style = true;
        share_info->right_tap_setting_new_style = g_dma_data.right_tap_setting_new_style;
    }

    if ((cmd & (1 << DMA_SYNC_LEFT_WEARING_ON))  && (DMA_SIDE_LEFT == side_type)) {
        if (DMA_SIDE_LEFT == side_type) {
            share_info->update_left_wearing_on = true;
            share_info->left_wearing_on = g_dma_data.left_wearing_on;
        }
    }

    if ((cmd & (1 << DMA_SYNC_RIGHT_WEARING_ON)) && (DMA_SIDE_RIGHT == side_type)) {
        if (DMA_SIDE_RIGHT == side_type) {
            share_info->update_right_wearing_on = true;
            share_info->right_wearing_on = g_dma_data.right_wearing_on;
        }
    }

    if ((cmd & (1 << DMA_SYNC_LEFT_BATTERY_LEVEL)) && (DMA_SIDE_LEFT == side_type)) {
        if (DMA_SIDE_LEFT == side_type) {
            share_info->update_left_battery_level = true;
            share_info->left_battery_level = dma_get_battery_level();
        }
    }

    if ((cmd & (1 << DMA_SYNC_RIGHT_BATTERY_LEVEL)) && (DMA_SIDE_RIGHT == side_type)) {
        if (DMA_SIDE_RIGHT == side_type) {
            share_info->update_right_battery_level = true;
            share_info->right_battery_level = dma_get_battery_level();
        }
    }

    if (cmd & (1 << DMA_SYNC_PULLUP_ENABLE)) {
        share_info->update_pullup_enable = true;
        share_info->pullup_enable = g_dma_data.pullup_enable;
    }

    if (cmd & (1 << DMA_SYNC_WAKEUP_ENABLE)) {
        share_info->update_wakeup_enable = true;
        share_info->wakeup_enable = g_dma_data.wakeup_enable;
    }

    if (cmd & (1 << DMA_SYNC_OOBE_REPORT_ENABLE)) {
        share_info->update_oobe_report_enable = true;
        memcpy(&share_info->on_link_addr_hash, &g_dma_data.on_link_addr_hash, sizeof(g_dma_data.on_link_addr_hash));
    }

    if (cmd & (1 << DMA_SYNC_DMA_CONNECT_STATE)) {
        share_info->update_dma_connect_state = true;
        share_info->dma_linked = g_dma_data.dma_linked;
        share_info->dma_started = g_dma_data.dma_started;
    }

    if (cmd & (1 << DMA_SYNC_UPSTREAM_CONFIG)) {
        share_info->update_upstream_config = true;
        share_info->audio_format = g_dma_data.audio_format;
        share_info->upstream_started = g_dma_data.upstream_started;
    }

    if (cmd & (1 << DMA_SYNC_DEVICE_INFO)) {
        share_info->update_device_info = true;
        memcpy(share_info->sn, g_dma_data.current_sn, sizeof(g_dma_data.current_sn));
        memcpy(share_info->version, g_dma_data.software_version, sizeof(g_dma_data.software_version));
        share_info->upgrade_flag = dma_get_upgrade_state();
        memcpy(share_info->dma_rand, g_dma_data.rand, sizeof(g_dma_data.rand));
    }

    if (cmd & (1 << DMA_SYNC_TWO_EAR_ANC_CYCLE_LIST)) {
        share_info->update_anc_cycle_list = true;
        share_info->anc_cycle_list = g_dma_data.anc_cycle_list;
    }

    return dma_send_custom_info_to_peer((uint8_t*)share_info, sizeof(*share_info));
}

bool dma_save_peer_data(void)
{
    DMA_TWS_SIDE_TYPE side_type = DMA_SIDE_UNKNOWN;
    bool ret = dma_get_tws_side(&side_type);

    if (!ret || (DMA_SIDE_UNKNOWN == side_type)) {
        return false;
    }

    DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
    ret = dma_get_tws_role(&role_type);

    if (!ret || (DMA_TWS_UNKNOWN == role_type)) {
        return false;
    }

    dma_mutex_lock(DMA_DATA_MUTEX_ID);
    DMA_SHARE_INFO* share_info = &g_dma_data.share_info_recv;
    bool need_update_config = false;

    if (DMA_INFO_MAGIC_NUMBER != share_info->magic_number) {
        dma_mutex_unlock(DMA_DATA_MUTEX_ID);
        return false;
    }

    DMA_TRACE("%s side_type = %d\n", __func__, side_type);

    if (DMA_SIDE_LEFT == side_type) {
        if (share_info->update_right_tap_setting &&
                (g_dma_data.right_tap_setting != share_info->right_tap_setting)) {
            g_dma_data.right_tap_setting = share_info->right_tap_setting;
            g_dma_data.dma_userdata_config.dma_config.right_tap_setting = g_dma_data.right_tap_setting;
            need_update_config = true;
        }
        
        if (share_info->update_right_tap_setting_new_style &&
                (g_dma_data.right_tap_setting_new_style != share_info->right_tap_setting_new_style)) {
            g_dma_data.right_tap_setting_new_style = share_info->right_tap_setting_new_style;
            g_dma_data.dma_userdata_config.dma_config.right_tap_setting_new_style = g_dma_data.right_tap_setting_new_style;
            need_update_config = true;
        }

        if (share_info->update_right_wearing_on &&
                (g_dma_data.right_wearing_on != share_info->right_wearing_on)) {
            g_dma_data.right_wearing_on = share_info->right_wearing_on;
        }

        DMA_TRACE("%s update_right_battery_level = %d right_battery_level = %d\n",
                  __func__, share_info->update_left_battery_level,
                  share_info->right_battery_level);

        if (share_info->update_right_battery_level) {
            g_dma_data.right_battery_level = share_info->right_battery_level;
            g_dma_data.recv_peer_battery_level = true;
        }
    } else {
        if (share_info->update_left_tap_setting &&
                (g_dma_data.left_tap_setting != share_info->left_tap_setting)) {
            g_dma_data.left_tap_setting = share_info->left_tap_setting;
            g_dma_data.dma_userdata_config.dma_config.left_tap_setting = g_dma_data.left_tap_setting;
            need_update_config = true;
        }
        
        if (share_info->update_left_tap_setting_new_style &&
                (g_dma_data.left_tap_setting_new_style != share_info->left_tap_setting_new_style)) {
            g_dma_data.left_tap_setting_new_style = share_info->left_tap_setting_new_style;
            g_dma_data.dma_userdata_config.dma_config.left_tap_setting_new_style = g_dma_data.left_tap_setting_new_style;
            need_update_config = true;
        }

        if (share_info->update_left_wearing_on &&
                (g_dma_data.left_wearing_on != share_info->left_wearing_on)) {
            g_dma_data.left_wearing_on = share_info->left_wearing_on;
        }

        DMA_TRACE("%s update_left_battery_level = %d left_battery_level = %d\n",
                  __func__, share_info->update_left_battery_level,
                  share_info->left_battery_level);

        if (share_info->update_left_battery_level) {
            g_dma_data.left_battery_level = share_info->left_battery_level;
            g_dma_data.recv_peer_battery_level = true;
        }
    }

    if (share_info->update_pullup_enable &&
            (g_dma_data.pullup_enable != share_info->pullup_enable)) {
        g_dma_data.pullup_enable = share_info->pullup_enable;
        dma_set_config(DMA_NVREC_SET_PULLUP_FLAG, (uint8_t*)&g_dma_data.pullup_enable, sizeof(g_dma_data.pullup_enable));
    }

    if (share_info->update_wakeup_enable &&
            (g_dma_data.wakeup_enable != share_info->wakeup_enable)) {
        g_dma_data.wakeup_enable = share_info->wakeup_enable;
        g_dma_data.dma_userdata_config.dma_config.wakeup_enable = g_dma_data.wakeup_enable;
        need_update_config = true;
    }

    if (share_info->update_oobe_report_enable) {
        memcpy(&g_dma_data.on_link_addr_hash, &share_info->on_link_addr_hash, sizeof(g_dma_data.on_link_addr_hash));
        dma_set_config(DMA_NVREC_SET_DMA_CONNECT_INFO, (uint8_t*)&g_dma_data.on_link_addr_hash,
                       sizeof(g_dma_data.on_link_addr_hash));
    }

    DMA_TRACE("%s update_dma_connect_state = %d mobile_connect_type = %d dma_started = %d",
              __func__, share_info->update_dma_connect_state, dma_get_mobile_connect_type(),
              g_dma_data.dma_started);

    if (share_info->update_dma_connect_state && (1 != dma_get_mobile_connect_type())) {
        g_dma_data.dma_linked = share_info->dma_linked;
        g_dma_data.dma_started = share_info->dma_started;
    }

    if (share_info->update_upstream_config) {
        g_dma_data.audio_format = share_info->audio_format;
        g_dma_data.upstream_started = share_info->upstream_started;
    }

    if (share_info->update_device_info) {
        DMA_TRACE("recv peer sn = %s\n", share_info->sn);
        memcpy(g_dma_data.peer_sn, share_info->sn, sizeof(g_dma_data.peer_sn));
        memcpy(g_dma_data.dma_userdata_config.dma_config.peer_sn, g_dma_data.peer_sn, sizeof(g_dma_data.peer_sn));
        memcpy(g_dma_data.peer_version, share_info->version, sizeof(g_dma_data.peer_version));
        g_dma_data.recv_peer_dma_version = true;

        if (DMA_TWS_SLAVE == role_type) {
            memcpy(g_dma_data.rand, share_info->dma_rand, sizeof(g_dma_data.rand));
            memcpy(g_dma_data.dma_userdata_config.dma_config.rand, g_dma_data.rand, sizeof(g_dma_data.rand));
        }

        need_update_config = true;
    }

    if (share_info->update_anc_cycle_list &&
        g_dma_data.anc_cycle_list != share_info->anc_cycle_list) {
        g_dma_data.anc_cycle_list = share_info->anc_cycle_list;
        g_dma_data.dma_userdata_config.dma_config.anc_cycle_list = g_dma_data.anc_cycle_list;
        need_update_config = true;
    }

    if (need_update_config) {
        dma_set_userdata_config((DMA_USER_DATA_CONFIG*)&g_dma_data.dma_userdata_config);
    }

    share_info->magic_number = 0;
    dma_mutex_unlock(DMA_DATA_MUTEX_ID);
    dma_sync_state(false);
    return true;
}

bool dma_process_data_send(void)
{
    DUEROS_DMA_HEADER audio_header = {0};
    uint8_t* output_buf_p = NULL;
    uint32_t data_len = 0;
    uint8_t* data_len_p = NULL;
    uint32_t send_len = 0;
process_again:

    if (!dma_get_prepare_state()) {
        return false;
    }

    dma_mutex_lock(DMA_DATA_MUTEX_ID);

    if (dma_length_of_queue(&g_dma_data.data_queue) < g_dma_data.trans_size) {
        dma_mutex_unlock(DMA_DATA_MUTEX_ID);
        return false;
    }

    dma_mutex_unlock(DMA_DATA_MUTEX_ID);
    send_len = g_dma_data.trans_size + sizeof(g_dma_data.stream_packet_index);
    memset(g_dma_data.send_buffer, 0, sizeof(g_dma_data.send_buffer));
    data_len_p   = g_dma_data.send_buffer + DUEROS_DMA_HEADER_SIZE;

    if (send_len < 0xff) {
        data_len = 1;
        *(uint8_t*)data_len_p = send_len;
    } else if (g_dma_data.trans_size < 0xffff) {
        *(uint8_t*)data_len_p = (send_len & 0xff00) >> 8;
        *((uint8_t*)data_len_p + 1) = send_len & 0xff;
        data_len = 2;
    } else {
        DMA_TRACE("%s error len %d\n", __func__, send_len);
        return false;
    }

    audio_header.length    = data_len - 1;
    audio_header.reserve   = 0;
    audio_header.streamID  = DUEROS_DMA_AUDIO_STREAM_ID;
    audio_header.version   = 1;

    output_buf_p = g_dma_data.send_buffer;
    output_buf_p[0] = ((uint8_t*)&audio_header)[1];
    output_buf_p[1] = ((uint8_t*)&audio_header)[0];

    memcpy(output_buf_p + DUEROS_DMA_HEADER_SIZE + data_len,
           &g_dma_data.stream_packet_index, sizeof(g_dma_data.stream_packet_index));

    dma_mutex_lock(DMA_DATA_MUTEX_ID);
    dma_dequeue(&g_dma_data.data_queue,
                output_buf_p + DUEROS_DMA_HEADER_SIZE + data_len + sizeof(g_dma_data.stream_packet_index), g_dma_data.trans_size);
    dma_mutex_unlock(DMA_DATA_MUTEX_ID);

    send_len = DUEROS_DMA_HEADER_SIZE + data_len + g_dma_data.trans_size + sizeof(g_dma_data.stream_packet_index);

    dma_process_cmd(DMA_SEND_DATA, g_dma_data.send_buffer, send_len);

    ++g_dma_data.stream_packet_index;

    if (dma_length_of_queue(&g_dma_data.data_queue)) {
        goto process_again;
    }

    return true;
}

bool dma_process_cmd_send(void)
{
    uint16_t cmd_len = 0;
cmd_send_process_again:

    if (!dma_get_prepare_state()) {
        return false;
    }

    dma_mutex_lock(DMA_CMD_OUTPUT_MUTEX_ID);

    if (dma_length_of_queue(&g_dma_data.cmd_queue_output) < AI_CMD_CODE_SIZE) {
        dma_mutex_unlock(DMA_CMD_OUTPUT_MUTEX_ID);
        return false;
    }

    dma_peek_queue_2_buf(&g_dma_data.cmd_queue_output, (DMA_ITEM_TYPE*)&cmd_len, AI_CMD_CODE_SIZE);

    if (dma_length_of_queue(&g_dma_data.cmd_queue_output) < AI_CMD_CODE_SIZE + cmd_len) {
        dma_mutex_unlock(DMA_CMD_OUTPUT_MUTEX_ID);
        return false;
    }

    dma_dequeue(&g_dma_data.cmd_queue_output, NULL, AI_CMD_CODE_SIZE);
    memset(g_dma_data.send_buffer, 0, sizeof(g_dma_data.send_buffer));
    dma_dequeue(&g_dma_data.cmd_queue_output, g_dma_data.send_buffer, cmd_len);
    dma_mutex_unlock(DMA_CMD_OUTPUT_MUTEX_ID);

    dma_process_cmd(DMA_SEND_CMD, g_dma_data.send_buffer, cmd_len);

    if (dma_length_of_queue(&g_dma_data.cmd_queue_output)) {
        goto cmd_send_process_again;
    }

    return true;
}

uint32_t dma_process_cmd_recv(void)
{
    ControlEnvelope* envelope = NULL ;
    DUEROS_DMA_HEADER cmd_header = {0};
    uint8_t* buf = NULL;
    uint32_t avail = 0;
    uint8_t* e1 = NULL, *e2 = NULL;
    uint32_t len1 = 0, len2 = 0;
    uint8_t* payload_buf = NULL;
    uint32_t payload_len = 0, process_len = 0;
    bool ret = false;

cmd_recv_process_again:
    dma_mutex_lock(DMA_CMD_INPUT_MUTEX_ID);
    avail = dma_length_of_queue(&g_dma_data.cmd_queue_input);

    if (avail > 4) {
        dma_peek_queque(&g_dma_data.cmd_queue_input, avail, &e1, &len1, &e2, &len2);
        dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);

        if (avail == (len1 + len2)) {
            memcpy(g_dma_data.recv_cmd_data, e1, len1);
            memcpy(g_dma_data.recv_cmd_data + len1, e2, len2);
            buf = g_dma_data.recv_cmd_data;
        } else {
            dma_mutex_lock(DMA_CMD_INPUT_MUTEX_ID);
            dma_reset_queue(&g_dma_data.cmd_queue_input);
            dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);
            DMA_TRACE("enter %s Line %d\n", __func__, __LINE__);
            return 0;
        }
    } else {
        dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);
        DMA_TRACE("enter %s Line %d\n", __func__, __LINE__);
        return 0;
    }

    if (g_dma_data.inited != DUEROS_DMA_INITED) {
        DMA_TRACE("g_dma_data.inited != DUEROS_DMA_INITED\n");
        return -1;
    }

    ((uint8_t*)&cmd_header)[0] = buf[1];
    ((uint8_t*)&cmd_header)[1] = buf[0];

    if (cmd_header.length) {
        payload_buf = buf + 4;
        payload_len = buf[2] << 8 | payload_buf[3];
        process_len = payload_len + 4;
    } else {
        payload_buf = buf + 3;
        payload_len = buf[2];
        process_len = payload_len + 3;
    }

    envelope = control_envelope__unpack(NULL, payload_len, payload_buf);

    switch (envelope->command) {
    case COMMAND__GET_DEVICE_INFORMATION: {
        DMA_TRACE("command  COMMAND__GET_DEVICE_INFORMATION id=%s\n", envelope->request_id);
        dma_send_get_dev_info_ack(envelope->request_id, true);
        DMA_TRACE("%s Line %d", __func__, __LINE__);
        break;
    }

    case COMMAND__GET_DEVICE_CONFIGURATION: {
        DMA_TRACE("COMMAND__GET_DEVICE_CONFIGURATION %s \n", envelope->request_id);
        dma_sent_get_device_configuration_ack(envelope->request_id, true);
        break;
    }

    case COMMAND__PAIR: {
        DMA_TRACE("command  COMMAND__PAIR id= %s\n", envelope->request_id);
        dma_paired_ack(envelope->request_id, true);
        g_dma_data.dma_started = true;
        dma_sync_state(true);
        break;
    }

    case COMMAND__GET_STATE: {
        DMA_TRACE("command  COMMAND__GET_STATE id=%s, f %x\n", envelope->request_id, envelope->getstate->feature);

        if (0x02 ==  envelope->getstate->feature) {
            DMA_TRACE("dueros_dma.rand %s", g_dma_data.rand);

            if (!dma_check_summary(envelope)) {
                dma_send_get_state_check_sum_fail(envelope->request_id, true);
                goto _go_exit;
            }

            g_dma_data.dma_started = true;
            dma_sync_state(true);
        }

        dma_send_get_state_ack(envelope->getstate->feature, envelope->request_id, true);
        break;
    }

    case COMMAND__SET_STATE: {
        DMA_TRACE("command  COMMAND__SET_STATE id=%s, f %x\n", envelope->request_id, envelope->setstate->state->feature);
        dma_send_set_state_ack(envelope->setstate, envelope->request_id, true);
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);
        uint32_t cmd = 0;

        if ((DMA_TWS_MASTER == role_type) && (1 == dma_get_mobile_connect_type())) {
            cmd = (1 << DMA_SYNC_LEFT_TAP_SETTING) |
                  (1 << DMA_SYNC_RIGHT_TAP_SETTING) |
                  (1 << DMA_SYNC_LEFT_WEARING_ON) |
                  (1 << DMA_SYNC_RIGHT_WEARING_ON) |
                  (1 << DMA_SYNC_LEFT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_RIGHT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_PULLUP_ENABLE) |
                  (1 << DMA_SYNC_WAKEUP_ENABLE) |
                  (1 << DMA_SYNC_OOBE_REPORT_ENABLE) |
                  (1 << DMA_SYNC_DMA_CONNECT_STATE) |
                  (1 << DMA_SYNC_UPSTREAM_CONFIG) |
                  (1 << DMA_SYNC_DEVICE_INFO);
            dma_sync_peer_state(cmd);
        }

        break;
    }

    case COMMAND__SYNCHRONIZE_STATE_ACK: {
        DMA_TRACE("command  COMMAND__SYNCHRONIZE_STATE_ACK id=%s\n", envelope->request_id);
        //do nothing
        break;
    }

    case COMMAND__PROVIDE_SPEECH : {
        DMA_TRACE("command  COMMAND__PROVIDE_SPEECH id = %s,rand=%s\n",
                  envelope->request_id, g_dma_data.rand);
        g_dma_data.wait_start_speech_ack = false;
        g_dma_data.dma_started = true;
        DMA_TRACE("command  envelope->providespeech->dialog =%x", envelope->providespeech->dialog->id);
        dma_set_stream_upload_enable(true);
        g_dma_data.upstream_started = true;
        g_dma_data.stream_packet_index = 0;
        dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_START, DMA_VOICE_STREAM_OPUS);
        dma_send_provide_speech_ack(envelope->providespeech->dialog->id, envelope->request_id, true);
        break;
    }

    case COMMAND__START_SPEECH_ACK: {
        DMA_TRACE("command  COMMAND__START_SPEECH_ACK  %s,rand=%s\n", envelope->request_id, g_dma_data.rand);
        g_dma_data.wait_start_speech_ack = false;
        break;;
    }

    case COMMAND__STOP_SPEECH: {
        DMA_TRACE("command  COMMAND__STOP_SPEECH id=%s,rand =%s\n", envelope->request_id, g_dma_data.rand);

        if (g_dma_data.wait_start_speech_ack) {
            DMA_TRACE("wait for start speech ack");
            break;
        }

        g_dma_data.upstream_started = false;
        dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_STOP, DMA_VOICE_STREAM_NONE);
        dma_set_stream_upload_enable(false);
        dma_send_ack_common(envelope->request_id, COMMAND__STOP_SPEECH_ACK, true);
        break;
    }

    case COMMAND__STOP_SPEECH_ACK: {
        DMA_TRACE("command  COMMAND__STOP_SPEECH_ACK id=%s\n\n", envelope->request_id);
        //do nothing
        break;
    }

    case COMMAND__NOTIFY_SPEECH_STATE: {
        DMA_TRACE("command  COMMAND__NOTIFY_SPEECH_STATE state=%d\n", envelope->notifyspeechstate->state);
        dma_notify_speech_state(envelope->notifyspeechstate->state);
        dma_send_ack_common(envelope->request_id, COMMAND__NOTIFY_SPEECH_STATE_ACK, true);
        break;
    }

    case COMMAND__NOTIFY_DEVICE_CONFIGURATION_ACK: {
        DMA_TRACE("command  COMMAND__NOTIFY_DEVICE_CONFIGURATION_ACK id=%s\n", envelope->request_id);
        //do nothing
        break;
    }

    case COMMAND__FORWARD_AT_COMMAND: {
#define AT_COMMAND_SIZE   (32)
        DMA_TRACE("command  COMMAND__FORWARD_AT_COMMAND id=%s\n", envelope->request_id);
        char cmd[AT_COMMAND_SIZE] = {0};
        strncpy(cmd, envelope->forwardatcommand->command, sizeof(cmd) - 1);

        if (strlen(cmd) < (sizeof(cmd) - 1)) {
            cmd[strlen(cmd)] = 0x0D;
        }

        cmd[AT_COMMAND_SIZE - 1] = '\0';
        dma_process_cmd(DMA_SEND_ATCMD, cmd, strlen(cmd));
        dma_send_ack_common(envelope->request_id, COMMAND__FORWARD_AT_COMMAND_ACK, true);
        break;
    }

    case COMMAND__END_POINT_SPEECH: {
        DMA_TRACE("command  COMMAND__END_POINT_SPEECH id=%s\n", envelope->request_id);
        dma_send_ack_common(envelope->request_id, COMMAND__END_POINT_SPEECH_ACK, true);
        break;
    }

    case COMMAND__FORWARD_TEST_COMMAND: {
        DMA_TRACE("command  COMMAND__FORWARD_TEST_COMMAND id=%s\n", envelope->request_id);
        dma_send_ack_common(envelope->request_id, COMMAND__FORWARD_TEST_COMMAND_ACK, true);
        break;
    }

    case COMMAND__ISSUE_MEDIA_CONTROL: {
        DMA_TRACE("command  COMMAND__ISSUE_MEDIA_CONTROL\n");
        DMA_TWS_ROLE_TYPE role_type;
        DMA_OPERATION_CMD cmd = DMA_OPERATION_NO_CMD;
        ret = dma_get_tws_role(&role_type);

        if (!ret || (DMA_TWS_MASTER != role_type)) {
            break;
        }

        switch (envelope->issue_media_control->control) {
        case MEDIA_CONTROL__PLAY:
            cmd = DMA_AUDIO_PLAY;
            break;

        case MEDIA_CONTROL__PAUSE:
            cmd = DMA_AUDIO_PAUSE;
            break;

        case MEDIA_CONTROL__NEXT:
            cmd = DMA_AUDIO_PLAY_FORWARD;
            break;

        case MEDIA_CONTROL__PREVIOUS:
            cmd = DMA_AUDIO_PLAY_BACKWARD;
            break;

        default:
            cmd = DMA_OPERATION_NO_CMD;
            break;
        }

        dma_process_cmd(cmd, NULL, 0);
        dma_send_ack_common(envelope->request_id, COMMAND__ISSUE_MEDIA_CONTROL_ACK, true);
        break;
    }

    default: {

        DMA_TRACE("COMMAND__default\n");
    }
    }

    DMA_TRACE("-end_ev_command=%d-\n", envelope->command);

_go_exit:

    control_envelope__free_unpacked(envelope, NULL);

    {
        int st = 0;
        dma_mutex_lock(DMA_CMD_INPUT_MUTEX_ID);
        st = dma_dequeue(&g_dma_data.cmd_queue_input, NULL, process_len);
        dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);

        if (st != DMA_OK) {
            DMA_TRACE("%s: DeCQueue fail, queue avail %d, want to dequeue %d\n", __func__,
                      dma_length_of_queue(&g_dma_data.cmd_queue_input), payload_len + 2);
        }
    }

    if (dma_length_of_queue(&g_dma_data.cmd_queue_input)) {
        goto cmd_recv_process_again;
    }

    return 0;
}

void dma_process(uint32_t timeout_ms)
{
    dma_sem_wait(timeout_ms);
    DMA_TRACE("[=======]dma thread\n");
    dma_process_cmd_send();
    dma_process_cmd_recv();
    dma_process_data_send();

    if (((dma_length_of_queue(&g_dma_data.cmd_queue_input)) || (dma_length_of_queue(&g_dma_data.cmd_queue_output))
            || (dma_length_of_queue(&g_dma_data.data_queue))) && (dma_get_prepare_state())) {
        dma_sem_signal();
    }

}

void dma_register_operation(DUER_DMA_OPER* p_dma_operation)
{
    if (p_dma_operation == NULL) {
        return;
    }

    g_dma_operation_ptr = p_dma_operation;
}

DMA_ERROR_CODE dma_protocol_init(const uint8_t* client_id, const uint8_t* client_secret, const uint8_t* vendor_id,
                                 uint8_t device_type, uint8_t device_version, uint8_t* protocol_buffer)
{
    memset(&g_dma_data, 0, sizeof(g_dma_data));
    g_dma_data.inited = DUEROS_DMA_INITED;

    if (!dma_get_all_config()) {
        g_dma_data.inited = 0;
        return DMA_SYSTEM_ERROR;
    }

    g_dma_data.client_id = (uint8_t*)client_id;
    g_dma_data.client_secret = (uint8_t*)client_secret;
    g_dma_data.vendor_id = (uint8_t*)vendor_id;
    g_dma_data.device_type = device_type;
    g_dma_data.device_version = device_version;

#define DUEROS_DMA_MAGIC 0xFDC2

    PROTOCOL_VER ver = \
    {
        \
        .magic = DUEROS_DMA_MAGIC, \
        .hVersion = 1, \
        .lVersion = 2, \
        .reserve = {0, 0, 0, 0,}, \
    };
    memcpy(&g_dma_data.protocol_version, &ver, sizeof(ver));

    dma_get_device_capability(&g_dma_data.dma_capability);

    if (g_dma_data.dma_capability.is_earphone) {
        g_dma_data.is_tws_device = true;
    }

    dma_init_queue(&g_dma_data.cmd_queue_input, DUEROS_DMA_CMD_INPUT_SIZE, protocol_buffer);
    dma_init_queue(&g_dma_data.cmd_queue_output, DUEROS_DMA_CMD_OUTPUT_SIZE, &protocol_buffer[DUEROS_DMA_CMD_INPUT_SIZE]);
    dma_init_queue(&g_dma_data.data_queue, DUEROS_DMA_DATA_SIZE,
                   &protocol_buffer[DUEROS_DMA_CMD_INPUT_SIZE + DUEROS_DMA_CMD_OUTPUT_SIZE]);

    DMA_TWS_SIDE_TYPE side_type = DMA_SIDE_UNKNOWN;
    bool ret = dma_get_tws_side(&side_type);

    if (!ret || (DMA_SIDE_UNKNOWN == side_type)) {
        return DMA_USER_INTERFACE_ERROR;
    }

    g_dma_data.is_left_side = (DMA_SIDE_LEFT == side_type);

    dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn);

    uint8_t fw_rev[4] = {0, 1, 0, 0};
    dma_get_firmeware_version(&fw_rev[0], &fw_rev[1], &fw_rev[2], &fw_rev[3]);
    sprintf((char*)g_dma_data.software_version, "%d.%d.%d.%d", fw_rev[0], fw_rev[1], fw_rev[2], fw_rev[3]);

    g_dma_data.inited = DUEROS_DMA_INITED;
    return DMA_SUCCESS;
}

DMA_ERROR_CODE dma_recv_mobile_data(const char* input_data, uint32_t date_len)
{
    bool ret = false;

    ret = dma_mutex_lock(DMA_CMD_INPUT_MUTEX_ID);

    if (!ret) {
        return DMA_SYSTEM_ERROR;
    }

    if (DMA_OK != dma_enqueue(&g_dma_data.cmd_queue_input, (DMA_ITEM_TYPE*)input_data, date_len)) {
        dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);
        return DMA_NOT_ENOUGH_MEMORY;
    }

    dma_mutex_unlock(DMA_CMD_INPUT_MUTEX_ID);

    ret = dma_sem_signal();

    if (!ret) {
        return DMA_SYSTEM_ERROR;
    }

    return DMA_SUCCESS;
}

DMA_ERROR_CODE dma_recv_peer_data(const char* input_data, uint32_t date_len)
{
    if (NULL == input_data) {
        return DMA_UNKNOWN;
    }

    DMA_SHARE_INFO* share_info = (DMA_SHARE_INFO*)input_data;

    if (share_info->magic_number != DMA_INFO_MAGIC_NUMBER) {
        return DMA_UNKNOWN;
    }

    dma_mutex_lock(DMA_DATA_MUTEX_ID);
    memset(&g_dma_data.share_info_recv, 0, sizeof(DMA_SHARE_INFO));
    memcpy(&g_dma_data.share_info_recv, input_data, date_len);
    dma_mutex_unlock(DMA_DATA_MUTEX_ID);

    dma_save_peer_data();

    dma_sem_signal();
    return DMA_SUCCESS;
}

DMA_ERROR_CODE dma_feed_compressed_data(const char* input_data, uint32_t date_len)
{
    bool ret = false;

    DMA_TRACE("%s recv %d bytes data\n", __func__, date_len);
    ret = dma_mutex_lock(DMA_DATA_MUTEX_ID);

    if (!ret) {
        return DMA_SYSTEM_ERROR;
    }

    if (DMA_OK != dma_enqueue(&g_dma_data.data_queue, (DMA_ITEM_TYPE*)input_data, date_len)) {
        dma_mutex_unlock(DMA_DATA_MUTEX_ID);
        return DMA_NOT_ENOUGH_MEMORY;
    }

    dma_mutex_unlock(DMA_DATA_MUTEX_ID);

    ret = dma_sem_signal();

    if (!ret) {
        return DMA_SYSTEM_ERROR;
    }

    return DMA_SUCCESS;
}

DMA_ERROR_CODE dma_notify_state(DMA_NOTIFY_STATE state, void* param_buf, uint32_t  param_size)
{
    DMA_TRACE("enter %s state = %d\n", __func__, state);

    switch (state) {
    case DMA_NOTIFY_STATE_DMA_CONNECTED: {
        dma_voice_connected();
        break;
    }

    case DMA_NOTIFY_STATE_DMA_DISCONNECTED: {
        dma_voice_disconnected();
        break;
    }

    case DMA_NOTIFY_STATE_MOBILE_CONNECTED: {
        dma_mobile_connected();
        break;
    }

    case DMA_NOTIFY_STATE_MOBILE_DISCONNECTED: {
        dma_mobile_disconnected();
        break;
    }

    case DMA_NOTIFY_STATE_SEND_PREPARE_DONE: {
        if ((dma_length_of_queue(&g_dma_data.cmd_queue_input)) || (dma_length_of_queue(&g_dma_data.cmd_queue_output))
                || (dma_length_of_queue(&g_dma_data.data_queue))) {
            dma_sem_signal();
        }

        break;
    }

    case DMA_NOTIFY_STATE_TWS_CONNECT: {
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);
        uint32_t cmd = 0;
        g_dma_data.tws_connected = true;

        //修复初次tws配对后，dma未更新右耳mac和sn，导致连不上app问题  -- yh 
        if (dma_get_mobile_connect_type()) {
            /* BT MAC */
            memset(g_dma_data.get_device_info, 0, sizeof(g_dma_data.get_device_info));
            dma_get_bt_address(DMA_BT_ADDR_DEFAULT, g_dma_data.get_device_info);
        } else if (role_type == DMA_TWS_SLAVE){
            dma_set_ble_advertise_enable(false); //从机关广播
        } else {
            dma_start_ble_adv(true); //主机更新master addr 重新广播
        }
        dma_get_serial_number(DMA_SN_DEFAULT, g_dma_data.current_sn); //更新sn


        if (DMA_TWS_MASTER == role_type) {
            cmd = (1 << DMA_SYNC_LEFT_TAP_SETTING) |
                  (1 << DMA_SYNC_RIGHT_TAP_SETTING) |
                  (1 << DMA_SYNC_LEFT_WEARING_ON) |
                  (1 << DMA_SYNC_RIGHT_WEARING_ON) |
                  (1 << DMA_SYNC_LEFT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_RIGHT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_PULLUP_ENABLE) |
                  (1 << DMA_SYNC_WAKEUP_ENABLE) |
                  (1 << DMA_SYNC_OOBE_REPORT_ENABLE) |
                  (1 << DMA_SYNC_DMA_CONNECT_STATE) |
                  (1 << DMA_SYNC_UPSTREAM_CONFIG) |
                  (1 << DMA_SYNC_DEVICE_INFO);
            dma_sync_peer_state(cmd);

            if (g_dma_data.is_left_side) {
                dma_send_sync_state(0xe005);
            } else {
                dma_send_sync_state(0xe004);
            }
        } else {
            cmd = (1 << DMA_SYNC_LEFT_WEARING_ON) |
                  (1 << DMA_SYNC_RIGHT_WEARING_ON) |
                  (1 << DMA_SYNC_LEFT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_RIGHT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_DEVICE_INFO);
            dma_sync_peer_state(cmd);
        }

        dma_sync_state(false);
        dma_sem_signal();
        break;
    }

    case DMA_NOTIFY_STATE_TWS_DISCONNECT: {
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);
        g_dma_data.tws_connected = false;
        g_dma_data.recv_peer_battery_level = false;
        g_dma_data.recv_peer_dma_version = false;

        if (DMA_TWS_MASTER == role_type) {
            if (g_dma_data.is_left_side) {
                dma_send_sync_state(0xe005);
            } else {
                dma_send_sync_state(0xe004);
            }

            dma_sem_signal();
        }

        break;
    }

    case DMA_NOTIFY_STATE_BOX_OPEN: {
        dma_start_ble_adv(true);
        break;
    }

    case DMA_NOTIFY_STATE_BOX_CLOSE: {
        dma_set_ble_advertise_enable(false);
        break;
    }

    case DMA_NOTIFY_STATE_ROLE_SWITCH_START: {
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);
        uint32_t cmd = 0;

        if (DMA_TWS_MASTER == role_type) {
            cmd = (1 << DMA_SYNC_LEFT_TAP_SETTING) |
                  (1 << DMA_SYNC_RIGHT_TAP_SETTING) |
                  (1 << DMA_SYNC_LEFT_WEARING_ON) |
                  (1 << DMA_SYNC_RIGHT_WEARING_ON) |
                  (1 << DMA_SYNC_LEFT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_RIGHT_BATTERY_LEVEL) |
                  (1 << DMA_SYNC_PULLUP_ENABLE) |
                  (1 << DMA_SYNC_WAKEUP_ENABLE) |
                  (1 << DMA_SYNC_OOBE_REPORT_ENABLE) |
                  (1 << DMA_SYNC_DMA_CONNECT_STATE) |
                  (1 << DMA_SYNC_UPSTREAM_CONFIG) |
                  (1 << DMA_SYNC_DEVICE_INFO);
            dma_sync_peer_state(cmd);

            if (1 == dma_get_mobile_connect_type()) {
                dma_send_sync_state(0xe00d);
            }
        }

        dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_STOP, DMA_VOICE_STREAM_NONE);
        dma_set_stream_upload_enable(false);
        dma_set_wakeup_enable(false);
        dma_sem_signal();
        break;
    }

    case DMA_NOTIFY_STATE_ROLE_SWITCH_FINISH: {
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);

        if ((DMA_TWS_MASTER == role_type) || (!g_dma_data.is_tws_device)) {
            uint32_t mtu;
            dma_get_mobile_mtu(&mtu);

            if (mtu >= 512) {
                g_dma_data.trans_size = 320;
            } else if (mtu >= 180) {
                g_dma_data.trans_size = 160;
            }

            //修复连接ios app主从切换后，新主机不开广播导致连接不上app
            if (1 == dma_get_mobile_connect_type()) {
                g_dma_data.dma_linked  = false;
            }

            dma_start_ble_adv(true);

            if (g_dma_data.upstream_started) {
                dma_set_stream_upload_enable(true);
                dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_START, g_dma_data.codec_type);
            }

            if (g_dma_data.wakeup_enable) {
                dma_set_wakeup_enable(true);
            }
        }

        break;
    }

    case DMA_NOTIFY_STATE_DOUBLE_CLICK: {
        bool is_left = *(bool*)param_buf;
        dma_custom_double_click_handler(is_left);
        break;
    }

    case DMA_NOTIFY_STATE_KEYWORD_DETECTED: {
        if (g_dma_data.dma_started) {
            dma_set_stream_upload_enable(true);
            g_dma_data.upstream_started = true;
            g_dma_data.stream_packet_index = 0;
            dma_set_voice_mic_stream_state(DMA_VOICE_STREAM_START, DMA_VOICE_STREAM_OPUS);
            dma_start_speech(0);
            dma_play_local_tts(DMA_TTS_KEYWORD_DETECTED, true);
        } else {
            dma_play_local_tts(DMA_TTS_USE_XIAODU_APP, true);
        }

        break;
    }

    case DMA_NOTIFY_STATE_BATTERY_LEVEL_UPDATE: {
        DMA_TWS_ROLE_TYPE role_type = DMA_TWS_UNKNOWN;
        dma_get_tws_role(&role_type);

        if (DMA_TWS_SLAVE == role_type && g_dma_data.tws_connected) {
            uint32_t cmd = (1 << DMA_SYNC_LEFT_WEARING_ON) |
                           (1 << DMA_SYNC_RIGHT_WEARING_ON) |
                           (1 << DMA_SYNC_LEFT_BATTERY_LEVEL) |
                           (1 << DMA_SYNC_RIGHT_BATTERY_LEVEL);
            dma_sync_peer_state(cmd);
        }
    }

    default:
        break;
    }

    return DMA_SUCCESS;
}


#endif
