#include "include.h"
#include "ab_mate_profile.h"

#if AB_MATE_APP_EN && GFPS_EN

#define ADV_CID_POS     (2)
#define ADV_VID_POS     (4)
#define ADV_MAC_POS     (7)
#define ADV_FMASK_POS   (13)
#define ADV_BID_POS     (14)

#define SPP_TX_BUF_INX      4
#define SPP_TX_BUF_LEN      128     //max=512
#define SPP_POOL_SIZE       (SPP_TX_BUF_LEN + sizeof(struct txbuf_tag)) * SPP_TX_BUF_INX

static u8 *p_adv_data = NULL;
static u8 adv_data_len = 0;
static u8 *p_scan_data = NULL;
static u8 scan_data_len = 0;

AT(.gfps.stack.spp)
uint8_t spp_ch1_tx_pool[SPP_POOL_SIZE];

void spp_ch1_txpkt_init(void)
{
    txpkt_init(&spp_tx_ch1, spp_ch1_tx_pool, SPP_TX_BUF_INX, SPP_TX_BUF_LEN);
    spp_tx_ch1.send_kick = spp_send_kick;
}

static const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x02,
    // GFPS, Tx Power, -20dBm
    0x02, 0x0A, 0xF5,
};

#if !AB_MATE_ADV0_EN
static const uint8_t scan_data_const[] = {
                //CID      VID   PID        MAC                            FMASK  BID
    0x10, 0xff, 0x42,0x06, 0x01, 0x01,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,  0x00,0x00,0x00,
    //Name
    0x04, 0x09, 'B','L','E',
};
#endif

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    printf("get_ab_scan 1\n");
#if AB_MATE_ADV0_EN
    //读取BLE配置的蓝牙名称
    char gfp_str[] = "-GFP";
    uint8_t offset = 0;
    uint8_t name_len = strlen(xcfg_cb.le_name);
    uint8_t gfp_len = strlen(gfp_str);
    name_len = name_len > (29 - gfp_len) ? (29 - gfp_len) : name_len;

    scan_buf[offset++] = name_len + gfp_len + 1;
    scan_buf[offset++] = 0x09; // COMPLETE_LOCAL_NAME TYPE
    memcpy(&scan_buf[offset], xcfg_cb.le_name, name_len);
    offset += name_len;
    memcpy(&scan_buf[offset], gfp_str, gfp_len);
    offset += name_len;

    return offset;
#else
    u8 edr_addr[6];
    u32 bid = AB_MATE_BID;

    p_scan_data = scan_buf;

    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);

    scan_buf[ADV_VID_POS] = AB_MATE_VID;

    bt_get_local_bd_addr(edr_addr);

    //广播包协议从版本1之后，经典蓝牙地址都做个简单的加密操作，不直接暴露地址
    if(AB_MATE_VID > 1){
        for(u8 i = 0; i < 6; i++){
            edr_addr[i] ^= 0xAD;
        }
    }

    memcpy(&scan_buf[ADV_MAC_POS], edr_addr, 6);

    memcpy(&scan_buf[ADV_BID_POS], &bid, 3);

#if AB_MATE_CTKD_EN
    scan_buf[ADV_FMASK_POS] |= BIT(1);
#endif

#ifdef AB_MATE_SPP_UUID
    scan_buf[ADV_FMASK_POS] |= BIT(4);
#endif

    //读取BLE配置的蓝牙名称
    int len;
    len = strlen(xcfg_cb.le_name);
    len = len > 12 ? 12 : len;

    if (len > 0) {
        memcpy(&scan_buf[19], xcfg_cb.le_name, len);
        data_len = 19 + len;
        scan_buf[17] = len + 1;
    }

    scan_data_len = data_len;

    return data_len;
#endif /* !AB_MATE_ADV0_EN */
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    printf("get_ab_adv 1\n");
    p_adv_data = adv_buf;

    memset(adv_buf, 0, buf_size);

    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

    adv_data_len = data_len;

    return data_len;
}

#if AB_MATE_ADV0_EN
static const uint8_t ab_mate_data_adv0[] = {
//  <--HEAD-->  <--CID-->   <VID>  <--PID-->   <-------------MAC----------->  <FMASK>  <-----BID----->
    0x10, 0xff, 0x42,0x06,  0x01,  0x01,0x00,  0x00,0x00,0x00,0x00,0x00,0x00,  0x00,   0x00,0x00,0x00,
};

uint8_t ble_adv0_get_adv_data_cb(uint8_t *adv_data)
{
    printf("get_ab_adv 2\n");
    u8 edr_addr[6];
    u32 data_len = 0;
    u32 bid = AB_MATE_BID;

    p_adv_data = adv_data;

    memset(adv_data, 0, 31); // adv0_data size 31
    data_len = sizeof(ab_mate_data_adv0);
    memcpy(adv_data, ab_mate_data_adv0, data_len);

    adv_data[ADV_VID_POS] = AB_MATE_VID;

    bt_get_local_bd_addr(edr_addr);

    //广播包协议从版本1之后，经典蓝牙地址都做个简单的加密操作，不直接暴露地址
    if (AB_MATE_VID > 1) {
        for (u8 i = 0; i < 6; i++) {
            edr_addr[i] ^= 0xAD;
        }
    }
    memcpy(&adv_data[ADV_MAC_POS], edr_addr, 6);
    memcpy(&adv_data[ADV_BID_POS], &bid, 3);

#if AB_MATE_CTKD_EN
    adv_data[ADV_FMASK_POS] |= BIT(1);
#endif
#ifdef AB_MATE_SPP_UUID
    adv_data[ADV_FMASK_POS] |= BIT(4);
#endif
    return data_len;
}

//设置广播内容格式，返回内容长度（最长31byte）
uint8_t ble_adv0_get_scan_rsp_data_cb(uint8_t *scan_rsp_data)
{
    printf("get_ab_scan 2\n");
    //读取BLE配置的蓝牙名称
    uint8_t offset = 0;
    uint8_t name_len = strlen(xcfg_cb.le_name);
    name_len = name_len > 29 ? 29 : name_len;

    scan_rsp_data[offset++] = name_len + 1;
    scan_rsp_data[offset++] = 0x09; // COMPLETE_LOCAL_NAME TYPE
    memcpy(&scan_rsp_data[offset], xcfg_cb.le_name, name_len);
    offset += name_len;

    return offset;
}

//tws时通常由主耳广播，该回调函数决定是否广播
//返回值: true=广播, false=不广播
bool ble_adv0_get_tws_role(void)
{
#if BT_TWS_EN
    if(bt_tws_is_slave()) {
        return true;
    }
    return false;
#else
    return false;
#endif
}

AT(.text.bb.ble.adv0)
void ble_adv0_get_type_cb(uint8_t *adv_type)
{
    *adv_type = 0;  //0=可连接，3=不可连接
}

//更新广播
void ble_adv0_update_adv(void)
{
    ble_adv0_set_ctrl(2);
}

//更新扫描响应
void ble_adv0_update_scan_rsp(void)
{
    ble_adv0_set_ctrl(3);
}
#endif /* AB_MATE_ADV0_EN */

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

void ble_scan_rsp_update_byte(u8 pos, u8* val, u8 len, u8 proc)
{
    if (p_scan_data) {
        memcpy(&p_scan_data[pos], val, len);

        if(proc){
            ble_set_scan_rsp_data(p_scan_data, scan_data_len);
        }
    }
}

void ble_scan_rsp_update_bit(u8 pos, u8 bit, u8 val, u8 len, u8 proc)
{
    if (p_scan_data) {
        for(u8 i= 0; i< len; i++){
            p_scan_data[pos] &= ~(1 << (bit + i));
        }

        p_scan_data[pos] |= (val << bit);

        if(proc){
            ble_set_scan_rsp_data(p_scan_data, scan_data_len);
        }
    }
}

void ab_mate_update_ble_adv_fmsk(u8 bit, u8 val, u8 len, u8 proc)
{
    ble_scan_rsp_update_bit(ADV_FMASK_POS, bit, val, len, proc);
}

void ab_mate_update_ble_adv_bt_sta(u8 val, u8 proc)
{
    ab_mate_update_ble_adv_fmsk(2, val, 2, proc);
}

void ab_mate_disable_adv_for_spp_connect(void)
{
    u8 cid[2] = {0xFF, 0xFF};

    ble_scan_rsp_update_byte(ADV_CID_POS, cid, 2, 1);
}

void ab_mate_enable_adv_for_spp_disconnect(void)
{
    u8 cid[2] = {0x42, 0x06};

    ble_scan_rsp_update_byte(ADV_CID_POS, cid, 2, 1);
}


#endif
