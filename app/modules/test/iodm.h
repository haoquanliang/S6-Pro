#ifndef _BSP_IODM_H
#define _BSP_IODM_H

enum {
    IODM_CMD_CLEAR_PAIR = VHOUSE_CMD_CLEAR_PAIR, //清除配对消息
    /// IODM 使用范围 11起 ， 29 结束，其余是充电仓使用，请勿随便增加
    IODM_CMD_DEV_RST = 11,                   //复位系统
    IODM_CMD_SET_BT_ADDR,               //设置蓝牙地址
    IODM_CMD_GET_BT_ADDR,               //获取蓝牙地址
    IODM_CMD_SET_BT_NAME,               //设置蓝牙名字
    IODM_CMD_GET_BT_NAME,               //获取蓝牙名字
    IODM_CMD_CBT_TEST_ENTER,            //进入CBT测试
    IODM_CMD_CBT_TEST_EXIT,             //退出CBT测试
    IODM_CMD_FCC_TEST_ENTER,            //进入FCC 测试
    IODM_CMD_FCC_SET_PARAM,             //设置 FCC 参数
    IODM_CMD_FCC_TEST_EXIT,             //退出FCC 模式
    IODM_CMD_SET_XOSC_CAP,              //设置频偏参数
    IODM_CMD_GET_XOSC_CAP,              //获取频偏参数
    IODM_CMD_GET_VER_INFO,              //获取版本号
    IODM_CMD_GET_INFO,                  //获取耳机的基本信息
    IODM_CMD_BT_SET_SCAN,               //设置蓝牙搜索状态
    IODM_CMD_MIC_LOOKBACK_ENTER,        //进入mic直通
    IODM_CMD_MIC_LOOKBACK_EXIT,         //退出mic直通
    IODM_CMD_PROTOCOL_VER,              //获取协议版本号
    IODM_CMD_MAX,
};

enum {
    IODM_CFG_MIC0         = 0,
    IODM_CFG_MIC1,
    IODM_CFG_MIC2,
    IODM_CFG_MIC3,
    IODM_CFG_MIC4,
    IODM_CFG_MIC_NONE,
};

//返回结果
#define IODM_RESULT_OK              0x00
#define IODM_RESULT_FAIL            0x01


#define IODM_HARDWARE_VER               "\x01\x00"
#define IODM_SOFTWARE_VER               "\x01\x00"

#define RST_FLAG_MAGIC_VALUE        0x5c       //复位标志值

void iodm_packet_huart_recv(u8 *rx_buf);
void iodm_packet_recv(void);
void iodm_reveice_data_deal(void);
bool bt_get_qr_addr(u8 *addr);
bool bt_get_new_name(char *name);
void iodm_cmd_mic_loopback_exit(void);
void iodm_cmd_mic_loopback_enter(void);
void iodm_test_loopback_set_mic_exit(u16 *channel);
void iodm_test_loopback_set_mic_ch(u16 *channel, u32 *anl_gain, u32 *dig_gain);
void iodm_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

#endif
