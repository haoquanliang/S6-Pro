#ifndef _CHARGE_BOX_H
#define _CHARGE_BOX_H

#define LEFT_CHANNEL_USER               0x11
#define RIGHT_CHANNEL_USER              0x22
#define NOTFIX_LCHANEL_USER             0x33
#define NOTFIX_RCHANEL_USER             0x44

#define VHOUSE_DISTINGUISH              0xFF           //昇生微充电仓
#define VH_DATA_LEN                     40

///昇生微充电仓命令
enum {
    VHOUSE_CMD_GET_VBAT = 0x01,           //获取电池电量，也可作为开始标志
    VHOUSE_CMD_PAIR,                    //充电仓发起的配对消息
    VHOUSE_CMD_GET_TWS_BTADDR,          //获取对耳的地址信息
    VHOUSE_CMD_CLEAR_PAIR,              //清除配对消息
    VHOUSE_CMD_PWROFF,                  //关机消息
    VHOUSE_CMD_ERR,                     //接收消息错误响应
    VHOUSE_CMD_SUCCESS,                 //成功响应
    VHOUSE_CMD_CLOSE_WINDOW,            //关盖
    VHOUSE_CMD_OPEN_WINDOW,             //开盖

    VHOUSE_CMD_CLOSE_WIN_GET_VBAT,      //关盖获取电量包



    /// IODM 使用范围 11起 ， 29 结束，请勿在此范围添加

    VHOUSE_CMD_ENABLE_POPUP = 0x80,     //开关广播功能控制

    VHOUSE_CMD_CUSTOM_RESV1=0xE0,       //客户保留指令
#if SWETZ_UART_CMD
    VHOUSE_CMD_CUSTOM_GET_NTC,  
    VHOUSE_CMD_CUSTOM_GET_VBAT,
    VHOUSE_CMD_CUSTOM_RESET_TO_FACTORY,
    VHOUSE_CMD_CUSTOM_GET_VERSION,
    VHOUSE_CMD_CUSTOM_GET_FIXED_BT_ADDR,
    VHOUSE_CMD_CUSTOM_TEST_MIC,
    VHOUSE_CMD_CUSTOM_TEST_ANC,
    VHOUSE_CMD_CUSTOM_DISABLE_KEY,
    VHOUSE_CMD_CUSTOM_POWER_OFF,  
    VHOUSE_CMD_CUSTOM_GET_TWS_ADDR,
    VHOUSE_CMD_CUSTOM_CLEAR_AG_ADDR,
    VHOUSE_CMD_CUSTOM_CHIP_MODE,
    VHOUSE_CMD_CUSTOM_CLEAR_ALL,
    VHOUSE_CMD_AG_PAIR,
#endif


    VHOUSE_CMD_GET_EAR_ADDR,            //获取耳机地址
  //  VHOUSE_CMD_CUSTOM_RESV_END=0xEF,
    VHOUSE_CMD_CUSTOM_RESV_END=0xFE,//SWETZ--加大范围

    VHOUSE_CMD_SYS_RST=0xFF,             //系统复位指令

};


typedef struct {
    u16 on_cnt;
    u16 off_cnt;
    u16 on_delay;
    u16 off_delay;
#if CHARGE_BOX_OUTBOX_FLIT
    u8 on_cnt_filt;    //防止有些通信仓在心跳包的时候会临时掉下通信电平
#endif
} inbox_cb_t;

typedef struct {
    u8  out2pwr_en;              //拿起开机
    u8  leakage_sta;
    u8  dc_in;
volatile u8  inbox_rtt_check;
} ch_box_cb_t;


typedef struct {
    vh_packet_t packet;
    u32 ticks;
    u32 loc_ticks;
    u32 win_ticks;
    volatile u8 need_ack;               //接收心跳包后5~10ms的时间需要response
    volatile u8 ack_dat;
    volatile u8 update_ear_flag;
    volatile u8 open_win_flag;
    bool rem_bat_ok;
    bool inbox_sta;
    u8  status;                         //仓的状态： 0->关盖充电  1->开盖状态  2->充满休眠状态
    u8 ack_dat_confirm;                 //仓的声道选择
    u8 rx_flag;                         //接收到数据标志位
    u8 cmd3_rx_flag;                    //兼容旧仓，长按3s接收不到数据的情况
} vhouse_cb_t;

extern ch_box_cb_t chbox_cb;

extern vhouse_cb_t vhouse_cb;


void charge_box_inbox_sta(u8 sta);
void charge_box_update_sta(void);
void charge_box_packet_recv(void);
void charge_box_packet_huart_recv(u8 *buf);
void charge_box_heart_beat_ack(void);
void charge_box_reinit(void);
void charge_box_enter(u8 out_auto_pwron);
void charge_box_ssw_init(void);
void charge_box_init(void);
void charge_box_inbox_chk_init(u16 off_delay, u16 on_delay);
void charge_box_leakage_set(u8 sta, u8 val);
void charge_box_inbox_wakeup_enable(void);
void charge_box_inbox_process(void);
void charge_box_process(void);
u8 charge_box_inbox_check(void);
u8 charge_box_sta(void);
u8 charge_box_popup_is_enable(void);
int charge_box_outbox_stable_check(void);
u32 charge_box_ssw_process(u32 charge_sta);
u8 charge_box_charge_on_process(void);
u8 charge_box_get_charge_box_bat_level(void);
bool charge_box_bat_is_ready(void);
void tws_update_local_addr(uint8_t *address);

void param_vuart_popup_flag_write(u8 data);
void param_vuart_popup_flag_read(void);
#endif
