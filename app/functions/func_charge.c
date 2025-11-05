#include "include.h"
#include "func.h"


AT(.text.func.charge)
void func_charge_process(void)
{
    while (1) {
        WDT_CLR();

#if QTEST_EN
        if(qtest_get_mode() || qtest_cb.sta){
            func_cb.sta = func_cb.last;
            break;;
        }
#endif

#if CHARGE_LOW_POWER_EN
        delay_us(200);
#else
        delay_5ms(1);
#endif

#if CHARGE_EN
        if (charge_charge_on_process()) {
            func_cb.sta = func_cb.last;
            break;
        }
#endif
    }
}

AT(.text.func.charge)
static void func_charge_enter(void)
{
#if ANC_EN
    if(xcfg_cb.anc_en && sys_cb.anc_user_mode) {
        bsp_anc_stop();     //bsp_anc_set_mode(0);
    }
#endif
#if BT_MUSIC_EFFECT_EN
    music_effect_alg_suspend(MUSIC_EFFECT_SUSPEND_FOR_CHARGE);
#endif // BT_MUSIC_EFFECT_EN

	dac_power_off();

    lock_code_charge();
#if CHARGE_EN
	charge_enter(1);
#endif
}

AT(.text.func.charge)
static void func_charge_exit(void)
{
#if CHARGE_EN
    charge_exit();
#endif
    unlock_code_charge();
    printf("dc out\n");

    bsp_res_set_enable(true);
    func_bt_init();
    en_auto_pwroff();

    if (xcfg_cb.bt_tws_pair_mode > 1) {
        bt_tws_set_scan(0x03);
    }
    if (!bsp_dac_off_for_bt_conn()) {
        dac_restart();
    }

#if BT_MUSIC_EFFECT_EN
    music_effect_alg_restart();
#endif // BT_MUSIC_EFFECT_EN

#if ANC_EN
    if(sys_cb.anc_user_mode) {
        bsp_anc_set_mode(sys_cb.anc_user_mode);
    }
    dac_fade_in();
#endif
#if SWETZ_POWER_ON_TONE
    if (xcfg_cb.bt_outbox_voice_pwron_en) {
        mp3_res_play(RES_BUF_POWERON, RES_LEN_POWERON);
    }
#endif

}

AT(.text.func.charge)
void func_charge(void)
{
    printf("%s\n", __func__);

    func_charge_enter();
    func_charge_process();
    func_charge_exit();
}
