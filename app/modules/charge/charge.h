#ifndef _CHARGE_H
#define _CHARGE_H

#define CHARGE_DC_IN_FILTER                     1

void lock_code_charge(void);
void unlock_code_charge(void);

void charge_off(void);
void charge_init(void);
void charge_detect(u8 mode);
void charge_set_leakage(u8 sta, u8 level);
void charge_status(u8 sta);
void charge_set_stop_time(u16 stop_time);
void charge_bled_ctrl(void);
void charge_exit(void);
void charge_enter(u8 out_auto_pwron);
u8 charge_dc_detect(void);
u8 charge_charge_on_process(void);
void charge_process(void);
bool charge_power_on_pre(u32 rtccon9);

#endif
