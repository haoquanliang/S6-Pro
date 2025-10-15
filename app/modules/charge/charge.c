#include "include.h"

#if CHARGE_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if CHARGE_LOW_POWER_EN
u32 charge_pll0con0;
#endif
void unlock_code_charge(void);

static charge_cfg_t charge_cfg;

//充满电亮蓝灯时间table表
AT(.rodata.charge)
const u8 ch_bled_timeout_tbl[8] = {0, 10, 20, 30, 60, 120, 180, 255};

//充满电亮蓝灯时间控制
AT(.com_text.charge)
void charge_bled_ctrl(void)
{
    if (sys_cb.charge_bled_flag) {
        if (sys_cb.ch_bled_cnt) {
            if (sys_cb.ch_bled_cnt != 255) {
                sys_cb.ch_bled_cnt--;
            }
        } else {
            sys_cb.charge_bled_flag = 0;
            bled_set_off();
        }
    }
}

///降低充电时主控耗电
AT(.text.charge)
static void charge_power_save(void)
{
    sys_clk_free_all();                     //释放其它模块sys_clk_req的系统时钟
#if CHARGE_LOW_POWER_EN
    sys_clk_set(SYS_RC2M);                  //跑RC2M
#if (CHARGE_BOX_TYPE == CBOX_SSW)
    set_uart1_baud_rc2m_clk(9600);
#endif
    sys_cb.charge_timr0_pr = TMR0PR;
    GLOBAL_INT_DISABLE();
    TMR0CON &= ~(7 << 1);                   //timer0 clk Select system clock
//    TMR0CON |= BIT(3);                      //timer0 CLK Select RC
    TMR0CNT = 0;
    TMR0PR = 2000000/1000 - 1;              //2M Hz, 1ms_isr
    CLKCON1 &= ~(BIT(25)|BIT(26));          //saradc clk select RC
    GLOBAL_INT_RESTORE();
    clk_xosc_dis();
#else
    sys_clk_set(SYS_24M);
    dac_clk_source_sel(2);                  //dac clk select xosc26m_clk
#endif

#if CHARGE_LOW_POWER_EN
    RSTCON0 &= ~BIT(4);                     //PLLSDM reset control
    charge_pll0con0 = PLL0CON0;
    PLL0CON0 = 0;                           //disable PLL
#else
    clk_pll0_dis();
#endif
}

AT(.text.charge)
static void charge_power_recover(void)
{
#if CHARGE_LOW_POWER_EN
    clk_xosc_en();
#endif

#if CHARGE_LOW_POWER_EN
    PLL0CON0 = charge_pll0con0;
    adpll_init(DAC_OUT_SPR);                //关了pll_ldo后需要重新init trim回频率
#else
    clk_pll0_en();
#endif

    sys_clk_set(SYS_CLK_SEL);
#if !CHARGE_LOW_POWER_EN
    dac_clk_source_sel(1);                  //dac clk select adda_clk48
#else
    GLOBAL_INT_DISABLE();
    CLKCON1  |= BIT(25);                    //saradc clk select x26m_clkdiv6
    TMR0CNT = 0;
    TMR0PR = sys_cb.charge_timr0_pr;
    TMR0CON &= ~(7 << 1);                   //timer0 clk
    TMR0CON |= 6 << 1;                      //tmr_inc select x26m_div_clk = 1M
    GLOBAL_INT_RESTORE();
#if (CHARGE_BOX_TYPE == CBOX_SSW)
    set_uart1_baud(9600);
#endif
#endif
}

#if TRACE_EN
AT(.com_rodata.tkey)
const char charge_on_str[] = "Charge on\n";
AT(.com_rodata.tkey)
const char charge_off_str[] = "Charge off: %d\n";
#endif

AT(.charge_text.det)
void charge_status(u8 sta)
{
    if (sta == 1) {
        //充电开启
        sys_cb.charge_sta = 1;
        sys_cb.charge_bled_flag = 0;
        charge_led_on();
        TRACE(charge_on_str);
    } else {
        //充电关闭
        TRACE(charge_off_str, sta);
        sys_cb.charge_sta = 0;
        sys_cb.charge_bled_flag = 0;
        charge_led_off();
        if (sta == 2) {
            sys_cb.charge_sta = 2;
            //充电满亮蓝灯
            if (BLED_CHARGE_FULL) {
                sys_cb.charge_bled_flag = 1;
                sys_cb.ch_bled_cnt = charge_cfg.bled_on_pr;
                charge_bled_on();
            }
        }
    }
}

AT(.text.charge)
u8 charge_charge_on_process(void)
{
    u8 charge_sta = 0;
    static u32 counter = 0;
    counter++;
    if ((counter % 20) == 0) {
        charge_detect(0);
    }
    if ((counter % 200) == 0) {
        vusb4s_reset_dis();
        vusb_reset_clr();
        charge_bled_ctrl();
    }
    if (CHARGE_DC_IN()) {
        if ((counter % 1000) == 0) {                           //5秒
            bsp_charge_bcnt_calibration((CHARGE_BOX_EN) ? 100 : 500);
        }
    }
#if CHARGE_BOX_EN
    charge_sta = charge_box_charge_on_process();
#endif
    tkey_proc_calibration_in_charge();

    return charge_sta;
}

AT(.text.charge.process)
void charge_process(void)
{
    if (CHARGE_DC_IN() && (sys_cb.dc_in_filter == CHARGE_DC_IN_FILTER)) {
        sys_cb.dc_in_filter = CHARGE_DC_IN_FILTER + 1;
        if (func_bt_charge_dcin()) {
            sys_cb.pwrdwn_tone_en = 0;
            func_cb.sta = FUNC_CHARGE;
        }
    }
    charge_detect(1);

#if CHARGE_BOX_EN
    charge_box_process();
#endif

}

AT(.text.charge)
void charge_enter(u8 out_auto_pwron)
{
    printf("charge_enter\n");
#if CHARGE_BOX_EN
    charge_box_enter(out_auto_pwron);
#endif
	charge_power_save();

    vusb4s_reset_dis();
}

AT(.text.charge)
void charge_exit(void)
{
    charge_power_recover();
    charge_off();
#if CHARGE_BOX_EN
    charge_box_reinit();
#endif
    tkey_stop_calibration_in_charge();
    printf("charge_exit\n");
}

bool charge_power_on_pre(u32 rtccon9)
{
    int ch_dc_sw_rst = 0;
    if (sys_cb.sw_rst_flag == SW_RST_DC_IN)  {
        ch_dc_sw_rst = 1;
    }
    u8 chbox_out2pwr_en = 0;
#if CHARGE_BOX_EN
    if ((rtccon9 & BIT(4)) && (xcfg_cb.ch_out_auto_pwron_en) && (xcfg_cb.ch_box_type_sel != 3)) {    //charge inbox wakeup直接开机(5V完全掉电无维持电压的仓不开机)
        if (charge_box_outbox_stable_check()) {
            sys_cb.outbox_pwron_flag = 1;
            printf("inbox wakeup\n");
            return true;
        }
    }
        //是否需要拿起开机
    if ((sys_cb.rst_reason & BIT(17)) || (rtccon9 & BIT(3)) || (ch_dc_sw_rst) || (sys_cb.inbox_wken_flag)) {    //vusb reset or wakeup
        chbox_out2pwr_en = xcfg_cb.ch_out_auto_pwron_en;
    }
#endif
    charge_enter(chbox_out2pwr_en);
    return false;
}

AT(.text.charge)
void charge_off(void)
{
    if (charge_cfg.chag_sta >= 3) {
        charge_stop(0);
    }
}

AT(.text.charge)
void charge_set_stop_time(u16 stop_time)
{
    charge_cfg.stop_time = stop_time;
}

void charge_init(void)
{
    charge_cfg_t *p = &charge_cfg;
    memset(p, 0, sizeof(charge_cfg_t));

    p->trick_curr_en    = CHARGE_TRICK_EN;
    p->const_curr       = CHARGE_CONSTANT_CURR;
    if (p->const_curr > 15) {
        p->const_curr = (p->const_curr - 15) * 2 + 15;
    }
    p->const_curr       = p->const_curr & 0x3f;
    p->trick_curr       = CHARGE_TRICKLE_CURR & 0xf;
    p->stop_curr        = CHARGE_STOP_CURR & 0x0f;
    p->stop_volt        = CHARGE_STOP_VOLT & 0x03;
    p->leakage          = xcfg_cb.ch_leakage_sel;
    p->inbox_voltage    = xcfg_cb.ch_inbox_sel;
    p->dcin_reset       = 0x01;
    p->trick_stop_volt  = CHARGE_TRICK_STOP_VOLT;
    p->stop_time        = 18000;               //默认30分钟
    p->bled_on_pr       = ch_bled_timeout_tbl[BLED_CHARGE_FULL];
    p->charge_sta_func  = charge_status;
    p->ldo_en           = 1;                              //默认普通充电使用LDO mode，
    p->volt_follow_en   = CHARGE_VOLT_FOLLOW_EN;
    p->volt_follow_diff        = CHARGE_VOLT_FOLLOW_DIFF & 0x03;
    if (CHARGE_VOLT_FOLLOW_EN) {
        p->ldo_en       = 0;                              //开启电压跟随模式，强制不能开LD0 充电mode
    }

    if (RTCCON3 & BIT(12)) {
        sys_cb.inbox_wken_flag = 1;                     //修正触摸唤醒拿出不能开机问题
    }
    RTCCON3 &= ~BIT(12);                                //RTCCON3[12], INBOX Wakeup disable
    RTCCON11 &= ~BIT(6);                                //VUSB pull out filter
    charge_init_do(p);

#if CHARGE_BOX_EN
    charge_box_init();
#endif

    vusb4s_reset_en();
}


#if SWETZ_NTC
void charge_0p3c_init(void)
{
    charge_cfg_t *p = &charge_cfg;
    memset(p, 0, sizeof(charge_cfg_t));

    p->trick_curr_en    = CHARGE_TRICK_EN;
    p->const_curr       = xcfg_cb.ntc_0p3c_constant_curr;
    if (p->const_curr > 15) {
        p->const_curr = (p->const_curr - 15) * 2 + 15;
    }
    p->const_curr       = p->const_curr & 0x3f;
    p->trick_curr       = xcfg_cb.ntc_0p3c_charge_trickle_curr & 0xf;
    p->stop_curr        = xcfg_cb.ntc_0p3c_charge_stop_curr & 0x0f;
    p->stop_volt        = CHARGE_STOP_VOLT & 0x03;
    p->leakage          = xcfg_cb.ch_leakage_sel;
    p->inbox_voltage    = xcfg_cb.ch_inbox_sel;
    p->dcin_reset       = 0x01;
    p->trick_stop_volt  = CHARGE_TRICK_STOP_VOLT;
    p->stop_time        = 18000;               //默认30分钟
    p->bled_on_pr       = ch_bled_timeout_tbl[BLED_CHARGE_FULL];
    p->charge_sta_func  = charge_status;
    p->ldo_en           = 1;                              //默认普通充电使用LDO mode，
    p->volt_follow_en   = xcfg_cb.ntc_0p3c_charge_voltage_follow;
    p->volt_follow_diff        = CHARGE_VOLT_FOLLOW_DIFF & 0x03;
    if (xcfg_cb.ntc_0p3c_charge_voltage_follow) {
        p->ldo_en       = 0;                              //开启电压跟随模式，强制不能开LD0 充电mode
    }

    if (RTCCON3 & BIT(12)) {
        sys_cb.inbox_wken_flag = 1;                     //修正触摸唤醒拿出不能开机问题
    }
    RTCCON3 &= ~BIT(12);                                //RTCCON3[12], INBOX Wakeup disable
    RTCCON11 &= ~BIT(6);                                //VUSB pull out filter
    charge_init_do(p);

#if CHARGE_BOX_EN
    charge_box_init();
#endif

    vusb4s_reset_en();
}


#endif

#endif // CHARGE_EN
