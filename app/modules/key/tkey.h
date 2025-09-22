#ifndef _BSP_TKEY_H
#define _BSP_TKEY_H

#define TKC_MAX_BITS               2
#define TKC_MAX_SIZE              (1 << TKC_MAX_BITS)

extern tk_cb_t tk_cb;
extern tk_cb_t te_cb;

void bsp_tkey_init(void);
bool bsp_tkey_wakeup_en(void);
u8 tkey_get_key(void);
u8 tkey_wakeup_status(void);
void bsp_tkey_spp_tx(void);
void bsp_tkey_str_spp_tx(char *str);
bool tkey_is_pressed(void);
void tkey_press_timeout_clr(void);
void tkey_press_timeout_process(void);

#if USER_TKEY
void bsp_charge_bcnt_calibration(u32 min_avg_cnt);
void tkey_stop_calibration_in_charge(void);
void tkey_proc_calibration_in_charge(void);

void bsp_tebcnt_temp_calibration_stop(void);
void bsp_tebcnt_temp_calibration_start(void);
void bsp_tebcnt_temp_calibration(void);
void bsp_tebcnt_temp_calibration_stop(void);
void bsp_tkey_bcnt_calibration_timeout(void);
#else
#define bsp_charge_bcnt_calibration(x)
#define tkey_proc_calibration_in_charge()
#define tkey_stop_calibration_in_charge()

#define bsp_tebcnt_temp_calibration_stop()
#define bsp_tebcnt_temp_calibration_start()
#define bsp_tebcnt_temp_calibration()
#define bsp_tebcnt_temp_calibration_stop()
#define bsp_tkey_bcnt_calibration_timeout()
#endif // USER_TKEY

#if USER_TKEY_INEAR
u8 tkey_is_inear(void);     //入耳检测，0：出耳, 1：入耳
#else
#define tkey_is_inear()     (0)
#endif // USER_TKEY_INEAR

#endif
