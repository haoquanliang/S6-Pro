#include "include.h"

xcfg_cb_t xcfg_cb;
sys_cb_t sys_cb AT(.buf.bsp.sys_cb);
volatile int micl2gnd_flag;
volatile u32 ticks_50ms;
uint8_t cfg_spiflash_speed_up_en = FLASH_SPEED_UP_EN;        //SPI FLASH提速。部份FLASH不支持提速
uint8_t cfg_pmu_vddio_lp_enable = SYS_VDDIO_LP_EN;

void sd_detect(void);
void tbox_uart_isr(void);
void testbox_init(void);
bool exspiflash_init(void);
void ledseg_6c6s_clr(void);
u8 getcfg_buck_mode_en(void);

#if BT_PWRKEY_5S_DISCOVER_EN
AT(.com_text.detect)
void pwrkey_5s_on_check(void)
{
    if (!xcfg_cb.bt_pwrkey_nsec_discover) {
        return;
    }
    if (sys_cb.pwrkey_5s_check) {
        if (PWRKEY_IS_PRESS() || TKEY_IS_PRESS()) {
            if (tick_check_expire(sys_cb.ms_ticks, 1000 * xcfg_cb.bt_pwrkey_nsec_discover)) {
                sys_cb.pwrkey_5s_flag = 1;
                sys_cb.pwrkey_5s_check = 0;
            }
        } else {
            sys_cb.pwrkey_5s_check = 0;
        }
    }
}
#endif // BT_PWRKEY_5S_DISCOVER_EN

#if PWRKEY_2_HW_PWRON
//软开机模拟硬开关，松开PWRKEY就关机。
AT(.com_text.detect)
void pwrkey_2_hw_pwroff_detect(void)
{
    static int off_cnt = 0;
    if (!PWRKEY_2_HW_PWRON) {
        return;
    }

    if (PWRKEY_IS_PRESS()) {
        off_cnt = 0;
    } else {
        if (off_cnt < 10) {
            off_cnt++;
        } else if (off_cnt == 10) {
            //pwrkey已松开，需要关机
            off_cnt = 20;
            sys_cb.pwrdwn_hw_flag = 1;
            sys_cb.poweron_flag = 0;
        }
    }
}
#endif // PWRKEY_2_HW_PWRON

#if USER_NTC

#if !SWETZ_NTC
AT(.com_text.ntc)
u8 sys_ntc_check(void)
{


    if (!xcfg_cb.ntc_en) {
        return  0;
    }
    static int n_cnt = 0;
    if (xcfg_cb.ntc_en) {
        if (saradc_get_value8(ADCCH_NTC) <= xcfg_cb.ntc_thd_val) {
            if (n_cnt >= 20) {
                sys_cb.ntc_2_pwrdwn_flag = 1;
                return 1;
            } else {
                n_cnt++;
            }
        } else {
            n_cnt = 0;
        }
    }
    return 0;


}

#else
AT(.com_text.str2) 
const char ntc[] = "====ntc\r\n";


enum 
{
    NTC_NORMAL = 0,
    NTC_OUT_RANGE,
};
u8 ntc_status = NTC_NORMAL;
AT(.com_text.ntc)
u8 user_ntc_check(void)
{
    if (!xcfg_cb.ntc_en) {
        return  0;
    }

    u8 ntc_value = saradc_get_value8(ADCCH_NTC);
    printf("ntc:%d vbat:%d\n", ntc_value,sys_cb.vbat);
    
        // if (func_cb.sta == FUNC_BT)
        // {
        //     if ((ntc_value > (xcfg_cb.ntc_discharge_thd_risk_high - 5)) || (ntc_value < (xcfg_cb.ntc_discharge_thd_risk_low + 5)))
        //     {
        //         sys_cb.ntc_discharge_out_normal_range = true;
        //     }
        //     else 
        //     {
        //         sys_cb.ntc_discharge_out_normal_range = false;
        //     }  
        //     if ((ntc_value > xcfg_cb.ntc_discharge_thd_risk_high) || (ntc_value < xcfg_cb.ntc_discharge_thd_risk_low))
        //     {
        //         if (ntc_status == NTC_NORMAL)
        //         {
        //             ntc_status = NTC_OUT_RANGE;
        //             printf("ntc:out range\n");
        //             sys_cb.discon_reason = 0;   //不同步关机
        //             sys_cb.pwrdwn_tone_en = 1;
        //             func_cb.sta = FUNC_PWROFF;  
        //         }
        //     }            
        // }
        // else 
        // {
        //     if ((ntc_value > xcfg_cb.ntc_charge_thd_risk_high) || (ntc_value < xcfg_cb.ntc_charge_thd_risk_low))
        //     {
        //         if (ntc_status == NTC_NORMAL)
        //         {
        //             ntc_status = NTC_OUT_RANGE;
        //             printf("ntc:out range\n");
        //             RTCCON8 = (RTCCON8 & ~BIT(6)) | BIT(1);     //disable charger function      
        //         }
        //     }
        //     else if ((ntc_value >= xcfg_cb.ntc_charge_thd_normal_low) && (ntc_value <= xcfg_cb.ntc_charge_thd_normal_high))
        //     {
        //         if (ntc_status == NTC_OUT_RANGE)    
        //         {
        //             ntc_status = NTC_NORMAL;
        //             printf("ntc:normal\n");
        //             RTCCON8 = (RTCCON8 & ~BIT(1)) | BIT(6);// enable charger function
        //         }
        //     }            
        // }

}

#endif

#endif

//timer tick interrupt(1ms)
AT(.com_text.timer)
void usr_tmr1ms_isr(void)
{
#if (GUI_SELECT & DISPLAY_LEDSEG)
    gui_scan();                     //7P屏按COM扫描时，1ms间隔
#endif

#if LED_DISP_EN
    port_2led_scan();
#endif // LED_DISP_EN

    plugin_tmr1ms_isr();
#if USER_KEY_KNOB2_EN
    bsp_key_scan();
#endif

    led_scan();
}



#if SWETZ_CHECK_INBOX
AT(.com_text.timer)
void check_ear_inbox()
{
            bool inbox = vhouse_cb.inbox_sta;
            
            if (sys_cb.flag_local_in_case)
            {
                if (!inbox)
                {
                    msg_enqueue(EVT_OUT_CASE);
                }
            }
            else 
            {
                if (inbox)
                {
#if SWETZ_OUTCASE_AFTER_NOT_KEY
                    sys_cb.flag_outcase_5s_kye_null = false;//入仓关掉出仓5s防误触
#endif
                    msg_enqueue(EVT_IN_CASE);
                }
            }
            sys_cb.flag_local_in_case = inbox;
}
#endif

#if STR_PRI
AT(.com_text.str2) 
const char str[] = "====str %d\r\n";
#endif

//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_thread(void)
{
#if APP_MESSAGE_RUN
    sys_cb.SWETZ_tick += 5;
#endif

    tmr5ms_cnt++;
    //5ms timer process
    dac_fade_process();
#if !USER_KEY_KNOB2_EN
    bsp_key_scan();
#endif
#if PWRKEY_2_HW_PWRON
    pwrkey_2_hw_pwroff_detect();
#endif
   
#if SWETZ_EVT_5S
     sys_cb.sys_5ms_cnt++;
    if (sys_cb.sys_5ms_cnt >= 1000)
    {
        sys_cb.sys_5ms_cnt = 0;        
        msg_enqueue(EVT_SYS_5S);
    }

#endif


#if (CHARGE_BOX_TYPE == CBOX_SSW)
    charge_box_heart_beat_ack();
#endif
    plugin_tmr5ms_isr();

#if MUSIC_SDCARD_EN
    sd_detect();
#endif // MUSIC_SDCARD_EN

#if USER_NTC
#if !SWETZ_NTC
    sys_ntc_check();
#endif    
#endif

#if USB_SUPPORT_EN
    usb_detect();
#endif // USB_SUPPORT_EN

#if LINEIN_DETECT_EN
    linein_detect();
#endif // LINEIN_DETECT_EN

#if USER_INEAR_DETECT_EN
    earin_detect();
#endif

#if WARNING_WSBC_EN
    warning_dec_check_kick();
#endif

    //20ms timer process
    if ((tmr5ms_cnt % 4) == 0) {
#if CHARGE_EN
        if (xcfg_cb.charge_en) {
            ///入仓自动主从切换
            if (CHARGE_DC_IN()) {
                if (sys_cb.dc_in_filter < CHARGE_DC_IN_FILTER) {
                    sys_cb.dc_in_filter++;
                }
            } else {
                sys_cb.dc_in_filter = 0;
            }
        }
#endif
    }
    
    //50ms timer process
    if ((tmr5ms_cnt % 10) == 0) {
        ticks_50ms++;
#if BT_PWRKEY_5S_DISCOVER_EN
        pwrkey_5s_on_check();
#endif // BT_PWRKEY_5S_DISCOVER_EN

    }

    //100ms timer process
    if ((tmr5ms_cnt % 20) == 0) {
        lowpwr_tout_ticks();
#if UDE_HID_EN
        if (func_cb.sta == FUNC_USBDEV) {
            ude_tmr_isr();
        }
#endif // UDE_HID_EN
        gui_box_isr();                  //显示控件计数处理

        if (sys_cb.lpwr_cnt > 0) {
            sys_cb.lpwr_cnt++;
        }

        if (sys_cb.key2unmute_cnt) {
            sys_cb.key2unmute_cnt--;
            if (!sys_cb.key2unmute_cnt) {
                msg_enqueue(EVT_KEY_2_UNMUTE);
            }
        }
        dac_fifo_detect();


    }

#if SWETZ_CHECK_INBOX
    if ((tmr5ms_cnt % 40) == 0) {//200ms
            check_ear_inbox();
    }
#endif

    //500ms timer process
    if ((tmr5ms_cnt % 100) == 0) {
        sys_cb.cm_times++;
#if FUNC_CLOCK_EN
        msg_enqueue(MSG_SYS_500MS);
#endif // FUNC_CLOCK_EN

    }

    //1s timer process
    if ((tmr5ms_cnt % 200) == 0) {
        msg_enqueue(MSG_SYS_1S);
        sys_cb.lpwr_warning_cnt++;
    }
    //10s timer process
    if ((tmr5ms_cnt % 6000) == 0) {
        msg_enqueue(EVT_MSG_SYS_30S);
        tmr5ms_cnt = 0;
    }
#if SWETZ_EVT_1S
    if ((tmr5ms_cnt % 200) == 0) {
        msg_enqueue(EVT_SYS_1S);
       
    }
#endif
}




#if SWETZ_VBAT_TO_PHONE

static uint8_t get_bat_level_from_volt_wo_charger(uint16_t volt)
{
    uint8_t level;

    if (volt >= VOLT_BAT_LEVEL_100)
    {
        level = 100;
    }
    else if (volt >= VOLT_BAT_LEVEL_95)
    {
        level = 95 + 5*(volt - VOLT_BAT_LEVEL_95)/(VOLT_BAT_LEVEL_100 - VOLT_BAT_LEVEL_95);
    }
    else if (volt >= VOLT_BAT_LEVEL_90)
    {
        level = 90 + 5*(volt - VOLT_BAT_LEVEL_90)/(VOLT_BAT_LEVEL_95 - VOLT_BAT_LEVEL_90);
    }
    else if (volt >= VOLT_BAT_LEVEL_85)
    {
        level = 85 + 5*(volt - VOLT_BAT_LEVEL_85)/(VOLT_BAT_LEVEL_90 - VOLT_BAT_LEVEL_85);
    }
    else if (volt >= VOLT_BAT_LEVEL_80)
    {
        level = 80 + 5*(volt - VOLT_BAT_LEVEL_80)/(VOLT_BAT_LEVEL_85 - VOLT_BAT_LEVEL_80);
    }
    else if (volt >= VOLT_BAT_LEVEL_75)
    {
        level = 75 + 5*(volt - VOLT_BAT_LEVEL_75)/(VOLT_BAT_LEVEL_80 - VOLT_BAT_LEVEL_75);
    }
    else if (volt >= VOLT_BAT_LEVEL_70)
    {
        level = 70 + 5*(volt - VOLT_BAT_LEVEL_70)/(VOLT_BAT_LEVEL_75 - VOLT_BAT_LEVEL_70);
    }
    else if (volt >= VOLT_BAT_LEVEL_65)
    {
        level = 65 + 5*(volt - VOLT_BAT_LEVEL_65)/(VOLT_BAT_LEVEL_70 - VOLT_BAT_LEVEL_65);
    }
    else if (volt >= VOLT_BAT_LEVEL_60)
    {
        level = 60 + 5*(volt - VOLT_BAT_LEVEL_60)/(VOLT_BAT_LEVEL_65 - VOLT_BAT_LEVEL_60);
    }
    else if (volt >= VOLT_BAT_LEVEL_55)
    {
        level = 55 + 5*(volt - VOLT_BAT_LEVEL_55)/(VOLT_BAT_LEVEL_60 - VOLT_BAT_LEVEL_55);
    }
    else if (volt >= VOLT_BAT_LEVEL_50)
    {
        level = 50 + 5*(volt - VOLT_BAT_LEVEL_50)/(VOLT_BAT_LEVEL_55 - VOLT_BAT_LEVEL_50);
    }
    else if (volt >= VOLT_BAT_LEVEL_45)
    {
        level = 45 + 5*(volt - VOLT_BAT_LEVEL_45)/(VOLT_BAT_LEVEL_50 - VOLT_BAT_LEVEL_45);
    }
    else if (volt >= VOLT_BAT_LEVEL_40)
    {
        level = 40 + 5*(volt - VOLT_BAT_LEVEL_40)/(VOLT_BAT_LEVEL_45 - VOLT_BAT_LEVEL_40);
    }
    else if (volt >= VOLT_BAT_LEVEL_35)
    {
        level = 35 + 5*(volt - VOLT_BAT_LEVEL_35)/(VOLT_BAT_LEVEL_40 - VOLT_BAT_LEVEL_35);
    }
    else if (volt >= VOLT_BAT_LEVEL_30)
    {
        level = 30 + 5*(volt - VOLT_BAT_LEVEL_30)/(VOLT_BAT_LEVEL_35 - VOLT_BAT_LEVEL_30);
    }
    else if (volt >= VOLT_BAT_LEVEL_25)
    {
        level = 25 + 5*(volt - VOLT_BAT_LEVEL_25)/(VOLT_BAT_LEVEL_30 - VOLT_BAT_LEVEL_25);
    }
    else if (volt >= VOLT_BAT_LEVEL_20)
    {
        level = 20 + 5*(volt - VOLT_BAT_LEVEL_20)/(VOLT_BAT_LEVEL_25 - VOLT_BAT_LEVEL_20);
    }
    else if (volt >= VOLT_BAT_LEVEL_15)
    {
        level = 15 + 5*(volt - VOLT_BAT_LEVEL_15)/(VOLT_BAT_LEVEL_20 - VOLT_BAT_LEVEL_15);
    }
    else if (volt >= VOLT_BAT_LEVEL_10)
    {
        level = 10 + 5*(volt - VOLT_BAT_LEVEL_10)/(VOLT_BAT_LEVEL_15 - VOLT_BAT_LEVEL_10);
    }
    else if (volt >= VOLT_BAT_LEVEL_5)
    {
        level = 5 + 5*(volt - VOLT_BAT_LEVEL_5)/(VOLT_BAT_LEVEL_10 - VOLT_BAT_LEVEL_5);
    }
    else if (volt >= VOLT_BAT_LEVEL_5)
    {
        level = 5 + 5*(volt - VOLT_BAT_LEVEL_5)/(VOLT_BAT_LEVEL_10 - VOLT_BAT_LEVEL_5);
    }
    else if (volt >= (LPWR_OFF_VBAT * 100 + 2700))
    {
        level = 5*(volt - (LPWR_OFF_VBAT * 100 + 2700))/(VOLT_BAT_LEVEL_5 - (LPWR_OFF_VBAT * 100 + 2700));
    }
    else
    {
        level = 0;
    }
    return level;
}

uint8_t app_bat_level_show_for_phone(uint8_t bat_real_level)
{
    uint8_t bat_show;

    if (bat_real_level < 15)
    {
        bat_show = 0;
    }
    else if (bat_real_level < 25)
    {
        bat_show = 1;
    }
    else if (bat_real_level < 35)
    {
        bat_show = 2;
    }
    else if (bat_real_level < 45)
    {
        bat_show = 3;
    }
    else if (bat_real_level < 55)
    {
        bat_show = 4;
    }
    else if (bat_real_level < 65)
    {
        bat_show = 5;
    }
    else if (bat_real_level < 75)
    {
        bat_show = 6;
    }
    else if (bat_real_level < 85)
    {
        bat_show = 7;
    }
    else if (bat_real_level < 95)
    {
        bat_show = 8;
    }
    else
    {
        bat_show = 9;
    }
    return bat_show;
}

uint8_t app_bat_level_show_for_app(uint8_t bat_real_level) 
{
    uint8_t bat_show;
    
    if (bat_real_level < 15)
    {
        bat_show = 10;
    }
    else if (bat_real_level < 25)
    {
        bat_show = 20;
    } 
    else if (bat_real_level < 35)
    {
        bat_show = 30;
    }    
    else if (bat_real_level < 45)
    {
        bat_show = 40;
    }  
    else if (bat_real_level < 55)
    {
        bat_show = 50;
    }  
    else if (bat_real_level < 65)
    {
        bat_show = 60;
    }
    else if (bat_real_level < 75)
    {
        bat_show = 70;
    }   
    else if (bat_real_level < 85)
    {
        bat_show = 80;
    }  
    else if (bat_real_level < 95)
    {
        bat_show = 90;
    }   
    else 
    {
        bat_show = 100;
    }
    return bat_show;
}
#endif

uint bsp_get_bat_level(void)
{
#if VBAT_DETECT_EN
    //计算方法：level = (实测电压 - 关机电压) / ((满电电压 - 关机电压) / 100)
    u16 bat_off = LPWR_OFF_VBAT * 100 + 2700;
    if (bat_off > sys_cb.vbat) {
        return 0;
    }

#if SWETZ_VBAT_TO_PHONE
     uint bat_level = get_bat_level_from_volt_wo_charger(sys_cb.vbat);
#else
    uint bat_level = (sys_cb.vbat - bat_off) / ((4200 - bat_off) / 100);
    //printf("bat level: %d %d\n", sys_cb.vbat, bat_level);
#endif
    if (bat_level > 100) {
        bat_level = 100;
    }
    return bat_level;
#else
    return 100;
#endif
}

AT(.text.bsp.sys.init)
void rtc_32k_configure(void)
{
    u32 temp = RTCCON0;

//    //xosc_div768_rtc
//    temp &= ~BIT(6);
//    temp |= BIT(9) | BIT(8);                        //sel xosc_div768_rtc
//    RTCCON0 = temp;
//    RTCCON4 |= BIT(17);                             //xosc24m low power clk enable
//    RTCCON2 = 31249;

    //clk2m_rtc_div32
    temp &= ~BIT(6);
    temp &= ~(BIT(9) | BIT(8));
    temp |= BIT(8);
    temp |= BIT(2) | BIT(0);
    RTCCON0 = temp;
    RTCCON2 = sys_get_rc2m_rtc_clk() / 32 - 1;
}

AT(.text.bsp.sys.init)
bool rtc_init(void)
{
    u32 temp;
    rtc_32k_configure();
    sys_cb.rtc_first_pwron = 0;
    temp = RTCCON0;
    if (temp & BIT(7)) {
        temp &= ~BIT(7);                            //clear first poweron flag
        RTCCON0 = temp;
        sys_cb.rtc_first_pwron = 1;
#if FUNC_CLOCK_EN
        rtc_clock_init();
#endif // FUNC_CLOCK_EN
        printf("rtc 1st pwrup\n");
        return false;
    }

    return true;
}

//UART0打印信息输出GPIO选择，UART0默认G1(PA7)
void uart0_mapping_sel(void)
{
    //等待uart0发送完成
    if(UART0CON & BIT(0)) {
        while (!(UART0CON & BIT(8)));
    }

    GPIOEDE  &= ~BIT(13);
    GPIOEPU  &= ~BIT(13);
    GPIOBPU  &= ~(BIT(2) | BIT(3));
    FUNCMCON0 = (0xf << 12) | (0xf << 8);           //clear uart0 mapping

#if (UART0_PRINTF_SEL == PRINTF_PA7)
    GPIOADE  |= BIT(7);
    GPIOAPU  |= BIT(7);
    GPIOADIR |= BIT(7);
    GPIOAFEN |= BIT(7);
    GPIOADRV |= BIT(7);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA7;           //RX0 Map To TX0, TX0 Map to G1
#elif (UART0_PRINTF_SEL == PRINTF_PB2)
    GPIOBDE  |= BIT(2);
    GPIOBPU  |= BIT(2);
    GPIOBDIR |= BIT(2);
    GPIOBFEN |= BIT(2);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB2;           //RX0 Map To TX0, TX0 Map to G2
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    GPIOBDE  |= BIT(3);
    GPIOBPU  |= BIT(3);
    GPIOBDIR |= BIT(3);
    GPIOBFEN |= BIT(3);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB3;           //RX0 Map To TX0, TX0 Map to G3
#elif (UART0_PRINTF_SEL == PRINTF_PE7)
    GPIOEDE  |= BIT(7);
    GPIOEPU  |= BIT(7);
    GPIOEDIR |= BIT(7);
    GPIOEFEN |= BIT(7);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE7;           //RX0 Map To TX0, TX0 Map to G4
#elif (UART0_PRINTF_SEL == PRINTF_PE0)
    GPIOEDE  |= BIT(0);
    GPIOEPU  |= BIT(0);
    GPIOEDIR |= BIT(0);
    GPIOEFEN |= BIT(0);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE0;           //RX0 Map To TX0, TX0 Map to G5
#elif (UART0_PRINTF_SEL == PRINTF_VUSB)
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_VUSB;          //RX0 Map To TX0, TX0 Map to G8
#endif
}

//开user timer前初始化的内容
AT(.text.bsp.sys.init)
static void sys_var_init(void)
{
    u32 rst_reason = sys_cb.rst_reason;
    memset(&sys_cb, 0, sizeof(sys_cb));
    sys_cb.rst_reason = rst_reason;
    sys_cb.loudspeaker_mute = 1;
    sys_cb.ms_ticks = tick_get();
    sys_cb.pwrkey_5s_check = 1;
    xcfg_cb.vol_max = (xcfg_cb.vol_max) ? 32 : 16;
    sys_cb.hfp2sys_mul = (xcfg_cb.vol_max + 2) / 16;
    if (SYS_INIT_VOLUME > xcfg_cb.vol_max) {
        SYS_INIT_VOLUME = xcfg_cb.vol_max;
    }
    if (WARNING_VOLUME > xcfg_cb.vol_max) {
        WARNING_VOLUME = xcfg_cb.vol_max;
    }

    sys_cb.sleep_time = -1L;
    sys_cb.pwroff_time = -1L;
    if (xcfg_cb.sys_sleep_time != 0) {
        sys_cb.sleep_time = (u32)xcfg_cb.sys_sleep_time * 10;   //100ms为单位
    }
    if (xcfg_cb.sys_off_time != 0) {
        sys_cb.pwroff_time = (u32)xcfg_cb.sys_off_time * 10;    //100ms为单位
    }

    sys_cb.sleep_delay = -1L;
    sys_cb.pwroff_delay = -1L;
    sys_cb.sleep_en = 0;
    sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
    sys_cb.led_scan_en = 1;

    if(xcfg_cb.osci_cap == 0 && xcfg_cb.osco_cap == 0) {        //没有过产测时，使用自定义OSC电容
        xcfg_cb.osci_cap = xcfg_cb.uosci_cap;
        xcfg_cb.osco_cap = xcfg_cb.uosco_cap;
    }
    if(xcfg_cb.ft_rf_param_en == 0 && xcfg_cb.bt_rf_param_en) { //使用自定义参数时，不需要微调
        xcfg_cb.bt_rf_pwrdec = 0;
    }

    saradc_var_init();
    key_var_init();
    plugin_var_init();

    msg_queue_init();
    bsp_res_init();

    dev_init(((u8)is_sd_support()) | ((u8)is_usb_support() * 0x02));
#if SD_SOFT_DETECT_EN
    if (SD_IS_SOFT_DETECT()) {
        dev_delay_times(DEV_SDCARD, 3);
    }
#endif

#if MUSIC_SDCARD_EN
    if((xcfg_cb.sddet_iosel == IO_MUX_SDCLK) || (xcfg_cb.sddet_iosel == IO_MUX_SDCMD)) {
        dev_delay_offline_times(DEV_SDCARD, 3); //复用时, 加快拔出检测. 这里拔出检测为3次.
    }
    sd_other_param_set(3);
#endif

    sdadc_var_init();
#if SDADC_5CH_EN
    sdadc_set_5ch_en();
#endif

#if (MUSIC_UDISK_EN || MUSIC_SDCARD_EN)
    fs_var_init();
#endif
    music_stream_var_init();
    msc_pcm_out_var_init();

    dac_cb_init((DAC_CH_SEL & 0x0f) | (0x200 * DAC_FAST_SETUP_EN) | (0x400 * DAC_VCM_CAPLESS_EN) \
                | (0x800 * DAC_MAXOUT_EN));

#if ANC_EN
    anc_var_init();
#endif
}

AT(.text.bsp.sys.init)
static void sys_io_init(void)
{
    //全部设置成模拟GPIO，防止漏电。使用时，自行配置对应数字GPIO
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    uart0_mapping_sel();        //调试UART IO选择或关闭

#if !USB_BC_EN
    usb_bc_init(1);
#endif

#if LINEIN_DETECT_EN
    LINEIN_DETECT_INIT();
#endif // LINEIN_DETECT_EN

#if MUSIC_SDCARD_EN
    SD_DETECT_INIT();
#endif // MUSIC_SDCARD_EN

    LOUDSPEAKER_MUTE_INIT();

#if USER_INEAR_DET_OPT
    INEAR_OPT_PORT_INIT()
#endif
}

void xosc_get_cfg_cap(u8 *osci_cap, u8 *osco_cap, u8 *both_cap)
{
    *osci_cap = xcfg_cb.osci_cap;
    *osco_cap = xcfg_cb.osco_cap;
    *both_cap = xcfg_cb.osc_both_cap;
}

#if  0 //port_int_example
AT(.com_text)
const char strisr0[] = ">>[0x%X]_[0x%X]\n";
const char strisr1[] = "portisr->";
AT(.com_text.timer)
void port_isr(void)
{
    printf(strisr0,WKUPEDG,WKUPCPND);
    if (WKUPEDG & (BIT(6) << 16)) {
        WKUPCPND = (BIT(6) << 16);  //CLEAR PENDING
        printf(strisr1);
    }

}

void port_int_example(void)     //sys_set_tmr_enable(1, 1); 前调用 测试OK
{
    GPIOFDE |= BIT(0);  GPIOFDIR |= BIT(0); GPIOFFEN &= ~BIT(0);
    GPIOFPU |= BIT(0);
    sys_irq_init(IRQ_PORT_VECTOR,0, port_isr);
    PORTINTEN |= BIT(21);
    PORTINTEDG |= BIT(21);  //falling edge;
    WKUPEDG |= BIT(6);     //falling edge
    WKUPCON = BIT(6) | BIT(16);  //falling edge wake iput //wakeup int en

    printf("PORTINTEN = 0x%X, PORTINTEDG = 0x%X  WKUPEDG = 0x%X, WKUPCON = 0x%X\n", PORTINTEN, PORTINTEDG, WKUPEDG, WKUPCON);
    WDT_CLR();
    while(1) {
//       printf("WKUPEDG = 0x%X\n", WKUPEDG);
//       printf("GPIOF = 0x%X\n", GPIOF);
//       delay_ms(500);
    }
}
#endif

AT(.text.bsp.power)
bool power_off_check(void)
{
#if CHARGE_EN
    u16 charge_cnt = 0;
#endif

    u32 pwron_press_nms;
    int pwrkey_pressed_flag, ticks = 0, up_cnt = 0;
    u8 restart_chk_en = 1;

    pwrkey_pressed_flag = 0;
    pwron_press_nms = PWRON_PRESS_TIME;
    if (pwron_press_nms == 0) {
        pwron_press_nms = 15;                                           //bootloader 80ms + 15ms, 最小开机时间在100ms左右
    }

    //要等PWRKEY开关释放后再次按下才能重新开机, 否则充电过程中5分钟关机, 低电关机等异常
    if ((PWRKEY_2_HW_PWRON) && (sys_cb.poweron_flag)) {
        restart_chk_en = 0;
        sys_cb.poweron_flag = 0;
    }

    while (1) {
        WDT_CLR();
        delay_ms(5);
        if ((bsp_key_pwr_scan() & K_PWR_MASK) == K_PWR) {
            up_cnt = 0;
            if (restart_chk_en) {
                if (!pwrkey_pressed_flag) {
                    ticks = tick_get();
                    pwrkey_pressed_flag = 1;
                    sys_cb.ms_ticks = tick_get();                                       //记录PWRKEY按键按下的时刻
                    sys_cb.pwrkey_5s_check = 1;
                }
                if (!sys_cb.poweron_flag) {
                    if (tick_check_expire(ticks, pwron_press_nms)) {                    //长按开机时间配置
                        sys_cb.poweron_flag = 1;
                        sys_cb.pwrdwn_hw_flag = 0;                                      //清PWRKEY硬开关的关机标志
                    }
                }
            }
        } else {
            if (up_cnt < 3) {
                up_cnt++;
            }
            if (up_cnt == 3) {
                up_cnt = 10;
                sys_cb.poweron_flag = 0;
                pwrkey_pressed_flag = 0;
                restart_chk_en = 1;
            }
        }

#if CHARGE_EN
        if (xcfg_cb.charge_en) {
			charge_cnt++;
			if (charge_cnt > 20) {
                charge_cnt = 0;
                charge_detect(0);
            }
        }
#endif // CHARGE_EN

        if (sys_cb.poweron_flag) {
            if ((CHARGE_DC_NOT_PWRON) && CHARGE_DC_IN()) {
                continue;
            }
#if LINEIN_2_PWRDOWN_EN
            if (dev_is_online(DEV_LINEIN)) {
                continue;
            }
#endif // LINEIN_2_PWRDOWN_EN
            //长按PP/POWER开机
            gui_display(DISP_POWERON);
            led_power_up();
            dac_restart();
            bsp_change_volume(sys_cb.vol);
#if WARNING_POWER_ON
            #if WARNING_WSBC_EN
            wsbc_res_play(RES_BUF_POWERON, RES_LEN_POWERON);
            #else
            mp3_res_play(RES_BUF_POWERON, RES_LEN_POWERON);
            DEBUG_SW("RES_BUF_POWERON\r\n");
            #endif
#endif // WARNING_POWER_ON

            if (PWRON_ENTER_BTMODE_EN) {
                func_cb.sta = FUNC_BT;
                if (dev_is_online(DEV_UDISK)) {
                    sys_cb.cur_dev = DEV_UDISK;
                } else {
                    sys_cb.cur_dev = DEV_SDCARD;
                }
            } else {
                if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK) || dev_is_online(DEV_SDCARD1)) {
                    func_cb.sta = FUNC_MUSIC;
                    if (dev_is_online(DEV_UDISK)) {
                        sys_cb.cur_dev = DEV_UDISK;
                    } else if (dev_is_online(DEV_SDCARD1)) {
                        sys_cb.cur_dev = DEV_SDCARD1;
                    } else {
                        sys_cb.cur_dev = DEV_SDCARD;
                    }
                } else {
#if FUNC_BT_EN
                    func_cb.sta = FUNC_BT;
#elif FUNC_CLOCK_EN
                    func_cb.sta = FUNC_CLOCK;
#else
                    func_cb.sta = FUNC_IDLE;
#endif
                }
            }
            return true;
        } else {
            if (CHARGE_DC_IN()) {
                continue;
            } else {
                return false;
            }
        }
    }
}

AT(.text.bsp.power)
static bool power_on_frist_enable(u32 rtccon9)
{
    if (!PWRON_FRIST_BAT_EN) {
        return false;
    }

    if (rtccon9 & BIT(4)) {                                        //charge inbox wakeup
        return false;
    }
    if (rtccon9 & BIT(2)) {                                        //WKO wakeup不能直接开机
        return false;
    }
    if (rtccon9 & BIT(6)) {                                        //TK Wakeup pending
        return false;
    }

    if (CHARGE_DC_IN() && (CHARGE_DC_NOT_PWRON) && (xcfg_cb.charge_en)) {   //VUSB充电禁止开机
        return false;
    }
    return true;
}

static bool power_on_check_do_pre(u32 rtccon9)
{
    bool ret = false;
     //第一次上电是否直接开机
    if (power_on_frist_enable(rtccon9)) {
        ret = true;
    }
    if(sys_cb.sw_rst_flag == SW_RST_FLAG){
        ret = true;
    }
    if (sys_cb.rst_reason & BIT(19)) {                         //is wko 10s reset pending
        if (PWRKEY_IS_PRESS()) {
            sys_cb.poweron_flag = 1;
        }
        ret = true;                                     //长按PWRKEY 10S复位后直接开机。
    }
#if QTEST_EN
    if(qtest_get_mode()){
        ret = true;
    }
#endif

#if IODM_TEST_EN
    if (cm_read8(PARAM_RST_FLAG) == RST_FLAG_MAGIC_VALUE) {
        cm_write8(PARAM_RST_FLAG, 0);
        cm_sync();
        printf("iodm rst power_on\n");
        ret = true;
    }
#endif
#if USER_PWRKEY
    if ((!xcfg_cb.user_pwrkey_en) && (!PWRKEY_2_HW_PWRON) && (!sys_cb.tkey_pwrdwn_en)) {
        ret = true;
    }
#endif
    return ret;
}

AT(.text.bsp.power)
static void power_on_check_do(void)
{
    int pwrkey_pressed_flag = 0;
    int up_cnt = 0, ticks = 0;
    u32 pwron_press_nms;
    u8 chbox_sta = 1;                               //默认offline
#if SWETZ_NTC
    static u16 ntc_cnt;
#endif
    u32 rtccon9 = RTCCON9;                          //wakeup pending
    printf("power_on_check_do: %08x\n", rtccon9);

    RTCCON9 = 0xffff;                               //Clr pending
    RTCCON10 = BIT(10) | BIT(1) | BIT(0);           //Clr pending
    CRSTPND = 0x1ff0000;                            //clear reset pending
    LVDCON &= ~(0x1f << 8);                         //clear software reset
    RTCCON13 &= ~BIT(16);                           //wko pin0 low level wakeup

    if (power_on_check_do_pre(rtccon9)) {
        return;
    }

#if CHARGE_EN
    if (charge_power_on_pre(rtccon9)) {
        return;
    }
#endif

#if USER_PWRKEY
    if (PWRKEY_IS_PRESS() || TKEY_IS_PRESS()) {                         //PWRKEY是否按下
        pwrkey_pressed_flag = 1;
        ticks = sys_cb.ms_ticks;
    }
#endif // USER_PWRKEY
    pwron_press_nms = PWRON_PRESS_TIME;
    if (pwron_press_nms == 0) {
        pwron_press_nms = 15;                                           //最小开机时间在100ms左右
    }

    while (1) {
        WDT_CLR();
#if CHARGE_LOW_POWER_EN
        delay_us(350);
#else
        delay_5ms(1);
#endif
        bsp_key_scan_do();
        if ((bsp_key_pwr_scan() & K_PWR_MASK) == K_PWR) {
            up_cnt = 0;
            if (!pwrkey_pressed_flag) {
                ticks = tick_get();
                sys_cb.ms_ticks = ticks;                                //记录PWRKEY按键按下的时刻
                pwrkey_pressed_flag = 1;
            }
            if (!sys_cb.poweron_flag) {
                if (tick_check_expire(ticks, pwron_press_nms)) {        //长按开机时间配置
                    sys_cb.poweron_flag = 1;
                }
            }
        } else {
            if (up_cnt < 3) {
                up_cnt++;
            }
            if (up_cnt == 3) {
                up_cnt = 10;
                pwrkey_pressed_flag = 0;
                sys_cb.poweron_flag = 0;
            }
        }

#if LINEIN_2_PWRDOWN_EN
        linein_detect();
#endif // LINEIN_2_PWRDOWN_EN

#if CHARGE_EN
        if (xcfg_cb.charge_en) {
            chbox_sta = charge_charge_on_process();
            if (chbox_sta == 2) {
                break;                  //充电仓拿起开机
            }
        }
#endif // CHARGE_EN

        if (sys_cb.poweron_flag) {
#if VBAT_DETECT_EN
            if (sys_cb.vbat <= 2950) {  //电压小于2.95v不开机
                continue;
            }
#endif
            if ((CHARGE_DC_NOT_PWRON) && CHARGE_DC_IN()) {
                continue;
            }
#if LINEIN_2_PWRDOWN_EN
            if (dev_is_online(DEV_LINEIN)) {
                continue;
            }
#endif // LINEIN_2_PWRDOWN_EN
            break;
        } else {
            //PWKKEY已松开, 不在充电仓或未充电直接进行关机
            if ((!pwrkey_pressed_flag) && (chbox_sta)) {
                if ((!SOFT_POWER_ON_OFF) || ((!sys_cb.wko_pwrkey_en) && (!USER_TKEY_SOFT_PWR_EN))) {
                    break;                          //没有按键软开关机功能，不在充电状态直接开机
                }
#if CHARGE_EN
                charge_exit();
#endif
                unlock_code_charge();
                bsp_saradc_exit();
                sfunc_pwrdown(1);
            }
        }
#if SWETZ_NTC
        ntc_cnt++;
        if(ntc_cnt == 980){
            ntc_gpio_power_supply();
        }
        if(ntc_cnt >= 1000){
            ntc_cnt = 0;
            user_ntc_check();
            ntc_gpio_power_down();
        }
#endif


    }
#if CHARGE_EN
     charge_exit();
#endif
}

AT(.text.bsp.power)
void power_on_check(void)
{
    lock_code_charge();
    power_on_check_do();
#if CHARGE_BOX_EN
    charge_box_reinit();
#endif
    unlock_code_charge();
}



AT(.text.bsp.sys.init)
void sys_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
        WDT_RST();
        while(1);
    }

    // io init
    sys_io_init();

    // var init
    sys_var_init();

#if SWETZ_NTC
    ntc_gpio_power_supply();
#endif

#if RES_USERBIN_EN
    extern void register_spi_read_function(void (* read_func)(void *buf, u32 addr, u32 len));
    // mp3格式提示音资源放在外部flash时需要注册spi_read
    register_spi_read_function((void*)os_spiflash_read);
#endif

    // power init
    pmu_init(getcfg_buck_mode_en());

    //audio pll init
    adpll_init(DAC_OUT_SPR);

    // clock init
    sys_clk_set(SYS_CLK_SEL);

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);

#if APP_MESSAGE_RUN
    message_init();
#endif
#if TWS_LR
    app_lr_init();
#endif

#if OFFLINE_LOG_EN
    offline_log_init(OFFLINE_LOG_START, OFFLINE_LOG_SIZE, OFFLINE_LOG_SIZE/2);
    sys_log_info("hello, reset reson %x\n", sys_cb.rst_reason);
    if(sys_cb.rst_reason & BIT(25)) {
        sys_log_info("UART DET reset\n");
    } else if(sys_cb.rst_reason & BIT(19)) {
        if (RTCCON10 & BIT(15)) {
            sys_log_info("VUSB4S timeout reset\n");
        } else {
            sys_log_info("WKO10S reset\n");
        }
    } else if(sys_cb.rst_reason & BIT(18)) {
        sys_log_info("WKUP reset\n");
    } else if(sys_cb.rst_reason & BIT(17)) {
        sys_log_info("VUSB reset\n");
    } else if(sys_cb.rst_reason & BIT(16)) {
        sys_log_info("WDT reset\n");
    } else if(sys_cb.rst_reason & 0xf00) {
        sys_log_info("SW reset%x\n",(sys_cb.rst_reason >> 8) & 0x0f);
    }
#endif

    //晶振配置
    xosc_init();

    plugin_init();

    if (POWKEY_10S_RESET) {
        WKO_10SRST_EN(0);                                   //10s reset source select  0: wko pin press, 1: touch key press
    } else {
        WKO_10SRST_DIS();
    }

    led_init();

#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_init();
    }
#endif
    key_init();

#if QTEST_EN
    if(QTEST_IS_ENABLE()) {
        qtest_init();
    }
#endif

#if HUART_EN
    if(xcfg_cb.huart_en){
        bsp_huart_init();
    }
#endif // HUART_EN

#if UPD_UART_EN
    upd_uart_init();
#endif

#if UART1_EN
    if (xcfg_cb.uart1_en) {
        bsp_uart1_init(9600);
    }
#elif UART2_EN
    if (xcfg_cb.uart2_en) {
        bsp_uart2_init(9600);
    }
#endif

    bt_init();

    power_on_check();               //在key_init之后
    gui_init();
#if PWM_RGB_EN
    pwm_init();
#endif // PWM_RGB_EN

    gsensor_init();

    en_auto_pwroff();

    /// enable user timer for display & dac
    sys_set_tmr_enable(1, 1);

    led_power_up();
    gui_display(DISP_POWERON);

#if ANC_EQ_RES2_EN
    copy_res2flash();
#endif

    if (bsp_dac_off_for_bt_conn()) {
        dac_init();
        func_bt_init();
    } else {
        func_bt_init();
        dac_init();
    }

    codecs_pcm_init();

#if TINY_TRANSPARENCY_EN
    bsp_ttp_init();
#endif

    app_init_do();

    bsp_change_volume(sys_cb.vol);

#if WARNING_POWER_ON
    if ((xcfg_cb.bt_outbox_voice_pwron_en) || (!sys_cb.outbox_pwron_flag)) {
        #if WARNING_WSBC_EN
        wsbc_res_play(RES_BUF_POWERON, RES_LEN_POWERON);
        #else
        mp3_res_play(RES_BUF_POWERON, RES_LEN_POWERON);
        DEBUG_SW("RES_BUF_POWERON\r\n");
        #endif
    }
    sys_cb.outbox_pwron_flag = 0;
#endif // WARNING_POWER_ON



    if (PWRON_ENTER_BTMODE_EN) {
        func_cb.sta = FUNC_BT;
        if (dev_is_online(DEV_UDISK)) {
            sys_cb.cur_dev = DEV_UDISK;
        } else {
            sys_cb.cur_dev = DEV_SDCARD;
        }
    } else {
#if SD_SOFT_DETECT_EN
        sd_soft_detect_poweron_check();
#endif

#if FUNC_MUSIC_EN
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK) || dev_is_online(DEV_SDCARD1)) {
            func_cb.sta = FUNC_MUSIC;
            if (dev_is_online(DEV_UDISK)) {
                sys_cb.cur_dev = DEV_UDISK;
            } else if (dev_is_online(DEV_SDCARD1)) {
                sys_cb.cur_dev = DEV_SDCARD1;
            } else {
                sys_cb.cur_dev = DEV_SDCARD;
            }
        } else
#endif // FUNC_MUSIC_EN
        {
#if FUNC_AUX_EN
            if (dev_is_online(DEV_LINEIN)) {
                func_cb.sta = FUNC_AUX;
            } else
#endif // FUNC_AUX_EN
            {
                func_cb.sta = FUNC_BT;
            }
        }
    }

#if SYS_MODE_BREAKPOINT_EN
    u8 sta = param_sys_mode_read();
    if (sta != 0 && sta != 0xff) {
        func_cb.sta = sta & 0xf;
        if (func_cb.sta == FUNC_MUSIC) {
            sys_cb.cur_dev = sta >> 4;
        }
    }
#endif // SYS_MODE_BREAKPOINT_EN

#if LINEIN_2_PWRDOWN_EN
    if (dev_is_online(DEV_LINEIN)) {
        sys_cb.pwrdwn_tone_en = LINEIN_2_PWRDOWN_TONE_EN;
        func_cb.sta = FUNC_PWROFF;
    }
#endif // LINEIN_2_PWRDOWN_EN

#if BT_BACKSTAGE_EN
    bsp_bt_init();
#endif

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
    eq_dbg_init();
#endif // EQ_DBG_IN_UART

#if PLUGIN_SYS_INIT_FINISH_CALLBACK
    plugin_sys_init_finish_callback(); //初始化完成, 各方案可能还有些不同参数需要初始化,预留接口到各方案
#endif

#if SYS_KARAOK_EN
    bsp_karaok_init();
#endif

#if BT_MUSIC_EFFECT_EN
    music_effect_init();
#endif // BT_MUSIC_EFFECT_EN




}


AT(.text.bsp.sys.init)
void sys_update_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb))) {           //获取配置参数
        printf("xcfg init error\n");
    }

    // io init
    sys_io_init();

    // var init
    sys_var_init();
    sys_cb.lang_id = 0;

    // power init
    pmu_init(getcfg_buck_mode_en());

    // peripheral init
    rtc_init();
    param_init(sys_cb.rtc_first_pwron);

    //晶振配置
    xosc_init();

    plugin_init();
    sys_set_tmr_enable(1, 1);

    adpll_init(DAC_OUT_SPR);
    dac_init();
    mp3_res_play(RES_BUF_UPDATE_DONE_MP3, RES_LEN_UPDATE_DONE_MP3);
}
