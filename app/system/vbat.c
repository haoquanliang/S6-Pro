#include "include.h"

#if VBAT_DETECT_EN

#define VBAT_CACL_VOLTAGE()     (u32)((VBAT_VALUE() * VBAT2_COEF / adc_cb.vbg) * VBG_VOLTAGE / 10000)

void sys_dump_vbat(void)
{
    printf("VBAT_VALUE():%d  VBAT2_COEF:%d adc_cb.vbg:%d VBG_VOLTAGE:%d VBAT_CACL_VOLTAGE:%d vbat:%d\n", VBAT_VALUE(), VBAT2_COEF,
                                                                adc_cb.vbg, VBG_VOLTAGE, VBAT_CACL_VOLTAGE(), sys_cb.vbat);
}

//AT(.com_rodata.bat)
//const char bat_str[] = "VBAT: %d.%03dV\n";
AT(.com_text.port.vbat)
uint16_t vbat_get_voltage(void)
{
    static u16 vbat_bak = 0;

    u32 vbat = VBAT_CACL_VOLTAGE();

    //不同方案可能采用不同 vbat 滤波算法, 在方案对应的plugin.c中处理
    plugin_vbat_filter(&vbat);
    //默认的取平均值算法.
    adc_cb.vbat_total = adc_cb.vbat_total - adc_cb.vbat_val + vbat; //均值
    adc_cb.vbat_val = adc_cb.vbat_total>>5;

    if(adc_cb.vbat_val > vbat_bak) {
        vbat = adc_cb.vbat_val - vbat_bak;
    } else {
        vbat = vbat_bak - adc_cb.vbat_val;
    }
    if (vbat >= 30) {   //偏差大于一定值则更新
        vbat_bak = adc_cb.vbat_val;
//        printf(bat_str, adc_cb.vbat_val/1000, adc_cb.vbat_val%1000);
    }
    return vbat_bak;
}

void vbat_init(void)
{
    saradc_set_channel(BIT(ADCCH_BGOP) | BIT(ADCCH_VBAT));
}

void vbat_voltage_init(void)
{
    adc_cb.vbat_val =  VBAT_CACL_VOLTAGE();
    adc_cb.vbat_total = adc_cb.vbat_val << 5;
    sys_cb.vbat = vbat_get_voltage();
}

int is_lowpower_vbat_warning(void)
{
    if (sys_cb.vbat <= ((u16)LPWR_OFF_VBAT*100+2700)) {
        if (LPWR_OFF_VBAT) {
            if (!sys_cb.lpwr_cnt) {
                sys_cb.lpwr_cnt = 1;
            } else if (sys_cb.lpwr_cnt >= 10) {
                return 1;       //VBAT低电关机
            }
        }
        return 0;               //VBAT低电不关机
    }
#if WARNING_LOW_BATTERY
    else {
        sys_cb.lpwr_cnt = 0;
#if SWETZ_SET_VBAT_LOW_WARNING
     if(sys_cb.local_bat_level <= VBAT_LOW_WARNING_VAL){
#else 
     if (sys_cb.vbat < ((u16)LPWR_WARNING_VBAT*100 + 2800)) {
#endif
      
    #if LED_LOWBAT_EN
            if (xcfg_cb.rled_lowbat_en) {
                if ((!CHARGE_DC_IN()) && (!RLED_LOWBAT_FOLLOW_EN)) {
                    led_lowbat();
                } else {
                    led_lowbat_recover();
                }
            }
    #endif // LED_LOWBAT_EN
            if (xcfg_cb.lowpower_warning_en) {
                return 2;       //低电压提示音播报
            }
        } else {
    #if LED_LOWBAT_EN
            if (xcfg_cb.rled_lowbat_en) {
                led_lowbat_recover();
            }
    #endif // LED_LOWBAT_EN
        }
        return 0;
    }
#endif // WARNING_LOW_BATTERY
    sys_cb.lpwr_cnt = 0;
    return 0;
}
#endif  //VBAT_DETECT_EN
