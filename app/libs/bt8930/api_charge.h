#ifndef _API_CHARGE_H
#define _API_CHARGE_H

typedef struct {
    u8 chag_sta         : 3,    //充电状态
       dcin_reset       : 1,    //DC插入复位
       leakage          : 3,    //漏电电流档
       inbox_voltage    : 1;    //充电仓维持电压

    u8 const_curr;              //恒流充电电流
    u8 trick_curr;              //涓流充电电流
    u8 stop_curr;               //充电截止电流
    u8 stop_volt;               //充电截止电压

    u8 trick_curr_en    : 1;    //是否使能涓流充电
    u8 trick_stop_volt  : 1;    //涓流充电截止电压
    u8 ldo_en           : 1;    //充电是否使用LDO mode
    u8 volt_follow_en   : 1;    //充电是否使用电压跟随模式
    u8 volt_follow_diff;        //恒压差充电差值
    u8 bled_on_pr;
    volatile u16 stop_time;
    void (*charge_sta_func)(u8 sta);
} charge_cfg_t;

void charge_stop(u8 mode);
void charge_init_do(charge_cfg_t *p);




#endif // _API_CHARGE_H
