#ifndef _APP_LR_H_
#define _APP_LR_H_

/*
 * packet format
 *  | header  | seq num | cmd id | payload size | payload     | crc8_maxim |
 *  | 1 byte  | 1 byte  | 1 byte | 1 byte       | size*1 byte | 1 byte     |
 *  | 0x55    | xx      | xx     | xx           | .......     | xx         |
 */

typedef enum 
{
    LR_NOTIFY_INVALID,
    LR_NOTIFY_SLAVE_KEY,
    LR_NOTIFY_BATTERY_LEVEL,  
    LR_NOTIFY_SLAVE_SYNC_INFO,
    LR_NOTIFY_MASTER_SYNC_INFO,
    LR_NOTIFY_MSG,
    LR_NOTIFY_CLEAR_TWS_AG_INFO,
    LR_NOTIFY_SYNC_DM_INFO,
    LR_NOTIFY_PAIRING_INFO,
    LR_NOTIFY_CLEAR_AG_INFO,  
    LR_NOTIFY_LED_PAIRING,
    LR_NOTIFY_LED_REMOTE_SET_ENABLE,
    LR_NOTIFY_SYNC_BOX_BT_ADDR,
    LR_NOTIFY_FIND_ME_LEFT_STOP,
    LR_NOTIFY_FIND_ME_RIGHT_STOP,
    LR_NOTIFY_IN_CASE_STATUS,
    LR_NOTIFY_SYNC_ACTIVE_ADDR,
    LR_NOTIFY_SYNC_L_FIND_STA,
    LR_NOTIFY_SYNC_R_FIND_STA,    
}LR_CMD_T;

void app_lr_init(void);
void app_lr_send_notification(u8 cmd_id, u8 payload_size, u8 *payload);
void app_lr_parse_command(void); //handle received data from peer device
void app_lr_send_msg(u16 msg);
u16 app_lr_tws_get_data(u8 *buf);
bool app_lr_tws_set_data(u8 *data, u16 size);
bool app_lr_is_idle(void);

#endif 
