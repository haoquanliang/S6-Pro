/*
* Copyright (c) 2021 - ruber.wang@qq.com
* All rights, including trade secret rights, reserved.
*/

#ifndef _APP_MESSAGE_H_
#define _APP_MESSAGE_H_

#include "include.h"

#define MESSAGE_NUM (20)

typedef struct {
    uint16_t  msg_id;
    uint16_t  param;
    uint32_t ticks_to;
} message_item_t;

typedef struct {
    message_item_t queue[MESSAGE_NUM];
    uint8_t number;
} message_mgr_t;

typedef enum {
    MSG_ID_INVALID = 0,
    MSG_ID_NOTIFY_PEER_LED_PAIR,
    MSG_ID_PRIV_BLE_CONNECTED_3S,
    MSG_ID_TIMEOUT_5S_AFTER_POWER_ON,
    MSG_ID_NOTIFY_PEER_LED_REMOTE_SET_ENABLE,
    MSG_ID_LED_BT_LINK_LOSS,
    MSG_ID_TIMEOUT_LINK_LOSS_RECONN,
    MSG_ID_SLAVE_RECONN_AG,
    MSG_ID_SLAVE_TIMEOUT_LINKLOSS_RECONN,
    MSG_ID_VP_BT1_CONN,
    MSG_ID_VP_BT1_DISC,
    MSG_ID_VP_MP3_BT1_CONN,
    MSG_ID_VP_PARING,
    MSG_ID_DBB_SWITCH,
    MSG_ID_LR_SYNC_BOX_BT_ADDR,
    MSG_ID_VP_POWER_ON,
    MSG_ID_ANC_AUTO_OFF,
    MSG_ID_CHECK_PAIRING_AFTER_POWER_ON,
    MSG_ID_TIMEOUT_3S_AFTER_POWER_ON,
    MSG_ID_2ND_RINGTONE,
    MSG_ID_TIMEOUT_5S_AFTER_OUT_BOX,
    MSG_ID_FIND_ME_LEFT,
    MSG_ID_FIND_ME_RIGHT,
    MSG_ID_ENABLE_PRIV_BLE_AFTER_POWER_ON,
    MSG_ID_UPDATE_BLE_ADV_NORMAL,
    MSG_ID_UPDATE_INCASE_STA,
} msg_id_t;

extern message_mgr_t message_mgr;

bool message_send(uint16_t msg_id, uint16_t param, uint32_t delay);
void message_cancel_first(uint16_t msg_id);
void message_cancel_all(uint16_t msg_id);
uint8_t message_search(uint16_t msg_id);
bool message_is_empty(void);
bool message_is_ready(void);
void message_init(void);
void message_runnable(void);

#endif 

