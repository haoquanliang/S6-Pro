#include "include.h"
#include "dueros_dma_tws.h"
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

static uint8_t tws_sync_data[128] AT(.buf.dueros_dma);
static uint16_t tws_sync_data_len;

//库里调用
uint16_t tws_get_dueros_dma_data(uint8_t *buf)
{
    uint16_t sync_len = 0;

    if (tws_sync_data_len) {
        memcpy(buf, tws_sync_data, tws_sync_data_len);
        sync_len = tws_sync_data_len;
        tws_sync_data_len = 0;
    }
    return sync_len;
}

//库里调用
uint16_t tws_set_dueros_dma_data(uint8_t *data_ptr, uint16_t size)
{
    dueros_dma_tws_recv_proc(data_ptr,size);
    return 0;
}

static void dueros_dma_tws_data_sync_do(void)
{
    if(bt_tws_is_connected()){
        bt_tws_sync_dueros_dma_data();
    }
}

void dueros_dma_tws_cmd_sync_byte(uint8_t cmd, uint8_t data)
{
    tws_sync_data[0] = cmd;
    tws_sync_data[1] = data;
    tws_sync_data_len = 2;
    dueros_dma_tws_data_sync_do();
}

void dueros_dma_tws_cmd_sync_nbyte(uint8_t cmd, uint8_t* data, uint8_t len)
{
    u8 offset = 0;

    tws_sync_data[offset++] = cmd;
    memcpy(&tws_sync_data[offset], data, len);
    offset += len;

    tws_sync_data_len = offset;
    dueros_dma_tws_data_sync_do();
}

/////////////////////////////////////////////////////////////////
/////////////////////////发送
/////////////////////////////////////////////////////////////////
void dueros_dma_tws_tws_conn_info_sync(void)
{
    uint8_t channel = 0; //1:左 、 0：右
    uint8_t offset = 0;
    uint8_t data[49];
    bt_tws_get_channel(&channel);
    DUEROS_DMA_DEBUG("[DMA_APP]%s, channel:%d, is_slave:%d\n", __func__, channel, bt_tws_is_slave());
    //TLV格式
    if (!bt_tws_is_slave()) {
        data[offset++] = DUEROS_DMA_INFO_DMA_CONN_TYPE;
        data[offset++] = 1;
        data[offset++] = dueros_dma_con_type_get();
    }
    
    if (channel == 1) {
        data[offset++] = DUEROS_DMA_INFO_TRIAD_ID;
        data[offset++] = 25;
        strcpy((char*)&data[offset], dueros_dma_triad_id_get());
        offset += 25;

        data[offset++] = DUEROS_DMA_INFO_TRIAD_SECRET;
        data[offset++] = 17;
        strcpy((char*)&data[offset], dueros_dma_triad_secret_get());
        offset += 17;
    }

    if (offset) {
        dueros_dma_tws_cmd_sync_nbyte(DUEROS_DMA_TWS_INFO_TWS_CONN, data, offset);
    }
}

void dueros_dma_tws_bt_conn_sta_sync(uint8_t sta)
{
    if (bt_tws_is_connected() && !bt_tws_is_slave()) {
        DUEROS_DMA_DEBUG("[DMA_APP]%s, sta:%d\n", __func__, sta);
        dueros_dma_tws_cmd_sync_byte(DUEROS_DMA_TWS_INFO_BT_CONNECT_STA, sta);
    }
}

void dueros_dma_tws_dma_conn_sta_sync(uint8_t con_type, uint8_t status)
{
    if (bt_tws_is_connected() && !bt_tws_is_slave()) {
        DUEROS_DMA_DEBUG("[DMA_APP]%s, status:%d\n", __func__, status);
        uint8_t offset = 0;
        uint8_t data[2];
        data[offset++] = con_type;
        data[offset++] = status;
        dueros_dma_tws_cmd_sync_nbyte(DUEROS_DMA_TWS_INFO_DMA_CONNECT_STA, data, offset);
    }
}

void dueros_dma_tws_tws_dma_data_sync(uint8_t* data, uint8_t len)
{
    DUEROS_DMA_DEBUG("[DMA_APP]%s, len:%d\n", __func__, len);
    DUEROS_DMA_DEBUG_R(data, len);
    dueros_dma_tws_cmd_sync_nbyte(DUEROS_DMA_TWS_INFO_TWS_DMA_DATA, data, len);
}

/////////////////////////////////////////////////////////////////
/////////////////////////接收
/////////////////////////////////////////////////////////////////
void dueros_dma_tws_info_tws_conn_info_proc(uint8_t *data_ptr, u16 size)
{
    DUEROS_DMA_DEBUG("[DMA_APP]%s, size:%d\n", __func__, size);
    char triad_id[25];
    char triad_secret[17];
    u8 read_offset = 0;
    u8 val_len = 0;
    while(read_offset < size){
        switch(data_ptr[read_offset]){
            case DUEROS_DMA_INFO_DMA_CONN_TYPE:
                DUEROS_DMA_DEBUG("DUEROS_DMA_INFO_DMA_CONN_TYPE\n");
                val_len = data_ptr[read_offset + 1];
                dueros_dma_con_type_set(data_ptr[read_offset + 2]);
            break;

            case DUEROS_DMA_INFO_TRIAD_ID:
                DUEROS_DMA_DEBUG("DUEROS_DMA_INFO_TRIAD_ID\n");
                val_len = data_ptr[read_offset + 1];
                memcpy(triad_id, &data_ptr[read_offset + 2], val_len);
                dueros_dma_triad_id_set((char *)triad_id, DUEROS_DMA_TRIAD_FROM_LEFT_EAR);
            break;

            case DUEROS_DMA_INFO_TRIAD_SECRET:
                DUEROS_DMA_DEBUG("DUEROS_DMA_INFO_TRIAD_SECRET\n");
                val_len = data_ptr[read_offset + 1];
                memcpy(triad_secret, &data_ptr[read_offset + 2], val_len);
                dueros_dma_triad_secret_set((char *)triad_secret, DUEROS_DMA_TRIAD_FROM_LEFT_EAR);
            break;

            default:
                val_len = data_ptr[read_offset + 1];
            break;
        }
        read_offset += (2 + val_len);
    }
}

void dueros_dma_tws_info_bt_conn_sta_proc(uint8_t *data_ptr, u16 size)
{
    DUEROS_DMA_DEBUG("[DMA_APP]%s, size:%d\n", __func__, size);
    DUEROS_DMA_DEBUG_R(data_ptr, size);
    if (data_ptr[0]) {
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_MOBILE_CONNECTED, NULL, 0, 0);
    } else {
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_MOBILE_DISCONNECTED, NULL, 0, 0);
    }
}

void dueros_dma_tws_info_dma_conn_sta_proc(uint8_t *data_ptr, u16 size)
{
    DUEROS_DMA_DEBUG("[DMA_APP]%s, size:%d\n", __func__, size);
    DUEROS_DMA_DEBUG_R(data_ptr, size);
    dueros_dma_con_type_set(data_ptr[0]);
    if (data_ptr[1]) {
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_CONNECTED, NULL, 0, 0);
    } else {
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_DISCONNECTED, NULL, 0, 0);
    }
}

void dueros_dma_tws_info_dma_data_proc(uint8_t *data_ptr, u16 size)
{
    DUEROS_DMA_DEBUG("[DMA_APP]%s, size:%d\n", __func__, size);
    DUEROS_DMA_DEBUG_R(data_ptr, size);
    dueros_dma_wrap_recv_peer_data((const char*)data_ptr, size);
}


void dueros_dma_tws_recv_proc(uint8_t *data_ptr, u16 size)
{
    uint8_t info_id = data_ptr[0];
    uint8_t *p_data = &data_ptr[1];
    uint16_t data_len = size - 1;

    dueros_dma_wakeup_set(1);

    switch(info_id){
        case DUEROS_DMA_TWS_INFO_TWS_CONN:
            DUEROS_DMA_DEBUG("DUEROS_DMA_TWS_INFO_TWS_CONN\n");
            dueros_dma_tws_info_tws_conn_info_proc(p_data, data_len);
            break;
            
        case DUEROS_DMA_TWS_INFO_BT_CONNECT_STA:
            DUEROS_DMA_DEBUG("DUEROS_DMA_TWS_INFO_BT_CONNECT_STA\n");
            dueros_dma_tws_info_bt_conn_sta_proc(p_data, data_len);
            break;

        case DUEROS_DMA_TWS_INFO_DMA_CONNECT_STA:
            DUEROS_DMA_DEBUG("DUEROS_DMA_TWS_INFO_DMA_CONNECT_STA\n");
            dueros_dma_tws_info_dma_conn_sta_proc(p_data, data_len);
            break;

        case DUEROS_DMA_TWS_INFO_TWS_DMA_DATA:
            DUEROS_DMA_DEBUG("DUEROS_DMA_TWS_INFO_TWS_DMA_DATA\n");
            dueros_dma_tws_info_dma_data_proc(p_data, data_len);
            break;

        default:
            break;
    }
}

#endif  //LE_DUEROS_DMA_EN
