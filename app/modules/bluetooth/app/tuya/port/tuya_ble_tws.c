#include "include.h"
#include "tuya_ble_log.h"
#include "tuya_ble_app.h"
#include "tuya_ble_ota.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_tws.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_data_handler.h"

#if LE_TUYA_EN

extern tuya_ble_parameters_settings_t tuya_ble_current_para;

#if BT_TWS_EN
u8 tuya_ble_tws_data[250] AT(.tuya_data);
u16 tuya_ble_tws_data_len;

#if TUYA_OTA_TWS_EN
extern u8 tuya_tws_ota_data[512 + 8];
extern u16 tuya_tws_ota_len;
#endif

#if TUYA_AUTH_INFO_SYNC
bool tuya_ble_tws_is_auth_master(void)
{
    return (sys_cb.tws_left_channel == 1);
}
#endif

AT(.text.fot.cache)
uint16_t tws_set_fot_data(uint8_t *data_ptr, uint16_t size)
{
    tuya_ble_app.data_receive_flag = 1;

#if (TUYA_BLE_OTA_EN && TUYA_OTA_TWS_EN)
    if((data_ptr[0] == TWS_INFO_OTA) && (data_ptr[1] == TUYA_BLE_TWS_OTA_DATA)){
        tuya_ble_tws_ota_set_data(&data_ptr[2], size - 2);
    }else
#endif
    {
        tuya_ble_custom_evt_send_with_data(CUSTOM_EVT_TWS_SYNC,data_ptr,size);
    }

    return 0;
}

AT(.text.fot.cache)
uint16_t tws_get_fot_data(uint8_t *buf)
{
    u16 len = 0;

#if (TUYA_BLE_OTA_EN && TUYA_OTA_TWS_EN)
    if(tuya_tws_ota_len){
        memcpy(buf, tuya_tws_ota_data, tuya_tws_ota_len);
        len = tuya_tws_ota_len;
    }else
#endif
    {
        if(tuya_ble_tws_data_len){
            memcpy(buf, tuya_ble_tws_data, tuya_ble_tws_data_len);
            len = tuya_ble_tws_data_len;
        }
    }

    return len;
}

void tuya_ble_tws_data_sync(void)
{
    if(bt_tws_is_connected() && tuya_ble_tws_data_len){
        bt_tws_sync_fot_data();
    }
}

void tuya_ble_tws_eq_sync(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_EQ;
    tuya_ble_tws_data[offset++] = tuya_ble_app.eq_en;
    tuya_ble_tws_data[offset++] = tuya_ble_app.eq_mode;
    memcpy(&tuya_ble_tws_data[offset], tuya_ble_app.eq_data, TUYA_EQ_BAND_CNT);
    offset += TUYA_EQ_BAND_CNT;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_info_all_sync(void)
{
    u8 offset = 0;
    tuya_ble_tws_data[offset++] = TWS_INFO_ALL;

    tuya_ble_tws_data[offset++] = tuya_ble_app.eq_en;
    tuya_ble_tws_data[offset++] = tuya_ble_app.eq_mode;
    memcpy(&tuya_ble_tws_data[offset], tuya_ble_app.eq_data, TUYA_EQ_BAND_CNT);
    offset += TUYA_EQ_BAND_CNT;

    tuya_ble_tws_data[offset++] = bsp_get_bat_level();

#if DP_ID_ANC_MODE
    tuya_ble_tws_data[offset++] = sys_cb.anc_user_mode;
#endif

#if DP_ID_LOW_LATENCY
    tuya_ble_tws_data[offset++] = tuya_ble_app.low_latency;
#endif

#if TUYA_AUTH_INFO_SYNC
    if(tuya_ble_tws_is_auth_master()){
        memcpy(&tuya_ble_tws_data[offset], TUYA_DEVICE_UUID, DEVICE_ID_LEN);
        offset += DEVICE_ID_LEN;
        memcpy(&tuya_ble_tws_data[offset], TUYA_DEVICE_AUTH_KEY, AUTH_KEY_LEN);
        offset += AUTH_KEY_LEN;
        memcpy(&tuya_ble_tws_data[offset], TUYA_DEVICE_MAC, MAC_STRING_LEN);
        offset += MAC_STRING_LEN;
    }
#endif

    tuya_ble_tws_data[offset++] = tuya_ble_app.bond_flag;

#if DP_ID_KEY_CTL_EN
    memcpy(&tuya_ble_tws_data[offset], &tuya_ble_app.local_key, sizeof(tuya_ble_key_info_t));
    offset += sizeof(tuya_ble_key_info_t);
#endif

#if DP_ID_KEY_SHORT_EN
    tuya_ble_tws_data[offset++] = sys_cb.key_en;
#endif

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();

    TUYA_APP_LOG_HEXDUMP_DEBUG("tuya_ble_tws_info_all_sync",tuya_ble_tws_data,offset);
}

void tuya_ble_tws_sys_settings_sync(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_TUYA_SYS_SETTING;
    memcpy(&tuya_ble_tws_data[offset], (uint8_t *)&tuya_ble_current_para.sys_settings, sizeof(tuya_ble_sys_settings_t));
    offset += sizeof(tuya_ble_sys_settings_t);

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}

#if DP_ID_VBAT_LEFT
void tuya_ble_tws_vbat_exchange(void)
{
    if(bt_tws_is_connected() && (!tuya_ble_ota_is_start()) && (bt_get_status() < BT_STA_INCOMING)){
        tuya_ble_tws_data[0] = TWS_INFO_VBAT;
        tuya_ble_tws_data[1] = bsp_get_bat_level();
        tuya_ble_tws_data[2] = 1;
        tuya_ble_tws_data_len = 3;
        tuya_ble_tws_data_sync();
    }
}
#endif

void tuya_ble_tws_reset_sync(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_RST_FACTORY;
    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}

#if DP_ID_BT_NAME
void tuya_ble_tws_bt_name_sync(void)
{
    u8 offset = 0;
    u8 name_len = strlen(xcfg_cb.bt_name);

    tuya_ble_tws_data[offset++] = TWS_INFO_BT_NAME;
    tuya_ble_tws_data[offset++] = name_len;
    memcpy(&tuya_ble_tws_data[offset], xcfg_cb.bt_name, name_len);
    offset += name_len;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}
#endif

#if DP_ID_PP
void tuya_ble_tws_play_sta_sync(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_PLAY_STA;
    tuya_ble_tws_data[offset++] = tuya_ble_app.play_sta;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}
#endif

void tuya_ble_tws_bond_sta_sync(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_BOND_STA;
    tuya_ble_tws_data[offset++] = tuya_ble_app.bond_flag;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}

#if DP_ID_KEY_CTL_EN
void tuya_ble_tws_key_info_sync(void)
{
    u8 offset = 0;
    tuya_ble_tws_data[offset++] = TWS_INFO_KEY;
    memcpy(&tuya_ble_tws_data[offset], &tuya_ble_app.remote_key, sizeof(tuya_ble_key_info_t));
    offset += sizeof(tuya_ble_key_info_t);

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}
#endif

#if DP_ID_KEY_SHORT_EN
void tuya_ble_tws_key_en_sync(void)
{
    u8 offset = 0;
    tuya_ble_tws_data[offset++] = TWS_INFO_KEY_EN;
    tuya_ble_tws_data[offset++] = sys_cb.key_en;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}
#endif

#if DP_ID_KEY_RST
void tuya_ble_tws_key_reset_sync(void)
{
    u8 offset = 0;
    tuya_ble_tws_data[offset++] = TWS_INFO_KEY_RST;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}
#endif

#if DP_ID_BT_RST
void tuya_ble_tws_bt_reset_sync(void)
{
    u8 offset = 0;
    tuya_ble_tws_data[offset++] = TWS_INFO_BT_RESET;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}
#endif

void tuya_ble_tws_device_unbond_sync(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_DEVICE_UNBOND;

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_device_unbond(void)
{
    tuya_ble_device_unbond();
    tuya_ble_tws_device_unbond_sync();
}

void tuya_ble_tws_info_all_rsp(void)
{
    u8 offset = 0;

    tuya_ble_tws_data[offset++] = TWS_INFO_ALL_RSP;

    tuya_ble_tws_data[offset++] = bsp_get_bat_level();

#if TUYA_AUTH_INFO_SYNC
    if(tuya_ble_tws_is_auth_master()){
        memcpy(&tuya_ble_tws_data[offset], TUYA_DEVICE_UUID, DEVICE_ID_LEN);
        offset += DEVICE_ID_LEN;
        memcpy(&tuya_ble_tws_data[offset], TUYA_DEVICE_AUTH_KEY, AUTH_KEY_LEN);
        offset += AUTH_KEY_LEN;
        memcpy(&tuya_ble_tws_data[offset], TUYA_DEVICE_MAC, MAC_STRING_LEN);
        offset += MAC_STRING_LEN;
    }
#endif

    tuya_ble_tws_data[offset++] = tuya_ble_app.bond_flag;

#if DP_ID_KEY_CTL_EN
    memcpy(&tuya_ble_tws_data[offset], &tuya_ble_app.local_key, sizeof(tuya_ble_key_info_t));
    offset += sizeof(tuya_ble_key_info_t);
#endif

    tuya_ble_tws_data_len = offset;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_auth_info_set(u8 *data_ptr)
{
#if TUYA_AUTH_INFO_SYNC
    u8 offset = 0;
    u16 flag = TUYA_AUTH_FLAG;
    uint8_t mac_temp[6];
    u8 remote_uuid[DEVICE_ID_LEN];

    memcpy(remote_uuid, &data_ptr[offset], DEVICE_ID_LEN);
    if(memcmp(remote_uuid, TUYA_DEVICE_UUID, DEVICE_ID_LEN)){
        memcpy(TUYA_DEVICE_UUID, remote_uuid, DEVICE_ID_LEN);
        offset += DEVICE_ID_LEN;
        memcpy(TUYA_DEVICE_AUTH_KEY, &data_ptr[offset], AUTH_KEY_LEN);
        offset += AUTH_KEY_LEN;
        memcpy(TUYA_DEVICE_MAC, &data_ptr[offset], MAC_STRING_LEN);
        offset += MAC_STRING_LEN;
        memcpy(tuya_ble_current_para.auth_settings.device_id, TUYA_DEVICE_UUID, DEVICE_ID_LEN);
        memcpy(tuya_ble_current_para.auth_settings.auth_key, TUYA_DEVICE_AUTH_KEY, AUTH_KEY_LEN);
        tuya_ble_str_to_hex((u8*)TUYA_DEVICE_MAC, MAC_STRING_LEN, mac_temp);
        tuya_ble_inverted_array(mac_temp,6);
        memcpy(tuya_ble_current_para.auth_settings.mac, mac_temp, MAC_LEN);
        tuya_ble_hextoascii(mac_temp, MAC_LEN, tuya_ble_current_para.auth_settings.mac_string);
        tuya_ble_adv_change();
        tuya_ble_nv_erase(TUYA_AUTH_INFO_ADDR,TUYA_NV_ERASE_MIN_SIZE);
        offset = 0;
        tuya_ble_nv_write(TUYA_AUTH_INFO_ADDR+offset, (const uint8_t *)&flag, 2);
        offset += 2;
        tuya_ble_nv_write(TUYA_AUTH_INFO_ADDR+offset, (const uint8_t *)TUYA_DEVICE_UUID, DEVICE_ID_LEN);
        offset += DEVICE_ID_LEN;
        tuya_ble_nv_write(TUYA_AUTH_INFO_ADDR+offset, (const uint8_t *)TUYA_DEVICE_AUTH_KEY, AUTH_KEY_LEN);
        offset += AUTH_KEY_LEN;
        tuya_ble_nv_write(TUYA_AUTH_INFO_ADDR+offset, (const uint8_t *)TUYA_DEVICE_MAC, MAC_STRING_LEN);
        offset += MAC_STRING_LEN;
//        TUYA_APP_LOG_DEBUG("tuya_ble_tws_auth_info_set:");
//        TUYA_APP_LOG_DEBUG("TUYA_DEVICE_UUID:%s\n",TUYA_DEVICE_UUID);
//        TUYA_APP_LOG_DEBUG("TUYA_DEVICE_AUTH_KEY:%s\n",TUYA_DEVICE_AUTH_KEY);
//        TUYA_APP_LOG_DEBUG("TUYA_DEVICE_MAC:%s\n",TUYA_DEVICE_MAC);
    }
#endif
}


void tuya_ble_tws_recv_proc(uint8_t *data_ptr, u16 size)
{
    u8 info_id = data_ptr[0];
    u8 offset = 1;

    if(TWS_INFO_OTA != info_id){
        TUYA_APP_LOG_HEXDUMP_DEBUG("tuya_ble_tws_recv_proc:",data_ptr,size);
    }

    switch(info_id){
        case TWS_INFO_EQ:
            TUYA_APP_LOG_DEBUG("TWS_INFO_EQ\n");
            if(tuya_ble_app.eq_en != data_ptr[1]){
                tuya_ble_app.eq_en = data_ptr[1];
                tuya_ble_cm_write(&tuya_ble_app.eq_en, TUYA_CM_EQ_EN, 1, 0);
            }
            if(memcmp(&tuya_ble_app.eq_mode, &data_ptr[2], 1 + TUYA_EQ_BAND_CNT)){
                tuya_ble_app.eq_mode = data_ptr[2];
                memcpy(tuya_ble_app.eq_data, &data_ptr[3], TUYA_EQ_BAND_CNT);
                tuya_ble_cm_write(&tuya_ble_app.eq_mode, TUYA_CM_EQ_DATA, 1 + TUYA_EQ_BAND_CNT, 0);
            }
            tuya_ble_app.do_flag |= TUYA_FLAG_EQ_SET;
            break;

#if DP_ID_VBAT_LEFT
        case TWS_INFO_VBAT:
            TUYA_APP_LOG_DEBUG("TWS_INFO_VBAT:%d",data_ptr[1]);
            tuya_ble_app.remote_vbat = data_ptr[1];
            if(data_ptr[2]){
                tuya_ble_tws_data[0] = TWS_INFO_VBAT;
                tuya_ble_tws_data[1] = bsp_get_bat_level();
                tuya_ble_tws_data[2] = 0;
                tuya_ble_tws_data_len = 3;
                tuya_ble_tws_data_sync();
            }else{
                tuya_ble_vbat_report_do();
            }
            break;
#endif

        case TWS_INFO_ALL:
            if(tuya_ble_app.eq_en != data_ptr[offset]){
                tuya_ble_app.eq_en = data_ptr[offset];
                tuya_ble_cm_write(&tuya_ble_app.eq_en, TUYA_CM_EQ_EN, 1, 0);
            }
            offset++;

            if(memcmp(&tuya_ble_app.eq_mode, &data_ptr[offset], 1 + TUYA_EQ_BAND_CNT)){
                tuya_ble_app.eq_mode = data_ptr[offset];
                memcpy(tuya_ble_app.eq_data, &data_ptr[offset+1], TUYA_EQ_BAND_CNT);
                tuya_ble_cm_write(&tuya_ble_app.eq_mode, TUYA_CM_EQ_DATA, 1 + TUYA_EQ_BAND_CNT, 0);
            }
            offset += (1 + TUYA_EQ_BAND_CNT);

            tuya_ble_app.remote_vbat = data_ptr[offset++];

#if DP_ID_ANC_MODE
            if(sys_cb.anc_user_mode != data_ptr[offset]){
                sys_cb.anc_user_mode = data_ptr[offset];
                tuya_ble_cm_anc_save();
            }
            offset++;
#endif

#if DP_ID_LOW_LATENCY
            if(tuya_ble_app.low_latency != data_ptr[offset]){
                tuya_ble_app.low_latency = data_ptr[offset];
                tuya_ble_cm_low_latency_save();
            }
            offset++;
#endif

#if TUYA_AUTH_INFO_SYNC
            if(tuya_ble_tws_is_auth_master() == false){
                tuya_ble_tws_auth_info_set(&data_ptr[offset]);
                offset += (DEVICE_ID_LEN + AUTH_KEY_LEN + MAC_STRING_LEN);
            }
#endif

            tuya_ble_app.remote_bond_flag = data_ptr[offset++];

#if DP_ID_KEY_CTL_EN
            if(memcmp(&tuya_ble_app.remote_key, &data_ptr[offset], sizeof(tuya_ble_key_info_t))){
                memcpy(&tuya_ble_app.remote_key, &data_ptr[offset], sizeof(tuya_ble_key_info_t));
                tuya_ble_cm_write(&tuya_ble_app.remote_key, TUYA_CM_KEY_REMOTE, sizeof(tuya_ble_key_info_t), 0);
            }
            offset += sizeof(tuya_ble_key_info_t);
#endif

#if DP_ID_KEY_SHORT_EN
            if(sys_cb.key_en != data_ptr[offset]){
                sys_cb.key_en = data_ptr[offset];
                tuya_ble_cm_write(&sys_cb.key_en, TUYA_CM_KEY_EN, 1, 0);
            }
            offset++;
#endif

            if(tuya_ble_app.init_flag){
                tuya_ble_eq_set();
#if DP_ID_ANC_MODE
                bsp_anc_set_mode(sys_cb.anc_user_mode);
#endif
            }

            tuya_ble_tws_info_all_rsp();
            tuya_ble_vbat_report_do();

            TUYA_APP_LOG_DEBUG("TWS_INFO_ALL");
            TUYA_APP_LOG_DEBUG("eq_en:%d",tuya_ble_app.eq_en);
            TUYA_APP_LOG_DEBUG("eq_mode:%d",tuya_ble_app.eq_mode);
            TUYA_APP_LOG_HEXDUMP_DEBUG("eq_data:",tuya_ble_app.eq_data,TUYA_EQ_BAND_CNT);
            TUYA_APP_LOG_DEBUG("remote_bat:%d",tuya_ble_app.remote_vbat);
#if DP_ID_ANC_MODE
            TUYA_APP_LOG_DEBUG("anc_mode:%d",sys_cb.anc_user_mode);
#endif
#if DP_ID_LOW_LATENCY
            TUYA_APP_LOG_DEBUG("low_latency:%d",tuya_ble_app.low_latency);
#endif
            TUYA_APP_LOG_DEBUG("remote_bond_flag:%d",tuya_ble_app.remote_bond_flag);
            break;

#if TUYA_BLE_OTA_EN
        case TWS_INFO_OTA:
            tuya_ble_ota_tws_data_proc(&data_ptr[1],size - 1);
            break;
#endif

        case TWS_INFO_TUYA_SYS_SETTING:{
            TUYA_APP_LOG_DEBUG("TWS_INFO_TUYA_SYS_SETTING");
            tuya_ble_sys_settings_t *sys_setting = (tuya_ble_sys_settings_t *)&data_ptr[1];
            tuya_ble_app.remote_bond_flag = sys_setting->bound_flag;
            if(tuya_ble_current_para.sys_settings.crc != sys_setting->crc){
                memcpy((uint8_t *)&tuya_ble_current_para.sys_settings,&data_ptr[1],size - 1);
                tuya_ble_storage_save_sys_settings();
                tuya_ble_adv_change();
#if TUYA_UNBOND_LINK_INFO_CLEAR
                if(tuya_ble_app.remote_bond_flag == 0){
                    bt_nor_delete_link_info();
                }
#endif
            }
        } break;

        case TWS_INFO_RST_FACTORY:
            TUYA_APP_LOG_DEBUG("TWS_INFO_RST_FACTORY");
            tuya_ble_device_reset_2_factory();
            break;

        case TWS_INFO_ALL_RSP:
            tuya_ble_app.remote_vbat = data_ptr[offset++];

#if TUYA_AUTH_INFO_SYNC
            if(tuya_ble_tws_is_auth_master() == false){
                tuya_ble_tws_auth_info_set(&data_ptr[offset]);
                offset += (DEVICE_ID_LEN + AUTH_KEY_LEN + MAC_STRING_LEN);
            }
#endif

            tuya_ble_app.remote_bond_flag = data_ptr[offset++];

#if DP_ID_KEY_CTL_EN
            if(memcmp(&tuya_ble_app.remote_key, &data_ptr[offset], sizeof(tuya_ble_key_info_t))){
                memcpy(&tuya_ble_app.remote_key, &data_ptr[offset], sizeof(tuya_ble_key_info_t));
                tuya_ble_cm_write(&tuya_ble_app.remote_key, TUYA_CM_KEY_REMOTE, sizeof(tuya_ble_key_info_t), 0);
            }
            offset += sizeof(tuya_ble_key_info_t);
#endif

            tuya_ble_vbat_report_do();
            break;

#if DP_ID_BT_NAME
        case TWS_INFO_BT_NAME:{
            TUYA_APP_LOG_DEBUG("TWS_INFO_BT_NAME");
            u8 name_len = data_ptr[1];
            memset(xcfg_cb.bt_name, 0, sizeof(xcfg_cb.bt_name));
            memcpy(xcfg_cb.bt_name, &data_ptr[2], name_len);
            tuya_ble_cm_write(xcfg_cb.bt_name, TUYA_CM_BT_NAME, name_len, 0);
            tuya_ble_cm_write(&name_len, TUYA_CM_BT_NAME_LEN, 1, 1);
            bt_set_stack_local_name(xcfg_cb.bt_name);
        } break;
#endif

#if DP_ID_PP
        case TWS_INFO_PLAY_STA:
            if(tuya_ble_app.play_sta != data_ptr[1]){
                tuya_ble_app.play_sta = data_ptr[1];
                if(tuya_ble_app.bond_flag){
                    tuya_ble_app_play_sta_report(NULL);
                }
            }
            break;
#endif

        case TWS_INFO_DEVICE_UNBOND:
            TUYA_APP_LOG_DEBUG("TWS_INFO_DEVICE_UNBOND");
            tuya_ble_device_unbond();
            break;

        case TWS_INFO_BOND_STA:
            TUYA_APP_LOG_DEBUG("TWS_INFO_BOND_STA");
            tuya_ble_app.remote_bond_flag = data_ptr[1];
            break;

#if DP_ID_KEY_CTL_EN
        case TWS_INFO_KEY:
            TUYA_APP_LOG_DEBUG("TWS_INFO_KEY");
            memcpy(&tuya_ble_app.local_key, &data_ptr[1], sizeof(tuya_ble_key_info_t));
            tuya_ble_key_set_do();
            break;
#endif

#if DP_ID_KEY_SHORT_EN
        case TWS_INFO_KEY_EN:
            if(sys_cb.key_en != data_ptr[1]){
                sys_cb.key_en = data_ptr[1];
                tuya_ble_cm_write(&sys_cb.key_en, TUYA_CM_KEY_EN, 1, 0);
            }
            break;
#endif

#if DP_ID_BT_RST
        case TWS_INFO_BT_RESET:
            TUYA_APP_LOG_DEBUG("TWS_INFO_BT_RESET");
            tuya_bt_reset_do();
            break;
#endif

#if DP_ID_KEY_RST
        case TWS_INFO_KEY_RST:
            TUYA_APP_LOG_DEBUG("TWS_INFO_KEY_RST");
            tuya_ble_key_reset_2_factory();
            break;
#endif

        default:
            break;
    }
}
#endif
#endif
