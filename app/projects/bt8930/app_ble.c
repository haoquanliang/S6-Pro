#include "app_include.h"

#define DEBUG_PRIV_BLE  1
#if DEBUG_PRIV_BLE
#define PRIV_BLE_DEBUG(...)              printf(__VA_ARGS__)
#else
#define PRIV_BLE_DEBUG(...)
#endif
#define TRACE_R(...)            print_r(__VA_ARGS__)

#define INDEX_HEADER        (0)
#define INDEX_CTRL          (1)
#define INDEX_FSN_NUM       (2)
#define INDEX_CMD           (3)
#define INDEX_PAYLOAD_SIZE  (4)
#define INDEX_PAYLOAD       (5)

#define FIXED_HEADER    (0xAA)
#define HEADER_SIZE     (5)
#define TX_PAYLOAD_SIZE (100)


typedef struct __attribute__((packed)){
    u8 header;                      //头为AA
    u8 ctrl;                        //控制
    u8 fsn_num;                     //数据帧编号
    u8 cmd;                         //指令
    u8 payload_size;                //数据长度
    u8 payload[TX_PAYLOAD_SIZE];    //数据
}app_priv_ble_cmd_send_t;

static app_priv_ble_cmd_send_t app_priv_ble_cmd_send;

enum 
{
    STATUS_SUCCESS = 0,
    STATUS_ERR,
};
//ctrl
enum{

    PRIV_BLE_DATA_MARK_PHONE    = 0x00,//数据方向 0：手机 1：固件端
    PRIV_BLE_DATA_MARK_EAR      = 0x01,
};
enum 
{
    PRIV_BLE_CMD_TYPE_REQUEST   = 0x00,//数据类型
    PRIV_BLE_CMD_TYPE_RESPONSE  = 0x02,
    PRIV_BLE_CMD_TYPE_NOTIFY    = 0x06,
};

enum{
    PRIV_BLE_NOT_SUBPACKAGE     = 0x00,//不分包
    PRIV_BLE_SUBPACKAGE         = 0x08,//分包
};
   
enum{
    PRIV_BLE_DATA_FRAMES_SATRT  = 0x00,
    PRIV_BLE_DATA_FRAMES_ING    = 0x10,
    PRIV_BLE_DATA_FRAMES_END    = 0x30,
};

enum{
    PRIV_BLE_DATA_NOT_ENCRYPE = 0x00,//0:不加密 1：加密
    PRIV_BLE_DATA_ENCRYPE    = 0x40,  
};

enum{
     PRIV_BLE_PLAYLOAD_NOT_TLV    = 0x00,//0:不是TLV 1：是TLV
     PRIV_BLE_PLAYLOAD_IS_TLV    = 0x80,
};

//ctrl

enum 
{
    CMD_GET_DEVICE_INFO = 1,
    CMD_NOTIFY_DEVICE_INFO,

};

enum 
{
    GET_DEVICE_EAR_ALL              = 0x00,
    GET_DEVICE_EAR_VBAT             = 0x01,
    GET_DEVICE_VERSIONE             = 0x02,
    GET_DEVICE_BLE_NAME             = 0x03,
    GET_DEVICE_EQ                   = 0x04,
    GET_DEVICE_KEY_CONFIG           = 0x05,
    GET_DEVICE_GAMEMODE             = 0x06,
    GET_DEVICE_WARNTONE             = 0x07,
    GET_DEVICE_ANC_MODE             = 0x08,
    GET_DEVICE_SHARED_AUDIO         = 0x09,
    GET_DEVICE_IS_SUPPORT_TWS       = 0x0A,
    GET_DEVICE_TWS_STATE            = 0x0B,
    GET_DEVICE_CRC                  = 0x0C,
    GET_DEVICE_IS_LEFT_EAR          = 0x0D,
    GET_DEVICE_SPACE_MUSIC_STATE    = 0x0E,
    GET_DEVICE_TWO_PAIR_STATE       = 0x0F,
    GET_DEVICE_BT_LINKSTATE         = 0x10,
    GET_DEVICE_LDAC_STATE           = 0x11,
    GET_DEVICE_CASE_ADDR            = 0x12,
    GET_DEVICE_ANC_DEEP             = 0x13,
    GET_DEVICE_FIND_LEFTEAR_STATE   = 0x14,
    GET_DEVICE_FIND_RIGHTEAR_STATE  = 0x15,
    GET_DEVICE_EAR_INCASE_STATE     = 0x16,
    GET_DEVICE_ANC_WIND_NOISE_STATE = 0x17,
    GET_DEVICE_SUPPORT_FUNCTION     = 0x18,
    GET_DEVICE_WEAR_STATE           = 0x19,
    GET_DEVICE_AI_MODE              = 0x1A,
    GET_DEVICE_MAX_MTU              = 0xFF,
};

enum 
{
    INFO_BATTERY_LEVEL              = 0x01,
    INFO_DEVICE_VERSIONE            = 0x02,
    INFO_DEVICE_BLE_NAME            = 0x03,
    INFO_DEVICE_EQ                  = 0x04,
    INFO_DEVICE_KEY_CONFIG          = 0x05,
    INFO_DEVICE_GAMEMODE            = 0x06,
    INFO_DEVICE_WARNTONE            = 0x07,
    INFO_DEVICE_ANC_MODE            = 0x08,
    INFO_DEVICE_SHARED_AUDIO        = 0x09,
    INFO_DEVICE_IS_SUPPORT_TWS      = 0x0A,
    INFO_DEVICE_TWS_STATE           = 0x0B,
    INFO_DEVICE_CRC                 = 0x0C,
    INFO_DEVICE_IS_LEFT_EAR         = 0x0D,
    INFO_DEVICE_SPACE_MUSIC_STATE   = 0x0E,
    INFO_DEVICE_TWO_PAIR_STATE      = 0x0F,
    INFO_DEVICE_BT_LINKSTATE        = 0x10,
    INFO_DEVICE_LDAC_STATE          = 0x11,
    INFO_DEVICE_CASE_ADDR           = 0x12,
    INFO_DEVICE_ANC_DEEP            = 0x13,
    INFO_DEVICE_FIND_LEFTEAR_STATE  = 0x14,
    INFO_DEVICE_FIND_RIGHTEAR_STATE = 0x15,
    INFO_DEVICE_EAR_INCASE_STATE    = 0x16,
    INFO_DEVICE_ANC_WIND_NOISE_STATE = 0x17,
    INFO_DEVICE_SUPPORT_FUNCTION    = 0x18,
    INFO_DEVICE_WEAR_STATE          = 0x19,
    INFO_DEVICE_AI_MODE             = 0x1A,

    INFO_DEVICE_MAX_MTU             = 0xFF,

};

uint16_t  CRC16(const uint8_t *buffer,uint32_t size)
{
    uint16_t  crc = 0xFFFF;
    if(NULL != buffer && size > 0){
        while (size--)
        {
           crc = (crc >> 8) | (crc << 8);
           crc ^= *buffer++;
           crc ^= ((unsigned char) crc) >> 4;
           crc ^= crc << 12;
           crc ^= (crc&0xFF) << 5;
        }
    }
    return crc;
}

static u8 get_left_bat(void)
{
    u8 level;
    
    if (sys_cb.tws_left_channel)
    {
        level = sys_cb.local_bat_level;
    }
    else 
    {
        if (bt_tws_is_connected())
        {
            level = sys_cb.peer_bat_level;
        }
        else 
        {
            level = 0xff;
        }
    }
    if (level != 0xff)
    {
        level = app_bat_level_show_for_app(level);
    }
    return level;
}

static u8 get_right_bat(void)
{
    u8 level;
    
    if (!sys_cb.tws_left_channel)
    {
        level = sys_cb.local_bat_level;
    }
    else 
    {
        if (bt_tws_is_connected())
        {
            level = sys_cb.peer_bat_level;
        }
        else 
        {
            level = 0xff;
        }
    }
    if (level != 0xff)
    {
        level = app_bat_level_show_for_app(level);
    }       
    return level;    
}

static void handle_cmd_get_dev_info(uint8_t *payload, uint16_t payload_len, uint8_t *fsn)
{
    // 打印 payload_len
    printf("payload_len: %u  fsn: 0x%02X\n", payload_len,*fsn);
    // 打印 payload 内容（十六进制格式）
    printf("payload: ");
    print_r(payload,payload_len);
    printf("\n");

    u8 read_offset = 0;
    u8 write_offset = 0;
    u8 *buf = app_priv_ble_cmd_send.payload;
    u8 val_len = 0;

    while (read_offset < payload_len){
        printf("payload[0]:%2x\r\n",payload[0]);
        switch (payload[read_offset])//TLV数据元标识
            {
                case GET_DEVICE_EAR_VBAT:
                        PRIV_BLE_DEBUG("GET_DEVICE_EAR_VBAT\n");  
                        val_len = payload[read_offset + 1];//拿TLV数据长度
                        buf[write_offset++] = INFO_BATTERY_LEVEL;//T Tyte
                        buf[write_offset++] = 9;//L len
                                                //V value
                        buf[write_offset++] = 0x01;
                        buf[write_offset++] = 1;
                        buf[write_offset++] = get_left_bat();//左耳电量

                        buf[write_offset++] = 0x02;
                        buf[write_offset++] = 1;
                        buf[write_offset++] = get_right_bat();//右耳电量

                        buf[write_offset++] = 0x03;
                        buf[write_offset++] = 1;
                        buf[write_offset++] =ab_mate_app.box_vbat;//仓电量

                    break;

                case GET_DEVICE_VERSIONE:
                        PRIV_BLE_DEBUG("GET_DEVICE_VERSIONE\n");  
                        val_len = payload[read_offset + 1];//拿TLV数据长度
                        buf[write_offset++] = INFO_DEVICE_VERSIONE;
                        buf[write_offset++] = sizeof(ab_mate_app.version);
                        memcpy(&buf[write_offset],ab_mate_app.version,sizeof(ab_mate_app.version));
                        write_offset += sizeof(ab_mate_app.version);
                    break;

                case GET_DEVICE_BLE_NAME:
                        PRIV_BLE_DEBUG("GET_DEVICE_BLE_NAME\n");  
                        val_len = payload[read_offset + 1];//拿TLV数据长度
                        buf[write_offset++] = INFO_DEVICE_BLE_NAME;
                        buf[write_offset++] = strlen(xcfg_cb.le_name);
                        memcpy(&buf[write_offset], xcfg_cb.le_name, strlen(xcfg_cb.le_name));
                        write_offset += strlen(xcfg_cb.le_name);
                    break;

                    case GET_DEVICE_EQ:
                        PRIV_BLE_DEBUG("GET_DEVICE_EQ\n");  
                        val_len = payload[read_offset + 1];//拿TLV数据长度
                        buf[write_offset++] = INFO_DEVICE_EQ;
                        buf[write_offset++] = sizeof(ab_mate_app.eq_info);
                        memcpy(&buf[write_offset], &ab_mate_app.eq_info, sizeof(ab_mate_app.eq_info));
                        write_offset += sizeof(ab_mate_app.eq_info);
                    break;

                    case GET_DEVICE_KEY_CONFIG:
                    {
                        PRIV_BLE_DEBUG("GET_DEVICE_KEY_CONFIG\n");  
                       
                       
                        ab_mate_key_info_t *key_left;
                        ab_mate_key_info_t *key_right;
                        #if BT_TWS_EN
                        if(sys_cb.tws_left_channel){
                            key_left = &ab_mate_app.local_key;
                            key_right = &ab_mate_app.remote_key;
                        }else
                        #endif
                        {
                            key_left = &ab_mate_app.remote_key;
                            key_right = &ab_mate_app.local_key;
                        }

                         val_len = payload[read_offset + 1];//拿TLV数据长度

                        u8 key_info[] = {KEY_LEFT_SHORT, 1, key_left->key_short,
                                        KEY_RIGHT_SHORT, 1, key_right->key_short,
                                        KEY_LEFT_DOUBLE, 1, key_left->key_double,
                                        KEY_RIGHT_DOUBLE, 1, key_right->key_double,
                                        KEY_LEFT_THREE, 1, key_left->key_three,
                                        KEY_RIGHT_THREE, 1, key_right->key_three,
                                        KEY_LEFT_LONG, 1, key_left->key_long,
                                        KEY_RIGHT_LONG, 1, key_right->key_long,
                                        };

                         buf[write_offset++] = INFO_DEVICE_KEY_CONFIG;  
                        buf[write_offset++] = sizeof(key_info);
                        memcpy(&buf[write_offset],key_info,sizeof(key_info));
                        write_offset += sizeof(key_info);
                    }       
                        break;

                    case GET_DEVICE_GAMEMODE:
                            PRIV_BLE_DEBUG("GET_DEVICE_GAMEMODE\n");  
                            val_len = payload[read_offset + 1];
                            buf[write_offset++] = INFO_DEVICE_GAMEMODE;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = ab_mate_app.latency_mode;
                        break;

                    case GET_DEVICE_WARNTONE:
                            PRIV_BLE_DEBUG("GET_DEVICE_WARNTONE\n");
                            val_len = payload[read_offset + 1];
#if AB_MATE_LANG_EN
                            buf[write_offset++] = INFO_DEVICE_WARNTONE;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = sys_cb.lang_id;//00:中 01:英 02:铃声
#endif
                        break;

                    case GET_DEVICE_ANC_MODE:
                            PRIV_BLE_DEBUG("GET_DEVICE_ANC_MODE\n");
                            val_len = payload[read_offset + 1];
#if AB_MATE_ANC_EN
                            buf[write_offset++] = INFO_DEVICE_ANC_MODE;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = sys_cb.anc_user_mode;
#endif
                        break;

                    case GET_DEVICE_SHARED_AUDIO:
                            PRIV_BLE_DEBUG("GET_DEVICE_SHARED_AUDIO -- NOT\n");
                            val_len = payload[read_offset + 1];
#if 0//SWETZ
                            buf[write_offset++] = INFO_DEVICE_SHARED_AUDIO;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = ab_mate_app.v3d_audio_en;
#endif
                        break;

                    case GET_DEVICE_IS_SUPPORT_TWS:
                            PRIV_BLE_DEBUG("GET_DEVICE_IS_SUPPORT_TWS\n");
                            val_len = payload[read_offset + 1];
                            buf[write_offset++] = INFO_DEVICE_IS_SUPPORT_TWS;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = BT_TWS_EN * xcfg_cb.bt_tws_en;
                            break;

                    case GET_DEVICE_TWS_STATE:
                            PRIV_BLE_DEBUG("GET_DEVICE_TWS_STATE\n");
                            val_len = payload[read_offset + 1];
                            buf[write_offset++] = INFO_DEVICE_TWS_STATE;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = bt_tws_is_connected();
                            break;
                    case GET_DEVICE_CRC:
                            PRIV_BLE_DEBUG("INFO_CRC:%x\n",ab_mate_app.flash_crc);
                            val_len = payload[read_offset + 1];
                            buf[write_offset++] = INFO_DEVICE_CRC;
                            buf[write_offset++] = 4;
                            memcpy(&buf[write_offset],&ab_mate_app.flash_crc,4);
                            write_offset += 4;                            
                            break;

                    case GET_DEVICE_IS_LEFT_EAR:
                            PRIV_BLE_DEBUG("GET_DEVICE_IS_LEFT_EAR\n");
                            val_len = payload[read_offset + 1];
                            buf[write_offset++] = INFO_DEVICE_IS_LEFT_EAR;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = func_bt_tws_get_channel();
                            break;

                    case GET_DEVICE_SPACE_MUSIC_STATE:
                            PRIV_BLE_DEBUG("GET_DEVICE_SPACE_MUSIC_STATE\n");
                            val_len = payload[read_offset + 1];
#if AB_MATE_V3D_AUDIO_EN
                            buf[write_offset++] = INFO_DEVICE_SPACE_MUSIC_STATE;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = ab_mate_app.v3d_audio_en;
#endif
                            break;        

                    case GET_DEVICE_TWO_PAIR_STATE: 
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_TWO_PAIR_STATE\n");
                            buf[write_offset++] = INFO_DEVICE_TWO_PAIR_STATE;
                            buf[write_offset++] = 1;
                            buf[write_offset++] = ab_mate_app.mult_dev.en;
                            break;

                    case GET_DEVICE_BT_LINKSTATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_TWO_PAIR_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_BT_LINKSTATE;
                            buf[write_offset++] = 30;
                            buf[write_offset++] = 0x01;
                            buf[write_offset++] = 0x01;
                            buf[write_offset++] = bt_get_connected_num();

                            buf[write_offset++] = 0x02;
                            buf[write_offset++] = (6+6+1+strlen(xcfg_cb.le_name));
                            
                          
                            uint addr_len = sizeof(xcfg_cb.bt_addr);
                            if (!bt_get_qr_addr(buf[write_offset])) {
                                bool bt_get_master_addr(u8 *addr);
                                if (!bt_get_master_addr(buf[write_offset])) {
                                    memcpy(buf[write_offset], xcfg_cb.bt_addr, 6);
                                }
                            }
                            buf[write_offset++] += 6;
                            
                            buf[write_offset++] = 0x11;
                            buf[write_offset++] = 0x01;
                            if(bt_decode_is_aac()){
                                    buf[write_offset++] = 0x02;
                            }else if(bt_decode_is_ldac()){
                                    buf[write_offset++] = 0x03;
                            }else{
                                    buf[write_offset++] = 0x01;
                            }

                             buf[write_offset++] = 0x12;
                             buf[write_offset++] = strlen(xcfg_cb.le_name);
                             memcpy(&buf[write_offset], xcfg_cb.le_name, strlen(xcfg_cb.le_name));
                             write_offset += strlen(xcfg_cb.le_name);



                            break;

                    case GET_DEVICE_LDAC_STATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_LDAC_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_LDAC_STATE;
                            break;

                    case GET_DEVICE_CASE_ADDR:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_CASE_ADDR --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_CASE_ADDR;
                            break;              

                    case GET_DEVICE_ANC_DEEP:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_ANC_DEEP --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_ANC_DEEP;
                            break;  

                    case GET_DEVICE_FIND_LEFTEAR_STATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_FIND_LEFTEAR_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_FIND_LEFTEAR_STATE;
                            break;  

                    case GET_DEVICE_FIND_RIGHTEAR_STATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_FIND_RIGHTEAR_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_FIND_RIGHTEAR_STATE;
                            break; 

                    case GET_DEVICE_EAR_INCASE_STATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_EAR_INCASE_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_EAR_INCASE_STATE;
                            break; 

                    case GET_DEVICE_ANC_WIND_NOISE_STATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_ANC_WIND_NOISE_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_ANC_WIND_NOISE_STATE;
                            break; 

                    case GET_DEVICE_SUPPORT_FUNCTION:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_SUPPORT_FUNCTION --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_SUPPORT_FUNCTION;
                            break;

                    case GET_DEVICE_WEAR_STATE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_WEAR_STATE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_WEAR_STATE;
                            break;       

                    case GET_DEVICE_AI_MODE:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_AI_MODE --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_AI_MODE;
                            break;  

                    case GET_DEVICE_MAX_MTU:
                            val_len = payload[read_offset + 1];
                            PRIV_BLE_DEBUG("GET_DEVICE_MAX_MTU --NOT\n");
                            buf[write_offset++] = INFO_DEVICE_MAX_MTU;
                            break;                    

                    default:
                    val_len = payload[read_offset + 1];
                    break;
                
            }
            read_offset += (2 + val_len);
    }
    app_priv_ble_cmd_send.payload_size = write_offset;
    app_priv_ble_cmd_send.header = FIXED_HEADER;
    app_priv_ble_cmd_send.fsn_num = *fsn;
    app_priv_ble_cmd_send.cmd = CMD_GET_DEVICE_INFO;
    //配置ctrl
     app_priv_ble_cmd_send.ctrl &= ~(0x00);//清ctrl
     app_priv_ble_cmd_send.ctrl |= (PRIV_BLE_DATA_MARK_EAR);     //设置方向
     app_priv_ble_cmd_send.ctrl |= (PRIV_BLE_CMD_TYPE_RESPONSE); //设置类型
     app_priv_ble_cmd_send.ctrl |= (PRIV_BLE_NOT_SUBPACKAGE); //设置分包
     app_priv_ble_cmd_send.ctrl |= (PRIV_BLE_DATA_FRAMES_SATRT); 
     app_priv_ble_cmd_send.ctrl |= (PRIV_BLE_DATA_NOT_ENCRYPE);//设置加密
     app_priv_ble_cmd_send.ctrl |= (PRIV_BLE_PLAYLOAD_IS_TLV); //设置是否TLV
    //计算CRC16
    uint16_t expected_crc = CRC16(&app_priv_ble_cmd_send.header, app_priv_ble_cmd_send.payload_size + 5);
    app_priv_ble_cmd_send.payload[write_offset++]   = (expected_crc >> 8) & 0xFF;
    app_priv_ble_cmd_send.payload[write_offset]     = expected_crc & 0xFF;
    //发送
    bt_spp_tx(SPP_SERVICE_CH0, &app_priv_ble_cmd_send.header, app_priv_ble_cmd_send.payload_size + 7);//spp发送
    printf("app tx:");
    print_r(&app_priv_ble_cmd_send.header,app_priv_ble_cmd_send.payload_size + 7);
    
}


static void handle_cmd_request(uint8_t *ptr, uint16_t len)
{
    PRIV_BLE_DEBUG("CMD:%d\r\n",ptr[INDEX_CMD]);
    switch (ptr[INDEX_CMD])
    {
        case CMD_GET_DEVICE_INFO:
            handle_cmd_get_dev_info(&ptr[INDEX_PAYLOAD], ptr[INDEX_PAYLOAD_SIZE], &ptr[INDEX_FSN_NUM]);
            break;

    }


}

void app_priv_ble_handle_received(uint8_t *ptr, uint16_t len)
{
    PRIV_BLE_DEBUG("app ble rx: ");
#if DEBUG_PRIV_BLE
    print_r(ptr, len);
#endif 
    uint16_t expected_crc = (ptr[len - 2] << 8) | ptr[len - 1];
    if(
        (len > HEADER_SIZE)
         && (ptr[INDEX_HEADER] == FIXED_HEADER)//检查包头
         && (((ptr[INDEX_CTRL] & 0x06) >=  PRIV_BLE_CMD_TYPE_REQUEST) && ((ptr[INDEX_CTRL] & 0x06)<= PRIV_BLE_CMD_TYPE_NOTIFY))//检查命令类型
        // && ((ptr[INDEX_PAYLOAD_SIZE] + 7) == len)//检查发来的数据长度
        // && (expected_crc == CRC16(ptr, len - 2))//检查crc16
        ){
             printf("data check succeed CMD_TYPE:%2x \r\n",ptr[INDEX_CTRL]  & 0x06);
        switch (ptr[INDEX_CTRL] & 0x06)
        {
            case PRIV_BLE_CMD_TYPE_REQUEST://请求
            PRIV_BLE_DEBUG("PRIV_BLE_CMD_TYPE_REQUEST\r\n");
            handle_cmd_request(ptr, len);
            break;

            case PRIV_BLE_CMD_TYPE_RESPONSE://通知
             PRIV_BLE_DEBUG("PRIV_BLE_CMD_TYPE_RESPONSE\r\n");
         //   handle_cmd_response(ptr, len);
            break; 

            case PRIV_BLE_CMD_TYPE_NOTIFY://回应
             PRIV_BLE_DEBUG("PRIV_BLE_CMD_TYPE_NOTIFY\r\n");
           // handle_cmd_notify(ptr, len);
            break;


            default:
            break;
        }

    }


}