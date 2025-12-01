#include "include.h"

volatile int pwrkey_detect_flag;            //pwrkey 820K用于复用检测的标志。


void plugin_init(void)
{
    CLKGAT0 = BIT(0)|BIT(12);
    CLKGAT1 = BIT(1)|BIT(2)|BIT(3)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30);
    CLKGAT2 = BIT(2)|BIT(10)|BIT(11)|BIT(12)|BIT(15)|BIT(17)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(29);
    CLKGAT3 = 0;

#if UART0_PRINTF_SEL != PRINTF_NONE
    CLKGAT0 |= BIT(6);
#endif

#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif

#if USB_SUPPORT_EN
    CLKGAT0 |= BIT(15);
#endif
#if SD_SUPPORT_EN
    CLKGAT0 |= BIT(16);
#endif

}

void plugin_var_init(void)
{
    pwrkey_detect_flag = 0;
    xcfg_cb.pwrkey_config_en = xcfg_cb.pwrkey_config_en & xcfg_cb.user_pwrkey_en;
    xcfg_cb.adkey_config_en  = xcfg_cb.adkey_config_en & xcfg_cb.user_adkey_en;
    xcfg_cb.iokey_config_en  = xcfg_cb.iokey_config_en & xcfg_cb.user_iokey_en;
    xcfg_cb.key_multi_config_en = xcfg_cb.key_multi_config_en & xcfg_cb.user_key_multi_press_en;

#if FUNC_AUX_EN
    if (!xcfg_cb.func_aux_en) {
        xcfg_cb.linein_2_pwrdown_en = 0;
        xcfg_cb.linein_det_iosel = 0;
        xcfg_cb.auxl_sel = 0;
        xcfg_cb.auxr_sel = 0;
        xcfg_cb.mode_2_aux_en = 0;
    }
#endif // FUNC_AUX_EN

    if (!xcfg_cb.led_disp_en) {
        xcfg_cb.bled_io_sel = 0;
        xcfg_cb.charge_full_bled = 0;
        xcfg_cb.led_sta_config_en = 0;
    }

    if (!xcfg_cb.func_music_en) {
        xcfg_cb.music_udisk_en = 0;
        xcfg_cb.music_sdcard_en = 0;
    }

    if (!xcfg_cb.music_sdcard_en) {
        xcfg_cb.sddet_iosel = 0;
    }

    if (!LINEIN_DETECT_EN) {
        xcfg_cb.linein_det_iosel = 0;
    }

    if (!xcfg_cb.led_pwr_en) {
        xcfg_cb.rled_io_sel = 0;
    }

    if (xcfg_cb.buck_mode_en) {
        xcfg_cb.vddbt_capless_en = 0;
    }

    if (!xcfg_cb.charge_en) {
        xcfg_cb.chbox_en = 0;
    }

    if (!xcfg_cb.chbox_en) {
        xcfg_cb.ch_box_type_sel            = 3;
        xcfg_cb.ch_out_auto_pwron_en       = 0;
        xcfg_cb.ch_leakage_sel             = 0;
        xcfg_cb.chg_inbox_pwrdwn_en        = 0;
        xcfg_cb.chbox_out_delay            = 0;
    }

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    if (xcfg_cb.chbox_en) {
        xcfg_cb.ch_box_type_sel            = 2;     //昇生微智能充电仓为维持电压仓
        xcfg_cb.chg_inbox_pwrdwn_en        = 0;
    }
#endif

    if (!xcfg_cb.bt_tws_en) {
        xcfg_cb.bt_tws_pair_mode = 0;
        xcfg_cb.bt_tws_pair_bonding_en = 0;
        xcfg_cb.tws_sel_left_gpio_sel = 0;
        xcfg_cb.bt_tswi_en = 0;
    }

    if (!xcfg_cb.bt_tswi_en) {
        xcfg_cb.bt_tswi_kpwr_en = 0;
        xcfg_cb.bt_tswi_msc_en = 0;
        xcfg_cb.bt_tswi_sco_en = 0;
    }

#if USER_PWRKEY
    if (xcfg_cb.user_pwrkey_en || PWRKEY_2_HW_PWRON) {
        sys_cb.wko_pwrkey_en = 1;
    }
#endif // USER_PWRKEY

#if USER_TKEY_SOFT_PWR_EN
    sys_cb.tkey_pwrdwn_en = 1;
#endif

    //PWRKEY模拟硬开关,需要关闭长按10S复位， 第一次上电开机， 长按5秒进配对等功能。
    if (PWRKEY_2_HW_PWRON) {
        xcfg_cb.powkey_10s_reset = 0;
//        xcfg_cb.pwron_frist_bat_en = 0;
        xcfg_cb.bt_pwrkey_nsec_discover = 0;
    }

#if LED_DISP_EN
	led_func_init();
#endif // LED_DISP_EN
}

AT(.com_text.plugin)
void plugin_tmr5ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_level_calc();
#endif
}

AT(.com_text.plugin)
void plugin_tmr1ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_scan();
#endif
}

#if (MUSIC_SDCARD_EN)
u8 get_sddet_io_num(u8 iosel)
{
    u8 io_num;
    if (iosel <= IO_MAX_NUM) {
        io_num = iosel;
    } else if (iosel == IO_MUX_SDCLK) {
        io_num = SDCLK_IO;
    } else if (iosel == IO_MUX_SDCMD) {
        io_num = SDCMD_IO;
    } else {
        io_num = IO_NONE;
    }
    return io_num;
}
#endif // MUSIC_SDCARD_EN

//蓝牙休眠时，唤醒IO配置
void sleep_wakeup_config(void)
{
#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        wakeup_gpio_config(get_adc_gpio_num(ADKEY_CH), 0, 0);           //配置ADKEY IO下降沿唤醒。
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        wakeup_gpio_config(get_adc_gpio_num(ADKEY2_CH), 0, 0);          //配置ADKEY1 IO下降沿唤醒。
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        wakeup_gpio_config(get_adc_gpio_num(SDCLK_AD_CH), 0 ,0);        //IO下降沿唤醒。
    }
#endif // USER_ADKEY_MUX_SDCLK

#if USER_IOKEY
    if (xcfg_cb.user_iokey_en) {
        if (xcfg_cb.iokey_config_en) {
            wakeup_gpio_config(xcfg_cb.iokey_io0, 0, 1);                //配置IO下降沿唤醒。
            wakeup_gpio_config(xcfg_cb.iokey_io1, 0, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io2, 0, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io3, 0, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io4, 0, 1);
        } else {
            //不用工具配置IOKEY时，根据实际使用的IOKEY IO进行修改。
            wakeup_gpio_config(IO_PB1, 0, 1);
            wakeup_gpio_config(IO_PB2, 0, 1);
        }
    }
#endif // USER_IOKEY

#if SC7A20_EN
	wakeup_wko_config(); /*配置PB5作为GPIO唤醒功能*/
	//printf("%s config pb5 wakeup\n", __func__);
#endif

#if USER_PWRKEY
    if ((xcfg_cb.user_pwrkey_en) && (!PWRKEY_2_HW_PWRON) && !bsp_tkey_wakeup_en()) {
        wakeup_wko_config();
    }
#endif // USER_PWRKEY

#if MUSIC_SDCARD_EN
    //sdcard insert/remove wakeup
    if (xcfg_cb.music_sdcard_en) {
        u8 edge;
        if (dev_is_online(DEV_SDCARD)) {
            edge = 1;                           //sdcard remove wakeup(rising edge)
        } else {
            edge = 0;                           //sdcard insert wakeup(falling edge)
        }
        wakeup_gpio_config(get_sddet_io_num(xcfg_cb.sddet_iosel), edge, 1);
    }
#endif // MUSIC_SDCARD_EN

#if MUSIC_UDISK_EN
    //udisk insert/remove wakeup
    if (xcfg_cb.music_udisk_en) {
        if (dev_is_online(DEV_UDISK)) {
            wakeup_gpio_config(IO_PB3, 0, 2);   //udisk remove wakeup(falling edge)
        } else {
            wakeup_gpio_config(IO_PB3, 1, 2);   //udisk insert wakeup(rising edge)
        }
    }
#endif // MUSIC_UDISK_EN
}

AT(.com_text.bsp.sys)
void lefmic_zero_detect(u8 *ptr, u32 samples)
{
}

bool bt_hfp_ring_number_en(void)
{
    return (!BT_HFP_INBAND_RING_EN && xcfg_cb.bt_hfp_ring_number_en);
}

AT(.com_text.bsp.sys)
bool is_sd_support(void)
{
#if (SD0_MAPPING == SD0MAP_G2) && EQ_DBG_IN_UART
    if (xcfg_cb.huart_en && xcfg_cb.huart_sel == 1) {
        return false;
    }
#endif
    return (MUSIC_SDCARD_EN * xcfg_cb.music_sdcard_en);
}

AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
#if EQ_DBG_IN_UART
    if (xcfg_cb.huart_en && xcfg_cb.huart_sel == 2) {
        return false;
    }
#endif
    return ((MUSIC_UDISK_EN * xcfg_cb.music_udisk_en) | FUNC_USBDEV_EN);
}

bool is_sleep_dac_off_enable(void)
{
    if (xcfg_cb.linein_det_iosel == IO_MUX_MICL) {
        return false;
    }

    return true;
}

//设置piano提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_piano_digvol(void)
{
    return bsp_volume_convert(WARNING_VOLUME);
}

//设置WAV RES提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_wav_res_digvol(void)
{
#if APP_FIND_TONE_MAX
    if (sys_cb.flag_playing_find_ear)
    {
        u8 vol_level;
        sys_cb.find_me_count++;

        if (sys_cb.find_me_count <= 4)
        {
            vol_level = 6;
        }
        else if (sys_cb.find_me_count <= 8)
        {
            vol_level = 13;
        }
        else 
        {
            vol_level = VOL_MAX;
        }
        if (sys_cb.find_me_count >= 9)
        {
            sys_cb.find_me_count = 9;
        }
      
       // printf("warning vol_level:%d  %d\r\n",vol_level,user_bsp_volume_convert(vol_level));
        return user_bsp_volume_convert(vol_level);

    }
    
    // else 
    // {
    //     return bsp_volume_convert(ab_mate_get_vp_vol());
    // }

#else

 return bsp_volume_convert(WARNING_VOLUME);

#endif
}

#if WARNING_WSBC_EN
//设置提示音播放的数字音量 (0 ~ 0x7fff)
u32 waring_get_digvol_cb(void)
{
    return bsp_volume_convert(WARNING_VOLUME);
}
#endif

void maxvol_tone_play(void)
{
	bsp_res_play(TWS_RES_MAX_VOL);
}

void minvol_tone_play(void)
{

}

void plugin_playmode_warning(void)
{

}

void plugin_lowbat_vol_reduce(void)
{
#if LPWR_REDUCE_VOL_EN
    music_src_set_volume(0x50c0);       //设置音乐源音量达到整体降低系统音量 (范围：0~0x7fff)
#endif // LPWR_REDUCE_VOL_EN
}

void plugin_lowbat_vol_recover(void)
{
#if LPWR_REDUCE_VOL_EN
    music_src_set_volume(0x7fff);       //还原音量
#endif // LPWR_REDUCE_VOL_EN
}

//用于调DAC音质接口函数
void plugin_music_eq(void)
{
    bsp_eq_init();
    sys_cb.eq_mode = 0;
    music_set_eq_by_num(sys_cb.eq_mode);

#if EQ_DAC_DIV_EN
    #ifdef RES_BUF_EQ_NORMAL_L_EQ
    delay_5ms(6);
    music_set_eq_by_res_ch(EQ_CHL, RES_BUF_EQ_NORMAL_L_EQ, RES_LEN_EQ_NORMAL_L_EQ);
    #endif
    #ifdef RES_BUF_EQ_NORMAL_R_EQ
    delay_5ms(6);
    music_set_eq_by_res_ch(EQ_CHL, RES_BUF_EQ_NORMAL_R_EQ, RES_LEN_EQ_NORMAL_R_EQ);
    #endif
#endif
}

AT(.com_text.port.vbat)
void plugin_vbat_filter(u32 *vbat)
{
#if  VBAT_FILTER_USE_PEAK
    //电源波动比较大的音箱方案, 取一定时间内的电量"最大值"或"次大值",更能真实反应电量.
    #define VBAT_MAX_TIME  (3000/5)   //电量峰值判断时间 3S
    static u16 cnt = 0;
	static u16 vbat_max_cnt = 0;
    static u32 vbat_max[2] = {0,0};
    static u32 vbat_ret = 0;
    u32 vbat_cur = *vbat;
    if (cnt++  < VBAT_MAX_TIME) {
        if (vbat_max[0] < vbat_cur) {
            vbat_max[1] = vbat_max[0];  //vbat_max[1] is less max (次大值)
            vbat_max[0] = vbat_cur;     //vbat_max[0] is max(最大值)
            vbat_max_cnt = 0;
        } else if (vbat_max[0] == vbat_cur) {
            vbat_max_cnt ++;
        }
    } else {
        if (vbat_max_cnt >= VBAT_MAX_TIME/5) {  //总次数的(1/5)都采到最大值,则返回最大值.
            vbat_ret = vbat_max[0];
        } else if (vbat_max[1] != 0) {   //最大值次数较少,则返回次大值(舍弃最大值)
            vbat_ret = vbat_max[1];
        }
        vbat_max[0] = 0;
        vbat_max[1] = 0;
        vbat_max_cnt = 0;
        cnt = 0;
    }
    //返回值
    if (vbat_ret != 0) {
        *vbat = vbat_ret;
    }
#endif
}

//初始化完成, 各方案可能还有些不同参数需要初始化,预留接口到各方案
void plugin_sys_init_finish_callback(void)
{
#if ENERGY_LED_EN
    energy_led_init();
#endif
}

bool plugin_func_idle_enter_check(void)
{
    //可以在这里决定否需要进入idle
    return true;
}



