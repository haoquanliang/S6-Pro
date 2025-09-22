#include "include.h"
#if APP_DEMO_EN

#if LE_EN

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#define LATT_MAX_NOTIFY_NUM          3
#define LATT_MAX_NOTIFY_LEN          128     //max=256
#define NOTIFY_POOL_SIZE            (LATT_MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * LATT_MAX_NOTIFY_NUM

AT(.ble_cache.att)
uint8_t latt_notify_tx_pool[NOTIFY_POOL_SIZE];

#if BT_ATT_EN
void latt_txpkt_init(void)
{
    printf("latt_txpkt_init\n");
    txpkt_init(&latt_notify_tx, latt_notify_tx_pool, LATT_MAX_NOTIFY_NUM, LATT_MAX_NOTIFY_LEN);
    latt_notify_tx.send_kick = latt_send_kick;
}
#endif

const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x02,
    // Manufacturer Specific Data
    0x03, 0xff, 0x42, 0x06,
    // Complete Local Name
};

const uint8_t scan_data_const[] = {
    // Name
    0x09, 0x09, 'A', 'P', 'P', '-', 'D', 'E', 'M', 'O',
};

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);

    //读取BLE配置的蓝牙名称
    int len;
    len = strlen(xcfg_cb.le_name);
    if (len > 0) {
        len = len > 29 ? 29 : len;
        memcpy(&scan_buf[2], xcfg_cb.le_name, len);
        data_len = 2 + len;
        scan_buf[0] = len + 1;
    }
    return data_len;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

//    //读取BLE配置的蓝牙名称
//    int len;
//    len = strlen(xcfg_cb.le_name);
//    if (len > 0) {
//        memcpy(&adv_buf[9], xcfg_cb.le_name, len);
//        adv_buf[7] = len + 1;
//        data_len = 9 + len;
//    }
    return data_len;
}
#endif

//----------------------------------------------------------------------------
//app service
const uint8_t app_demo_primay_uuid16[16]={0x01, 0x00};
static const gatts_uuid_base_st uuid_app_demo_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_demo_primay_uuid16,
};

const uint8_t app_demo_read_uuid16[16]={0x04,0x00};
static const gatts_uuid_base_st uuid_app_demo_read_base = {
    .props = ATT_NOTIFY | ATT_READ,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_demo_read_uuid16,
};
static gatts_service_base_st app_demo_notify_base AT(.app.buf.gatt);
static ble_gatt_characteristic_cb_info_t app_demo_notify_cb_info = {
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

const uint8_t app_demo_write_uuid16[16]={0x02,0x00};
static const gatts_uuid_base_st uuid_app_demo_write_base = {
    .props = ATT_READ | ATT_WRITE_WITHOUT_RESPONSE,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_demo_write_uuid16,
};
static gatts_service_base_st app_demo_write_base;

#if APP_DEMO_CTKD_EN
static int gatt_callback_app_demo(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);
const uint8_t app_demo_ctkd_uuid16[2] = {0x20,0xff};
static const gatts_uuid_base_st uuid_app_demo_ctkd_base = {
    .props = ATT_WRITE | ATT_AUTHENTICATION_REQUIRED,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_demo_ctkd_uuid16,
};
static ble_gatt_characteristic_cb_info_t gatts_app_demo_ctkd_base_cb_info = {
    .att_callback_func = gatt_callback_app_demo,
};
static gatts_service_base_st app_demo_ctkd_write_base;
#endif /* APP_DEMO_CTKD_EN */

#if LE_EN
int app_demo_ble_notify(u8 *buf, u8 len)
{
    return ble_tx_notify(app_demo_notify_base.handle, buf, len);
}
#endif

#if BT_ATT_EN
int app_demo_latt_notify(u8 *buf, u8 len)
{
    return latt_tx_notify(app_demo_notify_base.handle, buf, len);
}
#endif

#if BT_SPP_EN
int app_demo_spp_send(u8 *buf, u8 len)
{
    return bt_spp_tx(SPP_SERVICE_CH0, buf, len);
}

bool app_demo_spp_rx_callback(u8 *data, u16 len)
{
    TRACE("app demo spp<--[%d]:\n", len);
    TRACE_R(data, len);
    return true;
}
#endif

static int gatt_callback_app_demo(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    if (flag & ATT_CB_FALG_DIR_WRITE) {
        u8 con_type = gap_get_connection_type(con_handle);
        if (handle == app_demo_write_base.handle) {
            if (con_type == GAP_CONNECTION_LE) {
                TRACE("app demo ble<--[%d]:\n", len);
                TRACE_R(ptr, len);
            } else {
#if BT_ATT_EN
                TRACE("app demo latt<--(%d)\n:", len);
                TRACE_R(ptr, len);
#endif
            }
        }
#if APP_DEMO_CTKD_EN
        else if (app_demo_ctkd_write_base.handle) {
            TRACE("app_demo_ctkd_write[%d]\n", len);
            TRACE_R(ptr, len);
        }
#endif /* APP_DEMO_CTKD_EN */
    }
    return 0;
}

static ble_gatt_characteristic_cb_info_t gatts_app_demo_cb_info = {
    .att_callback_func = gatt_callback_app_demo,
};

void app_demo_gatts_service_init(void)
{
    int ret = 0;
    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_app_demo_primay_base.uuid,
                                uuid_app_demo_primay_base.type,
                                NULL);            //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_app_demo_read_base.uuid,
                                       uuid_app_demo_read_base.type,
                                       uuid_app_demo_read_base.props,
                                       &app_demo_notify_base.handle,
                                       &app_demo_notify_cb_info);      //characteristic

    ret = ble_gatts_characteristic_add(uuid_app_demo_write_base.uuid,
                                       uuid_app_demo_write_base.type,
                                       uuid_app_demo_write_base.props,
                                       &app_demo_write_base.handle,
                                       &gatts_app_demo_cb_info);      //characteristic
#if APP_DEMO_CTKD_EN
    ret = ble_gatts_characteristic_add(uuid_app_demo_ctkd_base.uuid,
                                       uuid_app_demo_ctkd_base.type,
                                       uuid_app_demo_ctkd_base.props,
                                       &app_demo_ctkd_write_base.handle,
                                       &gatts_app_demo_ctkd_base_cb_info);      //characteristic
#endif /* APP_DEMO_CTKD_EN */

    if(ret != BLE_GATTS_SUCCESS){
        printf("app demo gatt err: %d\n", ret);
        return;
    }
}

void bt_latt_connect_callback(uint16_t conn_handle)
{
    printf("[BT]: latt conn\n");
}

void bt_latt_disconnect_callback(uint16_t conn_handle)
{
    printf("[BT]: latt disconn\n");
}

#if APP_DEMO_CTKD_EN
int sm_pairing_fail_callback(u8 *param)
{
    TRACE("sm_pairing_fail_callback\n");
    ble_disconnect();

    return 0;
}

int sm_timeout_callback(u8 *param)
{
    TRACE("sm_timeout_callback\n");
    ble_disconnect();

    return 0;
}
#endif /* APP_DEMO_CTKD_EN */

//----------------------------------------------------------------------------
//
void app_demo_gatt_app_init(void)
{
    app_demo_gatts_service_init();
}
#endif
