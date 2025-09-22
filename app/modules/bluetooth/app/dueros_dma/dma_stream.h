#ifndef _DMA_STREAM_H_
#define _DMA_STREAM_H_

#include "dma_wrapper.h"
#include "dma.pb-c.h"
#include "dma_utils.h"
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BAIDU_DATA_RAND_LEN
#define BAIDU_DATA_RAND_LEN    (8)
#endif

#define RECV_CMD_LEN_MAX       (256)
#define SEND_BUFFER_LEN_MAX    (512)
#define GET_DEVICE_INFO_MAX    (32)
#define AI_CMD_CODE_SIZE       (2)
#define DUEROS_VERSION_LEN_MAX (24)
#define DUEROS_DMA_INITED      (2)
#define DUEROS_DMA_LINKED      (0x10101)

#define DUEROS_DMA_VERISON_SIZE     (20)
#define DUEROS_DMA_HEADER_SIZE      (sizeof(DUEROS_DMA_HEADER))
#define DUEROS_DMA_AUDIO_STREAM_ID  (1)
#define DUEROS_DMA_CMD_STREAM_ID    (0)

#define DUEROS_DMA_CMD_INPUT_MUTEX_ID     (0)
#define DUEROS_DMA_CMD_OUTPUT_MUTEX_ID    (1)
#define DUEROS_DMA_DATA_MUTEX_ID          (2)
#define DUEROS_DMA_SYNC_DATA_MUTEX_ID     (3)

#define BLE_ADV_VEND_ID_LEN       (2)
#define BLE_ADV_DATA_MAX_LEN      (31)
#define BLE_SCAN_RSP_DATA_MAX_LEN (31)

#define DUEROS_DMA_CMD_INPUT_SIZE   (1024)
#define DUEROS_DMA_CMD_OUTPUT_SIZE  (1024)
#define DUEROS_DMA_DATA_SIZE        (2048)
#define DUEROS_DMA_TRIAD_TOKEN_LEN  (256)

#define DMA_INFO_MAGIC_NUMBER  (0x44554552)

#define DMA_OPER_UNINITED_RETURN_VOID(x)              \
    do{                                             \
        if (!g_dma_data.inited ||         \
                g_dma_operation_ptr->x == NULL ){   \
            return ;                                \
        }                                           \
    }while(0)


#define DMA_OPER_UNINITED_RETURN(x,err)                 \
    do{                                                 \
        if(!g_dma_data.inited ||              \
            g_dma_operation_ptr->x == NULL){            \
            return err;                                 \
        }                                               \
    }while(0)

#define DUEROS_StoreBE8(buff,num) ( ((buff)[0] = (uint8_t) ((num)) ) )

#define DUEROS_StoreBE16(buff,num) ( ((buff)[0] = (uint8_t) ((num)>>8)),    \
            ((buff)[1] = (uint8_t) (num)) )

#define DUEROS_StoreBE32(buff,num) ( ((buff)[0] = (uint8_t) ((num)>>24)),  \
                              ((buff)[1] = (uint8_t) ((num)>>16)),  \
                              ((buff)[2] = (uint8_t) ((num)>>8)),   \
                              ((buff)[3] = (uint8_t) (num)) )

#define DUEROS_OFFSET_OF(s,m) (size_t)&(((s *)0)->m)

typedef struct protocol_version {
    uint16_t magic;
    uint8_t  hVersion;
    uint8_t  lVersion;
    uint8_t  reserve[16];
} PROTOCOL_VER;

typedef struct dueros_dma_header {
    uint16_t length: 1;
    uint16_t reserve: 6;
    uint16_t streamID: 5;
    uint16_t version: 4;
} DUEROS_DMA_HEADER;

typedef struct {
    MediaControl control;
    uint32_t volume;
} MEDIA_CONTROL;

typedef enum {
    DMA_NVREC_SET_RAND = 0x100,
    DMA_NVREC_GET_RAND,
    DMA_NVREC_SET_PEER_SN,
    DMA_NVREC_GET_PEER_SN,
    DMA_NVREC_SET_PULLUP_FLAG,
    DMA_NVREC_GET_PULLUP_FLAG,
    DMA_NVREC_SET_PULLUP_OFF_STATE,
    DMA_NVREC_GET_PULLUP_OFF_STATE,
    DMA_NVREC_SET_WAKEUP_ENABLE,
    DMA_NVREC_GET_WAKEUP_ENABLE,
    DMA_NVREC_SET_UPGRADE_FLAG,
    DMA_NVREC_GET_UPGRADE_FLAG,
    DMA_NVREC_SET_LEFT_CUSTOM_TAP_SETTING,
    DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING,
    DMA_NVREC_SET_RIGHT_CUSTOM_TAP_SETTING,
    DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING,
    DMA_NVREC_SET_LEFT_CUSTOM_TAP_SETTING_NEW_STYLE,
    DMA_NVREC_GET_LEFT_CUSTOM_TAP_SETTING_NEW_STYLE,
    DMA_NVREC_SET_RIGHT_CUSTOM_TAP_SETTING_NEW_STYLE,
    DMA_NVREC_GET_RIGHT_CUSTOM_TAP_SETTING_NEW_STYLE,
    DMA_NVREC_SET_DEBUG_INFO_TIME,
    DMA_NVREC_GET_DEBUG_INFO_TIME,
    DMA_NVREC_SET_WEARING_DETECT_ENABLE,
    DMA_NVREC_GET_WEARING_DETECT_ENABLE,
    DMA_NVREC_SET_DMA_CONNECT_INFO,
    DMA_NVREC_GET_DMA_CONNECT_INFO,
    DMA_NVREC_SET_BOX_BATTERY_LEVEL,
    DMA_NVREC_GET_BOX_BATTERY_LEVEL,
    DMA_NVREC_ADD_DEBUG_INFO,
    DMA_NVREC_SHOW_DEBUG_INFO,


    DMA_NVREC_SET_ONE_EAR_ANC_MODE,
    DMA_NVREC_GET_ONE_EAR_ANC_MODE,
    DMA_NVREC_SET_TWO_EAR_ANC_MODE,
    DMA_NVREC_GET_TWO_EAR_ANC_MODE,
    DMA_NVREC_SET_TWO_EAR_ANC_CYCLE_LIST,
    DMA_NVREC_GET_TWO_EAR_ANC_CYCLE_LIST,
    DMA_NVREC_SET_HEALTH_REMINDER_ENABLE,
    DMA_NVREC_GET_HEALTH_REMINDER_ENABLE,
    DMA_NVREC_SET_GAME_MODE_ENABLE,
    DMA_NVREC_GET_GAME_MODE_ENABLE,
    DMA_NVREC_SET_WIND_NOISE_DETECTION_ENABLE,
    DMA_NVREC_GET_WIND_NOISE_DETECTION_ENABLE,
    DMA_NVREC_SET_EQ_PARAM,
    DMA_NVREC_GET_EQ_PARAM,
    DMA_NVREC_SET_CUSTOM_EQ_ENABLE,
    DMA_NVREC_GET_CUSTOM_EQ_ENABLE,
    DMA_NVREC_ADD_USER_ACTION_INFO,
    DMA_NVREC_UPDATE_USER_ACTION_INFO,
    DMA_NVREC_CLEAR_USER_ACTION_INFO,
    DMA_NVREC_GET_USER_ACTION_INFO,
    DMA_NVREC_SET_HEARING_PROTECTION_ENABLE,
    DMA_NVREC_GET_HEARING_PROTECTION_ENABLE,
} DMA_NVREC_CMD;

typedef enum {
    DMA_SYNC_LEFT_TAP_SETTING = 0,
    DMA_SYNC_RIGHT_TAP_SETTING,
    DMA_SYNC_LEFT_WEARING_ON,
    DMA_SYNC_RIGHT_WEARING_ON,
    DMA_SYNC_LEFT_BATTERY_LEVEL,
    DMA_SYNC_RIGHT_BATTERY_LEVEL,
    DMA_SYNC_PULLUP_ENABLE,
    DMA_SYNC_WAKEUP_ENABLE,
    DMA_SYNC_OOBE_REPORT_ENABLE,
    DMA_SYNC_DMA_CONNECT_STATE,
    DMA_SYNC_UPSTREAM_CONFIG,
    DMA_SYNC_DEVICE_INFO,

    DMA_SYNC_LEFT_TAP_SETTING_NEW_STYLE,
    DMA_SYNC_RIGHT_TAP_SETTING_NEW_STYLE,
    DMA_SYNC_TWO_EAR_ANC_CYCLE_LIST,
    
} DMA_SYNC_PEER_CMD;

typedef enum {
    CUSTOM_TAP_TYPE_INVALID = 0,
    CUSTOM_TAP_TYPE_PULLUP,
    CUSTOM_TAP_TYPE_BACKWARD,
    CUSTOM_TAP_TYPE_FORWARD,
    CUSTOM_TAP_TYPE_PLAY,
} CUSTOM_TAP_TYPE;

typedef enum {
    DMA_CUSTOM_CMD_START = DMA_CUSTOM_CMD,
} DMA_CUSTOM_OPERATION_CMD;

typedef enum {
    DEBUG_INFO_TYPE_A2DP_MUTE_COUNT = 0,
    DEBUG_INFO_TYPE_A2DP_BLOCK_COUNT,
    DEBUG_INFO_TYPE_A2DP_TTS_DROP_COUNT,
    DEBUG_INFO_TYPE_A2DP_CPU_OVERLOAD_COUNT,
    DEBUG_INFO_TYPE_HFP_SCO_DISCONNECT_COUNT,
    DEBUG_INFO_TYPE_HFP_MUTE_COUNT,
    DEBUG_INFO_TYPE_HFP_CPU_OVERLOAD_COUNT,
    DEBUG_INFO_TYPE_VOICE_WAKEUP_COUNT,
    DEBUG_INFO_TYPE_DOUBLE_CLICK_WAKEUP_COUNT,
    DEBUG_INFO_TYPE_WAKEUP_DMA_CONNECTED_COUNT,
    DEBUG_INFO_TYPE_WAKEUP_DMA_DISCONNECTED_COUNT,
    DEBUG_INFO_TYPE_START_SPEECH_COUNT,
    DEBUG_INFO_TYPE_START_SPEECH_ACK_COUNT,
    DEBUG_INFO_TYPE_PROVIDE_START_SPEECH_COUNT,
    DEBUG_INFO_TYPE_WAKEUP_TTS_SUCCESSED_COUNT,
    DEBUG_INFO_TYPE_WAKEUP_TTS_FAILED_COUNT,
    DEBUG_INFO_TYPE_LAUNCH_TTS_SUCCESSED_COUNT,
    DEBUG_INFO_TYPE_LAUNCH_TTS_FAILED_COUNT,

    DEBUG_INFO_TYPE_NUMBER,
} DEBUG_INFO_TYPE;

typedef enum {
    USER_ACTION_INFO_TYPE_WORKING_TIME,
    USER_ACTION_INFO_TYPE_OUTSIDE_TIME,
    USER_ACTION_INFO_TYPE_WEARING_ON_TIME,
    USER_ACTION_INFO_TYPE_PLAYING_MUSIC_TIME,
    USER_ACTION_INFO_TYPE_TALKING_TIME,
    USER_ACTION_INFO_TYPE_DMA_CONNECTED_TIME,

    USER_ACTION_INFO_TYPE_NUMBER,
} USER_ACTION_INFO_TYPE;

typedef enum {
    USER_ACTION_INFO_OP_UPDATE_START_TIME,  // 不累加时间，只更新last tick
    USER_ACTION_INFO_OP_ADD,                // 时间累加，更新last tick

    USER_ACTION_INFO_OP_NUMBER,
} USER_ACTION_INFO_OP;

typedef enum {
    CUSTOM_TAP_TYPE_NEW_STYLE_INVALID = 0,
    CUSTOM_TAP_TYPE_NEW_STYLE_PULLUP,
    CUSTOM_TAP_TYPE_NEW_STYLE_ANC_CTRL,
    CUSTOM_TAP_TYPE_NEW_STYLE_NUM,
} DMA_CUSTOM_TAP_TYPE_NEW_STYLE;

typedef enum {
    DMA_ANC_CLOSE = 0,
    DMA_ANC_CLEAR = 1,
    DMA_ANC_OPEN = 2,
    DMA_ANC_MAX,
    DMA_ANC_INVALID = DMA_ANC_MAX,
} DMA_NOISE_REDUCTION_MODE;

typedef struct {
    uint8_t bdAddr[6];
    int32_t dma_connect_retry_times;
    bool dma_connected_before;
    bool dma_tts_report_onoff;
    bool enable_pullup;
    bool enable_pullup_timeout_off;
} MAC_ADDR_TO_CONNECT_TIMES;

typedef struct {
    uint32_t pairedDevNum;
    MAC_ADDR_TO_CONNECT_TIMES mac_addr_to_times[MAX_BT_PAIRED_DEVICE_COUNT];
} NV_DMA_CONNECTED_INFO_T;

#define CONFIGURATION_MAGIC_NUMBER (0x01010103)

typedef struct {
    uint64_t last_mobile_time;                // APP端下发的start time时间
    uint32_t run_time;                        // 距离上次get_log_confirm累积运行的时间(ms)
    uint16_t a2dp_mute_count;                 // 播放卡顿次数
    uint16_t a2dp_block_count;                // 上行阻塞次数
    uint16_t a2dp_tts_drop_count;             // tts吞字次数(暂时无法统计)
    uint16_t a2dp_cpu_overload_count;         // A2DP CPU过载次数
    uint16_t hfp_sco_disconnect_count;        // SCO链路断开次数
    uint16_t htp_mute_count;                  // HFP卡顿次数
    uint16_t hfp_cpu_overload_count;          // HFP CPU过载次数
    uint16_t voice_wakeup_count;              // 语音唤醒次数
    uint16_t double_click_wakeup_count;       // 双击唤醒次数
    uint16_t wakeup_dma_connnected_count;     // 唤醒时dma处于连接次数
    uint16_t wakeup_dma_disconnnected_count;  // 唤醒时dma处于断连次数
    uint16_t start_speech_count;              // 发起StartSpeech次数
    uint16_t start_speech_ack_count;          // 收到StartSpeechACK次数
    uint16_t provide_start_speech_count;      // 收到ProvideSpeech发起StartSpeech次数
    uint16_t wakeup_tts_succeeded_count;      // 播放唤醒在呢TTS成功次数
    uint16_t wakeup_tts_failed_count;         // 播放唤醒在呢TTS失败次数
    uint16_t launch_tts_succeeded_count;      // 播放正在连接小度APP TTS成功次数
    uint16_t launch_tts_failed_count;         // 播放正在连接小度APP TTS失败次数
} DMA_DEBUG_INFO;

typedef struct {
    uint32_t working_time;                    // 开机时长
    uint32_t outside_time;                    // 出仓时长
    uint32_t wearing_on_time;                 // 入耳时长
    uint32_t playing_music_time;              // 播放音乐时长
    uint32_t talking_time;                    // 通话时长
    uint32_t dma_connected_time;              // 连接DMA时长
} DMA_USER_ACTION_INFO;

typedef struct {
    int32_t magic_number;

    char rand[BAIDU_DATA_RAND_LEN + 1];
    uint8_t left_tap_setting;
    uint8_t right_tap_setting;

    bool master_sn_inited;
    uint8_t peer_sn[DEVICE_SN_LEN + 1];
    bool wakeup_enable;
    uint8_t upgrade_succeed;
    bool enable_wearing_detect;
    uint8_t box_battery_level;

    uint8_t left_tap_setting_new_style;
    uint8_t right_tap_setting_new_style;

    DMA_DEBUG_INFO debug_info;
    DMA_USER_ACTION_INFO user_action_info;

    bool clear_debug_info_ready;
    uint8_t two_ear_noise_reduction_mode;
    uint8_t one_ear_noise_reduction_mode;
    uint8_t anc_cycle_list;
    bool health_reminder_enable;
    bool game_mode_enable;
    bool wind_noise_detection_enable;
    bool custom_eq_enable;
    bool hearing_protection_enable;
} NV_DMA_CONFIGURATION_T;

typedef struct {
    NV_DMA_CONFIGURATION_T  dma_config;
    uint8_t dma_config_reserved[256 - sizeof(NV_DMA_CONFIGURATION_T)];
    NV_DMA_CONNECTED_INFO_T dma_connected_info;
    uint8_t dma_connected_info_reserved[256 - sizeof(NV_DMA_CONNECTED_INFO_T)];
} NV_DMA_USER_DATA;

typedef struct {
    uint32_t magic_number;

    bool update_left_tap_setting;
    uint8_t left_tap_setting;

    bool update_right_tap_setting;
    uint8_t right_tap_setting;

    bool update_left_wearing_on;
    bool left_wearing_on;

    bool update_right_wearing_on;
    bool right_wearing_on;

    bool update_left_battery_level;
    uint8_t left_battery_level;

    bool update_right_battery_level;
    uint8_t right_battery_level;

    bool update_pullup_enable;
    bool pullup_enable;

    bool update_wakeup_enable;
    bool wakeup_enable;

    bool update_oobe_report_enable;
    MAC_ADDR_TO_CONNECT_TIMES on_link_addr_hash;

    bool update_dma_connect_state;
    bool dma_linked;
    bool dma_started;

    bool update_upstream_config;
    uint8_t audio_format;
    bool upstream_started;

    bool update_device_info;
    uint8_t sn[DEVICE_SN_LEN + 1];
    uint8_t version[DUEROS_VERSION_LEN_MAX + 1];
    uint8_t upgrade_flag;
    uint8_t dma_rand[BAIDU_DATA_RAND_LEN + 1];

    bool update_left_tap_setting_new_style;
    uint8_t left_tap_setting_new_style;

    bool update_right_tap_setting_new_style;
    uint8_t right_tap_setting_new_style;

    bool update_anc_cycle_list;
    uint8_t anc_cycle_list;
} DMA_SHARE_INFO;

typedef struct {
    volatile uint32_t inited;
    volatile uint32_t dialog;
    volatile uint32_t audio_format;

    uint8_t send_buffer[SEND_BUFFER_LEN_MAX];

    volatile bool dma_linked;
    volatile bool dma_started;

    bool mobile_connected;
    bool use_dual_record;
    bool wakeup_enable;
    bool pullup_enable;
    bool is_left_side;
    bool left_wearing_on;
    bool right_wearing_on;
    bool enable_wearing_detect;
    bool audio_play;
    bool upstream_started;
    bool keyword_detected;
    bool silent_ota;
    bool left_version_sent;
    bool right_version_sent;
    bool wait_start_speech_ack;
    bool wait_for_dma_connect;
    bool use_ibeacon;
    bool is_tws_device;
    bool recv_peer_dma_version;
    bool recv_peer_battery_level;
    bool tws_connected;
    bool use_new_style_tap_setting;
    uint8_t left_tap_setting;
    uint8_t right_tap_setting;
    uint8_t left_battery_level;
    uint8_t right_battery_level;

    DMA_VOICE_STREAM_CODEC_TYPE codec_type;

    uint32_t trans_size;
    uint32_t stream_packet_index;
    PROTOCOL_VER protocol_version;

    uint8_t* client_id;
    uint8_t* client_secret;
    uint8_t* vendor_id;
    uint8_t device_type;
    uint8_t device_version;
    uint8_t ble_adv_data_len;
    uint8_t ble_adv_data[BLE_ADV_DATA_MAX_LEN];
    uint8_t ble_scan_rsp_len;
    uint8_t ble_scan_rsp[BLE_SCAN_RSP_DATA_MAX_LEN];
    uint8_t software_version[DUEROS_VERSION_LEN_MAX + 1];
    uint8_t peer_version[DUEROS_VERSION_LEN_MAX + 1];
    uint8_t current_sn[DEVICE_SN_LEN + 1];
    uint8_t peer_sn[DEVICE_SN_LEN + 1];
    uint8_t current_mac[6];
    uint8_t master_mac[6];
    uint8_t rand[BAIDU_DATA_RAND_LEN + 1];
    uint8_t serial_number[DEVICE_SN_LEN + 1];
    uint8_t recv_cmd_data[RECV_CMD_LEN_MAX];
    uint8_t get_device_info[GET_DEVICE_INFO_MAX];
    uint8_t triad_token[DUEROS_DMA_TRIAD_TOKEN_LEN];
    char triad_id[32];
    char triad_secret[32];
    NV_DMA_USER_DATA dma_userdata_config;
    SpeechState speech_state;
    SignMethod  signMode;

    MAC_ADDR_TO_CONNECT_TIMES on_link_addr_hash;
    uint8_t on_link_hash_id;
    bool dma_connect_need_report;

    uint8_t left_tap_setting_new_style;
    uint8_t right_tap_setting_new_style;
    uint8_t anc_cycle_list;

    DMA_QUEUE cmd_queue_input;
    DMA_QUEUE cmd_queue_output;
    DMA_QUEUE data_queue;

    DMA_SHARE_INFO share_info_send;
    DMA_SHARE_INFO share_info_recv;
    DUER_DMA_CAPABILITY dma_capability;
    ControlEnvelope envelope;
} DUEROS_DMA;

bool dma_get_all_config(void);

bool dma_get_config(DMA_NVREC_CMD cmd, uint8_t* param, uint8_t len);

bool dma_set_config(DMA_NVREC_CMD cmd, const uint8_t* param, uint8_t len);

void dma_construct_ble_advertise_data(void);

void dma_start_ble_adv(bool use_ibeacon);

void dma_reset_all_queue(void);

void dma_voice_connected(void);

void dma_voice_disconnected(void);

void dma_mobile_connected(void);

void dma_mobile_disconnected(void);

bool dma_sent_version(void);

void dma_get_setting(SpeechSettings* setting);

void dma_get_dev_config(DeviceConfiguration* device_config);

void dma_set_state(State* p_state);

void dma_get_state(uint32_t profile, State* p_state);

void dma_sync_state(bool send_online_state);

void dma_send_sync_state(uint32_t feature);

void dma_paired_ack(char* request_id, bool sync);

bool dma_check_summary(ControlEnvelope* envelop);

void dma_send_get_state_check_sum_fail(char* request_id, bool sync);

void dma_send_provide_speech_ack(uint32_t dialog_id, char* request_id, bool sync);

void  dma_sent_get_device_configuration_ack(char* request_id, bool sync);

bool dma_send_get_state_ack(uint32_t feature, char* request_id, bool sync);

void dma_send_set_state_ack(SetState* set_state, char* request_id, bool sync);

void dma_send_ack_common(char* requeset_id, Command command, bool sync);

bool dma_control_stream_send(const ControlEnvelope* message, uint32_t length, bool sync);

bool dma_sync_peer_state(uint32_t cmd);

bool dma_save_peer_data(void);

bool dma_process_data_send(void);

bool dma_process_cmd_send(void);

uint32_t dma_process_cmd_recv(void);

#ifdef __cplusplus
}
#endif
#endif

