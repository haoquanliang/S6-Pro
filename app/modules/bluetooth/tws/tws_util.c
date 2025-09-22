#include "include.h"


#if BT_TWS_EN
//TWS蓝牙名称是否增加_L或_R后缀
void tws_name_suffix_set(void)
{
    sys_cb.name_suffix_en = 0;
    if (xcfg_cb.bt_tws_name_suffix_en) {
        if ( (xcfg_cb.bt_tws_lr_mode < 9) ||
             (xcfg_cb.bt_tws_lr_mode == 9 && xcfg_cb.tws_sel_left_gpio_sel == 0) ) {
            return;
        }

        if (strlen(xcfg_cb.bt_name) < 30) {
            if (sys_cb.tws_left_channel) {
                strcat(xcfg_cb.bt_name, "L");
            } else {
                strcat(xcfg_cb.bt_name, "R");
            }
            sys_cb.name_suffix_en = 1;
            printf("bt_name_suffix: %s\n", xcfg_cb.bt_name);
        }
    }
}

AT(.text.tws.init)
bool tws_gpiox_2_gnd_check(void)  //TWS 特定GPIO 接地为左声道
{
    u8 i,cnt;
    bool res = false;
    gpio_t tws_sel_left_gpio;
    gpio_t *g = &tws_sel_left_gpio;
    u8 io_num = xcfg_cb.tws_sel_left_gpio_sel;
    //printf("io_num = %d\n",io_num);
    if ((!io_num) || (io_num > IO_MAX_NUM)) {
        return false;
    }
    bsp_gpio_cfg_init(&tws_sel_left_gpio, xcfg_cb.tws_sel_left_gpio_sel);
    //io sta backup
    u32 bk_de = g->sfr[GPIOxDE];
    u32 bk_pu = g->sfr[GPIOxPU];
    u32 bk_dir = g->sfr[GPIOxDIR];
    //数字IO, 开上拉.
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxPU] |= BIT(g->num);
    g->sfr[GPIOxDIR] |= BIT(g->num);

    i = 6;
    cnt = 0;
    while(i--) {
        delay_ms(5);
        if (!(g->sfr[GPIOx] & BIT(g->num)) ) {   //to GND
            //printf("GND\n");
            cnt++;
        } else {
            //printf("!!!PU\n");
        }
        if (cnt >= 3) {
            res = true;
            break;
        }
    }
    //检测完成, 恢复以前IO状态
    g->sfr[GPIOxDE] =  bk_de;
    g->sfr[GPIOxPU] =  bk_pu;
    g->sfr[GPIOxDIR] = bk_dir;
    //printf("res = %d,i = %d, cnt = %d\n",res,i,cnt);
    return res;
}

AT(.text.tws.init)
void tws_lr_xcfg_sel(void)
{
    static bool checked_flag = false;  //只检测一次.
    if ((!xcfg_cb.bt_tws_en) || (checked_flag) || xcfg_cb.bt_tws_lr_mode < 8) {
        return;
    }
    //printf("xcfg_cb.bt_tws_lr_mode = %d\n",xcfg_cb.bt_tws_lr_mode);
    if(9 == xcfg_cb.bt_tws_lr_mode) {  //有GPIOx接地则为左声道
        sys_cb.tws_left_channel = tws_gpiox_2_gnd_check();
    } else if(10 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为左声道
        sys_cb.tws_left_channel = 1;
    } else if(11 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为右声道
        sys_cb.tws_left_channel = 0;
    } else {
        return;
    }
    sys_cb.tws_force_channel = sys_cb.tws_left_channel + 1;
    checked_flag = true;
    tws_name_suffix_set();
}

void tws_get_lr_channel(void)
{
    bool first_role = bt_tws_get_first_role();

    if (xcfg_cb.bt_tws_lr_mode >= 8) {          //硬件选择（或配置选择）
        tws_lr_xcfg_sel();
    } else if(1 == xcfg_cb.bt_tws_lr_mode) {    //主右声道，副左声道
        sys_cb.tws_left_channel = first_role? true : false;
    } else if(2 == xcfg_cb.bt_tws_lr_mode) {    //主左声道，副右声道
        sys_cb.tws_left_channel = first_role? false : true;
    }
}
#endif
