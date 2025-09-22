#include "include.h"
#include "dueros_dma_profile.h"
#include "dma_wrapper.h"
#include "dueros_dma_app.h"

#if LE_DUEROS_DMA_EN

#define DUEROS_DMA_DEBUG_EN       1

#if DUEROS_DMA_DEBUG_EN
#define DUEROS_DMA_DEBUG(...)                  printf(__VA_ARGS__)
#define DUEROS_DMA_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define DUEROS_DMA_DEBUG(...)
#define DUEROS_DMA_DEBUG_R(...)
#endif

ALIGNED(4)
const uint8_t sdp_spp_dueros_dma_service_record[] = {
    0x36, 0x00, 0x62, 0x09, 0x00, 0x00, 0x0a, 0x50, 0x01, 0x00, 0x03, 0x09, 0x00, 0x01, 0x36, 0x00,
    0x11, 0x1C, DMA_RFCOMM_UUID,
    0x09, 0x00, 0x04, 0x36, 0x00, 0x0e, 0x36, 0x00, 0x03, 0x19, 0x01, 0x00, 0x36, 0x00,
    0x05, 0x19, 0x00, 0x03, 0x08, 0x05, 0x09, 0x00, 0x05, 0x36, 0x00, 0x03, 0x19, 0x10, 0x02, 0x09,
    0x00, 0x06, 0x36, 0x00, 0x09, 0x09, 0x65, 0x6e, 0x09, 0x00, 0x6a, 0x09, 0x01, 0x00, 0x09, 0x00,
    0x09, 0x36, 0x00, 0x09, 0x36, 0x00, 0x06, 0x19, 0x11, 0x01, 0x09, 0x11, 0x02, 0x09, 0x01, 0x00,
    0x25, 0x03, 0x53, 0x50, 0x50,
};

uint8_t adv_data_buf[31] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x02,
};

uint8_t scan_data_buf[31] = {
    //Name
    0x05, 0x09, 'D','U','E','R',
};

uint32_t ble_get_scan_data(uint8_t *scan_buf, uint32_t buf_size)
{
    memcpy(scan_buf, scan_data_buf, buf_size);
    return buf_size;
}

void ble_update_scan_data(uint8_t *scan_buf, uint32_t buf_size)
{
    DUEROS_DMA_DEBUG("ble_update_scan_data\n");
    DUEROS_DMA_DEBUG_R(scan_buf, buf_size);
    if (scan_buf) {
        memcpy(&scan_data_buf[0], scan_buf, buf_size);
        ble_set_scan_rsp_data(&scan_data_buf[0], buf_size);
    }
}

uint32_t ble_get_adv_data(uint8_t *adv_buf, uint32_t buf_size)
{
    memcpy(adv_buf, adv_data_buf, buf_size);
    return buf_size;
}

void ble_update_adv_data(uint8_t *adv_buf, uint32_t buf_size)
{
    DUEROS_DMA_DEBUG("ble_update_adv_data\n");
    DUEROS_DMA_DEBUG_R(adv_buf, buf_size);
    if (adv_buf) {
        memcpy(&adv_data_buf[0], adv_buf, buf_size);
        ble_set_adv_data(&adv_data_buf[0], buf_size);
    }
}


///////////////////////////////////////////////////////////////////////////
#define MAX_NOTIFY_NUM          4
#define MAX_NOTIFY_LEN          DUEROS_DMA_MTU_MAX_LEN     //max=256
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

uint8_t notify_tx_pool[NOTIFY_POOL_SIZE] AT(.buf.dueros_dma);

void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}

const uint8_t dueros_dma_primay_uuid16[2]={DMA_BLE_UUID16_SERVICE};
static const gatts_uuid_base_st uuid_dueros_dma_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = dueros_dma_primay_uuid16,
};

const uint8_t dueros_dma_notify_uuid128[16]={DMA_BLE_UUID_TX_CHARACTERISTIC};
static const gatts_uuid_base_st uuid_dueros_dma_notify_base = {
    .props = ATT_NOTIFY | ATT_DYNAMIC,
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = dueros_dma_notify_uuid128,
};
static gatts_service_base_st gatts_dueros_dma_notify_base;

const uint8_t dueros_dma_write_cmd_uuid128[16]={DMA_BLE_UUID_RX_CHARACTERISTIC};
static const gatts_uuid_base_st uuid_dueros_dma_write_cmd_base = {
    .props = ATT_WRITE_WITHOUT_RESPONSE | ATT_WRITE | ATT_DYNAMIC,
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = dueros_dma_write_cmd_uuid128,
};

static int gatt_callback_dueros_dma_write(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    // DUEROS_DMA_DEBUG("[<----]gatt_callback_dueros_dma_write.\n");
    // DUEROS_DMA_DEBUG_R(ptr, len);
    dueros_dma_recv_proc(ptr, len, DUEROS_DMA_CON_BLE);
    return 0;
}

uint8_t ble_client_cfg_callback(u16 handle, u16 cfg)
{
    DUEROS_DMA_DEBUG("===>ble_client_cfg_callback. handle:%d, cfg:%d\n", handle, cfg);
    dueros_dma_client_cfg_set(cfg);
    if (cfg == 1) {
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_SEND_PREPARE_DONE, NULL, 0, 1);
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_CONNECTED, NULL, 0, 1);
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_CONNECTED, NULL, 0, 0);  //此处为了同步给从机
    }
    return true;
}

bool ble_send_packet(uint8_t *buf, uint8_t len)
{
    int res = ble_tx_notify(gatts_dueros_dma_notify_base.handle, buf, len);
    return res;
}

static ble_gatt_characteristic_cb_info_t dueros_dma_notify_base_cb_info = {
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

static ble_gatt_characteristic_cb_info_t dueros_dma_write_base_cb_info = {
    .att_callback_func = gatt_callback_dueros_dma_write,
};

//----------------------------------------------------------------------------
//
void ble_app_gatts_service_init(void)
{
    int ret = 0;

    //ble_init_att();

    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_dueros_dma_primay_base.uuid,
                                uuid_dueros_dma_primay_base.type,
                                NULL);            //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_dueros_dma_notify_base.uuid,
                                       uuid_dueros_dma_notify_base.type,
                                       uuid_dueros_dma_notify_base.props,
                                       &gatts_dueros_dma_notify_base.handle,
                                       &dueros_dma_notify_base_cb_info);        //characteristic

    ret = ble_gatts_characteristic_add(uuid_dueros_dma_write_cmd_base.uuid,
                                       uuid_dueros_dma_write_cmd_base.type,
                                       uuid_dueros_dma_write_cmd_base.props,
                                       NULL,
                                       &dueros_dma_write_base_cb_info);         //characteristic

    if(ret != BLE_GATTS_SUCCESS){
        DUEROS_DMA_DEBUG("gatt err: %d\n", ret);
        return;
    }
}

//----------------------------------------------------------------------------
//

void dueros_dma_ble_app_init(void)
{
    ble_app_gatts_service_init();
}

#endif
