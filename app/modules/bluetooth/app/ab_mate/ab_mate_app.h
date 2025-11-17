#ifndef __AB_MATE_APP_H
#define __AB_MATE_APP_H

#if AB_MATE_APP_EN

#include "ab_mate_timer.h"
#include "ab_mate_profile.h"

#define AB_MATE_HEADER_LEN      5
#define AB_MATE_PAYLOAD_POS     5
#define AB_MATE_CM_TAG          0xab23
#define AB_MATE_BT_NAME_TAG     0x5d

#define AB_MATE_ADV_INTERVAL        480     //adv: x * 0.625ms
#define AB_MATE_ADV_SLEEP_INTERVAL  640
#define AB_MATE_CON_INTERVAL        52      //con: x * 1.25ms
#define AB_MATE_CON_SLEEP_INTERVAL  320

#define AB_MATE_RECORD_CONN_INTERVAL  12
#define OPUS_FRAM_NUMS 3

#define FLAG_EQ_SET                BIT(0)
#define FLAG_DEVICE_RESET          BIT(1)
#define FLAG_BT_LINK_INFO_CLEAR    BIT(2)
#define FLAG_TWS_STA_NOTIFY        BIT(3)
#define FLAG_BT_NAME_SET           BIT(4)
#define FLAG_BT_ROLE_SWITCH        BIT(5)
#define FLAG_V3D_AUDIO_SET         BIT(6)
#define FLAG_MULT_DEV_SET          BIT(7)
#define FLAG_MULT_DEV_DISCON_SET   BIT(8)
#define FLAG_MULT_DEV_UNPAIR_SET   BIT(9)
#define FLAG_RECORD_CTRL           BIT(10)
#define FLAG_RECORD_DATA_UPLOAD    BIT(11)
#define FLAG_DYNAMIC_BASS_SET      BIT(12)

//APP功能配置
#define AB_MATE_EQ_EN           EQ_MODE_EN              //EQ设置
#define AB_MATE_KEY_EN          1                       //按键功能设置
#define AB_MATE_VOL_EN          1                       //音量控制
#define AB_MATE_ANC_EN          ANC_EN                  //ANC控制
#define AB_MATE_LATENCY_EN      BT_LOW_LATENCY_EN       //游戏模式设置
#define AB_MATE_INERA_EN        USER_INEAR_DETECT_EN    //入耳检测设置
#define AB_MATE_LANG_EN         1                       //语言设置
#define AB_MATE_BT_NAME_EN      1                       //蓝牙名设置
#define AB_MATE_POWER_OFF_EN    0                       //关机时间设置
#define AB_MATE_LED_EN          LED_DISP_EN             //LED开关设置
#define AB_MATE_PP_EN           1                       //播放暂停控制
#define AB_MATE_DEVICE_FIND_EN  0                       //设备查找
#define AB_MATE_OTA_EN          (FOT_EN && 1)           //OTA升级功能
#define AB_MATE_V3D_AUDIO_EN    1//BT_MUSIC_AUDIO_EN       //3D音效开关设置
#define AB_MATE_CTKD_EN         LE_SM_SC_EN             //连接BLE后自动连接经典蓝牙功能
#define AB_MATE_CALL_EN         0                       //通话功能设置
#define AB_MATE_MIC_EN          0                       //MIC开关设置
#define AB_MATE_BT_STA_EN       1                       //蓝牙状态上报设置
#define AB_MATE_MULT_DEV_EN     BT_2ACL_EN              //一拖二设置
#define AB_MATE_DYNAMIC_BASS_EN 0                       //动态低音设置
#define AB_MATE_BT_ATT_EN       BT_ATT_EN               //GATT_OVER_BREDR
#define AB_MATE_ADV0_EN         (LE_ADV0_EN&&LE_ADV0_CON_EN) //是否使用adv0发送ab_mate广播
#define AB_MATE_RECORD_EN       OPUS_ENC_EN              

//Ex
#define AB_MATE_CUSTOM_CMD_EN       1                    //自定义命令
#define AB_MATE_CUSTOM_CMD_TONE_EN  1                    //自定义命令-提示音
#define AB_MATE_CUSTOM_CMD_CALL_EN  1                    //自定义命令-拨号

//Eq设置
#define AB_MATE_EQ_BAND_CNT     10      //EQ总段数，需要和APP界面相对应
#define AB_MATE_EQ_CUSTOM_INDEX 0x20    //自定义EQ对应的模式号
#define AB_MATE_EQ_USE_DEVICE   1       //0：各模式的EQ增益使用APP端默认定义的  1：使用设备端定义的增益值
#define AB_MATE_EQ_USE_RES      1       //非自定义EQ模式的EQ效果是否直接使用EQ资源文件里面的配置
#if APP_EQ_SET
#define AB_MATE_EQ_RES_CNT      8       //非自定义的EQ模式总数,AB_MATE_EQ_USE_RES设置为1才有效
#else
#define AB_MATE_EQ_RES_CNT      6       //非自定义的EQ模式总数,AB_MATE_EQ_USE_RES设置为1才有效
#endif

#define AB_MATE_EQ_FOR_IDX_EN   EQ_APP_EN //是否使能10条EQ独立调节(包括高低音)

//ANC 设置
#if AB_MATE_ANC_EN
#define AB_MATE_ANC_LEVEL       1       //0：降噪无等级设定              1:降噪有等级设定
#if AB_MATE_ANC_LEVEL
#define AB_MATE_ANC_TOTAL_LEVEL        4    //anc 等级总数
#define AB_MATE_TP_TOTAL_LEVEL         3    //tansparency 等级总数
#endif
#endif

//游戏模式设置
#define AB_MATE_LATENCY_USE_CM          0   //0：上电默认普通模式  1：上电使用上次保存的模式

//按键设置，只支持针对KEY_PLAY_USER_DEF和KEY_PLAY_PWR_USER_DEF进行设置
#define AB_MATE_KEY_USER_DEF_EN         APP_KEY_FUNTION_SELECT //使用ab mate定义的按键，0为使用配置文件定义的按键

#define AB_MATE_KEY_SHORT       xcfg_cb.user_def_ks_sel
#define AB_MATE_KEY_DOUBLE      xcfg_cb.user_def_kd_sel
#define AB_MATE_KEY_THREE       xcfg_cb.user_def_kt_sel
#define AB_MATE_KEY_LONG        xcfg_cb.user_def_kl_sel

#if AB_MATE_KEY_USER_DEF_EN
//按键默认功能，需要根据实际UI定义进行修改,固件将会使用这里设置的功能而不是配置文件里面的按键功能
#define KEY_SHORT_LEFT_DEF      APP_KEY_NONE
#define KEY_SHORT_RIGHT_DEF     APP_KEY_NONE
#define KEY_DOUBLE_LEFT_DEF     APP_KEY_PLAY_PAUSE
#define KEY_DOUBLE_RIGHT_DEF    APP_KEY_PLAY_PAUSE
#define KEY_THREE_LEFT_DEF      APP_KEY_SIRI
#define KEY_THREE_RIGHT_DEF     APP_KEY_LOW_LATENCY
#define KEY_LONG_LEFT_DEF       APP_KEY_PREV
#define KEY_LONG_RIGHT_DEF      APP_KEY_NEXT
#endif

//动态低音设置
#if AB_MATE_DYNAMIC_BASS_EN
#define AB_MATE_DYNAMIC_BASS_MIN_LEVEL -5  //动态低音最小范围，步进为1
#define AB_MATE_DYNAMIC_BASS_MAX_LEVEL  5  //动态低音最大范围，步进为1
#endif

/*Cm地址管理,需要请从后面添加*/
#define AB_MATE_CM_FLAG                     0x00                                                        //2byte
#define AB_MATE_CM_EQ_DATA                  (AB_MATE_CM_FLAG + 2)                                       //11byte,mode(1B)+gain(10B)
#define AB_MATE_CM_KEY_LOCAL                (AB_MATE_CM_EQ_DATA + 1 + AB_MATE_EQ_BAND_CNT)              //sizeof(ab_mate_key_info_t)
#define AB_MATE_CM_KEY_REMOTE               (AB_MATE_CM_KEY_LOCAL + sizeof(ab_mate_key_info_t))         //sizeof(ab_mate_key_info_t)
#define AB_MATE_CM_MODE                     (AB_MATE_CM_KEY_REMOTE + sizeof(ab_mate_key_info_t))        //1byte
#define AB_MATE_CM_IN_EAR                   (AB_MATE_CM_MODE + 1)                                       //1byte
#define AB_MATE_CM_BT_NAME_LEN              (AB_MATE_CM_IN_EAR + 1)                                     //1byte
#define AB_MATE_CM_BT_NAME                  (AB_MATE_CM_BT_NAME_LEN + 1)                                //32byte
#define AB_MATE_CM_LED_EN                   (AB_MATE_CM_BT_NAME + 32)                                   //1byte
#define AB_MATE_CM_EQ_CUSTOM1               (AB_MATE_CM_LED_EN + 1)                                     //10byte
#define AB_MATE_CM_EQ_CUSTOM2               (AB_MATE_CM_EQ_CUSTOM1 + AB_MATE_EQ_BAND_CNT)               //10byte
#define AB_MATE_CM_ANC_LEVEL                (AB_MATE_CM_EQ_CUSTOM2 + AB_MATE_EQ_BAND_CNT)               //1byte
#define AB_MATE_CM_TP_LEVEL                 (AB_MATE_CM_ANC_LEVEL + 1)                                  //1byte
#define AB_MATE_CM_V3D_AUDIO                (AB_MATE_CM_TP_LEVEL + 1)                                   //1byte
#define AB_MATE_CM_BT_NAME_FLAG             (AB_MATE_CM_V3D_AUDIO + 1)                                  //1byte
#define AB_MATE_CM_MULT_DEV_EN              (AB_MATE_CM_BT_NAME_FLAG + 1)                               //1byte
#define AB_MATE_CM_DYNAMIC_BASS_INFO        (AB_MATE_CM_MULT_DEV_EN + 1)                                //2byte(EN,LEVEL)
#define AB_MATE_CM_ANC_FADE                 (AB_MATE_CM_DYNAMIC_BASS_INFO + 2)                          //1byte


typedef enum{
    AB_MATE_SUCCESS = 0,
    AB_MATE_FAIL,
}ab_mate_result_t;

enum{
    AB_MATE_CON_NONE = 0,
    AB_MATE_CON_SPP,
    AB_MATE_CON_BLE,
    AB_MATE_CON_LATT,
};

enum{
    CMD_TYPE_REQUEST = 1,
    CMD_TYPE_RESPONSE,
    CMD_TYPE_NOTIFY,
};

enum{
    CMD_EQ_SET = 0x20,
    CMD_MUSIC_SET,
    CMD_KEY_SET,
    CMD_POWER_OFF_SET,
    CMD_DEVICE_RESET,
    CMD_MODE_SET,
    CMD_IN_EAR_SET,
    CMD_DEVICE_INFO_GET,
    CMD_DEVICE_INFO_NOTIFY,
    CMD_LANGUAGE_SET,
    CMD_DEVICE_FIND,
    CMD_AUTO_ANSWER_SET,
    CMD_ANC_SET,
    CMD_BT_NAME_SET,
    CMD_LED_SET,
    CMD_BT_LINK_INFO_CLEAR,
    CMD_ANC_LEVEL_SET,
    CMD_TP_LEVEL_SET,
    CMT_V3D_AUDIO_SET,
    CMD_MULT_DEV_SET,
    CMD_VOICE_RECOGNITION,
    CMD_ANC_INFO_SET,
    CMD_DYNAMIC_BASS_SET,

    CMD_CALL_CTRL = 0x39,
    CMD_MIC_CTRL,
    CMD_RECORD_CTRL,
    CND_RECORD_DATA_NOTIFY,
    CMD_OTA_REQ = 0xA0,
    CMD_OTA_DATA_START,
    CMD_OTA_DATA_CONTINUE,
    CMD_OTA_STA,

    CMD_CUSTOM = 0xE0,
};

enum{
    ANC_CTL_MODE = 1,
    ANC_CTL_ANC_LEVEL,
    ANC_CTL_TP_LEVEL,
    ANC_CTL_FADE,
};

enum{
    A2DP_CTL_VOICE = 1,
    A2DP_CTL_PLAY,
    A2DP_CTL_PASUE,
    A2DP_CTL_PREV,
    A2DP_CTL_NEXT,
};

enum{
    KEY_LEFT_SHORT = 1,
    KEY_RIGHT_SHORT,
    KEY_LEFT_DOUBLE,
    KEY_RIGHT_DOUBLE,
    KEY_LEFT_THREE,
    KEY_RIGHT_THREE,
    KEY_LEFT_LONG,
    KEY_RIGHT_LONG,
};

enum{
    MULT_DEV_CTR_EN = 1,
    MULT_DEV_CTR_CON,
    MULT_DEV_CTR_DISCON,
    MULT_DEV_CTR_UNPAIR,
    MULT_DEV_CTR_APP_DEV_ADDR,
};

enum{
    DYNAMIC_BASS_EN = 1,
    DYNAMIC_BASS_LEVEL_SET,
};

//按键功能，和APP定义的按键功能相对应,如果这里的值有修改，需要同步修改key_local_table的值
enum{
    APP_KEY_NONE            = 0,
    APP_KEY_REDIALING,
    APP_KEY_SIRI,
    APP_KEY_PREV,
    APP_KEY_NEXT,
    APP_KEY_VOL_UP,
    APP_KEY_VOL_DOWN,
    APP_KEY_PLAY_PAUSE,
    APP_KEY_LOW_LATENCY,
    APP_KEY_ANC,
};

enum{
    DEVICE_MODE_NORMAL = 0,
    DEVICE_MODE_GAME,
};

enum{
    INFO_POWER = 1,
    INFO_VERSION,
    INFO_BT_NAME,
    INFO_EQ,
    INFO_KEY,
    INFO_VOL,
    INFO_PLAY_STA,
    INFO_LATENCY_MODE,
    INFO_IN_EAR_EN,
    INFO_LANGUAGE,
    INFO_AUTO_ANSWER,
    INFO_ANC,
    INFO_TWS_SUPPORT,
    INFO_TWS_STA,
    INFO_LED,
    INFO_CRC,
    INFO_ANC_CUR_LEVEL,
    INFO_TRANSPARENCY_CUR_LEVEL,
    INFO_ANC_TOTAL_LEVEL,
    INFO_TRANSPARENCY_TOTAL_LEVEL,
    INFO_EQ_ALL_MODE,
    INFO_TWS_CHANNEL,
    INFO_DEV_COLOR,
    INFO_V3D_AUDIO,
    INFO_MULT_DEV,
    INFO_PAIRED_INFO,
    INFO_PAIRED_INFO_CUR,
    INFO_VOICE_RECOGNITION,
    INFO_ANC_FADE,
    INFO_DYNAMIC_BASS,
    INFO_DYNAMIC_BASS_LEVEL,
    INFO_DYNAMIC_BASS_RANGE,

    INFO_BT_STA = 0x23,
    INFO_PID,

#if SWETZ
    INFO_NI_CASE_STATE = 0x94,

#endif


    INFO_DEV_CAP = 0xFE,
    INFO_MTU = 0xFF,
};

enum{
    APP_LANG_EN = 0,
    APP_LANG_ZH,
};

enum{
    APP_ANC_STOP = 0,
    APP_ANC_START,
    APP_ANC_TRANSPARENCY,
};

enum{
    DEVICE_FIND_STOP = 0,
    DEVICE_FIND_START,
    DEVICE_FIND_START_L,
    DEVICE_FIND_STOP_L,
    DEVICE_FIND_START_R,
    DEVICE_FIND_STOP_R,
};

#if AB_MATE_CUSTOM_CMD_EN
enum{
    CUSTOM_CMD_TONE_CTRL = 1,   //提示音通知
    CUSTOM_CMD_MUSIC_CTRL,      //音乐控制/通知设置
    CUSTOM_CMD_HID_CTRL,        //hid控制
    CUSTOM_CMD_TIME_SYNC,       //时间同步
    CUSTOM_CMD_SIRI,            //语音助手
    CUSTOM_CMD_PHONE_CALL,      //拨号
};


enum 
{
    SWET_LANG_SET_TYPE = 1,
    SWET_AB_VOL,
};

enum{
    TONE_CTRL_COUNTDWON_PLAY = 0,
    TONE_CTRL_COUNTDWON_STOP,
};

enum{
    CMD_MUSIC_TITLE = 0,
    CMD_MUSIC_ARTIST,
    CMD_MUSIC_TOTAL_TIME,
    CMD_MUSIC_PLAY_TIME,
};

enum{
    CMD_HID_SLIDE_UP = 0,
    CMD_HID_SLIDE_DOWN,
    CMD_HID_SLIDE_LEFT,
    CMD_HID_SLIDE_RIGHT,
    CMD_HID_P,
    CMD_HID_PP,
    CMD_HID_TAKE_PIC,
};

enum{
    CMD_TIME_TIMEZONE = 0,
    CMD_TIME_TIMESTAMP_UTC_GMT,
    CMD_TIME_LOCALTIME,
};
#endif // AB_MATE_CUSTOM_CMD_EN

enum {
    RECORD_ONOFF_SET = 1,
};
enum{
    AUDIO_TYPE_MIC = 0,
    AUDIO_TYPE_SPEAKER,
};

typedef struct __attribute__((packed)){
    u8 band_cnt;
    u8 mode;
    s8 gain[AB_MATE_EQ_BAND_CNT];
}ab_mate_eq_info_t;

typedef struct __attribute__((packed)){
    u8 key_short;
    u8 key_double;
    u8 key_three;
    u8 key_long;
    u8 key_four;
    u8 key_five;
}ab_mate_key_info_t;

typedef struct __attribute__((packed)){
    u8 audio_type;        // 0: Mic       1 : Speaker
    u8 audio_source;      // 0: Right Ear 1: Left Ear
    u8 total_frame_nums;
    u8 frame_size;
    u8 data[];
}record_data_t;

typedef struct __attribute__((packed)){
    u32 seq     : 4;
    u32 reserve : 3;
    u32 encrypt : 1;
    u8 cmd;
    u8 cmd_type;
    u32 frame_seq : 4;
    u32 frame_total : 4;
    u8 payload_len;
}ab_mate_cmd_head_t;

typedef struct __attribute__((packed)){
    ab_mate_cmd_head_t cmd_head;
    u8 *payload;
}ab_mate_cmd_t;

typedef struct __attribute__((packed)){
    ab_mate_cmd_head_t cmd_head;
    u8 payload[128];
    u32 next_header_seq     : 4;
    u32 next_frame_seq      : 4;
    u8 recv_len;
    u16 total_len;
}ab_mate_cmd_recv_t;

typedef struct __attribute__((packed)){
    ab_mate_cmd_head_t cmd_head;
    u8 payload[250];
}ab_mate_cmd_send_t;

typedef struct __attribute__((packed)){
    u8 en;
    s8 level;
}ab_mate_dynamic_bass_t;

typedef struct __attribute__((packed)){
    u8 en;
    u8 app_dev_addr[6];     //用于保存APP端对应的手机地址
    u8 discon_addr[6];      //需要断开或者取消配对的手机蓝牙地址
}ab_mate_mult_dev_t;

typedef struct{
    ab_mate_eq_info_t eq_info;
    ab_mate_key_info_t local_key;
    ab_mate_key_info_t remote_key;
    u8 can_send_now;
    u8 play_sta;
    u8 poweroff_time;
    u8 latency_mode;
    u8 vol;
    u8 local_vbat;
    u8 remote_vbat;
    u8 box_vbat;
    u8 version[4];
    u8 con_sta;
    u8 rem_con_sta;
    u8 init_flag;
    u8 wakeup;
    u32 do_flag;
    u16 cm_flag;
    u32 flash_crc;
    u32 tick;
#if AB_MATE_ANC_EN
    u8 anc_mode;
    s8 anc_adjust_mic0;
    s8 anc_adjust_mic1;
    s8 tp_adjust_mic0;
    s8 tp_adjust_mic1;
    u8 anc_cur_level;
    u8 tp_cur_level;
    u8 anc_total_level;
    u8 tp_total_level;
    u8 anc_fade;
#endif
#if AB_MATE_BT_NAME_EN
    u8 bt_name[32];
#endif
    u8 device_find;
#if AB_MATE_DEVICE_FIND_EN
    u8 find_type;
#endif
#if AB_MATE_V3D_AUDIO_EN
    u8 v3d_audio_en;
#endif
    u8 update_param_flag;
#if AB_MATE_MULT_DEV_EN
    ab_mate_mult_dev_t mult_dev;
#endif
#if AB_MATE_CUSTOM_CMD_TONE_EN
    u8 custom_tone_type;
#endif
#if AB_MATE_DYNAMIC_BASS_EN
    ab_mate_dynamic_bass_t dynamic_bass;
#endif
#if AB_MATE_RECORD_EN
    u8 record_en;
#endif
}ab_mate_app_var_t;



extern ab_mate_app_var_t ab_mate_app;
extern ab_mate_cmd_send_t ab_mate_cmd_send;
extern ab_mate_cmd_recv_t ab_mate_cmd_recv;

void ab_mate_var_init(void);
void ab_mate_init_do(void);
bool ab_mate_receive_proc(u8 *data,u16 len, u8 con_type);
void ab_mate_poweroff_proc(void);
void ab_mate_tws_recv_proc(uint8_t *data_ptr, u16 size);
void ab_mate_process(void);
void ab_mate_cm_write(void *buf, u16 addr, u16 size, u8 sync);
void ab_mate_key_set_do(void);
u8 ab_mate_eq_set_do(void);
void ab_mate_mode_set_do(void);
void ab_mate_eq_custom_save(void);
void ab_mate_play_sta_notify(soft_timer_p timer);
bool ab_mate_system_need_wakeup(void);
void ab_mate_notify_do(u8 cmd,u8* payload,u8 len);
void ab_mate_data_send(u8* buf, u16 len);
void ab_mate_bt_evt_notice(uint evt, void *params);

void ab_mate_ble_app_init(void);
void ab_mate_ble_connect_callback(void);
void ab_mate_ble_disconnect_callback(void);
int ab_mate_ble_send_packet(u8 *buf, u8 len);
int ab_mate_latt_send_packet(u8 *buf, u8 len);
void ab_mate_spp_connect_callback(void);
void ab_mate_spp_disconnect_callback(void);
void ab_mate_disable_adv_for_spp_connect(void);
uint16_t ab_mate_ble_role_switch_get_data(uint8_t *data_ptr);
uint16_t ab_mate_ble_role_switch_set_data(uint8_t *data_ptr, uint16_t len);
void ab_mate_tp_level_set(u8 *payload, u8 payload_len);
void ab_mate_anc_level_set(u8 *payload, u8 payload_len);
void ab_mate_tp_level_set_do(u8 level, u8 sync);
void ab_mate_anc_level_set_do(u8 level, u8 sync);
void ab_mate_anc_fade_en_set_do(u8 en, u8 sync);
void ab_mate_enter_sleep(void);
void ab_mate_exit_sleep(void);
void ab_mate_disconnect_proc(u8 sync);
void ab_mate_connect_proc(u8 sync);
void ab_mate_device_find_side(void);
int ab_mate_connect_info_get(u8* buf);
int ab_mate_connect_info_set(u8* buf);
void ab_mate_mult_dev_en_set(u8 en, u8 sync);
void ab_mate_mult_dev_addr_decrypt(u8 *bt_addr);
void ab_mate_music_info_notify(u8 type, u16 data_offset, u8* payload, u8 len);
void ab_mate_time_info_notify(u8 type);
#if APP_USER_NOTIFY
void ab_mate_user_incase_sta_notify(void);
uint8_t user_check_incase_sta_pull(void);
#endif
#endif
#endif
