#ifndef _QTEST_H
#define _QTEST_H

typedef struct {
    volatile u8 flag;
    u8 sta;
    u8 ack;
    u8 inbox_cnt;
    u8 other_usage_txbuf[12];
    u8 pdn_boat_flag;
}qtest_cb_t;

#define QTEST_MODE_DUT           '3'
#define QTEST_MODE_CLR_INFO      '4'
#define QTEST_MODE_PWROFF        '5'
#define QTEST_MODE_PWROFF_LP     '6'              //船运模式

extern qtest_cb_t qtest_cb;

uint32_t bt_tws_get_pair_id(void);
extern u32 get_spiloader_offset(void);

u8 qtest_mode_init(u8 rst_source);
u8 qtest_get_mode(void);

bool qtest_is_send_btmsg(void);
void qtest_create_env(void);
void qtest_set_pickup_sta(u8 sta);              //设置耳机拿起后状态
u8 qtest_get_pickup_sta(void);                  //获取耳机拿起后状态

void qtest_packet_uart1_recv(u8 data);
void qtest_packet_huart_recv(u8 *rx_buf);
void qtest_deal_uart_message(void);

void qtest_init(void);
void qtest_exit(void);
void qtest_process(void);


#endif
