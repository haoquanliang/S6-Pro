#ifndef _BSP_SARADC_H
#define _BSP_SARADC_H

typedef struct {
    u16 vrtc;
    u16 vbg;
    u16 vbat2;
    u16 vbat_val;
#if TSEN_DETECT_EN
    u16 tsen;
#endif
    u32 vbat_total;
} adc_cb_t;
extern adc_cb_t adc_cb;

void bsp_saradc_init(void);
bool bsp_saradc_process(void);
uint16_t bsp_saradc_exit(void);

#define bsp_saradc_set_channel(n)       saradc_set_channel(n)

#endif // _BSP_SARADC_H
