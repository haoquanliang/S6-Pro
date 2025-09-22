#ifndef _DMA_WRAPPER_H_
#define _DMA_WRAPPER_H_

// #include <stdio.h>
// #include <stddef.h>
// #include <stdbool.h>
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

// #define DMA_BLE_UUID_SERVICE "0000FDC2-0000-1000-8000-00805F9B34FB"
// #define DMA_BLE_UUID_TX_CHARACTERISTIC  "B84AC9C6-29C5-46D4-BBA1-9D534784330F"
// #define DMA_BLE_UUID_RX_CHARACTERISTIC "C2E758B9-0E78-41E0-B0CB-98A593193FC5"
// #define DMA_BLE_UUID16_SERVICE "FDC2"
// #define DMA_RFCOMM_UUID "51DBA109-5BA9-4981-96B7-6AFE132093DE"

#define DMA_BLE_UUID_SERVICE             0x00, 0x00, 0xFD, 0xC2, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
#define DMA_BLE_UUID_TX_CHARACTERISTIC   0x0F, 0x33, 0x84, 0x47, 0x53, 0x9D, 0xA1, 0xBB, 0xD4, 0x46, 0xC5, 0x29, 0xC6, 0xC9, 0x4A, 0xB8
#define DMA_BLE_UUID_RX_CHARACTERISTIC   0xC5, 0x3F, 0x19, 0x93, 0xA5, 0x98, 0xCB, 0xB0, 0xE0, 0x41, 0x78, 0x0E, 0xB9, 0x58, 0xE7, 0xC2
#define DMA_BLE_UUID16_SERVICE           0xC2, 0xFD
#define DMA_RFCOMM_UUID                  0x51, 0xDB, 0xA1, 0x09, 0x5B, 0xA9, 0x49, 0x81, 0x96, 0xB7, 0x6A, 0xFE, 0x13, 0x20, 0x93, 0xDE

extern void dma_assert(int cond, ...);

#define DMA_TRACE  a_printf

#define DMA_ASSERT dma_assert

#ifndef BAIDU_DATA_RAND_LEN
#define BAIDU_DATA_RAND_LEN    (8)
#endif

#ifndef DEVICE_SN_LEN
#define DEVICE_SN_LEN   (20)
#endif

#ifndef MAX_BT_PAIRED_DEVICE_COUNT
#define MAX_BT_PAIRED_DEVICE_COUNT  (8)
#endif

#define DMA_USER_DATA_CONFIG_LEN    (512)

#define DMA_MALLOC_MEMORY_POOL_SIZE      (4 * 1024)
#define DMA_QUEUE_MEMORY_POOL_SIZE       (4 * 1024)

#define USE_TRIAD_ID

/**
* @brief 定义TWS类设备左右类型
*/
typedef enum {
    DMA_SIDE_UNKNOWN = 0,  /**< 未知状态 */
    DMA_SIDE_LEFT,         /**< 左侧设备 */
    DMA_SIDE_RIGHT,        /**< 右侧设备 */
} DMA_TWS_SIDE_TYPE;

/**
* @brief 定义TWS类主从类型
*/
typedef enum {
    DMA_TWS_UNKNOWN = 0,  /**< 未知状态 */
    DMA_TWS_MASTER,       /**< 主设备 */
    DMA_TWS_SLAVE,        /**< 从设备 */
} DMA_TWS_ROLE_TYPE;

/**
* @brief 定义盒仓状态
*/
typedef enum {
    DMA_BOX_STATE_UNKNOWN = 0,  /**< 未知状态 */
    DMA_BOX_STATE_OPEN,         /**< 开仓状态 */
    DMA_BOX_STATE_CLOSE,        /**< 关仓状态 */
} DMA_BOX_STATE_TYPE;

/**
* @brief BT MAC地址类型
*/
typedef enum {
    DMA_BT_ADDR_DEFAULT = 0,  /**< 出厂烧录的原始MAC地址 */
    DMA_BT_ADDR_IN_USED,      /**< 当前用户可见的MAC地址 */
} DMA_BT_ADDR_TYPE;

/**
* @brief BT MAC地址类型
*/
typedef enum {
    DMA_SN_DEFAULT = 0,   /**< 出厂烧录的原始SN号 */
    DMA_SN_IN_USED,       /**< 当前用户可见的SN号 */
} DMA_SN_TYPE;

/**
* @brief 控制设备MIC及编码状态
*/
typedef enum {
    DMA_VOICE_STREAM_STOP = 0,    /**< 关闭MIC及编码器 */
    DMA_VOICE_STREAM_START,       /**< 开启MIC及编码器 */
} DMA_VOICE_STREAM_CTRL_TYPE;

/**
* @brief 控制设备编码器类型
*/
typedef enum {
    DMA_VOICE_STREAM_NONE = 0,      /**< 关闭编码器 */
    DMA_VOICE_STREAM_OPUS,          /**< OPUS编码器 */
    DMA_VOICE_STREAM_BEAMFORMING,   /**< Beamforming编码器 */
} DMA_VOICE_STREAM_CODEC_TYPE;

/**
* @brief 设备控制指令
*/
typedef enum {
    DMA_OPERATION_NO_CMD = 0,       /**< 无指令 */

    DMA_AUDIO_PLAY,                 /**< AVRCP 播放 */
    DMA_AUDIO_PAUSE,                /**< AVRCP 暂停 */
    DMA_AUDIO_GET_PLAY_PAUSE,       /**< 获取AVRCP播放状态 */
    DMA_AUDIO_PLAY_BACKWARD,        /**< AVRCP 上一首 */
    DMA_AUDIO_PLAY_FORWARD,         /**< AVRCP 下一首 */

    DMA_SEND_CMD,                   /**< 向手机发送DMA指令 */
    DMA_SEND_DATA,                  /**< 向手机发送DMA数据 */

    DMA_SEND_ATCMD,                 /**< 向手机发送AT命令 */
    DMA_SEND_VOICE_COMMAND,         /**< 向手机发送Voice Recognition Activation */

    DMA_DISCONNECT_MOBILE,          /**< 与手机断连BT连接，但BLE连接保持 */
    DMA_CONNECT_MOBILE,             /**< 回连最后一次连接过的手机的BT */

    DMA_CUSTOM_CMD = 0x1000,        /**< 用户自定义命令 */
} DMA_OPERATION_CMD;

/**
* @brief 签名校验类型
*/
typedef enum {
    DMA_SIGN_METHOD__SHA256 = 0,    /**< SHA256 */
    DMA_SIGN_METHOD__SHA1 = 1,      /**< SHA1 */
    /*
     *不建议使用
     */
    DMA_SIGN_METHOD__MD5SUM = 2     /**< MD5,不建议使用 */
} DMA_SIGN_METHOD;

/**
* @brief 设备应用层通知DMA协议栈系统状态
*/
typedef enum {
    DMA_NOTIFY_STATE_DMA_CONNECTED = 0,     /**< 手机APP与设备建立DMA连接 */
    DMA_NOTIFY_STATE_DMA_DISCONNECTED,      /**< 手机APP与设备的DMA连接断开 */
    DMA_NOTIFY_STATE_MOBILE_CONNECTED,      /**< 手机与设备建立BT连接（A2DP、HFP、AVRCP） */
    DMA_NOTIFY_STATE_MOBILE_DISCONNECTED,   /**< 手机与设备BT连接（A2DP、HFP、AVRCP）断开 */
    DMA_NOTIFY_STATE_SEND_PREPARE_DONE,     /**< 设备端进入可向手机发送数据的状态 */
    DMA_NOTIFY_STATE_TWS_CONNECT,           /**< TWS类设备两端连接成功 */
    DMA_NOTIFY_STATE_TWS_DISCONNECT,        /**< TWS类设备两端断开连接 */
    DMA_NOTIFY_STATE_BOX_OPEN,              /**< 盒仓开启 */
    DMA_NOTIFY_STATE_BOX_CLOSE,             /**< 盒仓关闭 */
    DMA_NOTIFY_STATE_ROLE_SWITCH_START,     /**< TWS类设备开始进入主从切换流程 */
    DMA_NOTIFY_STATE_ROLE_SWITCH_FINISH,    /**< TWS类设备主从切换完成 */
    DMA_NOTIFY_STATE_ROLE_SWITCH_REQUEST,   /**< TWS类设备向APP发起主从切换请求 */
    DMA_NOTIFY_STATE_DOUBLE_CLICK,          /**< 设备双击按键 */
    DMA_NOTIFY_STATE_KEYWORD_DETECTED,      /**< 设备语音唤醒 */
    DMA_NOTIFY_STATE_BATTERY_LEVEL_UPDATE,  /**< 通知耳机电量更新 */

    DMA_NOTIFY_STATE_TRIPLE_CLICK,          /**< 设备三击按键 */
    DMA_NOTIFY_STATE_LONGPRESS,             /**< 设备触控长按 */
    DMA_NOTIFY_STATE_SLIDE_UP,              /**< 设备触控上滑 */
    DMA_NOTIFY_STATE_SLIDE_DOWN,            /**< 设备触控下滑 */
} DMA_NOTIFY_STATE;

/**
* @brief 接口返回值类型
*/
typedef enum {
    DMA_SUCCESS = 0,               /**< 成功 */
    DMA_NOT_ENOUGH_MEMORY = 1,     /**< 内存不足 */
    DMA_SYSTEM_ERROR = 2,          /**< 系统API异常 */
    DMA_USER_INTERFACE_ERROR = 3,  /**< 用户自定义接口返回错误 */
    DMA_UNKNOWN = 4                /**< 位置错误类型 */
} DMA_ERROR_CODE;

/**
* @brief 互斥锁ID
*/
typedef enum {
    DMA_CMD_INPUT_MUTEX_ID = 0,
    DMA_CMD_OUTPUT_MUTEX_ID,
    DMA_DATA_MUTEX_ID,
    DMA_SYNC_DATA_MUTEX_ID,

    DMA_MUTEX_NUM,
} DMA_MUTEX_ID;

/**
* @brief 小度自定义提示音ID
*/
typedef enum {
    DMA_TTS_WAITING_DMA_CONNECTING_0_TIMES_NEVER_CONNECTED = 0,         /**< 耳机首次与手机配对 */
    DMA_TTS_WAITING_DMA_CONNECTING_6_TIMES_NEVER_CONNECTED,             /**< 始终未连接APP，第6次配对时 */
    DMA_TTS_WAITING_DMA_CONNECTING_12_TIMES_NEVER_CONNECTED,            /**< 始终未连接APP，第12次配对时 */
    DMA_TTS_WAITING_DMA_CONNECTING_18or24or50n_TIMES_NEVER_CONNECTED,   /**< 始终未连接APP，第18、24、50次配对时*/
    DMA_TTS_WAITING_DMA_CONNECTING_MORETHAN_50_TIMES_AFTER_LASTCONNECT, /**< 连接过APP，第50次配对后，每50次配对时*/
    DMA_TTS_USE_XIAODU_APP,                   /**< 请连接小度APP使用语音功能*/
    DMA_TTS_KEYWORD_DETECTED,                 /**< 在呢*/
    DMA_TTS_WAITING_FOR_DMA_CONNECT,          /**< 正在连接小度APP */
    DMA_TTS_WAITING_FOR_DMA_CONNECT_IGNORE,   /**< 请稍候 */
    DMA_TTS_OPEN_APP_FAIL,                    /**< 启动失败，请手动打开小度APP */

    DMA_TTS_NUM,
} DMA_TTS_ID;


/**
* @brief DMA协议栈使用的配置信息，保存于Flash
*/
typedef struct {
    uint8_t dma_config_data[DMA_USER_DATA_CONFIG_LEN];
} DMA_USER_DATA_CONFIG;

/**
* @brief DMA设备能力集
*/
typedef struct {
    const char* device_type;     /*!< 设备类别，可选HEADPHONE, SPEAKER, DOCK */
    const char* manufacturer;    /*!< 厂商名称 */

    bool support_spp;   /*!< 蓝牙外设是否支持SPP链路 */
    bool support_ble;   /*!< 蓝牙外设是否支持BLE链路 */

    bool support_a2dp;  /*!< 蓝牙外设是否支持A2DP */
    bool support_at_command; /*!< 蓝牙外设是否支持AT指令 */
    bool support_media; /*!< 设备端是否支持AVRCP指令控制播放 */
    bool support_sota;  /*!< 是否支持静默升级 */
    bool is_earphone;   /*!< 是否是耳机类产品 */
    bool support_dual_record; /*!< 是否支持双路同时录音模式 */
    bool support_local_voice_wake;  /*!< 是否支持本地唤醒 */
    bool support_model_beamforming_asr; /*!< 是否支持波束算法特征值 */
    bool support_log;   /*!< 蓝牙外设是否支持打点日志并写入Flash */

    bool support_box_battery; /*!< 是否支持获取盒仓电量 */
    bool ble_role_switch_by_mobile; /*!< BLE是否需要通知手机后再进行主从切换 */
    bool support_tap_wakeup; /*!< 蓝牙外设是否支持敲击唤醒 */
    DMA_SIGN_METHOD support_sign_method; /*!< 蓝牙外设支持的签名校验类型 */
} DUER_DMA_CAPABILITY;

/**
* @brief 需用户根据芯片SDK实现并注册的接口，供DMA协议栈调用
*/
typedef struct {
    /*system operation*/

    /**
    * @brief 获取DMA设备能力集
    *
    * @param[out] device_capability      设备能力集
    *
    * @return 是否获取到设备能力集
    *     @retval false 获取能力集失败
    *     @retval true 获取能力集成功
    */
    bool (*get_device_capability)(DUER_DMA_CAPABILITY* device_capability);

    /**
    * @brief 获取DMA设备版本号
    *
    * @param[out] fw_rev_0      4位固件版本号第一位
    * @param[out] fw_rev_1      4位固件版本号第二位
    * @param[out] fw_rev_2      4位固件版本号第三位
    * @param[out] fw_rev_3      4位固件版本号第四位
    *
    * @return 是否获取DMA设备版本号
    *     @retval false 获取版本号失败
    *     @retval true 获取版本号成功
    * @note 使用小度APP的DMA及OTA功能要求设备版本号必须4位，格式为xx.xx.xx.xx \n
    * 每位取值范围为0-99,同一产品禁止不同固件使用相同版本号
    */
    bool (*get_firmeware_version)(uint8_t* fw_rev_0, uint8_t* fw_rev_1,
                                  uint8_t* fw_rev_2, uint8_t* fw_rev_3);

    /**
    * @brief DMA协议栈内存申请
    *
    * @param[in] size    内存申请大小
    *
    * @return 是否申请到内存
    *     @retval NULL 表示内存申请失败
    *     @retval 非NULL 表示内存申请成功
    * @note 该内存申请用于DMA协议栈，内存申请峰值不超过4KB \n
    * 该接口应保证DMA设备在任何使用模式下均可申请内存，线程安全
    */
    void* (*dma_heap_malloc)(size_t size);

    /**
    * @brief DMA协议栈内存释放
    *
    * @param[in] ptr    释放内存地址
    *
    * @note 该接口应保证DMA设备在任何使用模式下均可释放内存，线程安全
    */
    void (*dma_heap_free)(void* ptr);

    /**
    * @brief DMA协议栈等待信号量
    *
    * @param[in] timeout_ms    等待信号量超时时间，单位毫秒，0xFFFFFFFF表示wait for ever
    *
    * @return 是否等到信号
    *     @retval false 等待信号量超时
    *     @retval true 等待信号量成功
    *
    */
    bool (*dma_sem_wait)(uint32_t timeout_ms);

    /**
    * @brief DMA协议栈释放信号量
    *
    * @return 是否释放信号成功
    *     @retval false 释放信号量失败
    *     @retval true 释放信号量成功
    *
    */
    bool (*dma_sem_signal)(void);

    /**
    * @brief DMA协议栈互斥锁加锁
    *
    * @param[in] mutex_id    互斥锁id
    *
    * @return 互斥锁加锁是否成功
    *     @retval false 互斥锁加锁失败
    *     @retval true 互斥锁加锁成功
    *
    */
    bool (*dma_mutex_lock)(DMA_MUTEX_ID mutex_id);

    /**
    * @brief DMA协议栈互斥锁去锁
    *
    * @param[in] mutex_id    互斥锁id
    *
    * @return 互斥锁去锁是否成功
    *     @retval false 互斥锁加锁失败
    *     @retval true 互斥锁加锁成功
    *
    */
    bool (*dma_mutex_unlock)(int32_t mutex_id);

    /**
    * @brief 获取设备端的DMA自定义用户配置信息
    *
    * @param[out] dma_userdata_config     设备端存储的DMA自定义用户配置信息
    *
    * @return 获取用户配置信息是否成功
    *     @retval false 获取设备端的DMA自定义用户配置信息失败
    *     @retval true 获取设备端的DMA自定义用户配置信息成功
    *
    * @note 该接口应保证DMA设备在任何使用模式下均可获取，线程安全 \n
    * 该配置信息须保持在非掉电易失的存储空间
    *
    */
    bool (*dma_get_userdata_config)(DMA_USER_DATA_CONFIG* dma_userdata_config);

    /**
    * @brief 写入设备端的DMA自定义用户配置信息
    *
    * @param[in] dma_userdata_config     设备端存储的DMA自定义用户配置信息
    *
    * @return 写入用户配置信息是否成功
    *     @retval false 写入设备端的DMA自定义用户配置信息失败
    *     @retval true 写入设备端的DMA自定义用户配置信息成功
    *
    * @note 该接口应保证DMA设备在任何使用模式下均可写入，线程安全 \n
    * 该配置信息须保持在非掉电易失的存储空间
    *
    */
    bool (*dma_set_userdata_config)(const DMA_USER_DATA_CONFIG* dma_userdata_config);

    /**
    * @brief 随机数
    *
    * @return 无符号32bit随机数
    *
    */
    uint32_t (*dma_rand)(void);

    /**
    * @brief 获取DMA设备是否处于OTA升级状态
    *
    * @return 当前是否处于OTA状态
    *     @retval false 当前非OTA升级状态
    *     @retval true 当前为OTA升级状态
    * @note TWS类设备两侧调用该接口须有相同的返回值 \n
    */
    bool (*get_ota_state)(void);

    /**
    * @brief 获取DMA设备是否升级成功
    *
    * @return 是否升级成功
    *     @retval 0 未进行升级
    *     @retval 1 升级成功
    *     @retval 2 升级失败
    * @note TWS类设备两侧调用该接口须有相同的返回值 \n
    */
    uint8_t (*get_upgrade_state)(void);

    /**
    * @brief 获取DMA设备连接的手机类型
    *
    * @return 手机类型
    *     @retval 0 未连接手机
    *     @retval 1 连接iOS手机
    *     @retval 2 连接Android手机
    * @note TWS类设备两侧调用该接口须有相同的返回值 \n
    */
    int32_t (*get_mobile_connect_type)(void);

    /* BLE control */

    /**
    * @brief 设置DMA设备BLE广播包
    *
    * @param[in] adv_data   DMA广播的广播包，长度不超过31字节
    * @param[in] adv_data_len   DMA广播的广播包长度
    * @param[in] scan_response    DMA广播的响应包，长度不超过31字节
    * @param[in] scan_response_len   DMA广播的响应包长度
    * @param[in] ibeacon_adv_data    iBeacon广播的广播包，长度不超过31字节
    * @param[in] ibeacon_adv_data_len    iBeacon广播包长度

    * @return 广播包是否设置成功
    *     @retval false 设置DMA设备BLE广播包失败
    *     @retval true 设置DMA设备BLE广播包成功
    *
    * @note TWS类设备仅在Master使能有效，当ibeacon_adv_data非空时， 开启BLE广播后 \n
    * 须进行DMA和iBeacon的交错广播，交错格式为ABABAB……，两者的间隔不应大于100ms
    */
    bool (*set_ble_advertise_data)(const char* adv_data, uint8_t adv_data_len,
                                   const char* scan_response, uint8_t scan_response_len,
                                   const char* ibeacon_adv_data, uint8_t ibeacon_adv_data_len);

    /**
    * @brief 设置DMA设备是否使能BLE广播
    *
    * @param[in] on    BLE广播的使能标志
    *     @arg true 开启广播
    *     @arg false 关闭广播
    *
    * @return 开关BLE广播是否成功
    *     @retval false 开关BLE广播失败
    *     @retval true 开关BLE广播成功
    *
    * @note TWS类设备仅在Master开启广播有效 \n
    */
    bool (*set_ble_advertise_enable)(bool on);

    /**
    * @brief 获取DMA设备的BT MAC地址
    *
    * @param[in] addr_type    BTMAC地址类型
    *     @arg DMA_BT_ADDR_DEFAULT 出厂烧录的原始MAC地址
    *     @arg DMA_BT_ADDR_IN_USED 当前对外体现的MAC地址
    *
    * @param[out] bt_address    BTMAC地址，6字节数组
    *
    * @return 获取MAC地址是否成功
    *     @retval false 获取BT MAC地址失败
    *     @retval true 获取BT MAC地址成功
    *
    * @note TWS类设备当前对外提现的MAC地址和出厂烧录的原始MAC地址可能不一致 \n
    * 当入参为DMA_BT_ADDR_IN_USED，必须获取到手机扫描到的BT MAC地址
    */
    bool (*get_bt_address)(DMA_BT_ADDR_TYPE addr_type, uint8_t* bt_address);

    /**
    * @brief 获取DMA设备的蓝牙名称
    *
    * @param[out] bt_local_name    蓝牙名称，数组长度须大于产品蓝牙名称长度
    *
    * @return 获取蓝牙名称是否成功
    *     @retval false 获取蓝牙名称失败
    *     @retval true 获取蓝牙名称成功
    *
    * @note TWS类设备当前对外提现的蓝牙名称和出厂烧录的蓝牙名称可能不一致 \n
    * 必须获取到手机扫描到的蓝牙名称
    */
    bool (*get_bt_local_name)(char* bt_local_name);

    /**
    * @brief 获取DMA设备的BLE名称
    *
    * @param[out] ble_local_name    BLE名称，数组长度须大于产品BLE名称长度
    *
    * @return 获取BLE名称是否成功
    *     @retval false 获取BLE名称失败
    *     @retval true 获取BLE名称成功
    *
    * @note TWS类设备当前对外提现的BLE名称和出厂烧录的BLE名称可能不一致 \n
    * 必须获取到手机可连接的BLE名称
    */
    bool (*get_ble_local_name)(char* ble_local_name);

    /**
    * @brief 获取当前连接手机的BT MAC地址
    *
    * @param[out] bt_address    BTMAC地址，6字节数组
    *
    * @return 获取手机MAC地址是否成功
    *     @retval false 获取BT MAC地址失败
    *     @retval true 获取BT MAC地址成功
    */
    bool (*get_mobile_bt_address)(uint8_t* bt_address);

    /**
    * @brief 获取BT MAC地址是否存在于已配对列表
    *
    * @param[in] bt_address    BTMAC地址，6字节数组
    *
    * @return MAC地址是否已配对过
    *     @retval false 获取BT MAC地址不存在于已配对列表
    *     @retval true 获取BT MAC地址存在于已配对列表
    */
    bool (*get_linkkey_exist_state)(const uint8_t* bt_address);

    /**
    * @brief 获取DMA设备的Serial Number
    *
    * @param[in] sn_type    SN类型
    *     @arg DMA_SN_DEFAULT 出厂烧录的原始SN
    *     @arg DMA_SN_IN_USED 当前对外体现的MAC地址对应的SN
    *
    * @param[out] serial_number    设备SN，数组长度须大于产品SN长度
    *
    * @return 获取SN是否成功
    *     @retval false 获取SN失败
    *     @retval true 获取SN成功
    *
    * @note TWS类设备当前对外提现的MAC地址和出厂烧录的原始MAC地址可能不一致 \n
    * 当入参为DMA_BT_ADDR_IN_USED，必须获取到手机扫描到的BT MAC地址
    */
    bool (*get_serial_number)(DMA_SN_TYPE sn_type, uint8_t* serial_number);

    /**
    * @brief 获取DMA设备是否在通话状态
    *
    * @return 当前DMA是否在通话状态
    *     @retval false 当前DMA设备不在通话状态
    *     @retval true 当前DMA设备在通话状态
    *
    * @note 是否在通话状态，以建立SCO链路为准 \n
    * TWS类设备两侧调用该接口须有相同的返回值
    */
    bool (*get_sco_state)(void);

    /**
    * @brief 获取DMA设备是否在回连手机
    *
    * @return 设备是否处于回连状态
    *     @retval false 当前DMA设备未在回连手机
    *     @retval true 当前DMA设备在回连手机
    *
    * @note TWS类设备两侧调用该接口须有相同的返回值
    *
    */
    bool (*get_reconnect_state)(void);

    /*core dma protocol */

    /**
    * @brief 设置DMA设备MIC及编码状态
    *
    * @param[in] cmd    设置MIC状态命令字
    *     @arg DMA_VOICE_STREAM_STOP    关闭MIC及编码器
    *     @arg DMA_VOICE_STREAM_START    开启MIC及编码器
    *
    * @param[in] codec_type    编码器类型
    *     @arg DMA_VOICE_STREAM_NONE    无编码器
    *     @arg DMA_VOICE_STREAM_OPUS    opus编码器
    *     @arg DMA_VOICE_STREAM_BEAMFORMING    波束编码器
    * @return 开关MIC和编码器是否成功
    *     @retval false 设置MIC及编码器失败
    *     @retval true 设置MIC及编码器成功
    *
    */
    bool (*set_voice_mic_stream_state)(DMA_VOICE_STREAM_CTRL_TYPE cmd, DMA_VOICE_STREAM_CODEC_TYPE codec_type);

    /**
    * @brief 设置DMA设备是否使能上传音频
    *
    * @param[in] on  DMA设备上传音频的使能标志
    *     @arg true 开启DMA设备上传音频
    *     @arg false 关闭DMA设备上传音频
    *
    * @return 使能上行音频是否成功
    *     @retval false DMA设备音频上传设置失败
    *     @retval true DMA设备音频上传设置成功
    *
    * @note 该接口用于控制DMA设备开启或停止上传音频，不依赖是否已经开启MIC和编码器 \n
    * TWS类设备Master设备调用该接口表示向手机端上传音频 \n
    * Slave设备调用该接口表示向Master设备上传音频
    *
    */
    bool (*set_stream_upload_enable)(bool on);

    /**
    * @brief 获取DMA设备是否在上传音频状态
    *
    * @return 当前设备是否处于上传音频状态
    *     @retval false DMA设备不在上传音频状态
    *     @retval true DMA设备在上传音频状态
    *
    * @note 该接口表示DMA设备已进入上传音频状态，不依赖是否已经实际上传了音频数据 \n
    * TWS类设备两侧调用该接口须返回当前设备是否实际进入上传音频状态，其中 \n
    * Slave设备的上传音频意为向Master设备上传音频
    *
    */
    bool (*get_stream_upload_state)(void);

    /**
    * @brief 获取DMA设备与手机协商的MTU
    *
    * @param[out] mtu   DMA设备上行音频的MTU
    *
    * @return 获取手机MTU是否成功
    *     @retval false 获取DMA设备与手机协商的MTU失败
    *     @retval true 获取DMA设备与手机协商的MTU成功
    *
    * @note TWS类设备Master设备调用该接口获取和手机之间的MTU \n
    * Slave设备不调用该接口
    *
    */
    bool (*get_mobile_mtu)(uint32_t* mtu);

    /**
    * @brief 获取TWS设备之间的MTU
    *
    * @param[out] mtu   TWS设备间的MTU
    *
    * @return 获取对端MTU是否成功
    *     @retval false 获取TWS设备之间的MTU失败
    *     @retval true 获取TWS设备之间的MTU成功
    *
    * @note TWS类设备两侧调用该接口须有相同的返回值
    *
    */
    bool (*get_peer_mtu)(uint32_t* mtu);

    /**
    * @brief 设置DMA设备是否使能语音唤醒
    *
    * @param[in] on  语音唤醒的使能标志
    *     @arg true 开启语音唤醒
    *     @arg false 关闭语音唤醒
    *
    * @note TWS类设备仅在Master使能有效，当未开启MIC时使能语音唤醒，应在此接口内开启MIC
    */
    bool (*set_wakeup_enable)(bool on);

    //dma process

    /**
    * @brief 获取DMA设备的签名校验
    *
    * @param[in] input_data      待签名校验的原始数据
    *
    * @param[in] len      数据长度
    *
    * @param[out] output_string      字符串格式的签名校验结果
    *
    * @return 签名校验是否成功
    *     @retval false 签名校验失败
    *     @retval true 签名校验成功
    *
    */
    bool (*get_check_summary)(const void* input_data, uint32_t len, uint8_t* output_string);

    /*parse cmd interface*/

    /**
    * @brief 获取当前是否处于可发送DMA数据状态
    *
    * @return 当前是否处于可发送数据状态
    *     @retval false 当前未处于可发送状态
    *     @retval true 当前未处于可发送状态
    *
    */
    bool (*get_prepare_state)(void);

    /**
    * @brief 处理DMA命令
    *
    * @param[in] cmd      DMA命令类型
    *
    * @param[in] param_buf    DMA命令参数
    *
    * @param[in] param_size      DMA命令参数长度
    *
    * @return 处理DMA操作是否成功
    *     @retval false 处理DMA命令失败
    *     @retval true 处理DMA命令成功
    *
    */
    bool (*dma_process_cmd)(DMA_OPERATION_CMD cmd, void* param_buf, uint32_t  param_size);

    /*tws option*/
    /**
    * @brief 获取TWS设备的组对状态
    *
    * @return TWS是否已组对
    *     @retval false 获取TWS设备的组对状态失败或TWS设备未组对
    *     @retval true TWS设备的组对成功
    *
    * @note TWS类设备两侧调用该接口须有相同的返回值
    */
    bool (*get_peer_connect_state)(void);

    /**
    * @brief 发送自定义消息到TWS设备对端
    *
    * @param[in] param_buf      自定义消息数据
    *
    * @param[in] param_size     自定义消息数据长度
    *
    * @return 发送自定义消息到对端是否成功
    *     @retval false 发送消息失败
    *     @retval true 发送消息成功
    */
    bool (*send_custom_info_to_peer)(uint8_t* param_buf, uint16_t param_size);

    /**
    * @brief 获取TWS设备的角色
    *
    * @param[out] role_type      TWS设备的角色
    *     @arg DMA_TWS_UNKNOWN 未知类型
    *     @arg DMA_TWS_MASTER   主耳Master
    *     @arg DMA_TWS_SLAVE    从耳Slave
    *
    * @return 获取TWS设备的角色是否成功
    *     @retval false 获取TWS设备的角色失败
    *     @retval true 获取TWS设备的角色成功
    *
    * @note TWS类设备两侧建立连接后调用该接口，须获取到非UNKNOWN的角色类型
    */
    bool (*get_tws_role)(DMA_TWS_ROLE_TYPE* role_type);

    /**
    * @brief 获取TWS设备的左右信息
    *
    * @param[out] side_type    TWS设备的左右信息
    *     @arg DMA_SIDE_INVALID 无效类型
    *     @arg DMA_SIDE_LEFT  左侧设备
    *     @arg DMA_SIDE_RIGHT    右侧设备
    *
    * @return 获取TWS设备的左右信息是否成功
    *     @retval false 获取TWS设备的左右信息失败
    *     @retval true 获取TWS设备的左右信息成功
    *
    * @note TWS类设备两侧调用该接口，须获取到非INVALID的左右类型
    */
    bool (*get_tws_side)(DMA_TWS_SIDE_TYPE* side_type);

    /**
    * @brief 设置是否运行进行主从切换
    *
    * @param[in] on 主从切换的使能标志
    *     @arg true 允许主从切换
    *     @arg false 不允许主从切换
    *
    * @return 获取主从切换使能标志是否成功
    *     @retval false 使能主从切换失败
    *     @retval true 使能主从切换成功
    *
    * @note 该接口仅对应DMA_NOTIFY_STATE_ROLE_SWITCH_REQUEST时控制是否发起主从切换 \n
    * 不影响非DMA连接时的主从切换，不是主从切换的总开关
    */
    bool (*set_role_switch_enable)(bool on);

    /**
    * @brief 获取盒仓的开关状态信息
    *
    * @param[out] side_type    TWS设备的左右信息
    *     @arg DMA_BOX_STATE_UNKNOWN 无效类型
    *     @arg DMA_BOX_STATE_OPEN    盒仓开盖
    *     @arg DMA_BOX_STATE_CLOSE   盒仓关盖
    *
    * @return 获取盒仓的开关状态是否成功
    *     @retval false 获取盒仓的开关状态信息失败
    *     @retval true 获取盒仓的开关状态信息成功W
    */
    bool (*get_box_state)(DMA_BOX_STATE_TYPE* box_state);

    /**
    * @brief 获取当前设备入耳状态信息
    *
    * @return 获取设备是否已入耳
    *     @retval false 设备未入耳
    *     @retval true 设备已入耳
    */
    bool (*get_wearing_state)(void);

    /**
    * @brief 获取设备电量
    *
    * @return 设备电量 0~100
    *
    */
    uint8_t (*get_battery_level)(void);

    /**
    * @brief 获取盒仓电量
    *
    * @return 盒仓电量 0~100
    *
    * @note 该接口应返回当前设备最后从盒仓获取到的电量。如不支持获取盒仓电量，请直接返回0
    */
    uint8_t (*get_box_battery_level)(void);

    /**
    * @brief 获取三元组数据
    *
    * @param[out] triad_id    三元组id，不超过32字节数组
    * @param[out] triad_secret    三元组密钥，不超过32字节数组
    *
    * @return 是否获取到三元组
    *
    */
    bool (*get_triad_info)(char* triad_id, char* triad_secret);

    /**
    * @brief 播放小度自定义提示音
    *
    * @param[in] tts_id    提示音ID
    * @param[in] both_side    是否两边同时播放
    *
    * @return 是否成功播放小度自定义提示音
    *
    */
    bool (*play_local_tts)(DMA_TTS_ID tts_id, bool both_side);

} DUER_DMA_OPER;

/**
* @brief 向DMA协议栈注册接口
*
* @param[in] p_dma_operation  DMA使用者基于芯片SDK实现的接口
*
*/
void dma_register_operation(DUER_DMA_OPER* p_dma_operation);

/**
* @brief 初始化DMA协议栈
*
* @param[in] client_id  DMA设备的client id
* @param[in] client_secret  DMA设备的client secret
* @param[in] vendor_id  BLE广播包中的vendor id
* @param[in] device_type  DMA设备的设备类型
* @param[in] device_version  DMA设备的设备版本
* @param[in] protocol_buffer 用于DMA协议栈的内存池，4K Byte
*
* @return 是否初始化DMA协议栈成功
*
* @note 以上参数除vendor id外，请联系小度获取
*/
DMA_ERROR_CODE dma_protocol_init(const uint8_t* client_id, const uint8_t* client_secret, const uint8_t* vendor_id,
                                 uint8_t device_type, uint8_t device_version, uint8_t* protocol_buffer);

/**
* @brief 向DMA协议栈发送从手机端经DMA链路发送来的数据
*
* @param[in] input_data  收集发来的数据
* @param[in] date_len  数据长度
*
* @return 是否成功将数据发送到DMA协议栈
*
*/
DMA_ERROR_CODE dma_recv_mobile_data(const char* input_data, uint32_t date_len);

/**
* @brief 向DMA协议栈发送从对耳接受到的数据
*
* @param[in] input_data  对耳发来的数据
* @param[in] date_len  数据长度
*
* @return 是否成功将数据发送到DMA协议栈
*
*/
DMA_ERROR_CODE dma_recv_peer_data(const char* input_data, uint32_t date_len);

/**
* @brief 向DMA协议栈发送经过压缩编码的音频数据
*
* @param[in] input_data  经过压缩编码的音频数据
* @param[in] date_len  数据长度
*
* @return 是否成功将数据发送到DMA协议栈
*
*/
DMA_ERROR_CODE dma_feed_compressed_data(const char* input_data, uint32_t date_len);

/**
* @brief 向DMA协议栈发送当前系统状态
*
* @param[in] state  系统状态
* @param[in] param_buf    DMA命令参数
* @param[in] param_size      DMA命令参数长度
*
* @return 是否成功通知DMA协议栈当前状态
*
*/
DMA_ERROR_CODE dma_notify_state(DMA_NOTIFY_STATE state, void* param_buf, uint32_t  param_size);

/**
* @brief DMA协议栈的主处理函数
*
* @param[in] timeout_ms    处理函数的超时等待时间，单位毫秒，0xFFFFFFFF表示wait for ever
*
* @note 请在DMA专用线程内循环调用改函数
*
*/
void dma_process(uint32_t timeout_ms);




void dueros_dma_wrap_process_register(void);
void dueros_dma_wrap_heap_init(void);
void dueros_dma_wrap_thread_create(void);
void dueros_dma_wrap_operation_register(void);
bool dueros_dma_wrap_get_mobile_mtu(uint32_t* mtu);
void dueros_dma_wrap_protocol_stack_init(void);
DMA_ERROR_CODE dueros_dma_wrap_recv_mobile_data(const char* input_data, uint32_t date_len);
DMA_ERROR_CODE dueros_dma_wrap_recv_peer_data(const char* input_data, uint32_t date_len);
DMA_ERROR_CODE dueros_dma_wrap_feed_compressed_data(const char* input_data, uint32_t date_len);
DMA_ERROR_CODE dueros_dma_wrap_notify_state(DMA_NOTIFY_STATE state, void* param_buf, uint32_t  param_size);

#ifdef __cplusplus
}
#endif

#endif

