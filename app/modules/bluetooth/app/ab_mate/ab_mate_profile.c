#include "include.h"
#include "ab_mate_profile.h"

#if AB_MATE_APP_EN

#ifdef AB_MATE_SPP_UUID
const uint8_t sdp_ab_mate_spp_service_record[] = {
    0x36, 0x00, 0x62, 0x09, 0x00, 0x00, 0x0a, 0x50, 0x01, 0x00, 0x03, 0x09, 0x00, 0x01, 0x36, 0x00,
    0x11, 0x1C, AB_MATE_SPP_UUID, 0x09, 0x00, 0x04, 0x36, 0x00, 0x0e, 0x36, 0x00, 0x03, 0x19, 0x01,
    0x00, 0x36, 0x00, 0x05, 0x19, 0x00, 0x03, 0x08, 0x06, 0x09, 0x00, 0x05, 0x36, 0x00, 0x03, 0x19,
    0x10, 0x02, 0x09, 0x00, 0x06, 0x36, 0x00, 0x09, 0x09, 0x65, 0x6e, 0x09, 0x00, 0x6a, 0x09, 0x01,
    0x00, 0x09, 0x00, 0x09, 0x36, 0x00, 0x09, 0x36, 0x00, 0x06, 0x19, 0x11, 0x01, 0x09, 0x11, 0x02,
    0x09, 0x01, 0x00, 0x25, 0x03, 0x53, 0x50, 0x50,
};
#endif

#if !GFPS_EN
#define ADV_VID_POS     (3 + 4)
#define ADV_MAC_POS     (3 + 7)
#define ADV_FMASK_POS   (3 + 13)
#define ADV_BID_POS     (3 + 14)

static u8 *p_adv_data = NULL;
static u8 adv_data_len = 0;
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#if !APP_BLE_RADIO
static const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x02,
                //CID      VID   PID        MAC                            FMASK  BID
    0x10, 0xff, 0x42,0x06, 0x01, 0x01,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,  0x00,0x00,0x00,

    0x03, 0x03, 0xB3, 0xFD,
};
#else
static const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x18,
                //CID      VID   PID        MAC                            FMASK  BID
    0x10, 0xff, 0x42,0x06, 0x02, 0x01,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,  0x00,0x04,0x00,

    0x03, 0x03, 0xB3, 0xFD,
};


#endif
static const uint8_t scan_data_const[] = {
    //Name
    0x04, 0x09, 'B','L','E',
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
    u8 edr_addr[6];
    u32 data_len = sizeof(adv_data_const);
    u32 bid = AB_MATE_BID;

    p_adv_data = adv_buf;

    memset(adv_buf, 0, buf_size);

    memcpy(adv_buf, adv_data_const, data_len);

    adv_buf[ADV_VID_POS] = AB_MATE_VID;

    bt_get_local_bd_addr(edr_addr);

    //广播包协议从版本1之后，经典蓝牙地址都做个简单的加密操作，不直接暴露地址
    if(AB_MATE_VID > 1){
        for(u8 i = 0; i < 6; i++){
            edr_addr[i] ^= 0xAD;
        }
    }

    memcpy(&adv_buf[ADV_MAC_POS], edr_addr, 6);

    memcpy(&adv_buf[ADV_BID_POS], &bid, 3);

#if AB_MATE_CTKD_EN
    adv_buf[ADV_FMASK_POS] |= BIT(1);
#endif

#ifdef AB_MATE_SPP_UUID
    adv_buf[ADV_FMASK_POS] |= BIT(4);
#endif

    adv_data_len = data_len;

    return data_len;
}

void ble_adv_data_update_byte(u8 pos, u8* val, u8 len, u8 proc)
{
    if (p_adv_data) {
        memcpy(&p_adv_data[pos], val, len);

        if(proc){
            ble_set_adv_data(p_adv_data, adv_data_len);
        }
    }
}

void ble_adv_data_update_bit(u8 pos, u8 bit, u8 val, u8 len, u8 proc)
{
    if (p_adv_data) {
        for(u8 i= 0; i< len; i++){
            p_adv_data[pos] &= ~(1 << (bit + i));
        }

        p_adv_data[pos] |= (val << bit);

        if(proc){
            ble_set_adv_data(p_adv_data, adv_data_len);
        }
    }
}

void ab_mate_update_ble_adv_fmsk(u8 bit, u8 val, u8 len, u8 proc)
{
    ble_adv_data_update_bit(ADV_FMASK_POS, bit, val, len, proc);
}

void ab_mate_update_ble_adv_bt_sta(u8 val, u8 proc)
{
    ab_mate_update_ble_adv_fmsk(2, val, 2, proc);
}
#endif  //!GFPS_EN

///////////////////////////////////////////////////////////////////////////
#define MAX_NOTIFY_NUM          5
#if AB_MATE_EQ_USE_DEVICE
#define MAX_NOTIFY_LEN          256     //max=256
#else
#define MAX_NOTIFY_LEN          185     //max=256
#endif
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

AT(.ble_cache.att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

#if !AB_MATE_BT_ATT_EN
void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}
#else
void latt_txpkt_init(void)
{
    txpkt_init(&latt_notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    latt_notify_tx.send_kick = latt_send_kick;
    memset(&notify_tx, 0x00, sizeof(notify_tx)); // 需要将notify_tx清0，否则会是一个随机值，ble断连时可能会出现异常
}
#endif

const uint8_t ab_mate_primay_uuid16[2]={0xB3,0xFD};
static const gatts_uuid_base_st uuid_ab_mate_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_primay_uuid16,
};

const uint8_t ab_mate_notify_uuid16[2]={0x18,0xff};
static const gatts_uuid_base_st uuid_ab_mate_notify_base = {
    .props = ATT_NOTIFY | ATT_READ,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_notify_uuid16,
};
static gatts_service_base_st gatts_ab_mate_notify_base AT(.app.buf.gatt);

const uint8_t ab_mate_write_uuid16[2]={0x16,0xff};
static const gatts_uuid_base_st uuid_ab_mate_write_base = {
    .props = ATT_WRITE | ATT_READ,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_write_uuid16,
};

const uint8_t ab_mate_write_cmd_uuid16[2]={0x17,0xff};
static const gatts_uuid_base_st uuid_ab_mate_write_cmd_base = {
    .props = ATT_WRITE_WITHOUT_RESPONSE | ATT_WRITE,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_write_cmd_uuid16,
};

#if AB_MATE_CTKD_EN
const uint8_t ab_mate_ctkd_uuid16[2]={0x20,0xff};
static const gatts_uuid_base_st uuid_ab_mate_ctkd_base = {
    .props = ATT_WRITE | ATT_AUTHENTICATION_REQUIRED,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_ctkd_uuid16,
};
static ble_gatt_characteristic_cb_info_t ab_mate_ctkd_base_cb_info;
#endif

//Generic Access services
const uint8_t ab_mate_generic_access_primay_uuid16[2]={0x00,0x18};
static const gatts_uuid_base_st uuid_ab_mate_generic_access_primay_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_generic_access_primay_uuid16,
};

const uint8_t ab_mate_dev_name_uuid16[2]={0x00,0x2a};
static const gatts_uuid_base_st uuid_ab_mate_dev_name_base = {
    .props = ATT_READ,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_dev_name_uuid16,
};
static gatts_service_base_st gatts_ab_mate_dev_name_base AT(.app.buf.gatt);

//generic_attribute
const uint8_t ab_mate_generic_attribute_uuid[2]={0x01,0x18};
static const gatts_uuid_base_st uuid_ab_mate_generic_attribute_base = {
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_generic_attribute_uuid,
};

const uint8_t ab_mate_service_changed_uuid[16]={0x05,0x2a};
static const gatts_uuid_base_st uuid_ab_mate_service_changed_base = {
    .props = ATT_READ | ATT_INDICATE,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ab_mate_service_changed_uuid,
};
static gatts_service_base_st gatts_ab_mate_service_changed_base AT(.app.buf.gatt);

static gatts_service_base_st gatts_ab_mate_app_service_base AT(.app.buf.gatt);

static int gatt_callback_ab_mate_write(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    if (flag & ATT_CB_FALG_DIR_WRITE) {
        u8 con_type = AB_MATE_CON_NONE;
        if (gap_get_connection_type(con_handle) == GAP_CONNECTION_LE) {
            con_type = AB_MATE_CON_BLE;
        } else {
            con_type = AB_MATE_CON_LATT;
        }
#if AB_MATE_BT_ATT_EN
        u16 client_cfg;
        memcpy(&client_cfg, ptr, 2);
        if (con_type == AB_MATE_CON_LATT) {
            if(client_cfg == GATT_CLIENT_CONFIG_NOTIFY){
                printf("[AB_MATE]: latt conn\n");
                ab_mate_connect_proc(1);
            }else if(client_cfg == 0){
                printf("[AB_MATE]: latt disconn\n");
                ab_mate_disconnect_proc(1);
            }
        }
#endif
	    if (ab_mate_app.update_param_flag) {
	        ab_mate_app.update_param_flag = 0;
	        ble_update_conn_param(AB_MATE_CON_INTERVAL, 0, 400);
	    }

	    ab_mate_receive_proc(ptr,len, con_type);
	}
    return 0;
}

static ble_gatt_characteristic_cb_info_t ab_mate_write_cmd_cb_info = {
    .att_callback_func = gatt_callback_ab_mate_write,
};

static ble_gatt_characteristic_cb_info_t ab_mate_write_base_cb_info = {
    .att_callback_func = gatt_callback_ab_mate_write,
};

static ble_gatt_characteristic_cb_info_t ab_mate_notify_base_cb_info = {
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

static ble_gatt_characteristic_cb_info_t gatt_callback_ab_mate_service_changed = {
    .client_config = GATT_CLIENT_CONFIG_INDICATE,
};

static ble_gatt_characteristic_cb_info_t ab_mate_dev_name_cb_info;



int ab_mate_ble_send_packet(u8 *buf, u8 len)
{
#if SWETZ 
    TRACE("ear_ble_tx:");
    TRACE_R(buf, len);
#endif  
    return ble_tx_notify(gatts_ab_mate_notify_base.handle, buf, len);
}

#if AB_MATE_BT_ATT_EN
int ab_mate_latt_send_packet(u8 *buf, u8 len)
{
    return latt_tx_notify(gatts_ab_mate_notify_base.handle, buf, len);
}
#endif

#if AB_MATE_BT_ATT_EN
void gatt_callback_ab_mate_client_config_write(u16 handle, u16 cfg)
{
    if(handle == gatts_ab_mate_notify_base.handle){ //client_config判断IOS APP有没有连接
        if(cfg == GATT_CLIENT_CONFIG_NOTIFY){
            printf("[AB_MATE]: latt conn\n");
            ab_mate_connect_proc(1);
        }else if(cfg == 0){
            printf("[AB_MATE]: latt disconn\n");
            ab_mate_disconnect_proc(1);
        }
    }
}

// libs weak fun
void ble_client_cfg_callback(u16 handle, u16 cfg)
{
    gatt_callback_ab_mate_client_config_write(handle, cfg);
}
#endif

// 发送服务变更indicate
int ab_mate_gatt_service_changed_indicate(void)
{
    u16 offset = 0;
    u8 buf[4];
    u16 start_handle = gatts_ab_mate_app_service_base.handle;
    u16 end_handle = 0xffff;
    printf("%s start_handle:%x end_handle:%x\n", __func__, start_handle, end_handle);
    memcpy(&buf[offset], &start_handle, sizeof(u16));
    offset += sizeof(u16);
    memcpy(&buf[offset], &end_handle, sizeof(u16));
    offset += sizeof(u16);
    int ret = -1;
#if !AB_MATE_BT_ATT_EN
    ret = ble_tx_notify(gatts_ab_mate_service_changed_base.handle, buf, offset);
#else
    ret = latt_tx_notify(gatts_ab_mate_service_changed_base.handle, buf, offset);
#endif
    return ret;
}

//----------------------------------------------------------------------------
//
void ble_app_gatts_service_init(void)
{
    int ret = 0;
    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_ab_mate_generic_access_primay_base.uuid,
                                uuid_ab_mate_generic_access_primay_base.type,
                                NULL);                                                      //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_ab_mate_dev_name_base.uuid,
                                       uuid_ab_mate_dev_name_base.type,
                                       uuid_ab_mate_dev_name_base.props,
                                       &gatts_ab_mate_dev_name_base.handle,
                                       NULL
                                      );                                                    //characteristic
    ab_mate_dev_name_cb_info.value = (u8*)&xcfg_cb.le_name;
    ab_mate_dev_name_cb_info.value_len = strlen(xcfg_cb.le_name);
    ble_gatt_init_att_info(gatts_ab_mate_dev_name_base.handle, &ab_mate_dev_name_cb_info);

    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_ab_mate_generic_attribute_base.uuid,
                                uuid_ab_mate_generic_attribute_base.type,
                                NULL);                                                      //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_ab_mate_service_changed_base.uuid,
                                       uuid_ab_mate_service_changed_base.type,
                                       uuid_ab_mate_service_changed_base.props,
                                       &gatts_ab_mate_service_changed_base.handle,
                                       &gatt_callback_ab_mate_service_changed);             //characteristic

    ret = ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                uuid_ab_mate_primay_base.uuid,
                                uuid_ab_mate_primay_base.type,
                                &gatts_ab_mate_app_service_base.handle);                    //PRIMARY

    ret = ble_gatts_characteristic_add(uuid_ab_mate_notify_base.uuid,
                                       uuid_ab_mate_notify_base.type,
                                       uuid_ab_mate_notify_base.props,
                                       &gatts_ab_mate_notify_base.handle,
                                       &ab_mate_notify_base_cb_info);                       //characteristic

    ret = ble_gatts_characteristic_add(uuid_ab_mate_write_base.uuid,
                                       uuid_ab_mate_write_base.type,
                                       uuid_ab_mate_write_base.props,
                                       NULL,
                                       &ab_mate_write_base_cb_info);                        //characteristic

    ret = ble_gatts_characteristic_add(uuid_ab_mate_write_cmd_base.uuid,
                                       uuid_ab_mate_write_cmd_base.type,
                                       uuid_ab_mate_write_cmd_base.props,
                                       NULL,
                                       &ab_mate_write_cmd_cb_info);                         //characteristic

#if AB_MATE_CTKD_EN
    ret = ble_gatts_characteristic_add(uuid_ab_mate_ctkd_base.uuid,
                                       uuid_ab_mate_ctkd_base.type,
                                       uuid_ab_mate_ctkd_base.props,
                                       NULL,
                                       &ab_mate_ctkd_base_cb_info);      //characteristic
#endif

    if(ret != BLE_GATTS_SUCCESS){
        printf("gatt err: %d\n", ret);
        return;
    }
}


//----------------------------------------------------------------------------
//
void ab_mate_ble_app_init(void)
{
    ble_app_gatts_service_init();
}

#if AB_MATE_CTKD_EN

int sm_pairing_fail_callback(u8 *param)
{
    ble_disconnect();

    return 0;
}

int sm_timeout_callback(u8 *param)
{
    ble_disconnect();

    return 0;
}

#endif

#endif
