#include "include.h"

extern uint8_t pwr_usage_id;


AT(.com_text.bsp.key)
void pwrkey10s_counter_clr(void)
{
    if (POWKEY_10S_RESET) {
        RTCCON10 = BIT(10);                 //clear pwrkey10s pending and counter
    }
}

void pwrkey_off(void)
{
    GPIOBDE &= ~BIT(5);
    GPIOBDIR |= BIT(5);
    GPIOBPU &= ~BIT(5);
    GPIOBPD &= ~BIT(5);
    RTCCON13 &= ~(BIT(0) | BIT(4) | BIT(8) | BIT(12));   //wk pin0 disable
}

void pwrkey_init(void)
{
#if USER_PWRKEY
    if (sys_cb.wko_pwrkey_en) {
        adcch_io_pu10k_enable(ADCCH_WKO);
        pwr_usage_id = pwrkey_table[0].usage_id;
        if (xcfg_cb.pwrkey_config_en) {
            pwr_usage_id = key_config_table[xcfg_cb.pwrkey_num0];
        }
        RTCCON13 |= BIT(0) | BIT(4) | BIT(12);  //wk pin0 wakeup, input, pullup10k enable

        saradc_set_channel(BIT(ADCCH_WKO));
    } else
#endif // USER_PWRKEY
    {
        pwrkey_off();
    }
}

#if USER_PWRKEY
AT(.com_text.port.key)
uint8_t pwrkey_get_val(void)
{
    uint8_t num = 0;
    uint8_t *ptr;

//    //配置工具是否使能了PWRKEY？
    if ((!xcfg_cb.user_pwrkey_en) && (!PWRKEY_2_HW_PWRON)) {
        return NO_KEY;
    }

    while (saradc_get_value8(ADCCH_WKO) > pwrkey_table[num].adc_val) {
        num++;
    }

    //工具配置了PWRKEY的按键定义？
    ptr = get_pwrkey_configure(num);
    if (ptr != NULL) {
        if (num > 4) {
            return NO_KEY;
        }
        return key_config_table[*(ptr+num)];
    }
    return pwrkey_table[num].usage_id;
}

AT(.text.bsp.power)
bool pwrkey_get_status(void)
{
    u32 delay = 20;
    u32 pre_sta = 0;
    u32 sta_cnt = 0;
    while(delay--) {
        u32 tmp_sta = (RTCCON & BIT(19));
        if(pre_sta == tmp_sta) {
            sta_cnt++;
        } else {
            pre_sta = tmp_sta;
            sta_cnt = 0;
        }
        delay_ms(1);
        if(sta_cnt > 4) {
            break;
        }
    }

    return (pre_sta == 0)? true : false;
}
#endif
