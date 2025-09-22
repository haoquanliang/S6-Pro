#include "include.h"
#include "api.h"

#if BT_FCC_TEST_EN
    #define WORK_MODE           MODE_FCC_TEST
#elif BT_BQB_RF_EN
    #define WORK_MODE           MODE_BQB_RF_BREDR
#elif LE_BQB_RF_EN
    #define WORK_MODE           MODE_BQB_RF_BLE
#elif BT_DUT_MODE_EN
    #define WORK_MODE           MODE_NORMAL_DUT
#else
    #define WORK_MODE           MODE_NORMAL
#endif

#define BT_PROFILE          (PROF_A2DP*BT_A2DP_EN) | \
                            (PROF_HFP*BT_HFP_EN) | \
                            (PROF_SPP*BT_SPP_EN) | \
                            (PROF_HID*BT_HID_EN) | \
                            (PROF_HSP*BT_HSP_EN) | \
                            (PROF_GATT * BT_ATT_EN)

#define BT_CODEC            (CODEC_SBC) | \
                            (CODEC_AAC*BT_A2DP_AAC_AUDIO_EN) | \
                            (CODEC_MSBC * BT_HFP_MSBC_EN) | \
                            (CODEC_PLC * BT_PLC_EN) | \
                            (CODEC_LHDCV4*BT_A2DP_LHDC_AUDIO_EN) | \
                            (CODEC_LHDCV5*BT_A2DP_LHDC_AUDIO_EN) | \
                            (CODEC_LDAC*BT_A2DP_LDAC_AUDIO_EN)

#define HFP_FEATURE         (HFP_BAT_REPORT*BT_HFP_BAT_REPORT_EN) | \
                            (HFP_INBAND_RING_TONE*BT_HFP_INBAND_RING_EN) | \
                            (HFP_CALL_PRIVATE*BT_HFP_CALL_PRIVATE_FORCE_EN) | \
                            (HFP_RING_NUMBER_EN*BT_HFP_RING_NUMBER_EN)

#define HFP_FEATURE1        (HFP_RECORD_DEVICE_VOL * BT_HFP_RECORD_DEVICE_VOL_EN)

#define A2DP_FEATURE        (A2DP_AVRCP_VOL_CTRL*BT_A2DP_VOL_CTRL_EN) | \
                            (A2DP_RESTORE_PLAYING*BT_A2DP_SUPTO_RESTORE_PLAY_EN) | \
                            (A2DP_AVDTP_DELAY_REPORT) | \
                            (A2DP_AVDTP_DYN_LATENCY * BT_AVDTP_DYN_LATENCY_EN) | \
                            (A2DP_AVDTP_EXCEPT_REST_PLAY * BT_A2DP_EXCEPT_RESTORE_PLAY_EN)

#define A2DP_FEATURE1       (A2DP_AVRCP_RECORD_DEVICE_VOL*BT_A2DP_RECORD_DEVICE_VOL_EN) | \
                            (A2DP_RESET_DEVICE_VOL*BT_A2DP_VOL_REST_EN)

#define ACL2_FEATURE        (BT_2ACL_SNATCH * BT_SNATCH_EN)

///baseband
uint8_t cfg_bt_rf_def_txpwr         = 0;        //降低预置参数RF发射功率，单位0.5dbm
uint8_t cfg_bt_page_txpwr           = 0;        //降低回连RF发射功率，单位3dbm
uint8_t cfg_bt_inq_txpwr            = 0;        //降低搜索RF发射功率，单位3dbm
uint8_t cfg_ble_page_txpwr          = 0;        //降低组队RF发射功率，单位3dbm
uint8_t cfg_ble_page_rssi_thr       = 0;        //设置组队范围rssi
uint8_t cfg_ble_rf_delta_gain       = 0;        //降低BLE功率值，单位3dbm

///stack
uint8_t cfg_bt_work_mode            = WORK_MODE;
uint8_t cfg_bt_max_acl_link         = BT_2ACL_EN+1;

bool cfg_bt_dual_mode               = BT_DUAL_MODE_EN;
bool cfg_bt_tws_mode                = BT_TWS_EN;
uint8_t cfg_bt_scan_ctrl_mode       = BT_DISCOVER_CTRL_EN;
bool cfg_bt_simple_pair_mode        = BT_SIMPLE_PAIR_EN;

uint16_t cfg_bt_support_profile     = BT_PROFILE;
uint16_t cfg_bt_support_codec       = BT_CODEC;

uint8_t cfg_bt_a2dp_feature         = A2DP_FEATURE;
uint8_t cfg_bt_a2dp_feature1        = A2DP_FEATURE1;

uint8_t cfg_bt_hfp_feature          = HFP_FEATURE;
uint8_t cfg_bt_hfp_feature1         = HFP_FEATURE1;

uint8_t cfg_bt_hid_type             = BT_HID_TYPE;

uint16_t cfg_bt_2acl_feature        = ACL2_FEATURE;

#if BT_HID_DOUYIN_EN
uint8_t cfg_bt_hid_douyin_mode       = BT_HID_DOUYIN_EN;
uint8_t cfg_bt_hid_android_param    = 0;
#endif

const uint8_t cfg_bt_spp_rfcomm_server_ch0 = SPP_RFCOMM_SERVER_CHANNEL0;
const uint8_t cfg_bt_spp_rfcomm_server_ch1 = SPP_RFCOMM_SERVER_CHANNEL1;
const uint8_t cfg_bt_spp_rfcomm_server_ch2 = SPP_RFCOMM_SERVER_CHANNEL2;

uint8_t cfg_bt_connect_times        = 2;                                //按键回连重试次数, 5.12s * n
uint8_t cfg_bt_pwrup_connect_times  = BT_POWER_UP_RECONNECT_TIMES;      //上电回连重试次数, 5.12s * n
uint16_t cfg_bt_sup_to_connect_times = BT_TIME_OUT_RECONNECT_TIMES;     //超时断线回连重试次数, 5.12s * n, 设置(-1)为一直回连

#if BT_LINK_INFO_PAGE1_EN
uint8_t const cfg_bt_link_info_items = 8;   //保存回连信息的个数（最小1，最大8）
#endif

///app
const char bt_local_name[] = BT_NAME_DEFAULT;
const char bt_local_addr[6] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};


// uint8_t cfg_bb_rf_gfsk_pwr = 127;     //调节GFSK TXPWR，范围：90~127

#if BT_RF_POWER_BALANCE_EN
//使用空闲的IO,配置内部300欧上下拉,消耗电流来实现btrf tx,rx及空闲时电流不均衡问题
#define GPIOA_TEST_BIT  (BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7))

void rf_current_supply(void);
void rf_current_recover(void);
static volatile bool bt_reconnect_flag;
static volatile u8 btrf_power_level = 0;

AT(.com_text.btrf_pwrbalance)
static void btrf_power_balance_level_set(u8 level)   //unit 3.3V/600 = 5.5mA
{
    GPIOAPU300 &= ~GPIOA_TEST_BIT;
    GPIOAPD300 &= ~GPIOA_TEST_BIT;
    if (0 == level) {
        return;
    }
    if(level >= 1){
        GPIOAPU300 |= BIT(3);
        GPIOAPD300 |= BIT(3);
    }
    if(level >= 2) {
        GPIOAPU300 |= BIT(4);
        GPIOAPD300 |= BIT(4);
    }
    if(level >= 3) {
        GPIOAPU300 |= BIT(5);
        GPIOAPD300 |= BIT(5);
    }
    if(level >= 4) {
        GPIOAPU300 |= BIT(6);
        GPIOAPD300 |= BIT(6);
    }
    if(level >= 5) {
        GPIOAPU300 |= BIT(7);
        GPIOAPD300 |= BIT(7);
    }
}

AT(.com_text.btrf_pwrbalance) FIQ
static void timer3_isr(void)
{
    if (TMR3CON & BIT(9)) {
        TMR3CPND = BIT(9);              //Clear Pending
        TMR3CON &=  ~BIT(7);
        btrf_power_balance_level_set(btrf_power_level);
    }
}

AT(.com_text.btrf_pwrbalance)
static void btrf_power_balance_level_delay_set(u32 delay_us, u8 level)   //unit us
{
    btrf_power_level = level;
    TMR3CNT = 0;
    TMR3PR = delay_us;
    TMR3CPND = BIT(9);
    TMR3CON |=  BIT(7);
}

static void timer3_init(void)
{
    CLKGAT0 |= BIT(3);                          //timer3 clkgat 要放在最前面
    u8 div_sel = 2;                             //对应BIT4
    u32 tim3pr_1s = 24000000 / div_sel - 1;     //1s对应的CNT
    TMR3CON = 0;
    TMR3CON |= BIT(4) | BIT(2);                 //BIT2:Timer clk select xosc24mhz; BIT(4):prepare clk div2

	TMR3CNT = 0;
	TMR3PR  = tim3pr_1s;                        //示例1000ms
	TMR3CON |= BIT(7);                          //Timer works in Timer Mode，Timer overflow interrupt enable
    TMR3CON |= BIT(0);                          //Timer enable
    sys_irq_init(IRQ_TMR3_VECTOR, 1, timer3_isr);
}

static void btrf_power_balance_init(void)
{
    printf("%s\n",__func__);
    timer3_init();
    GPIOADE |= GPIOA_TEST_BIT;
    GPIOAFEN &= ~GPIOA_TEST_BIT;
    GPIOADIR |= GPIOA_TEST_BIT;
}

static void btrf_power_balance_exit(void)
{
    printf("%s\n",__func__);
    TMR3CON = 0;
    GPIOAPU300 &= ~GPIOA_TEST_BIT;
    GPIOAPD300 &= ~GPIOA_TEST_BIT;
    GPIOADE &= ~GPIOA_TEST_BIT;
}

AT(.com_text.btrf_pwrbalance)
static void btrf_power_balance_isr(u32 rf_sta)
{
    TMR3CON |= BIT(7);
    if(rf_sta & BIT(8)) {			//tx on
        btrf_power_balance_level_delay_set(750,0);  //TX开始后,TX延时一段时间才真正耗电,这里延时一段时间后才降低功耗
    } else if(rf_sta & BIT(9)){	    //tx down
        btrf_power_balance_level_delay_set(120,5);  //TX结束,增加耗电
    } else if(rf_sta & BIT(10)){	//rx on
        btrf_power_balance_level_set(2);
    } else if(rf_sta & BIT(11)){	//rx down
        btrf_power_balance_level_set(5);           //RX结束,增加耗电
    }
}

void bt_power_balance_reconnect(u8 start)
{
    bt_reconnect_flag = start;
    if (start) {
        rf_current_supply();
        btrf_power_balance_init();
    } else {
        btrf_power_balance_exit();
        rf_current_recover();
    }
}
#endif

#if BT_RF_EXT_CTL_EN || BT_RF_POWER_BALANCE_EN
const uint8_t cfg_bb_rf_ctl = BIT(0);   //bit0=RF_EXT_CTL_EN

AT(.com_text.isr.txrx)
void bb_rf_ext_ctl_cb(u32 rf_sta)
{
//	if(rf_sta & BIT(8)) {			//tx on
//	} else if(rf_sta & BIT(9)){	    //tx down
//	} else if(rf_sta & BIT(10)){	//rx on
//	} else if(rf_sta & BIT(11)){	//rx down
//	} else {						//idle
//	}
#if BT_RF_POWER_BALANCE_EN
    if (bt_reconnect_flag) {
        btrf_power_balance_isr(rf_sta);
    }
#endif
}
#endif

//自定义蓝牙类别图标，根据需要选择
u32 bt_get_class_of_device(void)
{
    if(cfg_bt_work_mode == MODE_BQB_RF_BREDR) {
        return 0x0000;
    }
#if BT_HID_TYPE == 2
    return 0x0025c0;    //GamePad           - 游戏手柄
#else
//    return 0x002540;    //Keyboard          - 键盘图标，Android带显示电量，IOS不带电量显示。全部IOS均可连接HID拍照。
//    return 0x240418;    //HeadPhone         - 耳机图标，Android和IOS均带电量显示。
    return 0x240404;    //WearableHeadset   - 耳机图标，Android和IOS均带电量显示。（默认使用）
#endif
}

//PIN配对码（最多16个字符），默认"0000"
//#if !BT_SIMPLE_PAIR_EN
//const char *bt_get_pin_code(void)
//{
//    return "0000";
//}
//#endif

//回连间隔（N+3000）mS，间隔越大，下一次回连越慢，更容易被其他手机搜索连接，关闭TWS 功能时 N应大于等于2000
//u32 bt_get_conn_fail_delay(void)
//{
//    return 200;
//}

//是否需要一直回连手机
//bool bt_is_always_reconn(void)
//{
//    return false;
//}

//自定义回连方式，order为回连信息序号
//uint8_t connect_addr[6];
//void bt_cocnnect_order(uint8_t order)
//{
//    if(bt_nor_get_link_info_addr(connect_addr, order)) {
//        bt_connect_address();
//    }
//}
//
//uint8_t bt_get_connect_addr(uint8_t *bd_addr, uint16_t *times)
//{
//    *times = 2;       //n*5.12s
//    memcpy(bd_addr, connect_addr, 6);
//    return 1;
//}

const char *bt_get_local_name(void)
{
#if 1
#if IODM_TEST_EN
    bt_get_new_name(xcfg_cb.bt_name);
#endif

#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        return xcfg_cb.bthid_name;
    }
#endif // FUNC_BTHID_EN
    return xcfg_cb.bt_name;
#else
    return bt_local_name;
#endif // 1
}


void bt_clr_master_addr(void)
{
    u8 addr[6];
    memset(addr, 0, 6);
    cm_write8(PARAM_MASTER_ADDR_VALID, 0);
    cm_write(addr, PARAM_MASTER_ADDR, 6);
    cm_sync();
}

void bt_save_master_addr(u8 *addr)
{
    if (bt_tws_is_ms_switch()||qtest_get_mode()) {
        u8 mst_addr[6];
        u8 valid = cm_read8(PAGE0(PARAM_MASTER_ADDR_VALID));

        if(valid == 1) {
            cm_read(mst_addr, PAGE0(PARAM_MASTER_ADDR), 6);
            if(memcmp(mst_addr, addr, 6) == 0) {
                return;
            }
        }

        cm_write8(PARAM_MASTER_ADDR_VALID, 1);
        cm_write(addr, PARAM_MASTER_ADDR, 6);
        cm_sync();
    }
}

bool bt_get_master_addr(u8 *addr)
{
    if ((bt_tws_is_ms_switch()||qtest_get_mode()) && cm_read8(PARAM_MASTER_ADDR_VALID)) {
        cm_read(addr, PARAM_MASTER_ADDR, 6);
        return true;
    }
    return false;
}

//删除所有配对信息
void bt_clr_all_link_info(void)
{
    bt_clr_master_addr();
    bt_nor_delete_link_info();
    bt_tws_delete_link_info();
}

//删除TWS配对信息
void bt_tws_clr_link_info(void)
{
    bt_clr_master_addr();
    bt_tws_delete_link_info();
}


void bt_get_local_bd_addr(u8 *addr)
{
    if (bt_get_master_addr(addr)&&(!qtest_is_send_btmsg())) {
        return;
    }
#if BT_LOCAL_ADDR
    param_random_key_read(&addr[2]);
    addr[0] = 0x41;
    addr[1] = 0x42;
#else
#if IODM_TEST_EN
    if(bt_get_qr_addr(addr)){
        return;
    }
#endif
    memcpy(addr, xcfg_cb.bt_addr, 6);
#endif
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        addr[5] ^= BIT(0);
    }
#endif // FUNC_BTHID_EN
}

u32 bt_get_rand_seed(void)
{
    return sys_cb.rand_seed;
}

#if SWETZ
void bt_get_fixed_bd_addr(u8 *addr)
{
#if IODM_TEST_EN
    if(bt_get_qr_addr(addr)){
        return;
    }
#endif
    memcpy(addr, xcfg_cb.bt_addr, 6);
}
#endif


void bt_get_link_info_flash(void *buf, u16 addr, u16 size)
{
//    printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cm_read(buf, BTHID_CM_PAGE(addr), size);
    } else
#endif // FUNC_BTHID_EN
    {
        if ((addr + size) <= PAGE_DATA_SIZE) {
            cm_read(buf, BT_CM_PAGE(addr), size);
#if BT_LINK_INFO_PAGE1_EN
        } else {
            cm_read(buf, BT_CM_PAGE1(addr - link_info_page_size), size);
#endif
        }
    }
//    print_r(buf, size);
}

void bt_put_link_info_flash(void *buf, u16 addr, u16 size)
{
//    printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
//    print_r(buf, size);
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cm_write(buf, BTHID_CM_PAGE(addr), size);
    } else
#endif // FUNC_BTHID_EN
    {
        if ((addr + size) <= PAGE_DATA_SIZE) {
            cm_write(buf, BT_CM_PAGE(addr), size);
#if BT_LINK_INFO_PAGE1_EN
        } else {
            cm_write(buf, BT_CM_PAGE1(addr - link_info_page_size), size);
#endif
        }
    }
}

void bt_get_ext_link_info_flash(void *buf, u16 addr, u16 size)
{
//    printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    cm_read(buf, EXT_CM_PAGE(addr), size);
//    print_r(buf, size);
}

void bt_put_ext_link_info_flash(void *buf, u16 addr, u16 size)
{
//    printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
//    print_r(buf, size);
    cm_write(buf, EXT_CM_PAGE(addr), size);
}

void bt_sync_link_info(void)
{
    cm_sync();
}

#if TEST_MODE_BT_INFO
u8 test_bt_buf[512] AT(.test_buf);
//u8 test_tws_buf[64] AT(.test_buf);
void bt_test_mode_init(void)
{
    memset(test_bt_buf,0x00,sizeof(test_bt_buf));
//    memset(test_tws_buf,0x00,sizeof(test_tws_buf));
}
static bool bt_is_test_mode(void)
{
    if (cfg_bt_work_mode == MODE_IODM_TEST ||\
        cfg_bt_work_mode == MODE_FCC_TEST ||\
        cfg_bt_work_mode == MODE_BQB_RF_BREDR || qtest_get_mode()) {
        return true;
    } else {
        return false;
    }
}

void bt_get_link_info_ram(void *buf, u16 addr, u16 size)
{
    memcpy(buf,&test_bt_buf[addr],size);
}

void bt_put_link_info_ram(void *buf, u16 addr, u16 size)
{
    memcpy(&test_bt_buf[addr],buf,size);
}

//void bt_get_ext_link_info_ram(void *buf, u16 addr, u16 size)
//{
//    memcpy(buf,&test_tws_buf[addr],size);
//}
//
//void bt_put_ext_link_info_ram(void *buf, u16 addr, u16 size)
//{
//    memcpy(&test_tws_buf[addr],buf,size);
//}

#endif

void bt_get_link_info(void *buf, u16 addr, u16 size)
{
#if TEST_MODE_BT_INFO
    if (bt_is_test_mode()) {
        bt_get_link_info_ram(buf,addr,size);
    } else
#endif
    {
        bt_get_link_info_flash(buf,addr,size);
    }
}

void bt_put_link_info(void *buf, u16 addr, u16 size)
{
#if TEST_MODE_BT_INFO
    if (bt_is_test_mode()) {
        bt_put_link_info_ram(buf,addr,size);
    } else
#endif
    {
        bt_put_link_info_flash(buf,addr,size);
    }
}

void bt_get_ext_link_info(void *buf, u16 addr, u16 size)
{
    bt_get_ext_link_info_flash(buf,addr,size);
}

void bt_put_ext_link_info(void *buf, u16 addr, u16 size)
{
    bt_put_ext_link_info_flash(buf,addr,size);
}

#if BT_FCC_TEST_EN
uint8_t set_delta_gain(uint8_t fcc_packet_mode) {
    uint8_t delta_dig_gain = 0;
	switch (fcc_packet_mode) {
    case FCC_TYPE_MODE_BLE1M:
        delta_dig_gain = 0;
        break;
    case FCC_TYPE_MODE_BLE2M:
        delta_dig_gain = 0;
        break;
    case FCC_TYPE_MODE_BR:
        delta_dig_gain = 0;
        break;
    case FCC_TYPE_MODE_EDR:
        delta_dig_gain = 0;
        break;
    default:
        break;
	}
    return delta_dig_gain;           //设置功率差值
}
#endif

void bt_decode_init_cb(uint8_t media_codec_type)
{
//    printf("bt_decode_init_cb %d\n", media_codec_type);
#if BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN
    if (media_codec_type == 0x10 || media_codec_type == 0x11) {     //LHDC LDAC
#if ANC_ALG_EN
        if (sys_cb.anc_alg_en) {
            sys_cb.anc_alg_type_bak = bsp_anc_alg_get_type();
            bsp_anc_alg_stop();
        }
#endif // ANC_ALG_EN
    } else {
#if ANC_ALG_EN
        if (sys_cb.anc_alg_type_bak) {
            if ((!bt_decode_is_lhdc()) && (!bt_decode_is_ldac())) {
                bsp_anc_alg_start(sys_cb.anc_alg_type_bak);
                sys_cb.anc_alg_type_bak = 0;
            }
        }
#endif // ANC_ALG_EN
    }
#endif
}

#if LE_BQB_RF_EN || BT_DUT_MODE_EN
//蓝牙线程获取测试功率衰减的回调函数（单位0.5dbm，取值范围0~15）
uint8_t rf_test_get_txpwr_dec_cb(uint8_t test_type)
{
    u8 txpwr_dec = 0;

    switch(test_type) {
    case TEST_TYPE_BR:
        break;
    case TEST_TYPE_EDR:
        if(bt_is_dut_en()) {
            txpwr_dec = 0;      //dut mode
        } else if (bt_is_fcc_test()) {
            txpwr_dec = 0;      //fcc mode
        } else {
            txpwr_dec = 0;      //iodm mode
        }
        break;

    case TEST_TYPE_LE_1M:
    case TEST_TYPE_LE_2M:
    case TEST_TYPE_LE_CODED_S8:
    case TEST_TYPE_LE_CODED_S2:
        if(bt_is_bqb_rf_ble()) {
            txpwr_dec = 0;     //dut mode
        } else if (bt_is_fcc_test()) {
            txpwr_dec = 0;     //fcc mode
        } else {
            txpwr_dec = 0;      //iodm mode
        }
        break;
    }

    return txpwr_dec;
}
#endif
