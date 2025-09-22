#include "include.h"
#include "api.h"

const uint8_t sdp_spp_service_record[] = {
    0x36, 0x00, 0x54, 0x09, 0x00, 0x00, 0x0a, 0x00, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x36, 0x00,
    0x03, 0x19, 0x11, 0x01, 0x09, 0x00, 0x04, 0x36, 0x00, 0x0e, 0x36, 0x00, 0x03, 0x19, 0x01, 0x00,
    0x36, 0x00, 0x05, 0x19, 0x00, 0x03, 0x08, SPP_RFCOMM_SERVER_CHANNEL0,   0x09, 0x00, 0x05, 0x36,
    0x00, 0x03, 0x19, 0x10, 0x02, 0x09, 0x00, 0x06, 0x36, 0x00, 0x09, 0x09, 0x65, 0x6e, 0x09, 0x00,
    0x6a, 0x09, 0x01, 0x00, 0x09, 0x00, 0x09, 0x36, 0x00, 0x09, 0x36, 0x00, 0x06, 0x19, 0x11, 0x01,
    0x09, 0x01, 0x02, 0x09, 0x01, 0x00, 0x25, 0x03, 0x53, 0x50, 0x50,
};

#if BT_SPP_EN
#if LE_DUEROS_DMA_EN
#define SPP_TX_BUF_INX      3
#define SPP_TX_BUF_LEN      DUEROS_DMA_MTU_MAX_LEN    //max=512
#else
#define SPP_TX_BUF_INX      4
#define SPP_TX_BUF_LEN      255     //max=512(ab_mate命令长度最长255,卡在255可以防止ota时spp粘包)
#endif
#define SPP_POOL_SIZE       (SPP_TX_BUF_LEN + sizeof(struct txbuf_tag)) * SPP_TX_BUF_INX


AT(.ble_buf.stack.spp)
uint8_t spp_tx_pool[SPP_POOL_SIZE];

void spp_txpkt_init(void)
{
    txpkt_init(&spp_tx_ch0, spp_tx_pool, SPP_TX_BUF_INX, SPP_TX_BUF_LEN);
    spp_tx_ch0.send_kick = spp_send_kick;
}

u16 get_spp_mtu_size(void)
{
    return SPP_TX_BUF_LEN;
}

void spp_rx_callback(uint8_t *bd_addr, uint8_t ch, uint8_t *packet, uint16_t size)
{
    if (ch == SPP_SERVICE_CH1) {
#if GFPS_EN
        gfps_spp_recv_callback(packet, size);
#endif
    }
    app_spp_rx_callback(ch, packet, size);
}

void spp_connect_callback(uint8_t *bd_addr, uint8_t ch)
{
    printf("--->spp_connect_callback ch:%d\n",ch);

    if (ch == SPP_SERVICE_CH1) {
#if GFPS_EN
        gfps_spp_connected_callback();
#endif
    }

    app_spp_connect_callback(ch);

#if ANC_TOOL_EN
    if (ch == SPP_SERVICE_CH0) {
        anc_tool_spp_connect_callback();
    }
#endif // ANC_TOOL_EN
}

void spp_disconnect_callback(uint8_t *bd_addr, uint8_t ch)
{
    printf("--->spp_disconnect_callback ch:%d\n",ch);
    app_spp_disconnect_callback(ch);

#if ANC_TOOL_EN
    if (ch == SPP_SERVICE_CH0) {
        anc_tool_spp_disconnect_callback();
    }
#endif // ANC_TOOL_EN
}

#endif

