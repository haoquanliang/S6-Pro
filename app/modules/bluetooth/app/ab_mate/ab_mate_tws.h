#ifndef __AB_MATE_TWS_H
#define __AB_MATE_TWS_H

#if AB_MATE_APP_EN

enum{
    TWS_INFO_ALL = 0,
    TWS_INFO_ALL_RSP,
    TWS_INFO_EQ,
    TWS_INFO_VBAT,
    TWS_INFO_KEY,
    TWS_INFO_BT_NAME,
    TWS_INFO_DEVICE_RST,
    TWS_INFO_IN_EAR,
    TWS_INFO_AUTO_ANSWER,
    TWS_INFO_PLAY_STA,
    TWS_INFO_CONNECT_STA,
    TWS_INFO_BT_LINK_CLEAR,
    TWS_INFO_LED,
    TWS_INFO_ANC_CUR_LEVEL,
    TWS_INFO_TP_CUR_LEVEL,
    TWS_INFO_V3D_AUDIO,
    TWS_INFO_DEVICE_FIND,
    TWS_INFO_MULT_DEV,
    TWS_INFO_ANC_FADE,
    TWS_INFO_DYNAMIC_BASS,

    TWS_INFO_OTA = 0x20,
    TWS_INFO_OTA_REQ,
    TWS_INFO_OTA_DATA,
    TWS_INFO_OTA_FILE_ADDR,
    TWS_INFO_OTA_UPDATE_DONE,
    TWS_INFO_OTA_ERR,
};

enum{
    TWS_SYNC_MULT_DEV_EN = 0,
    TWS_SYNC_MULT_DEV_INFO,
};

enum{
    TWS_SYNC_DYNAMIC_BASS_EN = 0,
    TWS_SYNC_DYNAMIC_BASS_LEVEL,
};

void ab_mate_tws_eq_info_sync(void);
void ab_mate_tws_key_info_sync(void);
void ab_mate_tws_info_all_sync(void);
void ab_mate_tws_recv_proc(uint8_t *data_ptr, u16 size);
void ab_mate_tws_vbat_sync(void);
void ab_mate_tws_device_reset_sync(void);
void ab_mate_tws_bt_name_sync(void);
void ab_mate_tws_mode_sync(void);
void ab_mate_tws_in_ear_en_sync(void);
void ab_mate_tws_auto_answer_sync(void);
void ab_mate_tws_play_sta_sync(void);
void ab_mate_tws_connect_sta_sync(void);
void ab_mate_tws_link_clear_sync(void);
void ab_mate_tws_led_info_sync(void);
void tws_data_sync_do(void);
void ab_mate_tws_anc_cur_level_sync(void);
void ab_mate_tws_tp_cur_level_sync(void);
void ab_mate_tws_anc_fade_en_sync(void);
void ab_mate_tws_v3d_audio_sync(void);
void ab_mate_tws_device_find_sync(void);
void ab_mate_tws_mult_dev_sync(void);
void ab_mate_tws_mult_dev_info_sync(void);

#endif

#endif
