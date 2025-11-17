#include "include.h"
#include "ab_mate_tws.h"
#include "ab_mate_ota.h"
#include "ab_mate_timer.h"
#include "ab_mate_profile.h"

#if AB_MATE_APP_EN && BT_TWS_EN

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

u8 tws_sync_data[128] AT(.app.buf.tws);
u16 tws_sync_data_len = 0;

#if AB_MATE_OTA_EN
u8 tws_sync_ota[512+16] AT(.app.buf.fot);
u16 tws_sync_ota_len = 0;
#endif

uint16_t tws_set_fot_data(uint8_t *data_ptr, uint16_t size)
{
    ab_mate_tws_recv_proc(data_ptr,size);

    return 0;
}


uint16_t tws_get_fot_data(uint8_t *buf)
{
    u16 sync_len = 0;

#if AB_MATE_OTA_EN
    if(tws_sync_ota_len){
        memcpy(buf, tws_sync_ota, tws_sync_ota_len);
        sync_len = tws_sync_ota_len;
        tws_sync_ota_len = 0;
    }else if(tws_sync_data_len){
        memcpy(buf, tws_sync_data, tws_sync_data_len);
        sync_len = tws_sync_data_len;
        tws_sync_data_len = 0;
    }
#else
    if(tws_sync_data_len){
        memcpy(buf, tws_sync_data, tws_sync_data_len);
        sync_len = tws_sync_data_len;
        tws_sync_data_len = 0;
    }
#endif

    return sync_len;
}

void tws_data_sync_do(void)
{
    if(bt_tws_is_connected()){
        bt_tws_sync_fot_data();
    }
}

void ab_mate_tws_cmd_sync_byte(u8 cmd, u8 data)
{
    tws_sync_data[0] = cmd;
    tws_sync_data[1] = data;
    tws_sync_data_len = 2;
    tws_data_sync_do();
}

void ab_mate_tws_cmd_sync_nbyte(u8 cmd, u8* data, u8 len)
{
    u8 offset = 0;

    tws_sync_data[offset++] = cmd;
    memcpy(&tws_sync_data[offset], data, len);
    offset += len;

    tws_sync_data_len = offset;
    tws_data_sync_do();
}

void ab_mate_tws_vbat_sync(void)
{
    if((bt_get_status() < BT_STA_INCOMING) && !ab_mate_ota_is_start()){
        u8 bat_info[2];
        bat_info[0] = ab_mate_app.local_vbat;
        bat_info[1] = ab_mate_app.box_vbat;
        ab_mate_tws_cmd_sync_nbyte(TWS_INFO_VBAT, bat_info, 2);
    }
}

void ab_mate_tws_eq_info_sync(void)
{
#if AB_MATE_EQ_EN
    ab_mate_tws_cmd_sync_nbyte(TWS_INFO_EQ, (u8*)&ab_mate_app.eq_info,sizeof(ab_mate_eq_info_t));
#endif
}

void ab_mate_tws_key_info_sync(void)
{
#if AB_MATE_KEY_EN
    ab_mate_tws_cmd_sync_nbyte(TWS_INFO_KEY, (u8*)&ab_mate_app.remote_key, sizeof(ab_mate_key_info_t));
#endif
}

void ab_mate_tws_device_reset_sync(void)
{
    ab_mate_tws_cmd_sync_byte(TWS_INFO_DEVICE_RST, 0);
}

void ab_mate_tws_bt_name_sync(void)
{
#if AB_MATE_BT_NAME_EN
    u8 bt_name_len = strlen(xcfg_cb.bt_name);
    u8 offset = 0;

    tws_sync_data[offset++] = TWS_INFO_BT_NAME;
    tws_sync_data[offset++] = bt_name_len;
    memcpy(&tws_sync_data[offset],xcfg_cb.bt_name,bt_name_len);
    offset += bt_name_len;

    tws_sync_data_len = offset;
    tws_data_sync_do();
#endif
}

void ab_mate_tws_in_ear_en_sync(void)
{
#if AB_MATE_INERA_EN
    ab_mate_tws_cmd_sync_byte(TWS_INFO_IN_EAR, sys_cb.in_ear_en);
#endif
}

void ab_mate_tws_connect_sta_sync(void)
{
    ab_mate_tws_cmd_sync_byte(TWS_INFO_CONNECT_STA, ab_mate_app.con_sta);
}

void ab_mate_tws_link_clear_sync(void)
{
    ab_mate_tws_cmd_sync_byte(TWS_INFO_BT_LINK_CLEAR, 0);
}

void ab_mate_tws_led_info_sync(void)
{
#if AB_MATE_LED_EN
    ab_mate_tws_cmd_sync_byte(TWS_INFO_LED, sys_cb.led_scan_en);
#endif
}

void ab_mate_tws_anc_cur_level_sync(void)
{
#if AB_MATE_ANC_LEVEL
    ab_mate_tws_cmd_sync_byte(TWS_INFO_ANC_CUR_LEVEL, ab_mate_app.anc_cur_level);
#endif
}

void ab_mate_tws_tp_cur_level_sync(void)
{
#if AB_MATE_ANC_LEVEL
    ab_mate_tws_cmd_sync_byte(TWS_INFO_TP_CUR_LEVEL, ab_mate_app.tp_cur_level);
#endif
}

void ab_mate_tws_anc_fade_en_sync(void)
{
#if AB_MATE_ANC_LEVEL
    ab_mate_tws_cmd_sync_byte(TWS_INFO_ANC_FADE, ab_mate_app.anc_fade);
#endif
}

#if AB_MATE_V3D_AUDIO_EN
void ab_mate_tws_v3d_audio_sync(void)
{
    ab_mate_tws_cmd_sync_byte(TWS_INFO_V3D_AUDIO, ab_mate_app.v3d_audio_en);
}
#endif

#if AB_MATE_MULT_DEV_EN
void ab_mate_tws_mult_dev_info_sync(void)
{
    u8 offset = 0;
    tws_sync_data[offset++] = TWS_INFO_MULT_DEV;
    tws_sync_data[offset++] = TWS_SYNC_MULT_DEV_EN;         //T
    tws_sync_data[offset++] = 1;                            //L
    tws_sync_data[offset++] = ab_mate_app.mult_dev.en;      //V
    tws_sync_data[offset++] = TWS_SYNC_MULT_DEV_INFO;       //T
    tws_sync_data[offset] = ab_mate_connect_info_get(&tws_sync_data[offset+1]) +1; //L
                                                            //v  tws_sync_data[offset+1]
    offset += tws_sync_data[offset];                        //��L
    tws_sync_data_len = offset;
    tws_data_sync_do();
}
#endif

#if AB_MATE_DYNAMIC_BASS_EN
void ab_mate_tws_dynamic_bass_info_sync(void)
{
    u8 offset = 0;
    tws_sync_data[offset++] = TWS_INFO_DYNAMIC_BASS;
    tws_sync_data[offset++] = TWS_SYNC_DYNAMIC_BASS_EN;
    tws_sync_data[offset++] = 1;
    tws_sync_data[offset++] = ab_mate_app.dynamic_bass.en;
    tws_sync_data[offset++] = TWS_SYNC_DYNAMIC_BASS_LEVEL;
    tws_sync_data[offset++] = 1;
    tws_sync_data[offset++] = ab_mate_app.dynamic_bass.level;
    tws_sync_data_len = offset;
    tws_data_sync_do();
}
#endif

#if AB_MATE_DEVICE_FIND_EN
void ab_mate_tws_device_find_sync(void)
{
    ab_mate_tws_cmd_sync_byte(TWS_INFO_DEVICE_FIND, ab_mate_app.find_type);
}
#endif


void ab_mate_tws_info_all_sync(void)
{
    u8 offset = 0;

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    if(sys_cb.loc_bat & BIT(7)){
        ab_mate_app.local_vbat = sys_cb.loc_bat;
    }else{
        ab_mate_app.local_vbat = bsp_get_bat_level();
    }
#else
    ab_mate_app.local_vbat = bsp_get_bat_level();
#endif

    tws_sync_data[offset++] = TWS_INFO_ALL;

#if AB_MATE_EQ_EN
    memcpy(&tws_sync_data[offset],&ab_mate_app.eq_info,sizeof(ab_mate_eq_info_t));
    offset += sizeof(ab_mate_eq_info_t);
#endif

    tws_sync_data[offset++] = ab_mate_app.local_vbat;

#if AB_MATE_ANC_EN
    tws_sync_data[offset++] = sys_cb.anc_user_mode;
#endif

#if AB_MATE_KEY_EN
    memcpy(&tws_sync_data[offset],&ab_mate_app.local_key,sizeof(ab_mate_key_info_t));
    offset += sizeof(ab_mate_key_info_t);
#endif

#if AB_MATE_LATENCY_EN
    tws_sync_data[offset++] = ab_mate_app.latency_mode;
#endif

#if AB_MATE_INERA_EN
    tws_sync_data[offset++] = sys_cb.in_ear_en;
#endif

#if AB_MATE_BT_NAME_EN
    tws_sync_data[offset++] = strlen(xcfg_cb.bt_name);
    memcpy(&tws_sync_data[offset],xcfg_cb.bt_name,strlen(xcfg_cb.bt_name));
    offset += strlen(xcfg_cb.bt_name);
#endif

    tws_sync_data[offset++] = ab_mate_app.con_sta;

#if AB_MATE_LED_EN
    tws_sync_data[offset++] = sys_cb.led_scan_en;
#endif

#if AB_MATE_V3D_AUDIO_EN
    tws_sync_data[offset++] = ab_mate_app.v3d_audio_en;
#endif

#if AB_MATE_MULT_DEV_EN
    offset += ab_mate_connect_info_get(&tws_sync_data[offset]);
    tws_sync_data[offset++] = ab_mate_app.mult_dev.en;
#endif

#if AB_MATE_DYNAMIC_BASS_EN
    tws_sync_data[offset++] = ab_mate_app.dynamic_bass.en;
    tws_sync_data[offset++] = ab_mate_app.dynamic_bass.level;
#endif

    tws_sync_data_len = offset;
    tws_data_sync_do();

    TRACE("ab_mate_tws_info_all_sync\n");
    TRACE_R(tws_sync_data,offset);
}

void ab_mate_tws_info_all_rsp(void)
{
    u8 offset = 0;

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    if(sys_cb.loc_bat & BIT(7)){
        ab_mate_app.local_vbat = sys_cb.loc_bat;
    }else{
        ab_mate_app.local_vbat = bsp_get_bat_level();
    }
#else
    ab_mate_app.local_vbat = bsp_get_bat_level();
#endif

    tws_sync_data[offset++] = TWS_INFO_ALL_RSP;

    tws_sync_data[offset++] = ab_mate_app.local_vbat;

#if AB_MATE_KEY_EN
    memcpy(&tws_sync_data[offset],&ab_mate_app.local_key,sizeof(ab_mate_key_info_t));
    offset += sizeof(ab_mate_key_info_t);
#endif

    tws_sync_data[offset++] = ab_mate_app.con_sta;

    tws_sync_data_len = offset;
    tws_data_sync_do();

    TRACE("ab_mate_tws_info_all_rsp\n");
    TRACE_R(tws_sync_data,offset);
}

void ab_mate_tws_info_eq_proc(uint8_t *data_ptr, u16 size)
{
    memcpy(&ab_mate_app.eq_info, data_ptr, sizeof(ab_mate_eq_info_t));

    ab_mate_app.do_flag |= FLAG_EQ_SET;

    ab_mate_cm_write(&ab_mate_app.eq_info.mode, AB_MATE_CM_EQ_DATA, 1 + AB_MATE_EQ_BAND_CNT, 2);

#if AB_MATE_EQ_USE_DEVICE
    if(ab_mate_app.eq_info.mode >= AB_MATE_EQ_CUSTOM_INDEX){
        ab_mate_eq_custom_save();
    }
#endif
}

void ab_mate_tws_info_bat_proc(uint8_t *data_ptr, u16 size)
{
    u8 offset = 0;

    ab_mate_app.remote_vbat = data_ptr[offset++];

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    sys_cb.rem_bat = ab_mate_app.remote_vbat;
    sys_cb.rem_house_bat = data_ptr[offset++];
#endif


    printf("sys_cb.rem_bat:%d\r\n",sys_cb.rem_bat);
}

void ab_mate_tws_info_key_proc(uint8_t *data_ptr, u16 size)
{
    memcpy(&ab_mate_app.local_key, data_ptr, sizeof(ab_mate_key_info_t));

    ab_mate_key_set_do();
}

void ab_mate_tws_info_all_proc(uint8_t *data_ptr, u16 size)
{
    u16 flag = 0;
    u16 offset = 0;

#if AB_MATE_EQ_EN
    if(memcmp(&ab_mate_app.eq_info, &data_ptr[offset], sizeof(ab_mate_eq_info_t))){
        memcpy(&ab_mate_app.eq_info, &data_ptr[offset], sizeof(ab_mate_eq_info_t));
        ab_mate_cm_write(&ab_mate_app.eq_info.mode, AB_MATE_CM_EQ_DATA, 1+AB_MATE_EQ_BAND_CNT, 2);
        flag |= BIT(0);
    }
    TRACE("eq_mode:%d\n",ab_mate_app.eq_info.mode);
    TRACE("eq_data:");
    TRACE_R(ab_mate_app.eq_info.gain,10);
    offset += sizeof(ab_mate_eq_info_t);
#endif

    ab_mate_app.remote_vbat = data_ptr[offset++];

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    sys_cb.rem_bat = ab_mate_app.remote_vbat;
#endif
    TRACE("remote_bat:%d\n",ab_mate_app.remote_vbat);

#if AB_MATE_ANC_EN
    if(sys_cb.anc_user_mode != data_ptr[offset]){
        sys_cb.anc_user_mode = data_ptr[offset];
        flag |= BIT(1);
    }
    TRACE("anc_mode:%d\n",sys_cb.anc_user_mode);
    offset++;
#endif

#if AB_MATE_KEY_EN
    if(memcmp(&ab_mate_app.remote_key, &data_ptr[offset], sizeof(ab_mate_key_info_t))){
        memcpy(&ab_mate_app.remote_key, &data_ptr[offset], sizeof(ab_mate_key_info_t));
        ab_mate_cm_write(&ab_mate_app.remote_key, AB_MATE_CM_KEY_REMOTE, sizeof(ab_mate_key_info_t), 2);
    }
    TRACE("key_info:");
    TRACE_R(&ab_mate_app.remote_key,sizeof(ab_mate_key_info_t));
    offset += sizeof(ab_mate_key_info_t);
#endif

#if AB_MATE_LATENCY_EN
    if(ab_mate_app.latency_mode != data_ptr[offset]){
        ab_mate_app.latency_mode = data_ptr[offset];
    #if AB_MATE_LATENCY_USE_CM
        ab_mate_cm_write(&ab_mate_app.latency_mode, AB_MATE_CM_MODE, 1, 2);
    #endif
    }
    TRACE("latency_mode:%d\n",ab_mate_app.latency_mode);
    offset++;
#endif

#if AB_MATE_INERA_EN
    if(sys_cb.in_ear_en != data_ptr[offset]){
        sys_cb.in_ear_en = data_ptr[offset];
        ab_mate_cm_write(&sys_cb.in_ear_en, AB_MATE_CM_IN_EAR, 1, 2);
    }
    TRACE("in_ear_en:%d\n",sys_cb.in_ear_en);
    offset++;
#endif

#if AB_MATE_BT_NAME_EN
    u8 bt_name_len = data_ptr[offset++];
    u8 name_tag = AB_MATE_BT_NAME_TAG;
    u8 bt_name_local_len = strlen(xcfg_cb.bt_name);
    if((bt_name_local_len != bt_name_len) || memcmp(xcfg_cb.bt_name, &data_ptr[offset], bt_name_len)){
        memset(xcfg_cb.bt_name, 0, sizeof(xcfg_cb.bt_name));
        memcpy(xcfg_cb.bt_name, &data_ptr[offset], bt_name_len);
        ab_mate_cm_write(xcfg_cb.bt_name, AB_MATE_CM_BT_NAME, bt_name_len, 2);
        ab_mate_cm_write(&bt_name_len, AB_MATE_CM_BT_NAME_LEN, 1, 2);
        ab_mate_cm_write(&name_tag, AB_MATE_CM_BT_NAME_FLAG, 1, 2);
    }
    TRACE("bt_name:%s\n",xcfg_cb.bt_name);
    offset += bt_name_len;
#endif

    ab_mate_app.rem_con_sta = data_ptr[offset++];
    TRACE("rem_con_sta:%d\n",ab_mate_app.rem_con_sta);

#if AB_MATE_LED_EN
    if(sys_cb.led_scan_en != data_ptr[offset]){
        sys_cb.led_scan_en = data_ptr[offset];
        ab_mate_cm_write(&sys_cb.led_scan_en, AB_MATE_CM_LED_EN, 1, 2);
    }
    TRACE("led_scan_en:%d\n",sys_cb.led_scan_en);
    offset++;
#endif

#if AB_MATE_V3D_AUDIO_EN
    if(ab_mate_app.v3d_audio_en != data_ptr[offset]){
        ab_mate_app.v3d_audio_en = data_ptr[offset];
        ab_mate_app.do_flag |= FLAG_V3D_AUDIO_SET;
    }
    TRACE("v3d_audio_en:%d\n",ab_mate_app.v3d_audio_en);
    offset++;
#endif

#if AB_MATE_MULT_DEV_EN
    offset += ab_mate_connect_info_set(&data_ptr[offset]);
    ab_mate_app.mult_dev.en = data_ptr[offset];
    ab_mate_app.do_flag |= FLAG_MULT_DEV_SET;
    TRACE("tws mult dev en:%d\n", ab_mate_app.mult_dev.en);
    offset++;
#endif

#if AB_MATE_DYNAMIC_BASS_EN
    if(ab_mate_app.dynamic_bass.en != data_ptr[offset] ||\
       ab_mate_app.dynamic_bass.level != data_ptr[offset+1]){
        ab_mate_app.dynamic_bass.en = data_ptr[offset];
        ab_mate_app.dynamic_bass.level = data_ptr[offset+1];
        ab_mate_app.do_flag |= FLAG_DYNAMIC_BASS_SET;
    }
    TRACE("tws sync dynamic_bass_en:%d, dynamic_bass_level:%d\n",ab_mate_app.dynamic_bass.en, ab_mate_app.dynamic_bass.level);
    offset+=2;
#endif

    if(ab_mate_app.init_flag){
#if AB_MATE_EQ_EN
        if(flag & BIT(0)){
            if(!sco_is_connected()){
                ab_mate_eq_set_do();
            }
        }
#endif

#if AB_MATE_ANC_EN
        if(flag & BIT(1)){
            bsp_anc_set_mode(sys_cb.anc_user_mode);
        }
#endif
    }

    ab_mate_tws_info_all_rsp();
}

void ab_mate_tws_info_name_proc(uint8_t *data_ptr, u16 size)
{
    u8 bt_name_len = data_ptr[0];

    memset(xcfg_cb.bt_name, 0, sizeof(xcfg_cb.bt_name));
    memcpy(xcfg_cb.bt_name, &data_ptr[1], bt_name_len);

    ab_mate_app.do_flag |= FLAG_BT_NAME_SET;
}

#if AB_MATE_INERA_EN
void ab_mate_tws_info_in_ear_proc(uint8_t *data_ptr, u16 size)
{
    if(sys_cb.in_ear_en != data_ptr[0]){
        sys_cb.in_ear_en = data_ptr[0];
        ab_mate_cm_write(&sys_cb.in_ear_en, AB_MATE_CM_IN_EAR, 1, 2);
    }
}
#endif

void ab_mate_tws_info_all_rsp_proc(uint8_t *data_ptr, u16 size)
{
    u16 offset = 0;

    ab_mate_app.remote_vbat = data_ptr[offset++];
#if (CHARGE_BOX_TYPE == CBOX_SSW)
    sys_cb.rem_bat = ab_mate_app.remote_vbat;
#endif

#if AB_MATE_KEY_EN
    if(memcmp(&ab_mate_app.remote_key, &data_ptr[offset], sizeof(ab_mate_key_info_t))){
        memcpy(&ab_mate_app.remote_key, &data_ptr[offset], sizeof(ab_mate_key_info_t));
        ab_mate_cm_write(&ab_mate_app.remote_key, AB_MATE_CM_KEY_REMOTE, sizeof(ab_mate_key_info_t), 2);
    }
    offset += sizeof(ab_mate_key_info_t);
#endif

    ab_mate_app.rem_con_sta = data_ptr[offset++];
}

void ab_mate_tws_info_connect_sta_proc(uint8_t *data_ptr, u16 size)
{
    ab_mate_app.rem_con_sta = data_ptr[0];

    if(ab_mate_app.rem_con_sta == 0){
#if AB_MATE_OTA_EN
        ab_mate_ota_disconnect_callback();
#endif
    }
}

#if AB_MATE_LED_EN
void ab_mate_tws_info_led_proc(uint8_t *data_ptr, u16 size)
{
    if(sys_cb.led_scan_en != data_ptr[0]){
        sys_cb.led_scan_en = data_ptr[0];
        ab_mate_cm_write(&sys_cb.led_scan_en, AB_MATE_CM_LED_EN, 1, 2);
    }
}
#endif

#if AB_MATE_ANC_EN
void ab_mate_tws_info_anc_cur_level_proc(uint8_t *data_ptr, u16 size)
{
    if(ab_mate_app.anc_cur_level != data_ptr[0]){
        ab_mate_app.anc_cur_level = data_ptr[0];
        ab_mate_anc_level_set(&ab_mate_app.anc_cur_level, 1);

    }
}

void ab_mate_tws_info_tp_cur_level_proc(uint8_t *data_ptr, u16 size)
{
    if(ab_mate_app.tp_cur_level != data_ptr[0]){
        ab_mate_app.tp_cur_level = data_ptr[0];
        ab_mate_tp_level_set(&ab_mate_app.tp_cur_level, 1);
    }
}
#endif

#if AB_MATE_V3D_AUDIO_EN
void ab_mate_tws_info_3d_audio_proc(uint8_t *data_ptr, u16 size)
{
    if(ab_mate_app.v3d_audio_en != data_ptr[0]){
        ab_mate_app.v3d_audio_en = data_ptr[0];
        ab_mate_app.do_flag |= FLAG_V3D_AUDIO_SET;
    }
}
#endif
#if AB_MATE_MULT_DEV_EN
void ab_mate_tws_info_mult_dev_en_set(uint8_t data)
{
    if(ab_mate_app.mult_dev.en != data){
        ab_mate_app.mult_dev.en = data;
        ab_mate_app.do_flag |= FLAG_MULT_DEV_SET;
    }
}
void ab_mate_tws_info_mult_dev_info_set(uint8_t *data_ptr)
{
    u8 bt_addr[6];
    char name[32];
    u8 name_len;
    char name_loc[32];
    u8 offset = 0;
    u8 cnt = data_ptr[offset++];
    TRACE("ab_mate_connect_info_set:%d\n",cnt);

    for(u8 i=0; i<cnt; i++){
        name_len = data_ptr[offset] - 6 - 1;
        offset++;
        memcpy(bt_addr, &data_ptr[offset], 6);
        ab_mate_mult_dev_addr_decrypt(bt_addr);
        offset += 7;
        TRACE("bt_addr:");
        TRACE_R(bt_addr, 6);
        memset(name, 0, 32);
        memcpy(name, &data_ptr[offset], name_len);
        offset += name_len;
        TRACE("name:%s\n",name);
        memset(name_loc, 0, 32);
        bt_nor_get_link_info_name(bt_addr, name_loc, 32);
        if(memcmp(name_loc, name, 32)){
            TRACE("bt_put_link_name\n");
            bt_nor_put_link_name(bt_addr, name);
        }
    }
}
void ab_mate_tws_info_mult_dev_proc(uint8_t *data_ptr, u16 size)
{
    u8 read_offset = 0;
    u8 val_len = 0;
    TRACE_R(data_ptr, size);

    while(read_offset < size){
        switch(data_ptr[read_offset]){
            case TWS_SYNC_MULT_DEV_EN:
                TRACE("TWS_SYNC_MULT_DEV_EN\n");
                val_len = data_ptr[read_offset + 1];
                ab_mate_tws_info_mult_dev_en_set(data_ptr[read_offset+2]);
            break;

            case TWS_SYNC_MULT_DEV_INFO:
                TRACE("TWS_SYNC_MULT_DEV_INFO\n");
                val_len = data_ptr[read_offset + 1];
                ab_mate_tws_info_mult_dev_info_set(&data_ptr[read_offset+2]);
            break;

            default:
                val_len = data_ptr[read_offset + 1];
            break;
        }
        read_offset += (2 + val_len);
    }
}

#endif /* AB_MATE_MULT_DEV_EN */

#if AB_MATE_DEVICE_FIND_EN
void ab_mate_tws_info_device_find_proc(uint8_t *data_ptr, u16 size)
{
    ab_mate_app.find_type = data_ptr[0];  
    printf("222333\r\n"); 
    ab_mate_device_find_side();

}
#endif

#if AB_MATE_DYNAMIC_BASS_EN
void ab_mate_tws_info_dynamic_bass_info_proc(uint8_t *data_ptr, u16 size)
{
    u8 read_offset = 0;
    u8 val_len = 0;
    while(read_offset < size){
        switch(data_ptr[read_offset]){
            case TWS_SYNC_DYNAMIC_BASS_EN:
                TRACE("TWS_SYNC_DYNAMIC_BASS_EN\n");
                val_len = data_ptr[read_offset + 1];
                ab_mate_app.dynamic_bass.en = data_ptr[read_offset+2];
                ab_mate_app.do_flag |= FLAG_DYNAMIC_BASS_SET;
            break;

            case TWS_SYNC_DYNAMIC_BASS_LEVEL:
                TRACE("TWS_SYNC_DYNAMIC_BASS_LEVEL\n");
                val_len = data_ptr[read_offset + 1];
                ab_mate_app.dynamic_bass.level = data_ptr[read_offset+2];
                ab_mate_app.do_flag |= FLAG_DYNAMIC_BASS_SET;
            break;

            default:
                val_len = data_ptr[read_offset + 1];
            break;
        }
        read_offset += (2 + val_len);
    }
}
#endif

void ab_mate_tws_recv_proc(uint8_t *data_ptr, u16 size)
{
    u8 info_id = data_ptr[0];

    u8 *p_data = &data_ptr[1];
    u16 data_len = size - 1;

    ab_mate_app.wakeup = 1;

    if(TWS_INFO_OTA != info_id){
        TRACE("tws_recv_proc[%d]:",size);
        TRACE_R(data_ptr,size);
    }

    switch(info_id){
#if AB_MATE_EQ_EN
        case TWS_INFO_EQ:
            TRACE("TWS_INFO_EQ\n");
            ab_mate_tws_info_eq_proc(p_data, data_len);
            break;
#endif

        case TWS_INFO_VBAT:
            TRACE("TWS_INFO_VBAT:%d\n",data_ptr[1]);
            ab_mate_tws_info_bat_proc(p_data, data_len);
            break;

#if AB_MATE_KEY_EN
        case TWS_INFO_KEY:
            TRACE("TWS_INFO_KEY\n");
            ab_mate_tws_info_key_proc(p_data, data_len);
            break;
#endif

        case TWS_INFO_ALL:
            TRACE("TWS_INFO_ALL\n");
            ab_mate_tws_info_all_proc(p_data, data_len);
            break;

#if AB_MATE_OTA_EN
        case TWS_INFO_OTA:
            ab_mate_ota_tws_data_proc(&data_ptr[1],size - 1);
            break;
#endif

#if AB_MATE_BT_NAME_EN
        case TWS_INFO_BT_NAME:
            TRACE("TWS_INFO_BT_NAME\n");
            ab_mate_tws_info_name_proc(p_data, data_len);
            break;
#endif

        case TWS_INFO_DEVICE_RST:
            TRACE("TWS_INFO_DEVICE_RST\n");
            ab_mate_app.do_flag |= FLAG_DEVICE_RESET;
            break;

#if AB_MATE_INERA_EN
        case TWS_INFO_IN_EAR:
            TRACE("TWS_INFO_IN_EAR\n");
            ab_mate_tws_info_in_ear_proc(p_data, data_len);
            break;
#endif

        case TWS_INFO_ALL_RSP:
            TRACE("TWS_INFO_ALL_RSP\n");
            ab_mate_tws_info_all_rsp_proc(p_data, data_len);
            break;

        case TWS_INFO_CONNECT_STA:
            TRACE("TWS_INFO_CONNECT_STA\n");
            ab_mate_tws_info_connect_sta_proc(p_data, data_len);
            break;

        case TWS_INFO_BT_LINK_CLEAR:
            TRACE("TWS_INFO_BT_LINK_CLEAR\n");
            ab_mate_app.do_flag |= FLAG_BT_LINK_INFO_CLEAR;
            break;

#if AB_MATE_LED_EN
        case TWS_INFO_LED:
            TRACE("TWS_INFO_LED\n");
            ab_mate_tws_info_led_proc(p_data, data_len);
            break;
#endif

#if AB_MATE_ANC_EN
        case TWS_INFO_ANC_CUR_LEVEL:
            TRACE("TWS_INFO_ANC_CUR_LEVEL\n");
            ab_mate_anc_level_set_do(p_data[0], 0);
            break;

        case TWS_INFO_TP_CUR_LEVEL:
            TRACE("TWS_INFO_TP_CUR_LEVEL\n");
            ab_mate_tp_level_set_do(p_data[0], 0);
            break;

        case TWS_INFO_ANC_FADE:
			TRACE("TWS_INFO_ANC_FADE\n");
            ab_mate_anc_fade_en_set_do(p_data[0], 0);
            break;
#endif

#if AB_MATE_V3D_AUDIO_EN
        case TWS_INFO_V3D_AUDIO:
            ab_mate_tws_info_3d_audio_proc(p_data, data_len);
            break;
#endif

#if AB_MATE_MULT_DEV_EN
        case TWS_INFO_MULT_DEV:
            TRACE("TWS_INFO_MULT_DEV\n");
            ab_mate_tws_info_mult_dev_proc(p_data, data_len);
            break;
#endif

#if AB_MATE_DEVICE_FIND_EN
        case TWS_INFO_DEVICE_FIND:
            printf("AB_MATE_DEVICE_FIND_EN\r\n");
            ab_mate_tws_info_device_find_proc(p_data, data_len);
            break;
#endif

#if AB_MATE_DYNAMIC_BASS_EN
        case TWS_INFO_DYNAMIC_BASS:
            TRACE("TWS_INFO_DYNAMIC_BASS\n");
            ab_mate_tws_info_dynamic_bass_info_proc(p_data, data_len);
            break;
#endif

        default:
            break;
    }
}


#endif
