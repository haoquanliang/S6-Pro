/*
 *  saradc.h
 *
 *  Created by zoro on 2021-3-12.
 */
#ifndef __API_SARADC_H
#define __API_SARADC_H

#define ADCCH15_ANA_VRTC    0x01
#define ADCCH15_ANA_BG      0x02
#define ADCCH15_ANA_TS      0x04
#define ADCCH15_ANA_VUSB    0x08

#define SARADC_MAX_CH       16

typedef struct {
    psfr_t sfr;             //SADCDAT SFR
    u16 channel;
    u16 value[SARADC_MAX_CH];
    u8 adc15_ana_en;
    u8 adc15_ana_cur;
} saradc_cb_t;

extern saradc_cb_t saradc_cb;

#define saradc_get_value10(n)   saradc_cb.value[n]                  //获取某个channel的ADC值
#define saradc_get_value8(n)    ((u8)(saradc_cb.value[n]>>2))       //获取某个channel的ADC值

u16 vbg_voltage_get(void);
u16 vbat2_voltage_get(void);
#define VBG_VOLTAGE             vbg_voltage_get()
#define VBAT2_COEF              vbat2_voltage_get()
#define VBAT_VALUE()            saradc_get_value10(ADCCH_VBAT)

#define saradc_adc15_is_vrtc()  (bool)(saradc_cb.adc15_ana_cur == ADCCH15_ANA_VRTC)
#define saradc_adc15_is_ts()    (bool)(saradc_cb.adc15_ana_cur == ADCCH15_ANA_TS)
#define saradc_adc15_is_bg()    (bool)(saradc_cb.adc15_ana_cur == ADCCH15_ANA_BG)
#define saradc_adc15_is_vusb()  (bool)(saradc_cb.adc15_ana_cur == ADCCH15_ANA_VUSB)

void saradc_var_init(void);
void saradc_init(void);                         //初始化ADC
uint16_t saradc_exit(void);                     //关闭ADC
void saradc_baud_set(u16 div);                  //设置ADC时钟分频
uint16_t saradc_set_channel(uint16_t channel);  //使能ADC channel，返回值：设置前的channel
void saradc_clr_channel(uint16_t channel);      //关闭ADC channel
uint16_t saradc_get_channel(void);              //获取ADC channel
void saradc_start(uint8_t auto_anl_en);         //启动ADC转换，全部使能的channel都会转换
bool saradc_is_finish(void);                    //ADC转换是否完成
void saradc_get_result(void);                   //获取使能的channel的ADC值到saradc_cb.value
void saradc_adc15_analog_select(u8 ana_ch);     //ADC15的模拟信号选择
void saradc_adc15_analog_next(void);            //ADC15自动选择下一个模拟信号
u8 saradc_adc15_ana_set_channel(u8 ana_ch);     //ADC15设置模拟通路
void saradc_adc15_ana_clr_channel(u8 ana_ch);   //ADC15清除模拟通路

#endif
