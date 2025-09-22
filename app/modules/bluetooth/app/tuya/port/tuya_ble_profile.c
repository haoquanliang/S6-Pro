#include "include.h"
#include "tuya_ble_api.h"

#if LE_TUYA_EN
///////////////////////////////////////////////////////////////////////////
const uint8_t adv_data_const[] = {
    0x02, 0x01, 0x06,
    0x03, 0x02, 0x50, 0xFD,
    0x17, 0x16, 0x50, 0xFD,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00
};

const uint8_t scan_data_const[] = {
    0x17, 0xFF, 0xD0, 0x07,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00,0x00,0x00,
    0x05, 0x09, 'T','u','y','a'
};

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);

    return data_len;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

    return data_len;
}

void ble_get_local_bd_addr(u8 *addr)
{
    tuya_ble_addr_get(addr);
}


///////////////////////////////////////////////////////////////////////////
#define MAX_NOTIFY_NUM          4
#define MAX_NOTIFY_LEN          185     //max=256
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

AT(.ble_cache.att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}

const uint8_t tuya_primay_uuid16[2]={0x50, 0xFD};
static const gatts_uuid_base_st uuid_tuya_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = tuya_primay_uuid16,
};

const uint8_t tuya_notify_uuid128[16]={0xD0,0x07,0x9B,0x5F,0x80,0x00,0x01,0x80,0x01,0x10,0x00,0x00,0x02,0x00,0x00,0x00};
static const gatts_uuid_base_st uuid_tuya_notify_base = {
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = tuya_notify_uuid128,
};
static gatts_service_base_st gatts_tuya_notify_base;

const uint8_t tuya_write_uuid128[16]={0xD0,0x07,0x9B,0x5F,0x80,0x00,0x01,0x80,0x01,0x10,0x00,0x00,0x01,0x00,0x00,0x00};
static const gatts_uuid_base_st uuid_tuya_write_base = {
    .props = ATT_WRITE_WITHOUT_RESPONSE,
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = tuya_write_uuid128,
};

static int gatt_callback_tuya_write(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    tuya_ble_gatt_receive_data(ptr,len);
    tuya_ble_app.data_receive_flag = 1;
    return true;
}


bool ble_send_packet(u8 *buf, u8 len)
{
    if(len > MAX_NOTIFY_LEN){
        printf("--->ERROR:ble_send_packet\n");
    }
    return (ble_tx_notify(gatts_tuya_notify_base.handle, buf, len) == 0);
}


static ble_gatt_characteristic_cb_info_t  tuya_notify_cb_info = {
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

static ble_gatt_characteristic_cb_info_t  tuya_write_cb_info = {
    .att_callback_func = gatt_callback_tuya_write,
};

//----------------------------------------------------------------------------
//
void ble_app_gatts_service_init(void)
{
    printf("tuya gatt init\n");
    int ret = 0;

    // ble_gatts_init(); // 893的 ble_gatt_init 不在这里做，在ble.c里就做完了

    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_tuya_primay_base.uuid,
                                uuid_tuya_primay_base.type,
                                NULL);            //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_tuya_notify_base.uuid,
                                       uuid_tuya_notify_base.type,
                                       uuid_tuya_notify_base.props,
                                       &gatts_tuya_notify_base.handle,
                                       &tuya_notify_cb_info);      //characteristic

    ret = ble_gatts_characteristic_add(uuid_tuya_write_base.uuid,
                                       uuid_tuya_write_base.type,
                                       uuid_tuya_write_base.props,
                                       NULL,
                                       &tuya_write_cb_info);      //characteristic


    if(ret != BLE_GATTS_SUCCESS){
        printf("gatt err: %d\n", ret);
        return;
    }
}


//----------------------------------------------------------------------------
//
void tuya_ble_app_ctrl_init(void)
{
    ble_app_gatts_service_init();
}

#endif // LE_APP_EN
