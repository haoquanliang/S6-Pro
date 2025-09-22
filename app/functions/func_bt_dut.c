#include "include.h"
#include "func.h"
#include "api.h"

void bt_test_mode_init(void);
#define TEST_BT_ADDR_EN                     0   //测试是否使用固定地址
#define TEST_MODE_PLL_DISABLE               0   //测试是否关闭PLL
#define TEST_MODE_MSC_EN                    0   //测试是否循环播放白噪声

AT(.rodata.bt.dut)
const u8 led_bt_cbt_tbl[] = {
    0xff, 0x00, 0x02, 0x00, 0xff, 0x00, 0x02, 0x00, 0xff, 0x00, 0x02, 0x00, 0xff, 0x00, 0x02, 0x00,
};


#if IODM_TEST_EN
//IODM测试模式
AT(.text.func.bt_iodm)
void func_bt_iodm(void)
{
    printf("%s\n", __func__);
    if (f_bt.bt_is_inited) {
        bt_disconnect(0);
        bt_off();
        f_bt.bt_is_inited = 0;
    }
    u32 pwroff_time = sys_cb.pwroff_time;
    u32 sleep_time = sys_cb.sleep_time;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = -1;                          //关闭未连接自动关机
    sys_cb.sleep_time = -1;                                                 //不进siff mode
    sys_cb.sleep_en = 0;
    memcpy(&xcfg_cb.led_btinit, led_bt_cbt_tbl, sizeof(led_bt_cbt_tbl));    //红灯常亮
#if TEST_BT_ADDR_EN
    u8 addr[6];
    memcpy(addr,xcfg_cb.bt_addr,6);
    memset(xcfg_cb.bt_addr, 0x68, 6);                                   //固定蓝牙地址
#endif
    cfg_bt_work_mode = MODE_IODM_TEST;
#if TEST_MODE_BT_INFO
    bt_test_mode_init();
#endif

    func_bt_enter();
    while (func_cb.sta == FUNC_BT_IODM) {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
#if TEST_BT_ADDR_EN
    memcpy(xcfg_cb.bt_addr,addr,6);
#endif
    cfg_bt_work_mode = MODE_NORMAL;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = pwroff_time;
    sys_cb.sleep_time = sleep_time;
}
#endif


#if FUNC_BT_FCC_EN
//FCC 测试模式
AT(.text.func.bt_fcc)
void func_bt_fcc(void)
{
    printf("%s\n", __func__);
    if (f_bt.bt_is_inited) {
        bt_disconnect(0);
        bt_off();
        f_bt.bt_is_inited = 0;
    }
    u32 pwroff_time = sys_cb.pwroff_time;
    u32 sleep_time = sys_cb.sleep_time;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = -1;                          //关闭未连接自动关机
    sys_cb.sleep_time = -1;                                                 //不进siff mode
    sys_cb.sleep_en = 0;
    memcpy(&xcfg_cb.led_btinit, led_bt_cbt_tbl, sizeof(led_bt_cbt_tbl));    //红灯常亮
#if TEST_BT_ADDR_EN
    u8 addr[6];
    memcpy(addr,xcfg_cb.bt_addr,6);
    memset(xcfg_cb.bt_addr, 0x68, 6);                                   //固定蓝牙地址
#endif
    cfg_bt_work_mode = MODE_FCC_TEST;
#if TEST_MODE_BT_INFO
    bt_test_mode_init();
#endif

    func_bt_enter();
    while (func_cb.sta == FUNC_BT_FCC) {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
#if TEST_BT_ADDR_EN
    memcpy(xcfg_cb.bt_addr,addr,6);
#endif
    cfg_bt_work_mode = MODE_NORMAL;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = pwroff_time;
    sys_cb.sleep_time = sleep_time;
}
#endif

#if FUNC_BT_DUT_EN

#if TEST_MODE_MSC_EN
static void bt_dut_play_msc(void)
{
    u32 tick = tick_get();
    while ((AUBUF1CON & BIT(8)) == 0) {
        s16 pcm = (s16)(sys_get_rand_key() & 0xffff);
        dac1_put_sample_16bit(pcm, pcm);
        if (tick_check_expire(tick, 200)) {
            break;
        }
    }
}
#endif

//CBT测试模式, 红灯常亮
AT(.text.func.bt_dut)
void func_bt_dut(void)
{
    printf("%s\n", __func__);
    if (f_bt.bt_is_inited) {
        bt_disconnect(0);
        bt_off();
        f_bt.bt_is_inited = 0;
    }
    u32 pwroff_time = sys_cb.pwroff_time;
    u32 sleep_time = sys_cb.sleep_time;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = -1;                          //关闭未连接自动关机
    sys_cb.sleep_time = -1;                                                 //不进siff mode
    sys_cb.sleep_en = 0;
    memcpy(&xcfg_cb.led_btinit, led_bt_cbt_tbl, sizeof(led_bt_cbt_tbl));    //红灯常亮
#if TEST_BT_ADDR_EN
    u8 addr[6];
    memcpy(addr,xcfg_cb.bt_addr,6);
    memset(xcfg_cb.bt_addr, 0x68, 6);                                   //固定蓝牙地址
#endif
    xcfg_cb.warning_bt_pair = 0;                                            //关闭配对提示音
    cfg_bt_work_mode  = MODE_BQB_RF_BREDR;                                  //使能DUT模式
    uint8_t bt_tws_pair_mode = cfg_bt_tws_pair_mode;                        //保留 配对模式
    cfg_bt_tws_pair_mode = TWS_PAIR_OP_API;                                 //手动配对
    u8 bt_tws_en = xcfg_cb.bt_tws_en;
    u8 ble_en = xcfg_cb.ble_en;
    u16 bt_tws_feature = cfg_bt_tws_feat;
    xcfg_cb.bt_tws_en = 0;
    xcfg_cb.ble_en = 0;
#if TEST_MODE_BT_INFO
    bt_test_mode_init();
#endif

#if TEST_MODE_PLL_DISABLE
    sys_clk_set(SYS_24M);
    dac_clk_source_sel(2);                                                  //dac clk select xosc26m_clk
    clk_pll0_dis();                                                         //disable PLL
#endif

#if TEST_MODE_MSC_EN
    dac1_spr_set(SPR_16000);
    dac1_aubuf_init();
    dac1_vol_set(0x3fff);
    dac1_fade_in();
#endif

    func_bt_enter();
    while (func_cb.sta == FUNC_BT_DUT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
#if TEST_MODE_MSC_EN
        bt_dut_play_msc();
#endif
    }
    func_bt_exit();

#if TEST_MODE_PLL_DISABLE
    adpll_init(DAC_OUT_SPR);
    dac_clk_source_sel(1);
#endif

#if TEST_BT_ADDR_EN
    memcpy(xcfg_cb.bt_addr,addr,6);
#endif
    cfg_bt_work_mode = MODE_NORMAL;
    cfg_bt_tws_pair_mode = bt_tws_pair_mode;
    cfg_bt_tws_feat = bt_tws_feature;
    sys_cb.pwroff_delay = sys_cb.pwroff_time = pwroff_time;
    sys_cb.sleep_time = sleep_time;
    xcfg_cb.bt_tws_en = bt_tws_en;
    xcfg_cb.ble_en = ble_en;
    cfg_bt_dual_mode = BT_DUAL_MODE_EN * xcfg_cb.ble_en;
}
#endif
