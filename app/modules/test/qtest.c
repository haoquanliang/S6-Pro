#include "include.h"

#if QTEST_EN

enum{
    QTEST_STA_INBOX,
    QTEST_STA_OUTBOX,
    QTEST_STA_DCIN,
};

typedef struct {
    u8 *bd_addr;
    u8 *link_key;
    u8 channel;
    u8 rsvd[3];
} tws_link_info_t;

qtest_cb_t qtest_cb;
u8 qtest_get_mode(void);
void qtest_send_msg2tbox(u8 *buf,u16 len);
u8 qtest_get_heart_pkt_delay(void);
void qtest_dec_heart_pkt_delay(void);
u8* qtest_get_txbuf(void);
void qtest_beforehand_process(void);

u8* qtest_get_txbuf(void)
{
    return qtest_cb.other_usage_txbuf;
}



u8 qtest_check_tws_master_addr(u8* bt_addr)
{
    u8 ms_role = bt_tws_get_tws_role();
    if(!ms_role) {
       bt_get_local_bd_addr(bt_addr);
    }
    return ms_role;
}

AT(.com_text.qtest)
void qtest_set_flag_cb(u8 flag)
{
    qtest_cb.flag = flag;
    qtest_cb.sta = 1;
}

static bool qtest_vusb_dma_en(void)
{
    if ((TEST_INTF_SEL == INTF_HUART) || qtest_get_mode()) {
        return true;
    }
    return false;
}

void qtest_send_msg_cb(u8 *buf, u16 len)
{
    if(qtest_vusb_dma_en()) {
        huart_tx(buf,len);
    } else {
        for(u8 i =0;i<len;i++){
            uart1_putchar(buf[i]);
        }
    }
}

//删除TWS配对信息
AT(.text.qtest.info)
void qtest_delete_tws_link_info_cb(void)
{
    bt_clr_master_addr();
    bt_nor_delete_link_info();
    bt_tws_delete_link_info_with_tag(BT_INFO_TAG_QTEST, (uint32_t)__builtin_return_address(0));
#if TRACE_EN
    printf("qtest_delete\n");
#endif
}

//保存新的TWS配对信息到flash
AT(.text.qtest.info)
bool qtest_store_tws_link_info_cb(tws_link_info_t *info)
{
    u8 lr_channel = info->channel;
    bt_tws_channel_check(lr_channel);
    qtest_delete_tws_link_info_cb();
    u8 ms_role = qtest_check_tws_master_addr(info->bd_addr);
    tws_update_local_addr(info->bd_addr);
    bt_tws_put_link_info_addr(info->bd_addr, info->link_key);
    bt_tws_put_link_info_feature(info->bd_addr, ms_role);
#if TRACE_EN
    printf("qtest_create: ch=%x\n",info->channel);
    print_r(info->bd_addr, 6);
    print_r(info->link_key, 16);
#endif // TRACE_EN
    return true;
}

//保存客户自定义信息到flash
AT(.text.qtest.info)
void qtest_store_custom_params_cb(u8 *param, u8 size)
{
#if TRACE_EN
    printf("qtest_custom%d:", size);
    print_r(param, size);
#endif
}



//传给测试盒数据用于显示，测试盒需要打开“显示耳机传入信息”的功能，可自定义显示内容
//第0byte为：字符串数据
//第1byte为：数值
void qtest_show_user_info(u8* param)
{
    param[0] = 'V';
    param[1] = (u8)bsp_get_bat_level();
}


void qtest_custom_pair_id(u8* param)
{
    uint pair_id = bt_tws_get_pair_id();
    memcpy(param,(u8*)&pair_id,4) ;
}

WEAK u32 xcfg_get_flash_addr(void)
{
    return 0;
}

//修正1拖24的校频会影响校验码对比的问题
AT(.text.qtest)
u32 qtest_get_osci_cap_addr(void)
{
    u32 offest_addr =  xcfg_get_flash_addr();
    if(offest_addr != 0) {
        offest_addr += (u8*)(&xcfg_cb.osci_cap) - (u8*)&xcfg_cb;
    }
    return offest_addr;
}

AT(.text.qtest)
static bool qtest_pwroff_check(u8 mode)
{
    static u32 inbox_tick = 0;
    static u8 ear_sta = 0;
    u8 pre_ear_sta = ear_sta;

    if(mode == 0) {
        return false;
    }
    if(!tick_check_expire(inbox_tick, 10)) {
        return false;
    }
    inbox_tick = tick_get();

    if(qtest_get_heart_pkt_delay()) {                           //发完心跳包后延时段时间再检测状态
        qtest_dec_heart_pkt_delay();
        if (qtest_cb.inbox_cnt) {
            qtest_cb.inbox_cnt--;
        }
        return false;
    }

    if(CHARGE_DC_IN()) {
        ear_sta = QTEST_STA_DCIN;
    } else if(CHARGE_INBOX()) {
        ear_sta = QTEST_STA_INBOX;
    } else {
        ear_sta = QTEST_STA_OUTBOX;
    }

    if((ear_sta == QTEST_STA_INBOX) || ( pre_ear_sta != ear_sta)) {
        qtest_cb.inbox_cnt = 150;
    }

//     my_printf("*%x_%d",ear_sta,qtest_cb.inbox_cnt);
    if(!qtest_cb.inbox_cnt--) {
        qtest_exit();
        //拿起复位
        if((qtest_get_pickup_sta() == 2) || (ear_sta == QTEST_STA_DCIN)) {
            if(bt_nor_is_connected()){
                bt_nor_disconnect();
            }
            sw_reset_kick(SW_RST_FLAG);
        } else if(qtest_get_pickup_sta() == 1) {
        //拿起关机
            if(mode == SW_RST_QTEST_NORMAL) {
                msg_enqueue(EVT_QTEST_PICKUP_PWROFF);
            } else if(mode == SW_RST_QTEST_ONLY_PAIR) {
                bsp_saradc_exit();
                sfunc_pwrdown(1);
            }
        } else if(mode == SW_RST_QTEST_ONLY_PAIR) {
             bsp_saradc_exit();
             sfunc_pwrdown(1);
        }
        return true;
    }
    return false;
}

AT(.text.qtest)
static void qtest_xcfg_init(void)
{
    //开快速测试关闭入仓关机
    xcfg_cb.chg_inbox_pwrdwn_en = 0;
    xcfg_cb.bt_tswi_charge_rst_en = 0;
    if (qtest_get_mode()) {
         xcfg_cb.bt_tws_en = 0;
         xcfg_cb.bt_spp_en = 0;
         xcfg_cb.bt_rf_page_pwrdec = 0;
//         xcfg_cb.bt_rf_inq_pwrdec=0;
         xcfg_cb.lpwr_off_vbat = 0;//低电不关机
		 xcfg_cb.charge_en = 0;
		 xcfg_cb.ble_en = 0;
         xcfg_cb.huart_sel = HUART_TR_VUSB;

         RTCCON8 |= BIT(4);    //仓的最低维持电压为1.7V
         charge_box_leakage_set(1, 0);                                   //enable leakage
//         param_tws_channel_write(0);
//         qtest_tws_message_init();
    }
}

AT(.text.qtest)
void qtest_other_usage_process(void)
{
    u8 *tx_buf = qtest_get_txbuf();

    if (qtest_cb.ack != qtest_cb.flag) {
        qtest_cb.ack = qtest_cb.flag;
        qtest_cb.flag = 0;
        if (qtest_cb.ack == QTEST_MODE_DUT) {
            func_cb.sta = FUNC_BT_DUT;
        } else if (qtest_cb.ack == QTEST_MODE_CLR_INFO) {
            qtest_delete_tws_link_info_cb();
            func_cb.sta = FUNC_NULL;
        } else if ((qtest_cb.ack == QTEST_MODE_PWROFF) || (qtest_cb.ack == QTEST_MODE_PWROFF_LP)) {
            if (qtest_cb.ack == QTEST_MODE_PWROFF_LP) {
                qtest_cb.pdn_boat_flag = 1;
            }
            sys_cb.discon_reason = 0;//不同步关耳机
            func_cb.sta = FUNC_PWROFF;
        }
        tx_buf[0] = 'S';
        tx_buf[1] = 'I';
        tx_buf[2] = 'G';
        tx_buf[3] = qtest_cb.ack;

        qtest_send_msg2tbox(tx_buf,4);
        qtest_cb.ack = 0;
    }
}




//快测快配的主程序
AT(.text.qtest)
void qtest_process_do(void)
{
    u8 mode = qtest_get_mode();
    qtest_pwroff_check(mode);
    qtest_deal_uart_message();
    WDT_CLR();
    vusb_reset_clr();
}

AT(.text.qtest)
void qtest_only_pair_process(void)
{
    qtest_xcfg_init();
    set_vusb_uart_flag(0);
    bsp_huart_init();
    while(1){
        qtest_process_do();
    }
}

///////////////////////////////////////////////////////////
AT(.text.qtest.process)
void qtest_process(void)
{
    if(qtest_get_mode()) {
        qtest_process_do();
    } else {
        if (qtest_cb.ack != qtest_cb.flag) {
            qtest_other_usage_process();
        }
    }
}

AT(.text.qtest)
void qtest_init(void)
{
//    TRACE("qtest_init\n");
    memset(&qtest_cb,0x00,sizeof(qtest_cb_t));
	qtest_cb.inbox_cnt = 150;
    u8 res = qtest_mode_init(sys_cb.sw_rst_flag);
    if (res) {
        if((res == 1) || ((TEST_INTF_SEL == INTF_HUART))) {
            //进入快测或者使用HUART接口时，使用HUART DMA的方式进行串口通讯
            sys_cb.vusb_uart_flag = 0;
            xcfg_cb.huart_en = 1;
            xcfg_cb.huart_sel = HUART_TR_VUSB;
            bsp_huart_init();
        } else {
            //HUART被其他调试接口占用时，可以用UART1进入快测模式
            bsp_uart1_init(9600);
        }
        bt_tws_channel_read();
        //先确定系统左右耳
        tws_lr_xcfg_sel();
        qtest_xcfg_init();
        //先发送蓝牙地址给到测试盒，加速快测
        if(res == 1){
        	qtest_beforehand_process();
        }
    }
}

//恢复5v复位
AT(.text.qtest)
void qtest_exit(void)
{
    xcfg_cb.charge_en = 1;
}

#endif
