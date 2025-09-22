#include "include.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_timer.h"
#include "tuya_ble_app.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_event.h"
#include "tuya_ble_ota.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "tuya_ble_tws.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_main.h"

#if LE_TUYA_EN

extern u32 __tuya_data_start, __tuya_data_size;
extern bt_state_info_t bt_state_info;
extern uint8_t tuya_ble_pair_rand[6];
extern uint8_t tuya_ble_pair_rand_valid;
extern uint32_t tuya_ble_receive_sn ;
extern uint32_t tuya_ble_send_sn;

extern void prvHeapInit( void );

/*********************************************************************
 * LOCAL VARIABLE
 */
#if DP_ID_ANC_MODE
static const u8 anc_mode_table[] = {TY_ANC_STOP, TY_ANC_START, TY_ANC_TRANSPARENCY};
#endif


#if DP_ID_KEY_CTL_EN
static const u8 key_local_table[] = {   UDK_VOL_DOWN,    UDK_VOL_UP,
                                        UDK_NEXT,        UDK_PREV,
                                        UDK_PLAY_PAUSE,  UDK_SIRI,
                                        UDK_LOW_LATENCY, UDK_NR,
                                        UDK_NONE
                                    };

#endif

static tuya_ble_timer_t tuya_cm_timer AT(.tuya_data);

static tuya_ble_timer_t tuya_500ms_timer AT(.tuya_data);

#if DP_ID_VBAT_LEFT
static tuya_ble_timer_t tuya_vbat_timer AT(.tuya_data);
#endif

#if DP_ID_DEVICE_FIND
static tuya_ble_timer_t tuya_warning_timer AT(.tuya_data);
#endif

#if DP_ID_PP
static tuya_ble_timer_t tuya_play_sta_timer AT(.tuya_data);
#endif

tuya_ble_app_data_t tuya_ble_app AT(.tuya_data);


/*********************************************************************
 * LOCAL FUNCTION
 */
static void tuya_ble_custom_data_process(int32_t evt_id,void *data);
static void tuya_ble_sdk_callback(tuya_ble_cb_evt_param_t* event);
void tuya_ble_app_dp_query_handle(u8* p_data,u16 data_len);


/****************************************************************/
uint32_t tuya_ble_dp_send_sn_get(void)
{
    return tuya_ble_app.dp_send_sn++;
}

AT(.com_text.tuya_text)
u8 tuya_ble_data_receive_flag_get(void)
{
    return tuya_ble_app.data_receive_flag;
}

void tuya_ble_cm_sync(tuya_ble_timer_t timer)
{
    cm_sync();
}

void tuya_ble_cm_timer_creat(void)
{
    tuya_ble_timer_create(&tuya_cm_timer, 2000, TUYA_BLE_TIMER_SINGLE_SHOT, tuya_ble_cm_sync);
}

void tuya_ble_cm_write(void *buf, u16 addr, u16 size, u8 sync)
{
    cm_write(buf, APP_CM_PAGE(addr), size);
    if(sync){
        cm_sync();  //sync now
        tuya_ble_timer_stop(tuya_cm_timer);
    }else{
        tuya_ble_timer_restart(tuya_cm_timer, 3000); //sync after 3000ms
    }
}

void tuya_ble_cm_read(void *buf, u16 addr, u16 size)
{
    cm_read(buf, APP_CM_PAGE(addr), size);
}

void tuya_ble_500ms_proc(tuya_ble_timer_t timer)
{
    tuya_ble_powerdown_process();
}

void tuya_ble_500ms_timer_creat(void)
{
    tuya_ble_timer_create(&tuya_500ms_timer, 500, TUYA_BLE_TIMER_REPEATED, tuya_ble_500ms_proc);

    tuya_ble_timer_start(tuya_500ms_timer);
}

#if DP_ID_ANC_MODE
void tuya_ble_cm_anc_save(void)
{
#if TUYA_BLE_ANC_USE_CM
    bsp_param_write(&sys_cb.anc_user_mode, PARAM_ANC_NR_STA, 1);
    bsp_param_sync();
#endif
}

void tuya_ble_cm_anc_load(void)
{
#if TUYA_BLE_ANC_USE_CM
    if(tuya_ble_app.cm_flag == TUYA_CM_TAG){
        bsp_param_read(&sys_cb.anc_user_mode, PARAM_ANC_NR_STA, 1);
    }else{
        sys_cb.anc_user_mode = 0;
        bsp_param_write(&sys_cb.anc_user_mode, PARAM_ANC_NR_STA, 1);
    }
#endif
}
#endif

#if DP_ID_LOW_LATENCY
void tuya_ble_cm_low_latency_save(void)
{
#if TUYA_BLE_LATENCY_USE_CM
    tuya_ble_cm_write(&tuya_ble_app.low_latency, TUYA_CM_LOW_LATENCY, 1, 0);
#endif
}

void tuya_ble_cm_low_latency_load(void)
{
#if TUYA_BLE_LATENCY_USE_CM
    if(tuya_ble_app.cm_flag == TUYA_CM_TAG){
        tuya_ble_cm_read(&tuya_ble_app.low_latency, TUYA_CM_LOW_LATENCY, 1);
    }else{
        tuya_ble_app.low_latency = 0;
        tuya_ble_cm_write(&tuya_ble_app.low_latency, TUYA_CM_LOW_LATENCY, 1, 0);
    }
#endif
}
#endif

#if DP_ID_DEVICE_FIND
void tuya_ble_warning_do(tuya_ble_timer_t timer)
{
    if(tuya_ble_app.dev_find){
        reset_sleep_delay();
        maxvol_tone_play();
    }
}

void tuya_ble_warning_timer_creat(void)
{
    tuya_ble_timer_create(&tuya_warning_timer, 2000, TUYA_BLE_TIMER_REPEATED, tuya_ble_warning_do);
}
#endif

#if DP_ID_VBAT_LEFT
void tuya_ble_vbat_report_do(void)
{
    if(tuya_ble_app.bond_flag && !tuya_ble_ota_is_start()){
#if BT_TWS_EN
        u8 report_data[10] = {DP_ID_VBAT_LEFT,  DT_VALUE,  0x00,  1,  0x00,
                              DP_ID_VBAT_RIGHT, DT_VALUE,  0x00,  1,  0x00,
                             };

        if(sys_cb.tws_left_channel){
            report_data[4] = bsp_get_bat_level();   //L
            report_data[9] = tuya_ble_app.remote_vbat;   //R
        }else{
            report_data[4] = tuya_ble_app.remote_vbat;   //L
            report_data[9] = bsp_get_bat_level();   //R
        }
        TUYA_APP_LOG_DEBUG("vbat_report:L %d, R %d",report_data[4],report_data[9]);
#else
        u8 report_data[5] = {DP_ID_VBAT_LEFT, DT_VALUE, 0x00, 1, 0x00};
        report_data[4] = bsp_get_bat_level();
        TUYA_APP_LOG_DEBUG("vbat_report:%d",report_data[4]);
#endif

        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, report_data, sizeof(report_data));
    }
}

void tuya_ble_vbat_report(tuya_ble_timer_t timer)
{
    if(bt_tws_is_connected()){
        tuya_ble_tws_vbat_exchange();
    }else{
        tuya_ble_app.remote_vbat = 0;
        tuya_ble_vbat_report_do();
    }
}

void tuya_ble_vbat_timer_creat(void)
{
    tuya_ble_timer_create(&tuya_vbat_timer, 15000, TUYA_BLE_TIMER_REPEATED, tuya_ble_vbat_report);
}
#endif

#if DP_ID_PP
void tuya_ble_app_play_sta_report(tuya_ble_timer_t timer)
{
    if(tuya_ble_app.bond_flag){
        if(!tuya_ble_ota_is_start()){
            u8 dp_pp_data[5] = {DP_ID_PP,DT_BOOL, 0x00, DT_BOOL_LEN, 0x00};
            dp_pp_data[4] = tuya_ble_app.play_sta;
            tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, dp_pp_data, sizeof(dp_pp_data));
            TUYA_APP_LOG_DEBUG("play_sta_report:%d\n",dp_pp_data[4]);
            tuya_ble_app.play_sta_check_time = 500;
        }
    }else if(tuya_ble_app.remote_bond_flag){
        tuya_ble_tws_play_sta_sync();
    }
}

void tuya_ble_play_sta_timer_creat(void)
{
    tuya_ble_timer_create(&tuya_play_sta_timer, 3000, TUYA_BLE_TIMER_SINGLE_SHOT, tuya_ble_app_play_sta_report);
}

void tuya_ble_play_sta_report_proc(void)
{
    if(!bt_tws_is_slave()){
        u8 play_sta = a2dp_is_playing_fast();
        if(tuya_ble_app.play_sta != play_sta){
            tuya_ble_app.play_sta = play_sta;
            tuya_ble_timer_restart(tuya_play_sta_timer, tuya_ble_app.play_sta_check_time);
        }
    }
}
#endif

void tuya_ble_addr_get(u8 *addr)
{
    tuya_ble_str_to_hex((u8*)TUYA_DEVICE_MAC, 12, addr);
}

#if EQ_MODE_EN
u8 tuya_ble_eq_set(void)
{
    if(tuya_ble_app.eq_en && (tuya_ble_app.eq_mode != 0)){
#if SYS_EQ_FOR_IDX_EN
        for(u8 i=0; i< TUYA_EQ_BAND_CNT; i++){
            WDT_CLR();
            music_set_eq_for_index(i, tuya_ble_app.eq_data[i]);
        }
        if(tuya_ble_app.eq_mode == 1){
            music_set_eq_overall_gain(-9);
        }else{
            music_set_eq_overall_gain(0);
        }
        music_set_eq_for_index_do();
#else
        sys_cb.eq_mode = tuya_ble_app.eq_mode;
        music_set_eq_by_num(sys_cb.eq_mode);
#endif
        return 1;
    }else{
        sys_cb.eq_mode = 0;
        music_set_eq_by_num(sys_cb.eq_mode);
    }
    return 0;
}

void tuya_ble_eq_var_init(void)
{
    if(tuya_ble_app.cm_flag == TUYA_CM_TAG){
        tuya_ble_cm_read(&tuya_ble_app.eq_en, TUYA_CM_EQ_EN, 1);
        tuya_ble_cm_read(&tuya_ble_app.eq_mode, TUYA_CM_EQ_DATA, 1 + TUYA_EQ_BAND_CNT);
        TUYA_APP_LOG_HEXDUMP_DEBUG("eq_init_data",tuya_ble_app.eq_data, TUYA_EQ_BAND_CNT);
    }else{
        tuya_ble_app.eq_en = 0;
        tuya_ble_app.eq_mode = 0;
        memset(tuya_ble_app.eq_data, 0, TUYA_EQ_BAND_CNT);
        tuya_ble_cm_write(&tuya_ble_app.eq_en, TUYA_CM_EQ_EN, 1, 0);
        tuya_ble_cm_write(&tuya_ble_app.eq_mode, TUYA_CM_EQ_DATA, 1 + TUYA_EQ_BAND_CNT, 0);
    }
}
#endif

#if DP_ID_KEY_CTL_EN
void tuya_ble_key_set_do(void)
{
#if DP_ID_KEY_LEFT_SHORT
    TUYA_KEY_SHORT = key_local_table[tuya_ble_app.local_key.key_short];
#endif

#if DP_ID_KEY_LEFT_DOUBLE
    TUYA_KEY_DOUBLE = key_local_table[tuya_ble_app.local_key.key_double];
#endif

#if DP_ID_KEY_LEFT_THREE
    TUYA_KEY_THREE = key_local_table[tuya_ble_app.local_key.key_three];
#endif

#if DP_ID_KEY_LEFT_LONG
    TUYA_KEY_LONG = key_local_table[tuya_ble_app.local_key.key_long];
#endif

    tuya_ble_cm_write(&tuya_ble_app.local_key, TUYA_CM_KEY_LOCAL, sizeof(tuya_ble_key_info_t), 0);
}

void tuya_ble_key_init(void)
{
    if(tuya_ble_app.cm_flag == TUYA_CM_TAG){
        tuya_ble_cm_read(&tuya_ble_app.local_key, TUYA_CM_KEY_LOCAL, sizeof(tuya_ble_key_info_t));
        tuya_ble_cm_read(&tuya_ble_app.remote_key, TUYA_CM_KEY_REMOTE, sizeof(tuya_ble_key_info_t));
    }else{
        tuya_ble_key_info_t *key_left;
        tuya_ble_key_info_t *key_right;

#if BT_TWS_EN
        if(sys_cb.tws_left_channel){
            key_left = &tuya_ble_app.local_key;
            key_right = &tuya_ble_app.remote_key;
        }else
#endif
        {
            key_left = &tuya_ble_app.remote_key;
            key_right = &tuya_ble_app.local_key;
        }

#if DP_ID_KEY_LEFT_SHORT
        key_left->key_short = TUYA_KEY_SHORT_LEFT_DEF;
        key_right->key_short = TUYA_KEY_SHORT_RIGHT_DEF;
#endif

#if DP_ID_KEY_LEFT_DOUBLE
        key_left->key_double = TUYA_KEY_DOUBLE_LEFT_DEF;
        key_right->key_double = TUYA_KEY_DOUBLE_RIGHT_DEF;
#endif

#if DP_ID_KEY_LEFT_THREE
        key_left->key_three = TUYA_KEY_THREE_LEFT_DEF;
        key_right->key_three = TUYA_KEY_THREE_RIGHT_DEF;
#endif

#if DP_ID_KEY_LEFT_LONG
        key_left->key_long = TUYA_KEY_LONG_LEFT_DEF;
        key_right->key_long = TUYA_KEY_LONG_RIGHT_DEF;
#endif

        tuya_ble_cm_write(&tuya_ble_app.local_key, TUYA_CM_KEY_LOCAL, sizeof(tuya_ble_key_info_t), 0);
        tuya_ble_cm_write(&tuya_ble_app.remote_key, TUYA_CM_KEY_REMOTE, sizeof(tuya_ble_key_info_t), 0);
    }

#if DP_ID_KEY_LEFT_SHORT
    TUYA_KEY_SHORT = key_local_table[tuya_ble_app.local_key.key_short];
#endif

#if DP_ID_KEY_LEFT_DOUBLE
    TUYA_KEY_DOUBLE = key_local_table[tuya_ble_app.local_key.key_double];
#endif

#if DP_ID_KEY_LEFT_THREE
    TUYA_KEY_THREE = key_local_table[tuya_ble_app.local_key.key_three];
#endif

#if DP_ID_KEY_LEFT_LONG
    TUYA_KEY_LONG = key_local_table[tuya_ble_app.local_key.key_long];
#endif

#if DP_ID_KEY_SHORT_EN
    if(tuya_ble_app.cm_flag == TUYA_CM_TAG){
        tuya_ble_cm_read(&sys_cb.key_en, TUYA_CM_KEY_EN, 1);
    }else{
        sys_cb.key_en = 1;
        tuya_ble_cm_write(&sys_cb.key_en, TUYA_CM_KEY_EN, 1, 0);
    }
#endif
}

#if DP_ID_KEY_RST
void tuya_ble_key_reset_2_factory(void)
{
    tuya_ble_key_info_t *key_left;
    tuya_ble_key_info_t *key_right;

#if BT_TWS_EN
    if(sys_cb.tws_left_channel){
        key_left = &tuya_ble_app.local_key;
        key_right = &tuya_ble_app.remote_key;
    }else
#endif
    {
        key_left = &tuya_ble_app.remote_key;
        key_right = &tuya_ble_app.local_key;
    }

#if DP_ID_KEY_LEFT_SHORT
    key_left->key_short = TUYA_KEY_SHORT_LEFT_DEF;
    key_right->key_short = TUYA_KEY_SHORT_RIGHT_DEF;

#endif

#if DP_ID_KEY_LEFT_DOUBLE
    key_left->key_double = TUYA_KEY_DOUBLE_LEFT_DEF;
    key_right->key_double = TUYA_KEY_DOUBLE_RIGHT_DEF;
#endif

#if DP_ID_KEY_LEFT_THREE
    key_left->key_three = TUYA_KEY_THREE_LEFT_DEF;
    key_right->key_three = TUYA_KEY_THREE_RIGHT_DEF;
#endif

#if DP_ID_KEY_LEFT_LONG
    key_left->key_long = TUYA_KEY_LONG_LEFT_DEF;
    key_right->key_long = TUYA_KEY_LONG_RIGHT_DEF;
#endif

    tuya_ble_cm_write(&tuya_ble_app.local_key, TUYA_CM_KEY_LOCAL, sizeof(tuya_ble_key_info_t), 0);
    tuya_ble_cm_write(&tuya_ble_app.remote_key, TUYA_CM_KEY_REMOTE, sizeof(tuya_ble_key_info_t), 0);

#if DP_ID_KEY_LEFT_SHORT
    TUYA_KEY_SHORT = key_local_table[tuya_ble_app.local_key.key_short];
#endif

#if DP_ID_KEY_LEFT_DOUBLE
    TUYA_KEY_DOUBLE = key_local_table[tuya_ble_app.local_key.key_double];
#endif

#if DP_ID_KEY_LEFT_THREE
    TUYA_KEY_THREE = key_local_table[tuya_ble_app.local_key.key_three];
#endif

#if DP_ID_KEY_LEFT_LONG
    TUYA_KEY_LONG = key_local_table[tuya_ble_app.local_key.key_long];
#endif
}

void tuya_ble_key_info_report(void)
{
    u8 report_data_len = 0;
    u8* report_data = (uint8_t *)tuya_ble_malloc(48);

    if(report_data == NULL){
        return;
    }

#if DP_ID_KEY_CTL_EN
    tuya_ble_key_info_t *key_left;
    tuya_ble_key_info_t *key_right;

#if BT_TWS_EN
    if(sys_cb.tws_left_channel){
        key_left = &tuya_ble_app.local_key;
        key_right = &tuya_ble_app.remote_key;
    }else
#endif
    {
        key_left = &tuya_ble_app.remote_key;
        key_right = &tuya_ble_app.local_key;
    }
#endif

#if DP_ID_KEY_LEFT_DOUBLE
    u8 dp_key_left_double[5] = {DP_ID_KEY_LEFT_DOUBLE, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_double};
    u8 dp_key_right_double[5] = {DP_ID_KEY_RIGHT_DOUBLE, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_double};
#endif

#if DP_ID_KEY_LEFT_THREE
    u8 dp_key_left_three[5] = {DP_ID_KEY_LEFT_THREE, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_three};
    u8 dp_key_right_three[5] = {DP_ID_KEY_RIGHT_THREE, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_three};
#endif

#if DP_ID_KEY_LEFT_LONG
    u8 dp_key_left_long[5] = {DP_ID_KEY_LEFT_LONG, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_long};
    u8 dp_key_right_long[5] = {DP_ID_KEY_RIGHT_LONG, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_long};
#endif

#if DP_ID_KEY_LEFT_DOUBLE
    memcpy(&report_data[report_data_len], dp_key_left_double, sizeof(dp_key_left_double));
    report_data_len += sizeof(dp_key_left_double);
    memcpy(&report_data[report_data_len], dp_key_right_double, sizeof(dp_key_right_double));
    report_data_len += sizeof(dp_key_right_double);
#endif

#if DP_ID_KEY_LEFT_THREE
    memcpy(&report_data[report_data_len], dp_key_left_three, sizeof(dp_key_left_three));
    report_data_len += sizeof(dp_key_left_three);
    memcpy(&report_data[report_data_len], dp_key_right_three, sizeof(dp_key_right_three));
    report_data_len += sizeof(dp_key_right_three);
#endif

#if DP_ID_KEY_LEFT_LONG
    memcpy(&report_data[report_data_len], dp_key_left_long, sizeof(dp_key_left_long));
    report_data_len += sizeof(dp_key_left_long);
    memcpy(&report_data[report_data_len], dp_key_right_long, sizeof(dp_key_right_long));
    report_data_len += sizeof(dp_key_right_long);
#endif

    TUYA_APP_LOG_DEBUG("tuya_ble_key_info_report:%d",report_data_len);

    tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, report_data, report_data_len);

    tuya_ble_free(report_data);
}

#endif

#endif

#if DP_ID_BT_RST
void tuya_bt_reset_do(void)
{
    if(bt_nor_is_connected()){
        bt_nor_disconnect();
        delay_5ms(10);
    }
    bt_nor_delete_link_info();
}
#endif

tuya_ble_status_t tuya_ble_dp_write_handler(u8 dp_id,u8 dp_type,u16 dp_len,u8* dp_data)
{
#if DP_ID_KEY_CTL_EN
    tuya_ble_key_info_t *key_left;
    tuya_ble_key_info_t *key_right;
    tuya_ble_key_info_t key_loc;
    tuya_ble_key_info_t key_rem;

    memcpy(&key_loc, &tuya_ble_app.local_key, sizeof(tuya_ble_key_info_t));
    memcpy(&key_rem, &tuya_ble_app.remote_key, sizeof(tuya_ble_key_info_t));

#if BT_TWS_EN
    if(sys_cb.tws_left_channel){
        key_left = &tuya_ble_app.local_key;
        key_right = &tuya_ble_app.remote_key;
    }else
#endif
    {
        key_left = &tuya_ble_app.remote_key;
        key_right = &tuya_ble_app.local_key;
    }
#endif

    if(dp_type == DT_VALUE){
       tuya_ble_inverted_array(dp_data,DT_VALUE_LEN); //big endian
    }

    switch(dp_id){
#if DP_ID_VOL
        case DP_ID_VOL:{
            u32 vol, vol_temp, vol_max = VOL_MAX;
            if(sys_cb.incall_flag){
                vol_max = 15;
            }
            memcpy(&vol_temp, dp_data, 4);
            vol = (vol_temp * vol_max) / 100;
            if((vol == 0) && (vol_temp != 0)){
                vol = 1;
            }
            if (sys_cb.incall_flag) {
                sys_cb.hfp_vol = vol;
                bt_hfp_set_spk_gain();
                if(sys_cb.incall_flag & INCALL_FLAG_SCO) {
                    bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
                }
                tuya_ble_app.hfp_vol = vol;
            } else {
                bsp_set_volume(vol);
                bsp_bt_vol_change();
                tuya_ble_app.vol = vol;
            }
        } break;
#endif

#if DP_ID_CTL
        case DP_ID_CTL:{
            if(dp_data[0] == 0){
                TUYA_APP_LOG_DEBUG("-->music prev");
                bt_music_prev();
                sys_cb.key2unmute_cnt = 15 * sys_cb.mute;
            }else if(dp_data[0] == 1){
                TUYA_APP_LOG_DEBUG("-->music next");
                bt_music_next();
                sys_cb.key2unmute_cnt = 15 * sys_cb.mute;
            }
        } break;
#endif

#if DP_ID_PP
        case DP_ID_PP:
            bt_music_play_pause();
            f_bt.pp_2_unmute = sys_cb.mute;
            delay_5ms(4);
            tuya_ble_app.play_sta = dp_data[0];     //置标志，防止手机没有播放器打开的情况下APP界面不会更新
            tuya_ble_app.play_sta_check_time = 3000;
            tuya_ble_timer_restart(tuya_play_sta_timer, tuya_ble_app.play_sta_check_time);
        break;
#endif

#if DP_ID_DEVICE_FIND
        case DP_ID_DEVICE_FIND:
            //TUYA_APP_LOG_DEBUG("--->DP_ID_DEVICE_FIND:%d",dp_data[0]);
            if(dp_data[0]){
                tuya_ble_app.dev_find = 1;
                tuya_ble_timer_start(tuya_warning_timer);
            }else{
                tuya_ble_app.dev_find = 0;
                tuya_ble_timer_stop(tuya_warning_timer);
            }
        break;
#endif

#if DP_ID_EQ_DATA
        case DP_ID_EQ_DATA:{
            u8 cnt = dp_data[1];    //EQ段数,TUYA_EQ_BAND_CNT
            tuya_ble_app.eq_mode = dp_data[2];   //当前是EQ哪个场景
            memcpy(tuya_ble_app.eq_data, &dp_data[3], cnt);

            for(u8 i=0; i< cnt; i++){
                WDT_CLR();
                tuya_ble_app.eq_data[i] = tuya_ble_app.eq_data[i] - 0xc0;
            }

            if(tuya_ble_app.eq_en == 0){
                tuya_ble_app.eq_en = 1;
                tuya_ble_cm_write(&tuya_ble_app.eq_en, TUYA_CM_EQ_EN, 1, 0);
            }

            tuya_ble_tws_eq_sync();

            tuya_ble_app.do_flag |= TUYA_FLAG_EQ_SET;

            tuya_ble_cm_write(&tuya_ble_app.eq_mode, TUYA_CM_EQ_DATA, 1 + TUYA_EQ_BAND_CNT, 0);
        } break;
#endif

#if DP_ID_ANC_MODE
        case DP_ID_ANC_MODE:
            TUYA_APP_LOG_DEBUG("--->ANC_MODE:%d",dp_data[0]);
            tuya_ble_app.anc_mode = dp_data[0];
            if(dp_data[0] == TY_ANC_STOP){
                sys_cb.anc_user_mode = 0;
                bsp_tws_res_music_play(TWS_RES_NR_DISABLE);                 //同步播放语言提示音,并切换ANC模式
            }else if(dp_data[0] == TY_ANC_START){
                sys_cb.anc_user_mode = 1;
                bsp_tws_res_music_play(TWS_RES_ANC);
            }else if(dp_data[0] == TY_ANC_TRANSPARENCY){
                sys_cb.anc_user_mode = 2;
                bsp_tws_res_music_play(TWS_RES_TRANSPARENCY);
            }
        break;
#endif

#if DP_ID_POWER_DOWN
        case DP_ID_POWER_DOWN:{
            u32 cnt;
            memcpy(&cnt, dp_data, 4);
            tuya_ble_app.powerdown_cnt = 60 * cnt * 2;
            TUYA_APP_LOG_DEBUG("tuya_powerdown_cnt:%ds\n",tuya_ble_app.powerdown_cnt/2);
        } break;
#endif

#if DP_ID_EQ_MODE
        case DP_ID_EQ_MODE:
            if(tuya_ble_app.eq_mode != dp_data[0]){
                tuya_ble_app.eq_mode = dp_data[0];
                tuya_ble_app.eq_en = 1;

                tuya_ble_tws_eq_sync();

                tuya_ble_app.do_flag |= TUYA_FLAG_EQ_SET;

                tuya_ble_cm_write(&tuya_ble_app.eq_mode, TUYA_CM_EQ_DATA, 1 + TUYA_EQ_BAND_CNT, 0);
            }
        break;
#endif

#if DP_ID_EQ_EN
        case DP_ID_EQ_EN:
            tuya_ble_app.eq_en = dp_data[0];

            tuya_ble_tws_eq_sync();

            tuya_ble_app.do_flag |= TUYA_FLAG_EQ_SET;

            tuya_ble_cm_write(&tuya_ble_app.eq_en, TUYA_CM_EQ_EN, 1, 0);
        break;
#endif

#if DP_ID_LOW_LATENCY
        case DP_ID_LOW_LATENCY:
            tuya_ble_app.low_latency = dp_data[0];
            if (tuya_ble_app.low_latency) {
                bsp_tws_res_music_play(TWS_RES_GAME_MODE);
            } else {
                bsp_tws_res_music_play(TWS_RES_MUSIC_MODE);
            }
        break;
#endif

#if DP_ID_BT_NAME
        case DP_ID_BT_NAME:
            TUYA_APP_LOG_DEBUG("-->DP_ID_BT_NAME:%s\n",dp_data);
            if(dp_len > 31){
                dp_len = 31;
            }
            memset(xcfg_cb.bt_name, 0, sizeof(xcfg_cb.bt_name));
            memcpy(xcfg_cb.bt_name, dp_data, dp_len);
            tuya_ble_tws_bt_name_sync();
            tuya_ble_cm_write(xcfg_cb.bt_name, TUYA_CM_BT_NAME, dp_len, 0);
            tuya_ble_cm_write(&dp_len, TUYA_CM_BT_NAME_LEN, 1, 1);
            bt_set_stack_local_name(xcfg_cb.bt_name);
        break;
#endif

#if DP_ID_BT_RST
        case DP_ID_BT_RST:
            TUYA_APP_LOG_DEBUG("-->DP_ID_BT_RST:%d\n",dp_data[0]);
            if(dp_data[0] == 0){
                tuya_ble_tws_bt_reset_sync();
                tuya_bt_reset_do();
                u8 dp_data_rsp[5] = {DP_ID_BT_RST,DT_BOOL, 0x00, DT_BOOL_LEN, 0x01};
                tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, (void*)dp_data_rsp, sizeof(dp_data_rsp));
            }
        break;
#endif

#if DP_ID_KEY_LEFT_SHORT
        case DP_ID_KEY_LEFT_SHORT:
            if(key_left->key_short != dp_data[0]){
                key_left->key_short = dp_data[0];
            }
        break;
#endif

#if DP_ID_KEY_RIGHT_SHORT
        case DP_ID_KEY_RIGHT_SHORT:
            if(key_right->key_short != dp_data[0]){
                key_right->key_short = dp_data[0];

            }
        break;
#endif

#if DP_ID_KEY_LEFT_DOUBLE
        case DP_ID_KEY_LEFT_DOUBLE:
            if(key_left->key_double != dp_data[0]){
                key_left->key_double = dp_data[0];
            }
        break;
#endif

#if DP_ID_KEY_RIGHT_DOUBLE
        case DP_ID_KEY_RIGHT_DOUBLE:
            if(key_right->key_double != dp_data[0]){
                key_right->key_double = dp_data[0];
            }
        break;
#endif

#if DP_ID_KEY_LEFT_THREE
        case DP_ID_KEY_LEFT_THREE:
            if(key_left->key_three != dp_data[0]){
                key_left->key_three = dp_data[0];
            }
        break;
#endif

#if DP_ID_KEY_RIGHT_THREE
        case DP_ID_KEY_RIGHT_THREE:
            if(key_right->key_three != dp_data[0]){
                key_right->key_three = dp_data[0];
            }
        break;
#endif

#if DP_ID_KEY_LEFT_LONG
        case DP_ID_KEY_LEFT_LONG:
            if(key_left->key_long != dp_data[0]){
                key_left->key_long = dp_data[0];
            }
            break;
#endif

#if DP_ID_KEY_RIGHT_LONG
        case DP_ID_KEY_RIGHT_LONG:
            if(key_right->key_long != dp_data[0]){
                key_right->key_long = dp_data[0];
            }
            break;
#endif

#if DP_ID_KEY_SHORT_EN
        case DP_ID_KEY_SHORT_EN:
            if(sys_cb.key_en != dp_data[0]){
                sys_cb.key_en = dp_data[0];
                tuya_ble_tws_key_en_sync();
                tuya_ble_cm_write(&sys_cb.key_en,TUYA_CM_KEY_EN,1,0);
            }
            break;
#endif

#if DP_ID_KEY_RST
        case DP_ID_KEY_RST:
            if(dp_data[0]){
                tuya_ble_tws_key_reset_sync();
                tuya_ble_key_reset_2_factory();
                tuya_ble_key_info_report();
            }
            break;
#endif

#if DP_ID_DEVICE_RESET
        case DP_ID_DEVICE_RESET:
            if(dp_data[0] == 0){
                tuya_ble_tws_reset_sync();
                tuya_ble_device_reset_2_factory();
            }
            break;
#endif
        default:
            break;
    }

#if DP_ID_KEY_CTL_EN
    if(memcmp(&key_loc, &tuya_ble_app.local_key, sizeof(tuya_ble_key_info_t))){
        tuya_ble_key_set_do();
    }
    if(memcmp(&key_rem, &tuya_ble_app.remote_key, sizeof(tuya_ble_key_info_t))){
        tuya_ble_tws_key_info_sync();
    }
#endif

    return TUYA_BLE_SUCCESS;
}

void tuya_ble_app_dp_query_handle(u8* p_data,u16 data_len)
{
    u8 report_data_len = 0;
    u8* report_data = (uint8_t *)tuya_ble_malloc(184);

    if(report_data == NULL){
        return;
    }

#if DP_ID_KEY_CTL_EN
    tuya_ble_key_info_t *key_left;
    tuya_ble_key_info_t *key_right;

#if BT_TWS_EN
    if(sys_cb.tws_left_channel){
        key_left = &tuya_ble_app.local_key;
        key_right = &tuya_ble_app.remote_key;
    }else
#endif
    {
        key_left = &tuya_ble_app.remote_key;
        key_right = &tuya_ble_app.local_key;
    }
#endif

#if DP_ID_WARNING_MODE
    u8 dp_warning_mode[5] = {DP_ID_WARNING_MODE, DT_ENUM, 0x00, DT_ENUM_LEN, 0x01};
#endif

#if DP_ID_PP
    u8 dp_pp_data[5] = {DP_ID_PP, DT_BOOL, 0x00, DT_BOOL_LEN, 0x00};
    dp_pp_data[4] = a2dp_is_playing_fast();
#endif

#if DP_ID_EQ_EN
    u8 dp_eq_en_data[5] = {DP_ID_EQ_EN, DT_BOOL, 0x00, DT_BOOL_LEN, 0x00};
    dp_eq_en_data[4] = tuya_ble_app.eq_en;
#endif

#if DP_ID_EQ_DATA
    u8 dp_eq_data[7+TUYA_EQ_BAND_CNT] = {DP_ID_EQ_DATA, DT_RAW, 0x00, 0x0d, 0x00, 0x0a};
    dp_eq_data[6] = tuya_ble_app.eq_mode;
    memcpy(&dp_eq_data[7], tuya_ble_app.eq_data, TUYA_EQ_BAND_CNT);
    for(u8 i=0; i < TUYA_EQ_BAND_CNT; i++){
        dp_eq_data[7+i] = dp_eq_data[7+i] + 0xc0;
    }
#endif

#if DP_ID_VOL
    u8 dp_vol_data[5] = {DP_ID_VOL, DT_VALUE, 0x00, 1, 0x00};
    dp_vol_data[4] = (sys_cb.vol * 100) / VOL_MAX;
#endif

#if DP_ID_VBAT_LEFT
#if BT_TWS_EN
    u8 dp_vbat_data[10] = {  DP_ID_VBAT_LEFT, DT_VALUE, 0x00, 1, 0x00,
                             DP_ID_VBAT_RIGHT,DT_VALUE, 0x00, 1, 0x00,
                          };

    if(sys_cb.tws_left_channel){
        dp_vbat_data[4] = bsp_get_bat_level();   //L
        dp_vbat_data[9] = tuya_ble_app.remote_vbat;   //R
    }else{
        dp_vbat_data[4] = tuya_ble_app.remote_vbat;   //L
        dp_vbat_data[9] = bsp_get_bat_level();   //R
    }
#else
    u8 dp_vbat_data[5] = {DP_ID_VBAT_LEFT, DT_VALUE, 0x00, 1, 0x00};
    dp_vbat_data[4] = bsp_get_bat_level();   //L
#endif
#endif

#if DP_ID_ANC_MODE
    u8 dp_anc_mode[5] = {DP_ID_ANC_MODE, DT_ENUM, 0x00, DT_ENUM_LEN, 0x00};
    dp_anc_mode[4] = anc_mode_table[sys_cb.anc_user_mode];
#endif

#if DP_ID_POWER_DOWN
    u8 dp_power_down_data[5] = {DP_ID_POWER_DOWN, DT_VALUE, 0x00, 1, 0x00};
#endif

#if DP_ID_LOW_LATENCY
    u8 dp_latency_data[5] = {DP_ID_LOW_LATENCY, DT_ENUM, 0x00, DT_ENUM_LEN, 0x00};
    dp_latency_data[4] = tuya_ble_app.low_latency;
#endif

#if DP_ID_BT_NAME
    u8 dp_bt_name[4+32] = {DP_ID_BT_NAME, DT_RAW, 0x00, 0x00, 0x00};
    u8 bt_name_len = strlen(bt_get_local_name());
    dp_bt_name[3] = bt_name_len;
    memcpy(&dp_bt_name[4], bt_get_local_name(), bt_name_len);
    bt_name_len += 4;
#endif

#if DP_ID_BT_STA
    u8 dp_bt_sta[5] = {DP_ID_BT_STA, DT_ENUM, 0x00, DT_ENUM_LEN, 0x00};
    if(bt_nor_is_connected()){
        dp_bt_sta[4] = TY_BT_STA_CONNECTED;
    }else{
        dp_bt_sta[4] = TY_BT_STA_DISCONNECT;
    }
#endif

#if DP_ID_BT_RST
    u8 dp_bt_rst[5] = {DP_ID_BT_RST, DT_BOOL, 0x00, DT_BOOL_LEN, 0x01};
#endif

#if DP_ID_DEVICE_FIND
    u8 dp_device_find[5] = {DP_ID_DEVICE_FIND, DT_BOOL, 0x00, DT_BOOL_LEN, 0x00};
#endif

#if DP_ID_KEY_LEFT_SHORT
    u8 dp_key_left_short[5] = {DP_ID_KEY_LEFT_SHORT, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_short};
    u8 dp_key_right_short[5] = {DP_ID_KEY_RIGHT_SHORT, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_short};
#endif

#if DP_ID_KEY_LEFT_DOUBLE
    u8 dp_key_left_double[5] = {DP_ID_KEY_LEFT_DOUBLE, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_double};
    u8 dp_key_right_double[5] = {DP_ID_KEY_RIGHT_DOUBLE, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_double};
#endif

#if DP_ID_KEY_LEFT_THREE
    u8 dp_key_left_three[5] = {DP_ID_KEY_LEFT_THREE, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_three};
    u8 dp_key_right_three[5] = {DP_ID_KEY_RIGHT_THREE, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_three};
#endif

#if DP_ID_KEY_LEFT_LONG
    u8 dp_key_left_long[5] = {DP_ID_KEY_LEFT_LONG, DT_ENUM, 0x00, DT_ENUM_LEN, key_left->key_long};
    u8 dp_key_right_long[5] = {DP_ID_KEY_RIGHT_LONG, DT_ENUM, 0x00, DT_ENUM_LEN, key_right->key_long};
#endif

#if DP_ID_KEY_SHORT_EN
    u8 dp_key_short_en[5] = {DP_ID_KEY_SHORT_EN, DT_BOOL, 0x00, DT_BOOL_LEN, sys_cb.key_en};
#endif

#if DP_ID_EQ_MODE
    u8 dp_eq_mode[5] = {DP_ID_EQ_MODE, DT_ENUM, 0x00, DT_ENUM_LEN, tuya_ble_app.eq_mode};
#endif

    if(data_len == 0){  //query all dp
#if DP_ID_WARNING_MODE
        memcpy(&report_data[report_data_len], dp_warning_mode, sizeof(dp_warning_mode));
        report_data_len += sizeof(dp_warning_mode);
#endif
#if DP_ID_PP
        memcpy(&report_data[report_data_len], dp_pp_data, sizeof(dp_pp_data));
        report_data_len += sizeof(dp_pp_data);
#endif
#if DP_ID_EQ_EN
        memcpy(&report_data[report_data_len], dp_eq_en_data, sizeof(dp_eq_en_data));
        report_data_len += sizeof(dp_eq_en_data);
#endif
#if DP_ID_EQ_DATA
        memcpy(&report_data[report_data_len], dp_eq_data, sizeof(dp_eq_data));
        report_data_len += sizeof(dp_eq_data);
#endif
#if DP_ID_VOL
        memcpy(&report_data[report_data_len], dp_vol_data, sizeof(dp_vol_data));
        report_data_len += sizeof(dp_vol_data);
#endif
#if DP_ID_VBAT_LEFT
        memcpy(&report_data[report_data_len], dp_vbat_data, sizeof(dp_vbat_data));
        report_data_len += sizeof(dp_vbat_data);
#endif
#if DP_ID_ANC_MODE
        memcpy(&report_data[report_data_len], dp_anc_mode, sizeof(dp_anc_mode));
        report_data_len += sizeof(dp_anc_mode);
#endif
#if DP_ID_POWER_DOWN
        memcpy(&report_data[report_data_len], dp_power_down_data, sizeof(dp_power_down_data));
        report_data_len += sizeof(dp_power_down_data);
#endif
#if DP_ID_LOW_LATENCY
        memcpy(&report_data[report_data_len], dp_latency_data, sizeof(dp_latency_data));
        report_data_len += sizeof(dp_latency_data);
#endif
#if DP_ID_BT_STA
        memcpy(&report_data[report_data_len], dp_bt_sta, sizeof(dp_bt_sta));
        report_data_len += sizeof(dp_bt_sta);
#endif
#if DP_ID_BT_NAME
        memcpy(&report_data[report_data_len], dp_bt_name, bt_name_len);
        report_data_len += bt_name_len;
#endif
#if DP_ID_BT_RST
        memcpy(&report_data[report_data_len], dp_bt_rst, sizeof(dp_bt_rst));
        report_data_len += sizeof(dp_bt_rst);
#endif
#if DP_ID_DEVICE_FIND
        memcpy(&report_data[report_data_len], dp_device_find, sizeof(dp_device_find));
        report_data_len += sizeof(dp_device_find);
#endif
#if DP_ID_KEY_LEFT_SHORT
        memcpy(&report_data[report_data_len], dp_key_left_short, sizeof(dp_key_left_short));
        report_data_len += sizeof(dp_key_left_short);
        memcpy(&report_data[report_data_len], dp_key_right_short, sizeof(dp_key_right_short));
        report_data_len += sizeof(dp_key_right_short);
#endif
#if DP_ID_KEY_LEFT_DOUBLE
        memcpy(&report_data[report_data_len], dp_key_left_double, sizeof(dp_key_left_double));
        report_data_len += sizeof(dp_key_left_double);
        memcpy(&report_data[report_data_len], dp_key_right_double, sizeof(dp_key_right_double));
        report_data_len += sizeof(dp_key_right_double);
#endif
#if DP_ID_KEY_LEFT_THREE
        memcpy(&report_data[report_data_len], dp_key_left_three, sizeof(dp_key_left_three));
        report_data_len += sizeof(dp_key_left_three);
        memcpy(&report_data[report_data_len], dp_key_right_three, sizeof(dp_key_right_three));
        report_data_len += sizeof(dp_key_right_three);
#endif
#if DP_ID_KEY_LEFT_LONG
        memcpy(&report_data[report_data_len], dp_key_left_long, sizeof(dp_key_left_long));
        report_data_len += sizeof(dp_key_left_long);
        memcpy(&report_data[report_data_len], dp_key_right_long, sizeof(dp_key_right_long));
        report_data_len += sizeof(dp_key_right_long);
#endif
#if DP_ID_KEY_SHORT_EN
        memcpy(&report_data[report_data_len], dp_key_short_en, sizeof(dp_key_short_en));
        report_data_len += sizeof(dp_key_short_en);
#endif
#if DP_ID_EQ_MODE
        memcpy(&report_data[report_data_len], dp_eq_mode, sizeof(dp_eq_mode));
        report_data_len += sizeof(dp_eq_mode);
#endif
        TUYA_APP_LOG_DEBUG("tuya_ble_app_dp_query:%d",report_data_len);

        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_PASSIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, report_data, report_data_len);

        tuya_ble_free(report_data);
    }
}

void tuya_ble_vol_report(void)
{
#if DP_ID_VOL
    if(tuya_ble_app.bond_flag && !tuya_ble_ota_is_start()){
        u8 dp_vol_data[5] = {DP_ID_VOL, DT_VALUE, 0x00, 1, 0x00};
        if (sys_cb.incall_flag) {
            dp_vol_data[4] = (sys_cb.hfp_vol * 100) / 15;
        }else{
            dp_vol_data[4] = (sys_cb.vol * 100) / VOL_MAX;
        }

        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, dp_vol_data, sizeof(dp_vol_data));
    }
#endif
}

void tuya_ble_anc_mode_report(void)
{
#if DP_ID_ANC_MODE
    if(tuya_ble_app.bond_flag && !tuya_ble_ota_is_start()){
        u8 dp_anc_mode_data[5] = {DP_ID_ANC_MODE, DT_ENUM, 0x00, DT_ENUM_LEN, 0x00};
        dp_anc_mode_data[4] = anc_mode_table[sys_cb.anc_user_mode];

        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, dp_anc_mode_data, sizeof(dp_anc_mode_data));
    }
#endif
}

void tuya_ble_low_latency_report(u8 val)
{
#if DP_ID_LOW_LATENCY
    tuya_ble_app.low_latency = val;
    tuya_ble_cm_low_latency_save();

    if(tuya_ble_app.bond_flag && !tuya_ble_ota_is_start()){
        u8 dp_data[5] = {DP_ID_LOW_LATENCY, DT_ENUM, 0x00, DT_ENUM_LEN, 0x00};
        dp_data[4] = tuya_ble_app.low_latency;
        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITH_RESPONSE, dp_data, sizeof(dp_data));
    }
#endif
}

void tuya_bt_name_init(void)
{
    u8 bt_name_len = 0;
    if(tuya_ble_app.cm_flag == TUYA_CM_TAG){
        tuya_ble_cm_read(&bt_name_len, TUYA_CM_BT_NAME_LEN, 1);
        if((bt_name_len > 0) && (bt_name_len <= 31)){
            memset(xcfg_cb.bt_name, 0, sizeof(xcfg_cb.bt_name));
            tuya_ble_cm_read(xcfg_cb.bt_name, TUYA_CM_BT_NAME, bt_name_len);
        }
    }else{
        bt_name_len = strlen(tuya_ble_app.bt_name);
        memcpy(xcfg_cb.bt_name, tuya_ble_app.bt_name, 32);
        tuya_ble_cm_write(xcfg_cb.bt_name,TUYA_CM_BT_NAME, bt_name_len, 0);
        tuya_ble_cm_write(&bt_name_len, TUYA_CM_BT_NAME_LEN, 1, 0);
    }
    bt_name_len = strlen(bt_get_local_name());
    memcpy(bt_state_info.name,bt_get_local_name(),bt_name_len);
}

void tuya_ble_timer_init(void)
{
    tuya_ble_cm_timer_creat();

    tuya_ble_500ms_timer_creat();

#if DP_ID_VBAT_LEFT
    tuya_ble_vbat_timer_creat();
#endif

#if DP_ID_DEVICE_FIND
    tuya_ble_warning_timer_creat();
#endif

#if DP_ID_PP
    tuya_ble_play_sta_timer_creat();
#endif
}

/*********************************************************
FN:
*/
void tuya_ble_var_init(void)
{
    TUYA_APP_LOG_DEBUG("-->tuya_ble_var_init\n");

    u16 flag;
    u8 offset = 0;
    static u8 init_flag = 0;

    memset(&__tuya_data_start, 0, (u32)&__tuya_data_size);

    tuya_ble_device_param_t tuya_ble_device_param ;

    u8 auth_info_len = 2 + DEVICE_ID_LEN + AUTH_KEY_LEN + MAC_STRING_LEN + TUYA_BLE_PRODUCT_ID_DEFAULT_LEN;
    u8 *auth_info = (u8*)&tuya_ble_device_param;

    memset(&tuya_ble_device_param,0,sizeof(tuya_ble_device_param));

    soft_timer_init();
    tuya_ble_timer_init();

    tuya_ble_nv_read(TUYA_AUTH_INFO_ADDR, auth_info, auth_info_len);
    memcpy(&flag,auth_info, 2);

    tuya_ble_app.bond_flag = false;

    if(xcfg_cb.tuya_auth_update){
        if(memcmp(TUYA_DEVICE_UUID, &auth_info[2], DEVICE_ID_LEN) ||
           memcmp(TUYA_DEVICE_PID, &auth_info[2+DEVICE_ID_LEN+AUTH_KEY_LEN+MAC_STRING_LEN], TUYA_BLE_PRODUCT_ID_DEFAULT_LEN)){
            flag = TUYA_AUTH_FLAG;
            offset = 0;
            memcpy(&auth_info[offset],&flag,2);
            offset += 2;
            memcpy(&auth_info[offset], &TUYA_DEVICE_UUID, DEVICE_ID_LEN);
            offset += DEVICE_ID_LEN;
            memcpy(&auth_info[offset], &TUYA_DEVICE_AUTH_KEY,  AUTH_KEY_LEN);
            offset += AUTH_KEY_LEN;
            memcpy(&auth_info[offset], &TUYA_DEVICE_MAC, MAC_STRING_LEN);
            offset += MAC_STRING_LEN;
            memcpy(&auth_info[offset], &TUYA_DEVICE_PID, TUYA_BLE_PRODUCT_ID_DEFAULT_LEN);
            tuya_ble_nv_erase(TUYA_AUTH_INFO_ADDR, TUYA_NV_ERASE_MIN_SIZE);
            tuya_ble_nv_write(TUYA_AUTH_INFO_ADDR, auth_info, auth_info_len);
			tuya_ble_app.unbond_flg = true;
            //tuya_ble_device_unbond();
        }
    }else{
        if(flag != TUYA_AUTH_FLAG){
            flag = TUYA_AUTH_FLAG;
            offset = 0;
            memcpy(&auth_info[offset], &flag, 2);
            offset += 2;
            memcpy(&auth_info[offset], &TUYA_DEVICE_UUID, DEVICE_ID_LEN);
            offset += DEVICE_ID_LEN;
            memcpy(&auth_info[offset], &TUYA_DEVICE_AUTH_KEY, AUTH_KEY_LEN);
            offset += AUTH_KEY_LEN;
            memcpy(&auth_info[offset], &TUYA_DEVICE_MAC, MAC_STRING_LEN);
            offset += MAC_STRING_LEN;
            memcpy(&auth_info[offset], &TUYA_DEVICE_PID, TUYA_BLE_PRODUCT_ID_DEFAULT_LEN);
            tuya_ble_nv_erase(TUYA_AUTH_INFO_ADDR, TUYA_NV_ERASE_MIN_SIZE);
            tuya_ble_nv_write(TUYA_AUTH_INFO_ADDR, auth_info, auth_info_len);
        }else{
            offset = 2;
            memcpy(&TUYA_DEVICE_UUID, &auth_info[offset], DEVICE_ID_LEN);
            offset += DEVICE_ID_LEN;
            memcpy(&TUYA_DEVICE_AUTH_KEY, &auth_info[offset], AUTH_KEY_LEN);
            offset += AUTH_KEY_LEN;
            memcpy(&TUYA_DEVICE_MAC, &auth_info[offset], MAC_STRING_LEN);
            //offset += MAC_STRING_LEN;
            //memcpy(&TUYA_DEVICE_PID, &auth_info[offset], TUYA_BLE_PRODUCT_ID_DEFAULT_LEN);
        }
    }
    //TUYA_APP_LOG_DEBUG("TUYA_DEVICE_UUID:%s\n",TUYA_DEVICE_UUID);
    //TUYA_APP_LOG_DEBUG("TUYA_DEVICE_AUTH_KEY:%s\n",TUYA_DEVICE_AUTH_KEY);
    //TUYA_APP_LOG_DEBUG("TUYA_DEVICE_MAC:%s\n",TUYA_DEVICE_MAC);
    //TUYA_APP_LOG_DEBUG("TUYA_DEVICE_PID:%s\n",TUYA_DEVICE_PID);

    memset(&tuya_ble_device_param, 0, sizeof(tuya_ble_device_param));

    tuya_ble_device_param.bound_flag = 0;
    tuya_ble_device_param.use_ext_license_key = 1;
    tuya_ble_device_param.device_id_len = DEVICE_ID_LEN;
    memcpy(tuya_ble_device_param.auth_key, (void *)TUYA_DEVICE_AUTH_KEY, AUTH_KEY_LEN);
    memcpy(tuya_ble_device_param.device_id, (void *)TUYA_DEVICE_UUID, DEVICE_ID_LEN);
    memcpy(tuya_ble_device_param.mac_addr_string, TUYA_DEVICE_MAC, MAC_STRING_LEN);
    tuya_ble_device_param.mac_addr.addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;

    memcpy(tuya_ble_device_param.product_id, (void *)TUYA_DEVICE_PID, TUYA_BLE_PRODUCT_ID_DEFAULT_LEN);
    tuya_ble_device_param.p_type = TUYA_BLE_PRODUCT_ID_TYPE_PID;
    tuya_ble_device_param.product_id_len = TUYA_BLE_PRODUCT_ID_DEFAULT_LEN;
    tuya_ble_device_param.firmware_version = TUYA_DEVICE_FVER_NUM;
    tuya_ble_device_param.hardware_version = TUYA_DEVICE_HVER_NUM;

    tuya_ble_device_param.adv_local_name_len = strlen(xcfg_cb.le_name);
    if(tuya_ble_device_param.adv_local_name_len > TUYA_BLE_ADV_LOCAL_NAME_MAX_LEN){
        tuya_ble_device_param.adv_local_name_len = TUYA_BLE_ADV_LOCAL_NAME_MAX_LEN;
    }
    memcpy(tuya_ble_device_param.adv_local_name, xcfg_cb.le_name, tuya_ble_device_param.adv_local_name_len);

    memcpy(tuya_ble_app.bt_name, bt_get_local_name(), 32);

    tuya_ble_app.play_sta_check_time = 500;

    tuya_ble_cm_read(&tuya_ble_app.cm_flag, TUYA_CM_FLAG, 2);

    tuya_bt_name_init();

    prvHeapInit();
    tuya_ble_sdk_init(&tuya_ble_device_param);
    tuya_ble_callback_queue_register(tuya_ble_sdk_callback);

#if BT_TWS_EN
    tws_lr_xcfg_sel();
#endif

#if DP_ID_KEY_CTL_EN
    tuya_ble_key_init();
#endif

#if TUYA_BLE_OTA_EN
    tuya_ble_app_ota_init();
#endif

#if EQ_MODE_EN
    tuya_ble_eq_var_init();
#endif

#if DP_ID_ANC_MODE
    tuya_ble_cm_anc_load();
#endif

#if DP_ID_LOW_LATENCY
    tuya_ble_cm_low_latency_load();
#endif

    if(init_flag == 0){
        init_flag = 1;
    }else{
        tuya_ble_app.init_flag = 1;
    }
}

void tuya_ble_dev_unbond(void)
{
	if(tuya_ble_app.unbond_flg){
        tuya_ble_device_unbond();
	}
}

void tuya_ble_app_init(void)
{
    TUYA_APP_LOG_DEBUG("-->tuya_ble_app_init\n");

    tuya_ble_dev_unbond();

#if EQ_MODE_EN
    tuya_ble_eq_set();
#endif

#if DP_ID_LOW_LATENCY
    if(tuya_ble_app.low_latency){
        bt_low_latency_enble();
    }
#endif

#if DP_ID_ANC_MODE
    tuya_ble_app.anc_mode = anc_mode_table[sys_cb.anc_user_mode];
    if(sys_cb.anc_user_mode){
        bsp_anc_set_mode(sys_cb.anc_user_mode);
    }
#endif

    tuya_ble_app.init_flag = 1;

    if(tuya_ble_app.cm_flag != TUYA_CM_TAG){
        TUYA_APP_LOG_DEBUG("tuya_system_default_info\n");
        tuya_ble_app.cm_flag = TUYA_CM_TAG;
        tuya_ble_cm_write(&tuya_ble_app.cm_flag,TUYA_CM_FLAG,2,1);
    }
}

void tuya_ble_device_reset_2_factory(void)
{
    tuya_ble_app.cm_flag = 0xFFFF;
    tuya_ble_cm_write(&tuya_ble_app.cm_flag,TUYA_CM_FLAG,2,1);

#if DP_ID_ANC_MODE
    tuya_ble_cm_anc_load();
#endif

#if DP_ID_LOW_LATENCY
    tuya_ble_cm_low_latency_load();
#endif

#if DP_ID_KEY_CTL_EN
    tuya_ble_key_init();
#endif

#if EQ_MODE_EN
    tuya_ble_eq_var_init();
    tuya_ble_eq_set();
#endif

#if DP_ID_LOW_LATENCY
    if(tuya_ble_app.low_latency){
        bt_low_latency_enble();
    }else{
        bt_low_latency_disable();
    }
#endif

#if DP_ID_ANC_MODE
    bsp_anc_set_mode(sys_cb.anc_user_mode);
#endif

#if DP_ID_BT_NAME
    tuya_bt_name_init();
    bt_set_stack_local_name(xcfg_cb.bt_name);
#endif

    if(tuya_ble_app.cm_flag != TUYA_CM_TAG){
        TUYA_APP_LOG_DEBUG("tuya_system_default_info\n");
        tuya_ble_app.cm_flag = TUYA_CM_TAG;
        tuya_ble_cm_write(&tuya_ble_app.cm_flag,TUYA_CM_FLAG,2,1);
    }

    if(tuya_ble_app.bond_flag){
        tuya_ble_app_dp_query_handle(NULL,0);
    }
}

void tuya_bt_link_addr_save(u8 *addr)
{
    tuya_ble_cm_write(addr, TUYA_CM_LINK_ADDR, 6, 1);
}

void tuya_bt_link_addr_load(u8 *addr)
{
    tuya_ble_cm_read(addr, TUYA_CM_LINK_ADDR, 6);
}

void tuya_bt_disconnect_callback(u8 *buf)
{
#if DP_ID_BT_STA
    if(tuya_ble_app.bond_flag){
        u8 dp_data[5] = {DP_ID_BT_STA,DT_ENUM, 0x00, DT_ENUM_LEN, 0x00};
        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, (void*)dp_data, sizeof(dp_data));
    }
#endif
}

void tuya_bt_connect_callback(u8 *buf)
{
    u8 bt_addr[6];
    tuya_bt_link_addr_load(bt_addr);
    if(memcmp(bt_addr, buf, 6)){
       tuya_bt_link_addr_save(buf);
    }
#if DP_ID_BT_STA
    if(tuya_ble_app.bond_flag){
        u8 dp_data[5] = {DP_ID_BT_STA, DT_ENUM, 0x00, DT_ENUM_LEN, 0x02};
        tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(), DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, (void*)dp_data, sizeof(dp_data));
    }
#endif
}

void tuya_tws_connect_callback(void)
{
    if(bt_tws_is_slave() == 0) {
        tuya_ble_tws_info_all_sync();
    }
}

void tuya_tws_disconnect_callback(void)
{
    if(ble_is_connect() == 0){
        ble_adv_en();
    }

    tuya_ble_app.remote_bond_flag = 0;
    tuya_ble_app.remote_vbat = 0;
    tuya_ble_vbat_report_do();
}

/*********************************************************
FN:
*/
static void tuya_ble_sdk_callback(tuya_ble_cb_evt_param_t* event)
{
    //TUYA_APP_LOG_DEBUG("-->evt_cb:0x%x",event->evt);

    switch(event->evt)
    {
        case TUYA_BLE_CB_EVT_CONNECTE_STATUS: {
            if(event->connect_status == BONDING_CONN) {
                TUYA_APP_LOG_DEBUG("bonding and connecting");
                tuya_ble_app.bond_flag = 1;
#if DP_ID_POWER_DOWN
                tuya_ble_app.powerdown_cnt = 0;
#endif
#if DP_ID_VBAT_LEFT
                tuya_ble_timer_start(tuya_vbat_timer);
#endif
                tuya_ble_tws_sys_settings_sync();
            }else{
                if(event->connect_status == BONDING_UNCONN){
                #if TUYA_BLE_OTA_EN
                    tuya_ble_app_ota_disconn_handler();
                #endif
                    tuya_ble_app.bond_flag = 0;
                    tuya_ble_tws_bond_sta_sync();
                }else if(event->connect_status == UNBONDING_UNCONN){
                    tuya_ble_tws_sys_settings_sync();
#if TUYA_UNBOND_LINK_INFO_CLEAR
                    //if(tuya_ble_app.bond_flag){
                        if(bt_nor_is_connected()){
                            bt_nor_disconnect();
                        }
                        bt_nor_delete_link_info();
                    //}
#endif
                }else{
                    ble_update_conn_param(TUYA_CON_INTERVAL, 0, 4000);
                    delay_5ms(2);
                }
                tuya_ble_app.bond_flag = 0;
#if DP_ID_VBAT_LEFT
                tuya_ble_timer_stop(tuya_vbat_timer);
#endif
#if DP_ID_DEVICE_FIND
                tuya_ble_timer_stop(tuya_warning_timer);
#endif
            }
        } break;

        case TUYA_BLE_CB_EVT_DP_DATA_RECEIVED:{
            tuya_ble_dp_data_send(tuya_ble_dp_send_sn_get(),DP_SEND_TYPE_ACTIVE,DP_SEND_FOR_CLOUD_PANEL,DP_SEND_WITHOUT_RESPONSE,(void*)event->dp_received_data.p_data,event->dp_received_data.data_len);
            tuya_ble_dp_data_t* p_dp_data = (tuya_ble_dp_data_t*)event->dp_received_data.p_data;
            tuya_ble_inverted_array((u8*)&p_dp_data->dp_data_len,sizeof(uint16_t));
            TUYA_APP_LOG_HEXDUMP_DEBUG("dp_data", (void*)p_dp_data, p_dp_data->dp_data_len+4);
            tuya_ble_dp_write_handler(p_dp_data->dp_id,p_dp_data->dp_type,p_dp_data->dp_data_len,p_dp_data->dp_data);
        } break;

        case TUYA_BLE_CB_EVT_DP_DATA_SEND_RESPONSE:
            //TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_DP_DATA_SEND_RESPONSE = %d,%d",event->dp_send_response_data.sn,event->dp_send_response_data.status);
            break;

        case TUYA_BLE_CB_EVT_DP_DATA_WITH_TIME_SEND_RESPONSE:
            //TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_DP_DATA_SEND_RESPONSE = %d,%d",event->dp_with_time_send_response_data.sn,event->dp_with_time_send_response_data.status);
            break;

        case TUYA_BLE_CB_EVT_UNBOUND: {
            TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_UNBOUND");
        } break;

        case TUYA_BLE_CB_EVT_ANOMALY_UNBOUND: {
            TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_ANOMALY_UNBOUND");
        } break;

        case TUYA_BLE_CB_EVT_DEVICE_RESET: {
            TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_DEVICE_RESET");
            tuya_ble_tws_reset_sync();
            tuya_ble_device_reset_2_factory();
        } break;

        case TUYA_BLE_CB_EVT_DP_QUERY: {
            TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_DP_QUERY");
            tuya_ble_app_dp_query_handle(event->dp_query_data.p_data,event->dp_query_data.data_len);
        } break;

#if TUYA_BLE_OTA_EN
        case TUYA_BLE_CB_EVT_OTA_DATA: {
            tuya_ble_app_ota_handler(event->ota_data.type,event->ota_data.p_data,event->ota_data.data_len);
        } break;
#endif

        case TUYA_BLE_CB_EVT_TIME_STAMP: {
            TUYA_APP_LOG_DEBUG("TUYA_BLE_CB_EVT_TIME_STAMP: %s, time_zone: %d",event->timestamp_data.timestamp_string,event->timestamp_data.time_zone);
        } break;

        case TUYA_BLE_CB_EVT_TIME_NORMAL: {
        } break;

        case TUYA_BLE_CB_EVT_DATA_PASSTHROUGH: {
        } break;

        default: {
            TUYA_APP_LOG_DEBUG("tuya_ble_sdk_callback unknown event type 0x%04x", event->evt);
        } break;
    }
}

/*********************************************************
FN:
*/
static void tuya_ble_custom_data_process(int32_t evt_id, void *data)
{
    tuya_ble_custom_evt_data_t* custom_data = data;

    switch (evt_id){

        case CUSTOM_EVT_TWS_SYNC:
            tuya_ble_tws_recv_proc(custom_data->value,custom_data->len);
            break;

        default:
            break;
    }

    if(custom_data != NULL){
        tuya_ble_free((void*)custom_data);
    }
}

void tuya_ble_custom_evt_send(custom_evt_id_t evtid)
{
    tuya_ble_custom_evt_t custom_evt;

    custom_evt.evt_id = evtid;
    custom_evt.data = NULL;
    custom_evt.custom_event_handler = tuya_ble_custom_data_process;

    tuya_ble_custom_event_send(custom_evt);
}

void tuya_ble_custom_evt_send_with_data(custom_evt_id_t evtid, void* buf, uint32_t size)
{
    tuya_ble_custom_evt_data_t* custom_data = tuya_ble_malloc(sizeof(tuya_ble_custom_evt_data_t) + size);

    if(custom_data){
        tuya_ble_custom_evt_t custom_evt;

        custom_data->len = size;
        memcpy(custom_data->value, buf, size);

        custom_evt.evt_id = evtid;
        custom_evt.data = custom_data;
        custom_evt.custom_event_handler = tuya_ble_custom_data_process;

        tuya_ble_custom_event_send(custom_evt);
    }
    else {
        TUYA_APP_LOG_DEBUG("tuya_ble_custom_evt_send_with_data: malloc failed");
    }
}

void tuya_device_info_update(void)
{
#if DP_ID_ANC_MODE
    if(tuya_ble_app.anc_mode != anc_mode_table[sys_cb.anc_user_mode]){
        tuya_ble_app.anc_mode = anc_mode_table[sys_cb.anc_user_mode];
        tuya_ble_anc_mode_report();
    }
#endif

#if DP_ID_VOL
    if((tuya_ble_app.vol != sys_cb.vol) || (tuya_ble_app.hfp_vol != sys_cb.hfp_vol) || (tuya_ble_app.incall_flag != sys_cb.incall_flag)){
        tuya_ble_app.incall_flag = sys_cb.incall_flag;
        tuya_ble_app.vol = sys_cb.vol;
        tuya_ble_app.hfp_vol = sys_cb.hfp_vol;
        tuya_ble_vol_report();
    }

#endif

#if DP_ID_LOW_LATENCY
    if(tuya_ble_app.low_latency != bt_is_low_latency()){
        tuya_ble_app.low_latency = bt_is_low_latency();
        tuya_ble_low_latency_report(tuya_ble_app.low_latency);
    }
#endif
}

void tuya_ble_flag_do(void)
{
    if(tuya_ble_app.do_flag & TUYA_FLAG_EQ_SET){
        if(!sco_is_connected()){
            tuya_ble_app.do_flag &= ~TUYA_FLAG_EQ_SET;
            tuya_ble_eq_set();
        }
    }
}

void tuya_ble_process(void)
{
    soft_timer_run();

#if TUYA_BLE_OTA_EN
    if(tuya_ble_ota_is_start()){
        reset_sleep_delay();
        reset_pwroff_delay();
        while(tuya_ble_sched_queue_events_get()){
            tuya_ble_main_tasks_exec();
            WDT_CLR();
        }
        tuya_ble_ota_process();
    }else
#endif
    {
#if DP_ID_PP
        tuya_ble_play_sta_report_proc();
#endif
        tuya_device_info_update();
        tuya_ble_main_tasks_exec();
    }

    if(tuya_ble_app.do_flag){
        tuya_ble_flag_do();
    }

    tuya_ble_app.data_receive_flag = 0;
}

AT(.com_text.tuya_text)
void tuya_ble_powerdown_process(void)
{
#if DP_ID_POWER_DOWN
    if(tuya_ble_app.powerdown_cnt){
       tuya_ble_app.powerdown_cnt--;
       reset_pwroff_delay();
       if(tuya_ble_app.powerdown_cnt == 0){
            sys_cb.discon_reason = 1;               //同步关机
            sys_cb.pwrdwn_tone_en = 1;
            func_cb.sta = FUNC_PWROFF;
            tuya_ble_app.data_receive_flag = 1;     //如果在sleep模式下强制退出sleep
       }
    }
#endif
}

AT(.com_text.tuya_text)
void tuya_ble_sleep_mode_process(void)
{
    tuya_ble_powerdown_process();
}

uint16_t tuya_ble_role_switch_get_data(uint8_t *data_ptr)
{
    u8 offset = 0;

    data_ptr[offset++] = tuya_ble_app.dp_send_sn;
    data_ptr[offset++] = tuya_ble_app.dev_find;
    data_ptr[offset++] = tuya_ble_app.bond_flag;

    memcpy(&data_ptr[offset], tuya_ble_pair_rand, 6);
    offset += 6;
    data_ptr[offset++] =  tuya_ble_pair_rand_valid;
    memcpy(&data_ptr[offset], &tuya_ble_receive_sn, 4);
    offset += 4;
    memcpy(&data_ptr[offset], &tuya_ble_send_sn, 4);
    offset += 4;

#if DP_ID_KEY_CTL_EN
    memcpy(&data_ptr[offset], &tuya_ble_app.local_key, sizeof(tuya_ble_key_info_t));
    offset += sizeof(tuya_ble_key_info_t);
#endif

    if(tuya_ble_app.dev_find){
        tuya_ble_app.dev_find = 0;
        tuya_ble_timer_stop(tuya_warning_timer);
    }

    if(tuya_ble_app.bond_flag){
        tuya_ble_app.bond_flag = 0;
        tuya_ble_timer_stop(tuya_vbat_timer);
    }

    return offset;
}

uint16_t tuya_ble_role_switch_set_data(uint8_t *data_ptr, uint16_t len)
{
    u8 offset = 0;

    tuya_ble_app.dp_send_sn = data_ptr[offset++];
    tuya_ble_app.dev_find = data_ptr[offset++];
    tuya_ble_app.bond_flag = data_ptr[offset++];

    memcpy(tuya_ble_pair_rand, &data_ptr[offset], 6);
    offset += 6;
    tuya_ble_pair_rand_valid = data_ptr[offset++];
    memcpy(&tuya_ble_receive_sn, &data_ptr[offset], 4);
    offset += 4;
    memcpy(&tuya_ble_send_sn, &data_ptr[offset], 4);
    offset += 4;

#if DP_ID_KEY_CTL_EN
    memcpy(&tuya_ble_app.remote_key,  &data_ptr[offset], sizeof(tuya_ble_key_info_t));
    offset += sizeof(tuya_ble_key_info_t);
#endif

    if(tuya_ble_app.dev_find){
        tuya_ble_timer_start(tuya_warning_timer);
    }

    if(tuya_ble_app.bond_flag){
        tuya_ble_connect_status_set(BONDING_CONN);
        tuya_ble_timer_start(tuya_vbat_timer);
    }

    return 0;
}

void tuya_bt_evt_notice(uint evt, void *params)
{
    u8 *packet = params;

    switch(evt){
        case BT_NOTICE_INIT_FINISH:
            tuya_ble_adv_change();
            ble_enable_adv();
            break;

        case BT_NOTICE_DISCONNECT:
            tuya_bt_disconnect_callback(NULL);
            break;

        case BT_NOTICE_CONNECTED:
            tuya_bt_connect_callback(&packet[2]);
            break;

        case BT_NOTICE_TWS_DISCONNECT:
            tuya_tws_disconnect_callback();
            break;

        case BT_NOTICE_TWS_CONNECTED:
            tuya_tws_connect_callback();
            break;

        case BT_NOTICE_TWS_ROLE_CHANGE:
            break;
    }
}

void tuya_ble_enter_sleep(void)
{
    if(ble_get_status() == LE_STA_ADVERTISING){
        ble_set_adv_interval(TUYA_ADV_SLEEP_INTERVAL);
    }else if(ble_get_status() == LE_STA_CONNECTION){
        ble_update_conn_param(TUYA_CON_SLEEP_INTERVAL, 0, 400);
    }
}

void tuya_ble_exit_sleep(void)
{
    if(ble_get_status() == LE_STA_ADVERTISING){
        ble_set_adv_interval(TUYA_ADV_INTERVAL);
    }else if(ble_get_status() == LE_STA_CONNECTION){
        ble_update_conn_param(TUYA_CON_INTERVAL, 0, 400);
    }
}

#endif
