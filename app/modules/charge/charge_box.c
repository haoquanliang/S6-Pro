#include "include.h"

#if CHARGE_BOX_EN

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

int btstack_get_link_key(uint8_t *bd_addr, uint8_t *link_key, uint8_t *type, bool ext);

ch_box_cb_t chbox_cb;
inbox_cb_t inbox_cb;

#if (CHARGE_BOX_TYPE == CBOX_SSW)
vhouse_cb_t vhouse_cb;
#endif

///设置leakage状态， 返回值 0：disable, 1: enable
AT(.com_text.charge_box)
void charge_box_leakage_set(u8 sta, u8 val)
{
    u32 level = (val) ? val : xcfg_cb.ch_leakage_sel;
    chbox_cb.leakage_sta = sta & 0xfc;

    charge_set_leakage(sta, level);
}

///使能出仓唤醒功能
AT(.text.charge_box)
void charge_box_inbox_wakeup_enable(void)
{
#if !SWETZ_CLSOE_INBOX_WALEUP
    RTCCON3 |= BIT(12);                                                         //RTCCON3[12], INBOX Wakeup enable
#endif
    if ((xcfg_cb.ch_box_type_sel == 2) || (xcfg_cb.ch_box_type_sel == 0)) {     //有维持电压的充电仓关闭leakage漏电
        charge_box_leakage_set(0, 0);                                           //leakage disable
    }
}

///检测INBOX状态   返回值  0: 正在检测, 1：检测完成,INBOX不在线， 2：检测完成,INBOX在线
AT(.com_text.charge_box)
u8 charge_box_inbox_check(void)
{
    inbox_cb_t *s = &inbox_cb;
    if (CHARGE_INBOX()) {
#if CHARGE_BOX_OUTBOX_FLIT
        s->on_cnt_filt = 0;
#endif
        s->off_cnt = 0;
        if (s->on_cnt < s->on_delay) {              //等待VUSB放电，确认INBOX是否在线
            s->on_cnt++;
            return 0;
        }
        if (s->on_cnt == s->on_delay) {
            s->on_cnt = s->on_delay + 5;
            return 2;                               //inbox在线
        }
        return 0x82;
    } else {
#if CHARGE_BOX_OUTBOX_FLIT
        s->on_cnt_filt ++;
        if (s->on_cnt_filt >= 8) {
            s->on_cnt_filt = 0;
            s->on_cnt = 0;
        }
#else
        s->on_cnt = 0;
#endif
        if (s->off_cnt < s->off_delay) {
            s->off_cnt++;
            if ((s->off_cnt == 4) && (chbox_cb.leakage_sta & BIT(7))) {
                charge_box_leakage_set(0, 0);
            }
            return 0;
        }
        if (s->off_cnt == s->off_delay) {
            s->off_cnt = s->off_delay + 5;
            return 1;                               //inbox不在线
        }
        return 0x81;
    }
}

///用于兼容短暂掉0V充电仓（充满后 5V -> 0V -> 2.xV）
AT(.text.bsp.charge)
int charge_box_outbox_stable_check(void)
{
    int i;
    u8 inbox_sta = 0;

    if (xcfg_cb.ch_box_type_sel) {
        return 1;                                                           //非此类型仓, 直接返回出仓开机
    }
    charge_box_leakage_set(0, 0);                                           //disable leakage
    delay_5ms(60);                                                          //delay 300ms
    charge_box_inbox_chk_init(12+(u16)xcfg_cb.chbox_out_delay*10, 20);
    for (i = 0; i < 200; i++) {
        delay_5ms(2);
        WDT_CLR();
        inbox_sta = charge_box_inbox_check();                               //检测INBOX状态判断是否为充满状态
        if (inbox_sta) {
            break;
        }
    }

    if (inbox_sta == 2) {                                                   //inbox stable online
        if (CHARGE_DC_IN()) {
            charge_box_leakage_set(1, 0);                                   //enable leakage
            return 0;                                                       //5V online, 继续充电
        }
        charge_box_inbox_wakeup_enable();                                   //INBOX Wakeup enable
        bsp_saradc_exit();
        sfunc_pwrdown(1);                                                   //开VUSB复位，且并机
    }
    charge_box_leakage_set(1, 0);                                           //enable leakage
    return 1;                                                               //出仓开机
}



///兼容某些维持电压的仓: 耳机入仓, 电池仓的电压不会自动升到5V, 需要手动按仓的按键。
AT(.com_text.charge_box)
void charge_box_inbox_process(void)
{
    static u32 inbox_tick = 0;
    u8 sta;
    if ((chbox_cb.inbox_rtt_check) && (!CHARGE_DC_IN())) {
        if (!tick_check_expire(inbox_tick, 10)) {
            return;
        }
        inbox_tick = tick_get();
        sta = charge_box_inbox_check();
#if (CHARGE_BOX_TYPE == CBOX_SSW)
        charge_box_inbox_sta(sta);
#endif
        if (1 == sta) {
            if (bt_is_connected()) {
                dis_auto_pwroff();
            }
        }
        if ((2 == sta) && (xcfg_cb.chg_inbox_pwrdwn_en)) {
            msg_enqueue(EVT_CHARGE_INBOX);
        }
    }
}

///5ms调用一次
AT(.text.charge_box)
u8 charge_box_charge_on_process(void)
{
    u8 inbox_sta, chbox_sta = 0;

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    chbox_sta = charge_box_ssw_process(1);
#endif
    if ((charge_box_sta() == 2) || (chbox_sta == 2)) {             //是否充满
        charge_exit();
        unlock_code_charge();
        charge_box_inbox_wakeup_enable();                               //INBOX Wakeup enable
        bsp_saradc_exit();
        sfunc_pwrdown(0);
    }

    if (!CHARGE_DC_IN()) {                                              //DC IN online?
//        if ((chbox_cb.dc_in == 3) && CHARGE_INBOX()) {
//            charge_box_leakage_set(0x81, 6);                            //用大电流leakage加速VUSB掉电, 在检测到低于INBOX电压后关闭leakage(防止VUSB电容导致掉电慢)
//        }
//        if (chbox_cb.dc_in < 10) {
//            chbox_cb.dc_in++;
//        }
        charge_box_leakage_set(0, 0);
        inbox_sta = charge_box_inbox_check() & 0x3;                     //检查inbox online状态
#if (CHARGE_BOX_TYPE == CBOX_SSW)
        if (chbox_sta) {                                               //智能充电仓已开盖, 需要退出充电流程
            charge_box_leakage_set(1, 0);
            if (inbox_sta == 1) {                                       //dc_in = 0, 且inbox = 0
                sys_cb.outbox_pwron_flag = 1;
            }
            return 2;
        }
#endif
        if (inbox_sta == 1) {                                           //dc_in = 0, 且inbox = 0
            charge_box_leakage_set(1, 0);
            if ((chbox_cb.out2pwr_en) || (chbox_sta)) {                //拿起开机
                sys_cb.outbox_pwron_flag = 1;
                return 2;
            }
            return 1;
        } else if (inbox_sta == 2) {                                    //dc_in = 0, inbox = 1, 需要关机
#if UART0_PRINTF_SEL != PRINTF_VUSB                                     //vusb打印不关机
            charge_exit();
            unlock_code_charge();
            charge_box_inbox_wakeup_enable();                           //INBOX Wakeup enable
            bsp_saradc_exit();
            sfunc_pwrdown(1);
#endif
        }
    } else {
        chbox_cb.dc_in = 1;
        chbox_cb.out2pwr_en = xcfg_cb.ch_out_auto_pwron_en;
        inbox_cb.off_cnt = 0;
        inbox_cb.on_cnt = 0;
        charge_box_leakage_set(1, 0);

    }
    return 0;
}

AT(.text.charge_box.process)
void charge_box_process(void)
{
    charge_box_inbox_process();
#if (CHARGE_BOX_TYPE == CBOX_SSW)
    charge_box_ssw_process(0);
#endif
}


///耳机在充电仓充满自动关机，拿起自动开机处理。
AT(.text.bsp.charge)
u8 charge_box_sta(void)
{
    if ((sys_cb.charge_sta == 2) && (!sys_cb.charge_bled_flag)) {
        //充满且蓝灯灭时进入关机
        if (xcfg_cb.ch_full_auto_pwrdwn_en) {
            //printf("charge full auto pwrdwn\n");
            charge_box_inbox_wakeup_enable();
            return 2;                                                   //充满关机
        }
    }
    return 0;
}

AT(.text.charge_box)
void charge_box_inbox_chk_init(u16 off_delay, u16 on_delay)
{
    inbox_cb.off_cnt = 0;
    inbox_cb.on_cnt = 0;
    inbox_cb.off_delay = off_delay;
    inbox_cb.on_delay = on_delay;
}

AT(.text.charge_box)
void charge_box_enter(u8 out_auto_pwron)
{
    memset(&chbox_cb, 0, sizeof(chbox_cb));
    chbox_cb.out2pwr_en = out_auto_pwron;
    charge_box_inbox_chk_init(20, 120);                                 //inbox online检测长点，等待VUSB电容放电
    if ((chbox_cb.out2pwr_en) && (!xcfg_cb.ch_box_type_sel)) {          //短暂掉0V的仓
        inbox_cb.off_delay = 100+(u16)xcfg_cb.chbox_out_delay*20;       //inbox offline check 500ms + n*100ms
    }
}

void charge_box_reinit(void)
{
    sys_cb.inbox_wken_flag = 0;
    charge_box_inbox_chk_init(10, 20);
    chbox_cb.leakage_sta = 0;
    if ((xcfg_cb.ch_box_type_sel == 2) || (xcfg_cb.ch_box_type_sel == 0)) {
        chbox_cb.inbox_rtt_check = 1;
    }
}

void charge_box_init(void)
{
#if (CHARGE_BOX_TYPE == CBOX_SSW)
    charge_box_ssw_init();
#endif
}

#if (CHARGE_BOX_TYPE == CBOX_SSW)
AT(.com_text.charge_box)
void charge_box_inbox_sta(u8 sta)
{
    if (sta) {
        bool new_sta = vhouse_cb.inbox_sta;
        if (sta & BIT(1)) {
            new_sta = true;
        } else if (sta & BIT(0)) {
            new_sta = false;
        }

        if(new_sta != vhouse_cb.inbox_sta) {
            vhouse_cb.inbox_sta = new_sta;
            vhouse_cb.update_ear_flag = true;
        }
    }
}

AT(.text.charge_box)
void charge_box_update_sta(void)
{
    vhouse_cb.update_ear_flag = true;
}

AT(.text.charge_box)
static void charge_box_update_bat_adv(void)
{
    if(vhouse_cb.update_ear_flag) {
        vhouse_cb.update_ear_flag = false;

        bool old_sta = (bool)(sys_cb.loc_bat & BIT(7));
        bool new_sta = vhouse_cb.inbox_sta;
        if(old_sta != new_sta) {
            if(new_sta) {
#if SWETZ_INBOX_STA                
            sys_cb.loc_bat &= ~BIT(7);
#else
                 
            sys_cb.loc_bat |= BIT(7);
#endif                
            } else {
                sys_cb.loc_bat &= ~BIT(7);
            }
        }
    }
}

AT(.text.charge_box)
static void charge_box_open_windows(void)
{
    vhouse_cb.open_win_flag = 0;
    vhouse_cb.status = 1;                           //开窗，停止充电
}

u8 charge_box_popup_is_enable(void)
{
    return sys_cb.popup_en;
}

AT(.text.charge_box)
static void charge_box_popup_ctrl(vh_packet_t *packet)
{
    u8 channel = packet->buf[0];
    u8 data = packet->buf[1];
    if (!bt_tws_channel_check(channel)) {
        return;
    }

    if ((data == 1) || (data == 2)) {                   //开或关广播功能
        param_vuart_popup_flag_write(data);
        sys_cb.popup_en = data - 1;
        ble_adv0_set_ctrl(0);

    } else {                                            //翻转广播功能
        if (sys_cb.popup_en) {
            sys_cb.popup_en = 0;
            param_vuart_popup_flag_write(1);            //disable
            ble_adv0_set_ctrl(0);
        } else {
            sys_cb.popup_en = 1;
            param_vuart_popup_flag_write(2);            //enable
            ble_adv0_set_ctrl(2);
        }
    }
}

bool charge_box_bat_is_ready(void)
{
    return vhouse_cb.rem_bat_ok;
}

u8 charge_box_get_charge_box_bat_level(void)
{
    if(bt_tws_is_connected()){
        if((sys_cb.loc_bat & BIT(7)) == 0 && (sys_cb.rem_bat & BIT(7)) != 0) {
#if APP_CASE_VBAT_BIT7           
            return sys_cb.rem_house_bat;
#else
            return sys_cb.rem_house_bat & 0x7f;
#endif
        }
    }
#if APP_CASE_VBAT_BIT7
    return sys_cb.loc_house_bat;
#else
    return sys_cb.loc_house_bat & 0x7f;
#endif
    
    
}

#if (CHARGE_BOX_INTF_SEL == INTF_UART1) || (CHARGE_BOX_INTF_SEL == INTF_UART2)
//uart中断解析并匹配命令包
AT(.com_text.charge_box)
void charge_box_packet_recv(void)
{
#if QTEST_EN
    if(qtest_get_mode()){
        return ;
    }
#endif
    vh_packet_t *p = &vh_packet;
    vhouse_cb.ticks = tick_get();
    if (p->cmd == VHOUSE_CMD_OPEN_WINDOW) {
        vhouse_cb.win_ticks = vhouse_cb.ticks;
        vhouse_cb.open_win_flag = 1;
    }

    if((!vhouse_cb.ack_dat_confirm)||(vhouse_cb.ack_dat == p->buf[0])){
        vhouse_cb.ack_dat = p->buf[0];
        if((p->cmd != VHOUSE_CMD_PAIR)&&(p->cmd != VHOUSE_CMD_GET_TWS_BTADDR)){
            vhouse_cb.need_ack = 1;
        }
    }
}
#elif (CHARGE_BOX_INTF_SEL == INTF_HUART)
AT(.com_rodata.charge_box_tbl)
const char charge_box_header[]={0x55,0xAA,0xFF};
//uart中断解析并匹配命令包
AT(.com_text.charge_box)
void charge_box_packet_huart_recv(u8 *buf)
{
    if(qtest_get_mode()) {
        return ;
    }
    if(!memcmp(buf,charge_box_header,3)) {
        if(buf[4] < VH_DATA_LEN){
            vh_packet_t *packet = &vhouse_cb.packet;
            vhouse_cb.ticks = tick_get();
            if (buf[3] == VHOUSE_CMD_OPEN_WINDOW) {
                vhouse_cb.win_ticks = vhouse_cb.ticks;
                vhouse_cb.open_win_flag = 1;
            }
            if((!vhouse_cb.ack_dat_confirm) || (vhouse_cb.ack_dat == buf[5])){
                vhouse_cb.ack_dat = buf[5];
                if((buf[3] != VHOUSE_CMD_PAIR) && (buf[3] != VHOUSE_CMD_GET_TWS_BTADDR)) {
                    vhouse_cb.need_ack = 1;
                }
            }

            memcpy(packet, buf, buf[4]+5);
            packet->checksum = buf[packet->length+5];
            vhouse_cb.rx_flag=1;
        }
    }
}

#endif

//接收心跳包后需要5~10ms回应1字节
AT(.com_text.timer)
void charge_box_heart_beat_ack(void)
{
    if (vhouse_cb.need_ack == 1) {
        if (tick_check_expire(vhouse_cb.ticks, 6)) {
#if (CHARGE_BOX_INTF_SEL == INTF_HUART)
            huart_tx((u8*)&vhouse_cb.ack_dat,1);
#elif (CHARGE_BOX_INTF_SEL == INTF_UART1)
            UART1DATA = vhouse_cb.ack_dat;
#elif (CHARGE_BOX_INTF_SEL == INTF_UART2)
            UART2DATA = vhouse_cb.ack_dat;
#endif
            vhouse_cb.need_ack = 2;
        }
    }
}


AT(.text.charge_box)
static void charge_box_send_data(u8 *buf,u8 len)
{
    //超过5ms没有接收到VBUS程序才发送数据
    while (!tick_check_expire(vhouse_cb.ticks, 5)) {
         WDT_CLR();
    }
    if(vhouse_cb.need_ack == 2) {
        while (!(UART1CON & BIT(8)));
    }
    vhouse_cb.need_ack = 0;

    if(tick_check_expire(vhouse_cb.ticks, 80)){
        return;
    }

#if (CHARGE_BOX_INTF_SEL == INTF_HUART)
    huart_tx(buf,len);
#elif (CHARGE_BOX_INTF_SEL == INTF_UART1)
    for(u8 i = 0; i < len; i++){
        uart1_putchar(buf[i]);
    }
#elif (CHARGE_BOX_INTF_SEL == INTF_UART2)
    for(u8 i = 0; i < len; i++){
        uart2_putchar(buf[i]);
    }
#endif
#if SWETZ 
    if(buf[3] != 0x01){
        printf("box tx:");
        print_r(buf, len);
    }

#endif 
}

AT(.text.charge_box)
static void charge_box_cmd_ack(vh_packet_t *packet)
{
    u8 *buf = (u8*)packet;
    u8 length = 5 + packet->length;
    buf[length] = crc8_maxim(buf, length);
    charge_box_send_data(buf, length + 1);
}

AT(.text.charge_box)
static void charge_box_send_msg_suceess(void)
{
    vh_packet_t *packet = &vhouse_cb.packet;
    packet->header = 0xAA55;
    packet->distinguish = VHOUSE_DISTINGUISH;
    packet->cmd = VHOUSE_CMD_SUCCESS;
    packet->length = 0;
    charge_box_cmd_ack(packet);
}

AT(.text.charge_box)
static void charge_box_update_bat_value(vh_packet_t *packet)
{
    bool update_flag = false;
    if ((packet->cmd == VHOUSE_CMD_OPEN_WINDOW) || (packet->cmd == VHOUSE_CMD_GET_VBAT)||packet->cmd == VHOUSE_CMD_CLOSE_WIN_GET_VBAT) {
        if(sys_cb.loc_house_bat != packet->buf[1]) {
            sys_cb.loc_house_bat = packet->buf[1];
            sys_cb.rem_house_bat = packet->buf[1];
            update_flag = true;
        }
        if(packet->cmd == VHOUSE_CMD_CLOSE_WIN_GET_VBAT) {
            return;
        }
        if(packet->buf[2] & BIT(7)) {
            if(sys_cb.rem_bat != packet->buf[2]) {              //对方在仓内，且电量不为0时，用仓互传
                if((packet->buf[2]&0x7f) == 0) {
                    sys_cb.rem_bat |= BIT(7);
                } else {
                    sys_cb.rem_bat = packet->buf[2];
                    vhouse_cb.rem_bat_ok = true;
                }
                update_flag = true;
            }
        } else {
            if(vhouse_cb.rem_bat_ok) {
                update_flag = true;
            }
            vhouse_cb.rem_bat_ok = false;                       //对方出仓后，电量通过TWS互传
        }

        if(!vhouse_cb.inbox_sta) {
            vhouse_cb.inbox_sta = true;
            update_flag = true;
        }
    }
    vhouse_cb.update_ear_flag = update_flag;
}

//分析电量接收包并回应相关电量数据
AT(.text.charge_box)
static void charge_box_vbat_ack(vh_packet_t *packet)
{
    vhouse_cb.cmd3_rx_flag = 0;
    u8 channel = packet->buf[0];
    if (!bt_tws_channel_check(channel)) {
//        vhouse_send_message_err();
        return;
    }
    charge_box_update_bat_value(packet);

    //发送电量响应包
    packet->header = 0xAA55;
    packet->distinguish = VHOUSE_DISTINGUISH;
    packet->length = 0x04;
    packet->buf[0] = channel;
    packet->buf[2] = sys_cb.loc_bat;
    packet->buf[3] = sys_cb.charge_sta;
    charge_box_cmd_ack(packet);
}

#if CHARGE_BOX_LK_CRC_EN
AT(.text.charge_box)
static u16 charge_box_get_tws_link_key_crc(u8* tws_addr)
{
    u8 link_key_info[16];
    u16 crc = get_random(0xffff);
    bool ret = bt_tws_get_link_info_key(tws_addr,link_key_info);
    if (ret) {
        crc = calc_crc(link_key_info, 16, 0xffff);
    }
    return crc;
}
#endif

AT(.text.charge_box)
static void charge_box_pair_ack(vh_packet_t *packet)
{
    u8 channel = packet->buf[0];
    if (!bt_tws_channel_check(channel)) {
        return;
    }
    if((channel != LEFT_CHANNEL_USER) || vhouse_cb.cmd3_rx_flag){
        //发送蓝牙地址包
        u8 bt_tws_addr[6];
        u8 feature = bt_tws_get_link_info(bt_tws_addr);
        memset(packet, 0, sizeof(vh_packet_t));
        packet->header = 0xAA55;
        packet->distinguish = VHOUSE_DISTINGUISH;
        packet->cmd = VHOUSE_CMD_GET_TWS_BTADDR;
        packet->length = 14;
        packet->buf[0] = (channel == RIGHT_CHANNEL_USER) ? LEFT_CHANNEL_USER : RIGHT_CHANNEL_USER; //发送对方声道
        memcpy(packet->buf+1, bt_tws_addr, 6);              //TWS地址
        memcpy(packet->buf+7, xcfg_cb.bt_addr, 6);          //本地的地址
        packet->buf[13] = feature;                          //TWS主从Feature
#if CHARGE_BOX_LK_CRC_EN
        u16 linkkey_crc = charge_box_get_tws_link_key_crc(bt_tws_addr);
        packet->length += 2;
        packet->buf[14] = linkkey_crc;
        packet->buf[15] = linkkey_crc >> 8;
#endif
        charge_box_cmd_ack(packet);
        if (!bt_tws_is_slave()) {
            bt_nor_disconnect();
            ble_adv0_idx_update();
        }
    }else{
        vhouse_cb.cmd3_rx_flag = 1;
    }

}

AT(.text.charge_box)
static void charge_box_clr_tws_link_info(void)
{
    bt_clr_master_addr();
    bt_tws_delete_link_info_with_tag(BT_INFO_TAG_CHARGE_BOX, (uint32_t)__builtin_return_address(0));
}

#if !SWETZ_FACTORY_RESET
AT(.text.iodm)
static void charge_box_clr_bt_all_link_info(void)
{
    bt_clr_master_addr();
    bt_nor_delete_link_info();
    bt_tws_delete_link_info_with_tag(BT_INFO_TAG_CHARGE_BOX, (uint32_t)__builtin_return_address(0));
}

#else 

AT(.text.iodm)
void charge_box_factory_reset(void)
{
    ab_mate_device_reset();
}

#endif




AT(.text.charge_box)
static void charge_box_get_tws_btaddr_ack(vh_packet_t *packet)
{
    vhouse_cb.cmd3_rx_flag = 1;
    u8 tws_role = 0;
    u8 bt_tws_addr[6], new_addr[6];
    u8 channel = packet->buf[0];
    u8 pkt_feature = packet->buf[13];
    u8 feature = bt_tws_get_link_info(bt_tws_addr);     //获取TWS地址及feature

#if SWETZ 
    printf("charge_box_get_tws_btaddr_ack: feature 0x%x, tws_addr ", feature);
    print_r(bt_tws_addr, 6);  
    print_r(packet, sizeof(vh_packet_t));
#endif 

    if (!bt_tws_channel_check(channel)) {
        return;
    }
#if CHARGE_BOX_LK_CRC_EN
    u16 loc_linkkey_crc = charge_box_get_tws_link_key_crc(bt_tws_addr);
	u16 rem_linkkey_crc = packet->buf[14] | (packet->buf[15] << 8);
#endif
    //TWS地址不匹配或Feature不对需要清除配对信息
    if ((memcmp(packet->buf+1, bt_tws_addr, 6) != 0) ||
#if CHARGE_BOX_LK_CRC_EN
        (loc_linkkey_crc != rem_linkkey_crc) ||
#endif
        (feature == pkt_feature) || (feature == 0) || (pkt_feature == 0)) {

		if(bt_tws_is_connected()){
			bt_tws_disconnect();
		}

        tws_role = bt_tws_get_tws_role();
        TRACE("BT_ADDDR_IS_DIFF: %d\n", tws_role);
        charge_box_clr_tws_link_info();
        if (tws_role) {
            memcpy(new_addr, packet->buf + 7, 6);     //保存master
        } else {
            memcpy(new_addr, xcfg_cb.bt_addr, 6);
        }
		tws_update_local_addr(new_addr);
        u8 data[4];
        data[0] = new_addr[2];
        data[1] = new_addr[3];
        data[2] = new_addr[4];
        data[3] = new_addr[5];
        bt_tws_put_link_info_addr(new_addr,data);
        bt_tws_put_link_info_feature(new_addr, tws_role);
    }
    if (sys_cb.tws_force_channel == 1) {
         charge_box_send_msg_suceess();
    }

	if(tws_role){
		bt_tws_connect();
	}
}

AT(.text.charge_box)
void bsp_vhouse_get_ear_addr(vh_packet_t *packet)
{
    u8 channel = packet->buf[0];
    if (!bt_tws_channel_check(channel)) {
        return;
    }

    //发送蓝牙地址包
    u8 bt_tws_addr[6];
    u8 feature = bt_tws_get_link_info(bt_tws_addr);
    memset(packet, 0, sizeof(vh_packet_t));
    packet->header = 0xAA55;
    packet->distinguish = VHOUSE_DISTINGUISH;
    packet->cmd = VHOUSE_CMD_GET_EAR_ADDR;
    packet->length = 16;
    packet->buf[0] = channel;
    memcpy(packet->buf+1, bt_tws_addr, 6);              //TWS地址
    memcpy(packet->buf+7, xcfg_cb.bt_addr, 6);          //本地的地址
    packet->buf[13] = feature;                          //TWS主从Feature

    u16 link_key_sum = 0;
    u8 link_key[16];
    memset(link_key, 0, 16);
    btstack_get_link_key(bt_tws_addr, link_key, NULL, 1);
    for (u8 i = 0; i < 16; i++) {
        link_key_sum += link_key[i];
    }
    memcpy(packet->buf+14, (u8 *)&link_key_sum, 2);

    charge_box_cmd_ack(packet);
}

AT(.text.charge_box)
static void charge_box_analysis_packet(vh_packet_t *packet)
{
    u8 cmd = packet->cmd;

    switch (cmd) {
        case VHOUSE_CMD_GET_VBAT:
          //  TRACE("VHOUSE_CMD_GET_VBAT\n");
            charge_box_vbat_ack(packet);
            break;

        case VHOUSE_CMD_PAIR:
            TRACE("VHOUSE_CMD_PAIR\n");
            charge_box_pair_ack(packet);
            break;

        case VHOUSE_CMD_GET_TWS_BTADDR:
            TRACE("VHOUSE_CMD_GET_TWS_BTADDR\n");
            charge_box_get_tws_btaddr_ack(packet);
            break;

        case  VHOUSE_CMD_CLEAR_PAIR:
#if !SWETZ_FACTORY_RESET        
            TRACE("VHOUSE_CMD_CLEAR_PAIR\n");
            charge_box_clr_bt_all_link_info();                         //删除所有配对信息
#else
            TRACE("VHOUSE_CMD_FACTORY_RESET\n"); 
            charge_box_factory_reset();
#endif


            break;

        case  VHOUSE_CMD_PWROFF:
            TRACE("VHOUSE_CMD_PWROFF\n");
            vhouse_cb.status = 2;                           //充满电
            func_cb.sta = FUNC_PWROFF;
            break;

        case VHOUSE_CMD_CLOSE_WINDOW:
            TRACE("VHOUSE_CMD_CLOSE_WINDOW\n");
            vhouse_cb.open_win_flag = 0;
            vhouse_cb.status = 0;                           //关盖, 充电
            charge_box_vbat_ack(packet);
            break;

        case VHOUSE_CMD_OPEN_WINDOW:
            TRACE("VHOUSE_CMD_OPEN_WINDOW\n");
            charge_box_open_windows();
            charge_box_vbat_ack(packet);
            break;

        case VHOUSE_CMD_ENABLE_POPUP:
            TRACE("VHOUSE_CMD_ENABLE_POPUP\n");
            charge_box_popup_ctrl(packet);
            break;

        case VHOUSE_CMD_GET_EAR_ADDR:
            TRACE("VHOUSE_CMD_GET_EAR_ADDR\n");
            bsp_vhouse_get_ear_addr(packet);
            break;

        case VHOUSE_CMD_SYS_RST:
            sw_reset_kick(SW_RST_FLAG);
            break;
#if SWETZ_BT_TO_PAIR
        case VHOUSE_CMD_AG_PAIR:
            printf("VHOUSE_CMD_AG_PAIR\r\n");
            msg_enqueue(EVT_BOX_CMD_AG_PAIR);
            break;
#endif

        default:
            break;
    }

    le_popup_vhouse_cmd_notice(cmd);
}

AT(.text.charge_box)
static void charge_box_analysis_packet_for_charge(vh_packet_t *packet)
{
    switch (packet->cmd) {
        //仓关盖后获取电量
        case VHOUSE_CMD_CLOSE_WIN_GET_VBAT:
            TRACE("VHOUSE_DISP_VBAT\n");
            charge_box_vbat_ack(packet);
            vhouse_cb.open_win_flag = 0;
			vhouse_cb.status = 0;                           //关盖, 充电
            break;

        //仓开盖后获取电量
        case VHOUSE_CMD_OPEN_WINDOW:
            TRACE("VHOUSE_CMD_OPEN_WINDOW\n");
        case VHOUSE_CMD_GET_VBAT:
        //    TRACE("VHOUSE_CMD_GET_VBAT\n");
            charge_box_vbat_ack(packet);
            vhouse_cb.status = 1;                           //开窗，停止充电
            break;

        case VHOUSE_CMD_PWROFF:
            TRACE("VHOUSE_CMD_PWROFF\n");
            vhouse_cb.status = 2;                           //充满电
            break;

#if SWETZ
        case VHOUSE_CMD_PAIR:
            TRACE("VHOUSE_CMD_PAIR\n");
            charge_box_pair_ack(packet);
            break;

        case VHOUSE_CMD_GET_TWS_BTADDR:
            TRACE("VHOUSE_CMD_GET_TWS_BTADDR\n");
            charge_box_get_tws_btaddr_ack(packet);
            break;

#endif

        default:
            break;
    }
}

AT(.text.charge_box)
u32 charge_box_ssw_process(u32 charge_sta)
{
#if QTEST_EN
    if(qtest_get_mode()){
        return 0;
    }
#endif
    vh_packet_t *packet = &vhouse_cb.packet;
    if(vhouse_cb.open_win_flag && tick_check_expire(vhouse_cb.win_ticks, 180)) {
        charge_box_open_windows();                          //防止开盖命令被堵而丢失的问题
    }
    if(tick_check_expire(vhouse_cb.loc_ticks, 500)) {      //没必要一直检测，容易跳来跳去
        u8 loc_bat = bsp_get_bat_level();
        //修正会充不满电的问题
        if(charge_sta&&(loc_bat==100)) {
            loc_bat=99;
        }
        if((sys_cb.loc_bat & 0x7f) != loc_bat) {

            sys_cb.loc_bat = (sys_cb.loc_bat & 0x80) | loc_bat;
           
            vhouse_cb.update_ear_flag = true;
        }
        vhouse_cb.loc_ticks = tick_get();
    }
#if (CHARGE_BOX_INTF_SEL == INTF_HUART)
    if(vhouse_cb.rx_flag) {
        if(packet->checksum == crc8_maxim((u8 *)packet, 5 + packet->length)) {
            TRACE("cmd [%d], %d\n", packet->cmd, charge_sta);
            print_r(packet,20);
            if (charge_sta) {
                charge_box_analysis_packet_for_charge(packet);
            } else {
                reset_sleep_delay();                    //耳机在仓内不进休眠, 需要接收电量心跳包数据
                charge_box_analysis_packet(packet);
                charge_box_update_bat_adv();
             }
        }
        vhouse_cb.rx_flag=0;
    }
#elif ((CHARGE_BOX_INTF_SEL == INTF_UART1) || (CHARGE_BOX_INTF_SEL == INTF_UART2))
    u8 parse_done;
    u8 ch;
    #if (CHARGE_BOX_INTF_SEL == INTF_UART1)
    while (bsp_uart1_get(&ch))
    #elif (CHARGE_BOX_INTF_SEL == INTF_UART2)
    while (bsp_uart2_get(&ch))
    #endif
    {
        WDT_CLR();
#if UPD_UART_EN
        upd_uart_check_process(ch);
#endif
        parse_done = bsp_uart_packet_parse(packet, ch);
        if ((parse_done) && (packet->checksum == crc8_maxim((u8 *)packet, 5 + packet->length))) {
            TRACE("cmd [%d], %d\n", packet->cmd, charge_sta);
            if (charge_sta) {
                charge_box_analysis_packet_for_charge(packet);
            } else {
                reset_sleep_delay();                    //耳机在仓内不进休眠, 需要接收电量心跳包数据
                charge_box_analysis_packet(packet);
                charge_box_update_bat_adv();
            }
        }
    }
#endif
    if (charge_sta == 0) {
        charge_box_update_bat_adv();
    }
    return vhouse_cb.status;
}


void charge_box_channel_confirm(void)
{
    u8 channel = bt_tws_get_tws_channel();
    if(!channel){
        return ;
    }
    vhouse_cb.ack_dat_confirm=1;
    if (channel == LEFT_CHANNEL) {
        vhouse_cb.ack_dat = LEFT_CHANNEL_USER;
    }else if(channel == RIGHT_CHANNEL){
         vhouse_cb.ack_dat = RIGHT_CHANNEL_USER;
    }
}

void charge_box_ssw_init(void)
{
    printf("charge_box_ssw_init\n");
    memset(&vhouse_cb, 0, sizeof(vhouse_cb));
	tws_lr_xcfg_sel();
    charge_box_channel_confirm();
    bt_tws_channel_read();

}
#endif

#endif
