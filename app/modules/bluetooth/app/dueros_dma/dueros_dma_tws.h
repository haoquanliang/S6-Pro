#ifndef __DUEROS_DMA_TWS_H
#define __DUEROS_DMA_TWS_H
#include "include.h"
#if LE_DUEROS_DMA_EN

typedef enum {
    DUEROS_DMA_INFO_DMA_CONN_TYPE = 0,
    DUEROS_DMA_INFO_TRIAD_ID,
    DUEROS_DMA_INFO_TRIAD_SECRET,
} DMA_TWS_CONN_INFO;

typedef enum {
    DUEROS_DMA_TWS_INFO_TWS_CONN = 0,
    DUEROS_DMA_TWS_INFO_TWS_DMA_DATA,
    DUEROS_DMA_TWS_INFO_BT_CONNECT_STA,
    DUEROS_DMA_TWS_INFO_DMA_CONNECT_STA,
} DMA_TWS_SYNC_INFO;

void dueros_dma_tws_tws_conn_info_sync(void);
void dueros_dma_tws_bt_conn_sta_sync(uint8_t sta);
void dueros_dma_tws_tws_dma_data_sync(uint8_t* data, uint8_t len);
void dueros_dma_tws_dma_conn_sta_sync(uint8_t con_type, uint8_t status);
void dueros_dma_tws_recv_proc(uint8_t *data_ptr, u16 size);
#endif

#endif //LE_DUEROS_DMA_EN
