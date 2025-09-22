//***************************************************************************************************************//
//***************************************************************************************************************//
//                                               README                                                          //
// 1.在xcfg.xm文件 config(UPD, 0x1F, 0x0); 之前添加 “小度AI配置” 并把旧的 “蓝牙地址” 配置删除，使小度配置存于KEEP区。  //
// 当需要更换三元组或蓝牙地址时，只能使用download方式更更换，需把download的 "擦除" 勾上并把 "保持" 去掉然后重新下载。    //
// 一旦耳机连接过一次app，app云服务就会把蓝牙地址和三元组绑定，如果更换蓝牙地址，那么三元组也需一同更换。                 //
// config(LEVEL, 0x0F);                                                                                          //
// config(SUB, "小度AI", "小度的相关配置");                                                                        //
// config(MAC, "蓝牙地址", "蓝牙的MAC地址", BT_ADDR, 6, 41:42:00:00:00:00, 41:42:FF:FF:FF:FF, 41:42:00:00:00:01);  //
// config(TEXT, "Triad_ID",  "小度Triad_ID", DMA_TRIAD_ID, 48, "002AiaGy0000002400000006");                      //
// config(TEXT, "Secret", "小度Secret", DMA_SECRET, 48, "c2d8d88c0b4664e1");                                     //
//                                                                                                               //
// 2.需联系小度获取CLIENT ID、 CLIENT SECRET、 TRIAD ID、 TRIAD SECRET                                             //
//                                                                                                               //
// 3.使用固定蓝牙地址和某种方式固定左耳，左耳必须使用小度提供的三元组，初次使用必须先tws配对一次（左耳同步三元组给右耳）   //
//                                                                                                               //
// 4.DUEROS DMA协议整体消耗14K左右ram资源，使用前请评估好ram资源是否足够，目前通话算法只够支持自研单mic dnn             //
//                                                                                                               //
// 5.语音唤醒：dueros_ai_speech_start();    按键功能：void dueros_dma_key_msg_send(uint8_t key_click);             //
//   获取小度连接状态：dueros_dma_con_type_get()  0：未连接、1：连接安卓app、2：连接ios app                          //
//***************************************************************************************************************//
//***************************************************************************************************************//

#ifndef __DUEROS_DMA_APP_H
#define __DUEROS_DMA_APP_H

#include "dma_wrapper.h"
#include "dueros_dma_tws.h"

#define DEVICE_TYPE                           "HEADPHONE"    /*!< 设备类别，可选HEADPHONE, SPEAKER, DOCK */
#define MANUFACTURER                          "BLUETRUM"     /*!< 厂商名称 */
#define SUPPORT_SPP                           1              /*!< 蓝牙外设是否支持SPP链路 */
#define SUPPORT_BLE                           1              /*!< 蓝牙外设是否支持BLE链路 */
#define SUPPORT_A2DP                          1              /*!< 蓝牙外设是否支持A2DP */
#define SUPPORT_AT_COMMAND                    1              /*!< 蓝牙外设是否支持AT指令 */
#define SUPPORT_MEDIA                         1              /*!< 设备端是否支持AVRCP指令控制播放 */
#define SUPPORT_SOTA                          0              /*!< 是否支持静默升级 */
#define IS_EARPHONE                           1              /*!< 是否是耳机类产品 */
#define SUPPORT_DUAL_RECORD                   0              /*!< 是否支持双路同时录音模式 */
#define SUPPORT_LOCAL_VOICE_WAKE              0              /*!< 是否支持本地唤醒 */
#define SUPPORT_MODEL_BEAMFORMING_ASR         0              /*!< 是否支持波束算法特征值 */
#define SUPPORT_LOG                           0              /*!< 蓝牙外设是否支持打点日志并写入FLASH */
#define SUPPORT_BOX_BATTERY                   1              /*!< 是否支持获取盒仓电量 */
#define BLE_ROLE_SWITCH_BY_MOBILE             0              /*!< BLE是否需要通知手机后再进行主从切换 */
#define SUPPORT_TAP_WAKEUP                    1              /*!< 蓝牙外设是否支持敲击唤醒 */
#define SUPPORT_SIGN_METHOD                   0              /*!< 蓝牙外设支持的签名校验类型 */

#define DUEROS_DMA_FW_VERSION                 "00.00.00.01"

#define DUEROS_DMA_DEVICE_TYPE                0x01                                //联系小度获取
#define DUEROS_DMA_DEVICE_VERSION             0x01                                //联系小度获取
#define DUEROS_DMA_VERDOR_ID                  "4206"                              //联系小度获取
#define DUEROS_DMA_CLIENT_ID                  "VGofdbsaDOruVTN8ioCaO5zZBYq2AqmR"  //联系小度获取
#define DUEROS_DMA_CLIENT_SECRET              "mjyrk7nQrHjuayEEFbd80IiQAdDxEsbv"  //联系小度获取

#define DUEROS_DMA_MTU_MAX_LEN                180
#define DUEROS_DMA_HFP_AT_CMD_LEN             40
#define DUEROS_DMA_OPUS_DATA_LEN              40*4
#define DUEROS_DMA_NTF_STA_BUF_LEN            8
#define DUEROS_DMA_NTF_STA_MSG_MASK           (DUEROS_DMA_NTF_STA_BUF_LEN - 1)
#define DUEROS_DMA_NTF_STA_PARAM_BUF_LEN      8

//每个page 250 Byte, dma协议栈消耗512 Byte
//如用到SM，注意BLE_CM_PAGE的使用是否有冲突
#define DUEROS_DMA_PARAM_PAGE0(x)             APP_CM_PAGE(x)    //250 Byte
#define DUEROS_DMA_PARAM_PAGE1(x)             GFPS_PAGE(x)      //250 Byte
#define DUEROS_DMA_PARAM_PAGE2(x)             BLE_CM_PAGE(x)    //12 Byte
#define DUEROS_DMA_PARAM_TRIAD_ID             BLE_CM_PAGE(12)   //25 Byte
#define DUEROS_DMA_PARAM_TRIAD_SECRET         BLE_CM_PAGE(37)   //17 Byte

#define DUEROS_DMA_ADV_INTERVAL               400     //adv: x * 0.625ms
#define DUEROS_DMA_ADV_SLEEP_INTERVAL         640
#define DUEROS_DMA_CON_INTERVAL               48      //con: x * 1.25ms
#define DUEROS_DMA_CON_SLEEP_INTERVAL         320

//flag
#define DUEROS_DMA_FLAG_AUDIO_BYPASS          BIT(0)
#define DUEROS_DMA_FLAG_AUDIO_ENABLE          BIT(1)
#define DUEROS_DMA_FLAG_KEY_MSG_CTL           BIT(2)
#define DUEROS_DMA_FLAG_VOICE_START           BIT(3)
#define DUEROS_DMA_FLAG_VOICE_STOP            BIT(4)

typedef enum {
    DUEROS_DMA_TRIAD_FROM_INIT = 0,
    DUEROS_DMA_TRIAD_FROM_LEFT_EAR,
}TRIAD_SOURCE;

typedef enum{
    DUEROS_DMA_CON_NONE = 0,
    DUEROS_DMA_CON_SPP,
    DUEROS_DMA_CON_BLE,
}DMA_CONN_TYPE;

typedef struct __attribute__((packed)) {
    uint8_t state;
    uint8_t param_size;
    uint8_t param_buf[DUEROS_DMA_NTF_STA_PARAM_BUF_LEN];
}dueros_dma_app_ntf_sta_buf_t;

typedef struct __attribute__((packed)) {
    dueros_dma_app_ntf_sta_buf_t state_buf[DUEROS_DMA_NTF_STA_BUF_LEN];
    uint16_t rptr;
    uint16_t wptr;
    uint16_t num;
}dueros_dma_app_ntf_sta_t;

typedef struct __attribute__((packed)) {
    uint8_t dev_type;
    uint8_t dev_version;
    char triad_id[48];
    char triad_secret[48];
}dueros_dma_app_dev_info_t;

typedef struct __attribute__((packed)) {
    bool wakeup;
    bool stream_upload_flag;
    bool role_switch_flag;
    bool bt_bypass;
    bool opus_enc_flag;
    uint8_t key_channel;
    uint8_t key_click;
    uint8_t con_type;
    uint16_t client_cfg;
	uint32_t do_flag;
    char hfp_atcmd[DUEROS_DMA_HFP_AT_CMD_LEN];
    dueros_dma_app_ntf_sta_t ntf_sta;
    dueros_dma_app_dev_info_t dev_info;
}dueros_dma_app_data_t;

void dueros_dma_key_msg_send(uint8_t key_click);
bool dueros_dma_prepare_state_get(void);
void dueros_dma_ntf_sta_enqueue(uint8_t sta, uint8_t *param_buf, uint8_t param_size, bool priority);
uint16_t dueros_dma_notify_state_number_get(void);
bool dueros_dma_app_get_device_capability(void* device_capability);
void dueros_dma_hfp_at_cmd_set(char *cmd, uint8_t len);
char * hfp_get_at_cmd(void);
bool dueros_dma_role_switch_flag_get(void);
void dueros_dma_device_type_set(uint8_t dev_type);
uint8_t dueros_dma_device_type_get(void);
uint8_t dueros_dma_stream_upload_flag_get(void);
void dueros_dma_stream_upload_flag_set(bool flag);
void dueros_dma_device_version_set(uint8_t dev_version);
uint8_t dueros_dma_device_version_get(void);
void dueros_dma_triad_id_set(char *triad_id, uint8_t from);
char *dueros_dma_triad_id_get(void);
void dueros_dma_triad_secret_set(char *triad_secret, uint8_t from);
char *dueros_dma_triad_secret_get(void);
void dueros_dma_send_packet(uint8_t *packet, uint32_t len);
void dueros_dma_con_type_set(uint8_t con_type);
uint8_t dueros_dma_con_type_get(void);
void dueros_dma_client_cfg_set(uint16_t cfg);
uint16_t dueros_dma_client_cfg_get(void);
void dueros_dma_wakeup_set(bool wk);
void dueros_dma_do_flag_set(uint32_t flag);
void dueros_dma_do_flag_clr(uint32_t flag);
uint32_t dueros_dma_do_flag_get(void);
bool dueros_dma_opus_enc_flag_get(void);
void dueros_dma_spp_disconnect_callback(void);
void dueros_dma_spp_connect_callback(void);
bool dueros_dma_recv_proc(uint8_t *data,uint8_t len, uint8_t con_type);
void dueros_dma_app_ble_disconnect_callback(void);
void dueros_dma_app_ble_connect_callback(void);
void dueros_dma_app_process(void);
void dueros_dma_app_app_init(void);
void dueros_dma_app_enter_sleep(void);
void dueros_dma_app_exit_sleep(void);
bool dueros_dma_app_is_need_wakeup(void);
void dueros_dma_app_bt_evt_notice(uint16_t evt, void *params);
void dueros_dma_tws_user_key_process(uint32_t *opcode);
void dueros_dma_var_init(void);
void dueros_dma_app_init(void);
void dueros_dma_ble_app_init(void);

#endif //__DUEROS_DMA_APP_H
