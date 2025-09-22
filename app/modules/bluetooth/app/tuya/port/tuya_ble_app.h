#ifndef __TUYA_BLE_APP_H
#define __TUYA_BLE_APP_H

#include "tuya_ble_type.h"

enum{
    TY_ANC_START = 0,
    TY_ANC_TRANSPARENCY,
    TY_ANC_STOP,
};

enum{
    TY_BT_STA_DISCONNECT = 0,
    TY_BT_STA_CONNECTING,
    TY_BT_STA_CONNECTED,
};

//按键功能，需和涂鸦平台定义的按键功能对应,如果这里的值有修改，需要同步修改key_local_table的值
enum{
    TUYA_KEY_VOL_DOWN = 0,
    TUYA_KEY_VOL_UP,
    TUYA_KEY_NEXT,
    TUYA_KEY_PREV,
    TUYA_KEY_PLAY_PAUSE,
    TUYA_KEY_SIRI,
    TUYA_KEY_LOW_LATENCY,
    TUYA_KEY_ANC,
    TUYA_KEY_NONE,
};

#define TUYA_ADV_INTERVAL           96     //adv: x * 0.625ms
#define TUYA_ADV_SLEEP_INTERVAL     640
#define TUYA_CON_INTERVAL           80     //con: x * 1.25ms
#define TUYA_CON_SLEEP_INTERVAL     320

#define TUYA_FLAG_EQ_SET             BIT(0)

//dp id,需要根据涂鸦平台实际产品的DP ID定义,平台没有定义的功能请置0
#define DP_ID_WARNING_MODE      0
#define DP_ID_VBAT_HOUSE        0
#define DP_ID_VOL               5
#define DP_ID_CTL               6
#define DP_ID_PP                7
#define DP_ID_AWAY_ALARM        0
#define DP_ID_RECONNECT         0
#define DP_ID_DISCONNCET        0
#define DP_ID_DEVICE_FIND       12
#if ANC_EN
#define DP_ID_ANC_MODE          8
#else
#define DP_ID_ANC_MODE          0
#endif
#define DP_ID_VBAT_RIGHT        4
#define DP_ID_VBAT_LEFT         3
#define DP_ID_POWER_DOWN        0
#if EQ_MODE_EN
#define DP_ID_EQ_MODE           0
#define DP_ID_EQ_DATA           18
#define DP_ID_EQ_EN             44
#else
#define DP_ID_EQ_MODE           0
#define DP_ID_EQ_DATA           0
#define DP_ID_EQ_EN             0
#endif
#define DP_ID_DISCONNECT_ALARM  0
#define DP_ID_LOW_LATENCY       0
#define DP_ID_BT_NAME           43
#define DP_ID_BT_STA            33
#define DP_ID_BT_RST            42
#define DP_ID_KEY_LEFT_SHORT    0
#define DP_ID_KEY_RIGHT_SHORT   0
#define DP_ID_KEY_LEFT_DOUBLE   19
#define DP_ID_KEY_RIGHT_DOUBLE  20
#define DP_ID_KEY_LEFT_THREE    21
#define DP_ID_KEY_RIGHT_THREE   22
#define DP_ID_KEY_LEFT_LONG     23
#define DP_ID_KEY_RIGHT_LONG    24
#define DP_ID_KEY_CTL_EN        (DP_ID_KEY_LEFT_SHORT || DP_ID_KEY_LEFT_DOUBLE || DP_ID_KEY_LEFT_THREE || DP_ID_KEY_LEFT_LONG)
#define DP_ID_KEY_SHORT_EN      0
#define DP_ID_KEY_RST           25
#define DP_ID_DEVICE_RESET      0


/*产品授权信息，通过配置文件进行修改，请打开xcfg.xm里面的涂鸦配置，实际生产可通过downloader的生产配置功能将授权信息从excel表格读取进行写入*/
#define TUYA_AUTH_INFO_SYNC     1                                   //TWS同步授权信息使能
#define TUYA_DEVICE_PID         xcfg_cb.tuya_pid                    //8byte,PID
#define TUYA_DEVICE_MAC         xcfg_cb.le_mac                      //12byte,mac
#define TUYA_DEVICE_UUID        xcfg_cb.tuya_device_uuid            //16Byte,uuid
#define TUYA_DEVICE_AUTH_KEY    xcfg_cb.tuya_auth_key               //32Byte,key
/********************************************************************************/

//Version Config
#define TUYA_DEVICE_FIR_NAME    "tuya_ble_bluetrum_bt892x"          //固件标识名
#define TUYA_DEVICE_FVER_NUM    0x0100                              //固件版本
#define TUYA_DEVICE_HVER_NUM    0x0100                              //硬件版本
#define TUYA_DEVICE_FVER_STR    "1.0"                               //固件版本str
#define TUYA_DEVICE_HVER_STR    "1.0"                               //硬件版本str
#define TUYA_DEVICE_FIR_KEY     "bluetrum_bt892x_key"               //固件key

//Const
#define TUYA_CM_TAG             0xa1b2
#define TUYA_AUTH_FLAG          0x5566
#define TUYA_AUTH_INFO_ADDR     (TUYA_NV_START_ADDR - 0x1000)

//Eq Config
#define TUYA_EQ_BAND_CNT        10

//Key Config
#define TUYA_KEY_SHORT              xcfg_cb.user_def_ks_sel
#define TUYA_KEY_DOUBLE             xcfg_cb.user_def_kd_sel
#define TUYA_KEY_THREE              xcfg_cb.user_def_kt_sel
#define TUYA_KEY_LONG               xcfg_cb.user_def_kl_sel

#define TUYA_KEY_SHORT_LEFT_DEF     TUYA_KEY_PLAY_PAUSE         //按键默认功能，需要根据实际UI定义进行修改
#define TUYA_KEY_SHORT_RIGHT_DEF    TUYA_KEY_PLAY_PAUSE
#define TUYA_KEY_DOUBLE_LEFT_DEF    TUYA_KEY_NEXT
#define TUYA_KEY_DOUBLE_RIGHT_DEF   TUYA_KEY_PREV
#define TUYA_KEY_THREE_LEFT_DEF     TUYA_KEY_VOL_DOWN
#define TUYA_KEY_THREE_RIGHT_DEF    TUYA_KEY_VOL_UP
#define TUYA_KEY_LONG_LEFT_DEF      TUYA_KEY_SIRI
#define TUYA_KEY_LONG_RIGHT_DEF     TUYA_KEY_SIRI

//Cm Addr Config
#define TUYA_CM_EQ_EN                   0x00                                        //1byte
#define TUYA_CM_EQ_DATA                 (TUYA_CM_EQ_EN + 1)                         //11byte,mode(1B)+gain(10B)
#define TUYA_CM_FOT_FILE_LEN            (TUYA_CM_EQ_DATA + 1 + TUYA_EQ_BAND_CNT)    //4byte
#define TUYA_CM_FOT_FILE_CRC            (TUYA_CM_FOT_FILE_LEN + 4)                  //4byte
#define TUYA_CM_LOW_LATENCY             (TUYA_CM_FOT_FILE_CRC + 4)                  //1byte
#define TUYA_CM_BT_NAME_LEN             (TUYA_CM_LOW_LATENCY + 1)                   //1byte
#define TUYA_CM_BT_NAME                 (TUYA_CM_BT_NAME_LEN + 1)                   //32byte
#define TUYA_CM_KEY_LOCAL               (TUYA_CM_BT_NAME + 32)                      //sizeof(tuya_ble_key_info_t)
#define TUYA_CM_KEY_REMOTE              (TUYA_CM_KEY_LOCAL + sizeof(tuya_ble_key_info_t))   //sizeof(tuya_ble_key_info_t)
#define TUYA_CM_KEY_EN                  (TUYA_CM_KEY_REMOTE + sizeof(tuya_ble_key_info_t))  //1byte
#define TUYA_CM_FLAG                    (TUYA_CM_KEY_EN + 1)                        //2byte
#define TUYA_CM_MAX                     160
//与恢复出厂设置无关的放TUYA_CM_MAX下面
#define TUYA_CM_LINK_ADDR               (160)                                       //6byte
/*****************************************************************************************************************************************/

//其他功能配置
#define TUYA_UNBOND_LINK_INFO_CLEAR     1   //解绑后清除经典蓝牙配对信息
#define TUYA_BLE_OTA_EN                 0   //是否打开涂鸦OTA功能
#define TUYA_BLE_LATENCY_USE_CM         0   //0：默认  1：使用上次保存的值
#define TUYA_BLE_ANC_USE_CM             0   //0：默认  1：使用上次保存的值

//event id
typedef enum
{
    CUSTOM_EVT_TWS_SYNC,
} custom_evt_id_t;

/*********************************************************************
 * STRUCT
 */

//dp point
typedef struct __attribute__((packed)){
    uint8_t dp_id;
    uint8_t dp_type;
    uint16_t dp_data_len;
    union
    {
        uint8_t dp_data[250];
    };
}tuya_ble_dp_data_t;

typedef struct
{
    uint32_t len;
    uint8_t  value[];
}tuya_ble_custom_evt_data_t;


typedef struct __attribute__((packed)){
    u8 key_short;
    u8 key_double;
    u8 key_three;
    u8 key_long;
}tuya_ble_key_info_t;


typedef struct __attribute__((packed)){
    uint8_t eq_en;
    uint8_t eq_mode;    //eq_mode和eq_data顺序不能改
    int8_t eq_data[TUYA_EQ_BAND_CNT];

#if DP_ID_ANC_MODE
    uint8_t anc_mode;
#endif

    uint8_t vol;
    uint8_t hfp_vol;
    volatile uint8_t incall_flag;

    volatile uint8_t play_sta;
    volatile uint16_t play_sta_check_time;

    uint8_t low_latency;

#if DP_ID_POWER_DOWN
    uint32_t powerdown_cnt;
#endif

    uint8_t remote_vbat;

    uint32_t dp_send_sn;

#if DP_ID_KEY_CTL_EN
    tuya_ble_key_info_t local_key;
    tuya_ble_key_info_t remote_key;
#endif

    char bt_name[32];

    uint8_t dev_find;

    uint8_t data_receive_flag;
    uint8_t remote_bond_flag;
    uint8_t bond_flag;
    uint16_t cm_flag;
    uint8_t init_flag;
    uint32_t do_flag;
    bool unbond_flg;
}tuya_ble_app_data_t;


/*********************************************************************
 * EXTERNAL VARIABLE
 */
extern tuya_ble_app_data_t tuya_ble_app;

/*********************************************************************
 * EXTERNAL FUNCTION
 */
void tuya_ble_var_init(void);
void tuya_ble_app_init(void);
void tuya_ble_custom_evt_send(custom_evt_id_t evtid);
void tuya_ble_custom_evt_send_with_data(custom_evt_id_t evtid, void* buf, uint32_t size);
void tuya_ble_addr_get(u8 *addr);
void tuya_ble_process(void);
uint32_t tuya_ble_app_ota_disconn_handler(void);
void tuya_ble_cm_write(void *buf, u16 addr, u16 size, u8 sync);
void tuya_ble_cm_read(void *buf, u16 addr, u16 size);
void tuya_ble_anc_mode_report(void);
void tuya_ble_vbat_report_do(void);
void tuya_ble_low_latency_report(u8 val);
u8 tuya_ble_eq_set(void);
void tuya_ble_powerdown_process(void);
void tuya_ble_tws_info_all_sync(void);
u8 tuya_ble_data_receive_flag_get(void);
void tuya_ble_adv_change(void);
tuya_ble_status_t tuya_ble_common_uart_receive_data(uint8_t *p_data,uint16_t len);
void tuya_ble_disconnected_handler(void);
void tuya_ble_connected_handler(void);
void tuya_ble_cm_anc_save(void);
void tuya_ble_cm_low_latency_save(void);
void tuya_ble_device_reset_2_factory(void);
void tuya_bt_link_addr_save(u8 *addr);
void tuya_bt_link_addr_load(u8 *addr);
void tuya_bt_disconnect_callback(u8 *buf);
void tuya_bt_connect_callback(u8 *buf);
void tuya_tws_connect_callback(void);
void tuya_tws_disconnect_callback(void);
void tuya_ble_app_play_sta_report(tuya_ble_timer_t timer);
void tuya_ble_key_set_do(void);
void tuya_bt_reset_do(void);
void tuya_ble_key_reset_2_factory(void);
void tuya_bt_evt_notice(uint evt, void *params);
void tuya_ble_sleep_mode_process(void);
uint16_t tuya_ble_role_switch_get_data(uint8_t *data_ptr);
uint16_t tuya_ble_role_switch_set_data(uint8_t *data_ptr, uint16_t len);
void tuya_ble_enter_sleep(void);
void tuya_ble_exit_sleep(void);

#endif
