#include "app_include.h"

#if DEBUG_LR
#define LR_DEBUG(...)              printf(__VA_ARGS__)
#else
#define LR_DEBUG(...)
#endif

static ring_buffer lr_tx_buf;//由于系统原因，有时候发送数据不是立即发走的，为了避免丢数据，用ring buffer缓存。
u8 tws_rx_buf[TWS_SYNC_BUF_SIZE];
u16 tws_rx_data_size;
static u8 seq_num_tx = 0;
static u8 seq_num_rx = 0;

#define HEADER_INDEX        (0)
#define SEQ_NUM_INDEX       (1)
#define CMD_ID_INDEX        (2)
#define PAYLOAD_SIZE_INDEX  (3)
#define PAYLOAD_INDEX       (4)

void app_lr_init(void)
{
    app_ring_buffer_init(&lr_tx_buf);
}

u16 app_lr_tws_get_data(u8 *buf)
{
    uint16_t len = 0;

    app_ring_buffer_dequeue(&lr_tx_buf, buf, &len);
    return len;
}

bool app_lr_tws_set_data(u8 *data, u16 size)
{
    //printf("recv size:%d\n", size);
    //print_r(data, size);
    if ((size <= TWS_SYNC_BUF_SIZE)
        && (size >= 5)
        && ((data[HEADER_INDEX] == 0x55))
        )
    {
        tws_rx_data_size = size;
        memcpy(tws_rx_buf, data, size);
        app_lr_parse_command();
        return true;
    }
    return false;
}

static void lr_send(u8 cmd_id, u8 payload_size, u8 *payload)
{
    if (bt_tws_is_connected())
    {
        if (payload_size <= (TWS_SYNC_BUF_SIZE-5))
        {
            u8 offset = 0;
            u8 crc8;
            u8 tws_tx_buf[TWS_SYNC_BUF_SIZE];

            sys_cb.tick_event = tick_get();

            tws_tx_buf[offset++] = 0x55;
            tws_tx_buf[offset++] = seq_num_tx++;
            tws_tx_buf[offset++] = cmd_id;
            tws_tx_buf[offset++] = payload_size;
            if (payload_size > 0)
            {
                memcpy(&tws_tx_buf[offset], payload, payload_size);
                offset += payload_size;
            }
            crc8 = crc8_maxim(&tws_tx_buf[0], offset);
            tws_tx_buf[offset++] = crc8;
            if (app_ring_buffer_enqueue(&lr_tx_buf, tws_tx_buf, offset))
            {
                bt_tws_sync_custom_data();
            }
            else
            {
                seq_num_tx--;
                printf("---lr:tx buf full\n");
            }
        }
        else
        {
            printf("lr:send err\n");
        }
    }
}

void app_lr_send_notification(u8 cmd_id, u8 payload_size, u8 *payload)
{
    lr_send(cmd_id, payload_size, payload);
}

static void lr_parse_notification(void)
{
    u8 payload_size = tws_rx_buf[PAYLOAD_SIZE_INDEX];
    u8 cmd_id = tws_rx_buf[CMD_ID_INDEX];

    printf("lr:cmd %d\n", cmd_id);
    switch (cmd_id)
    {

        case LR_NOTIFY_BATTERY_LEVEL:
        {
            if (payload_size == 1)
            {
                sys_cb.peer_bat_level = tws_rx_buf[PAYLOAD_INDEX];

            }
        }
        break;

        case LR_NOTIFY_MSG:
        if (payload_size == 2)
        {
            msg_enqueue(BYTES_2_UINT16(tws_rx_buf[PAYLOAD_INDEX], tws_rx_buf[PAYLOAD_INDEX + 1]));
        }
        break;

        case LR_NOTIFY_IN_CASE_STATUS:
        if (payload_size == 1)    
        {
            if (tws_rx_buf[PAYLOAD_INDEX])
            {
                sys_cb.flag_peer_in_case = true;
            }
            else 
            {
                sys_cb.flag_peer_in_case = false;
            }
           // msg_enqueue(EVT_INBOX_CHANGED);
        }
        break;


        default:
        LR_DEBUG("lr:unknown notify %d\n", cmd_id);
        break;
    }
}

void app_lr_parse_command(void)
{
    if (crc8_maxim(tws_rx_buf, tws_rx_data_size - 1) == tws_rx_buf[tws_rx_data_size - 1])
    {
        if (tws_rx_buf[1] != seq_num_rx)
        {
            printf("---lr:seq_num_rx err!---\n");
            seq_num_rx = tws_rx_buf[1];
        }
        seq_num_rx++;
        lr_parse_notification();
    }
    else
    {
        printf("---lr:crc err!---\n");
    }
}

void app_lr_send_msg(u16 msg)//同步消息事件,发给对耳执行
{
    u8 msg_buf[2];

    msg_buf[0] = HIGH_BYTE(msg);
    msg_buf[1] = LOW_BYTE(msg);
    app_lr_send_notification(LR_NOTIFY_MSG,2, &msg_buf[0]);
}

bool app_lr_is_idle(void)
{
    return app_ring_buffer_is_empty(&lr_tx_buf);
}

