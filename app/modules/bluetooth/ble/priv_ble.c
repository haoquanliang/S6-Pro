/*
 *
 *  BLE私有连接相关
 */
#include "include.h"
#if LE_PRIV_EN
int priv_le_gatt_callback_func(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);
void reverse_bd_addr(const u8 *src, u8 *dest);;
//-------------------------------------------------------------------
//------------------libbtstack.a回调函数-----------------------------
//-------------------------------------------------------------------

//tws时通常由主耳广播，该回调函数决定是否广播
//返回值: true=广播, false=不广播
bool ble_priv_adv_get_tws_role(void) {
    if(bt_tws_is_slave()) {
        return true;
    }
    return false;
}

void ble_priv_adv_get_addr_cb(uint8_t *addr, uint8_t *addr_type)
{
    u8 temp_addr[6];
    bt_get_local_bd_addr(temp_addr);
    *addr_type = 0; //0:pubilc 1:rand
	reverse_bd_addr(temp_addr,addr);
}

void ble_priv_connect_callback(void)
{
    printf("--->ble_priv_connect_callback\n");
}

void ble_priv_disconnect_callback(void)
{
    printf("--->ble_priv_disconnect_callback\n");
}

//设置广播内容格式，返回内容长度（最长31byte）
uint8_t ble_priv_adv_get_adv_data_cb(uint8_t *adv_data)
{
    uint8_t data_len = 0;
    adv_data[data_len++] = 0x03;         //[0]   len
    adv_data[data_len++] = 0xff;         //[1]   data_type
    adv_data[data_len++] = 0x42;         //[2:3] comany_id
    adv_data[data_len++] = 0x06;         //[2:3] comany_id

    //读取BLE配置的蓝牙名称
    int len;
    len = strlen(xcfg_cb.le_name);
    if (len > 0) {
        len = len > 20 ? 20 : len;
        adv_data[data_len++] = len + 1 + 5; //ad type + 'priv' + name len
        adv_data[data_len++] = 0x09;
        memcpy(&adv_data[data_len], xcfg_cb.le_name, len);
        data_len += len;
        adv_data[data_len++] = '-';
        adv_data[data_len++] = 'P';
        adv_data[data_len++] = 'R';
        adv_data[data_len++] = 'I';
        adv_data[data_len++] = 'V';
    }
    return data_len;
}

#if !AB_MATE_APP_EN
#define MAX_NOTIFY_NUM          5
#define MAX_NOTIFY_LEN          256     //max=256
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

AT(.ble_cache.att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}
#endif

const uint8_t priv_primay_uuid16[2] =   {0xB5,0xFD};
const uint8_t priv_cha_uuid16[2]    =   {0x20,0xFF};
static gatts_service_base_st priv_cha_base;

static ble_gatt_characteristic_cb_info_t priv_cha_base_cb_info = {
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
    .att_callback_func = priv_le_gatt_callback_func,
};

int priv_le_gatt_callback_func(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    if (flag & ATT_CB_FALG_DIR_WRITE) { // write
        printf("priv_le_gatt_callback_func\n");
    } else { //read
    }
    return 0;
}

void priv_ble_tx_data(uint8_t *buf, uint16_t len)
{
    ble_priv_tx_notify(priv_cha_base.handle, buf, len);
}

void priv_ble_gatts_service_init(void)
{
    ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                priv_primay_uuid16,
                                BLE_GATTS_UUID_TYPE_16BIT,
                                NULL);            //PRIMARY

    ble_gatts_characteristic_add(priv_cha_uuid16,
                                 BLE_GATTS_UUID_TYPE_16BIT,
                                 ATT_WRITE | ATT_READ | ATT_NOTIFY,
                                 &priv_cha_base.handle,
                                 &priv_cha_base_cb_info);      //characteristic
}
#else
void priv_ble_gatts_service_init(void)
{
}
#endif // LE_PRIV_EN
