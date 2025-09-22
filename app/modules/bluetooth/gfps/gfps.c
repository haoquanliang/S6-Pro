#include "include.h"
#include "gfps.h"

#if GFPS_EN

const uint8_t sdp_gfps_spp_service_record[] = {
    0x36, 0x00, 0x62, 0x09, 0x00, 0x00, 0x0a, 0x50, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x36, 0x00,
    0x11, 0x1C, 0xdf, 0x21, 0xfe, 0x2c, 0x25, 0x15, 0x4f, 0xdb, 0x88, 0x86, 0xf1, 0x2c, 0x4d, 0x67,
    0x92, 0x7c, 0x09, 0x00, 0x04, 0x36, 0x00, 0x0e, 0x36, 0x00, 0x03, 0x19, 0x01, 0x00, 0x36, 0x00,
    0x05, 0x19, 0x00, 0x03, 0x08, 0x05, 0x09, 0x00, 0x05, 0x36, 0x00, 0x03, 0x19, 0x10, 0x02, 0x09,
    0x00, 0x06, 0x36, 0x00, 0x09, 0x09, 0x65, 0x6e, 0x09, 0x00, 0x6a, 0x09, 0x01, 0x00, 0x09, 0x00,
    0x09, 0x36, 0x00, 0x09, 0x36, 0x00, 0x06, 0x19, 0x11, 0x01, 0x09, 0x11, 0x02, 0x09, 0x01, 0x00,
    0x25, 0x03, 0x53, 0x50, 0x50,
};

#if !AB_MATE_APP_EN

// ---------------------------------------------------------------------------
const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
    // GFPS, Tx Power, -20dBm
    0x02, 0x0A, 0xF5,
};

const uint8_t scan_data_const[] = {
    // Name
    0x08, 0x09, 'B', 'L', 'E', '-', 'B', 'O', 'X',
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
    return data_len;
}

///////////////////////////////////////////////////////////////////////////
#define MAX_NOTIFY_NUM          8
#define MAX_NOTIFY_LEN          256     //max=256
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

AT(.gfps.ble_att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}

#endif

#endif

