#include "include.h"

#if SYS_KARAOK_EN

karaok_vol_t karaok_vol;

static s16 echo_cache_0[0x2800] AT(.echo_cache);

AT(.text.bsp.dac.table)
const u16 karaok_dvol_table_12[12 + 1] = {
    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N22DB,  DIG_N18DB,  DIG_N14DB,  DIG_N12DB,  DIG_N10DB,
    DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
};

AT(.text.bsp.dac.table)
const u16 karaok_dvol_table_16[16 + 1] = {
    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N24DB,  DIG_N22DB,  DIG_N20DB,  DIG_N18DB, DIG_N16DB,
    DIG_N14DB,  DIG_N12DB,  DIG_N10DB,  DIG_N8DB,   DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
};

#if SYS_ECHO_EN
//混响间隔：(echo_delay[0, 87] * 128 / 44100)ms
const u16 echo_delay_12[12 + 1] = {
    0,
    30,       35,        38,         40,
    45,       48,        55,         60,
    75,       80,        85,         87,
};

//混响间隔：(echo_delay[0, 87] * 128 / 44100)ms
const u16 echo_delay_16[16 + 1] = {
     0,
     3,          5,        10,        15,
    20,         25,        40,        48,
    55,         60,        65,        70,
    74,         78,        83,        87,
};
#endif

kara_cfg_t kara_cfg = {
    .res[0]         = 0,
    .res[1]         = 0,
    .echo_en        = SYS_ECHO_EN,
};

#if SYS_ECHO_EN
AT(.text.bsp.karaok)
void bsp_echo_set_level(void)
{
    echo_set_level(karaok_vol.echo_level_gain[sys_cb.echo_level]);
}

AT(.text.bsp.karaok)
void bsp_echo_set_delay(void)
{
    echo_set_delay(karaok_vol.echo_delay[sys_cb.echo_delay]);
}
#endif

AT(.text.bsp.karaok)
void bsp_karaok_set_mic_volume(void)
{
    karaok_set_mic_volume(karaok_vol.micvol_table[sys_cb.mic_vol]);
}

AT(.text.bsp.karaok)
void bsp_karaok_set_music_volume(void)
{
    karaok_set_music_volume(karaok_vol.dvol_table[sys_cb.music_vol]);
}

void bsp_karaok_music_vol_adjust(u8 up)
{
    if (up) {
        if (sys_cb.music_vol <  USER_KEY_KNOB_LEVEL -1) {
            sys_cb.music_vol++;
        } else {
            sys_cb.music_vol = USER_KEY_KNOB_LEVEL -1;
        }
    } else {
        if (sys_cb.music_vol > 0) {
            sys_cb.music_vol--;
        }
    }
    printf("karaok music_vol = %d\n", sys_cb.music_vol);
    bsp_karaok_set_music_volume();
}

void bsp_karaok_mic_vol_adjust(u8 up)
{
    if (up) {
        if (sys_cb.mic_vol <  USER_KEY_KNOB_LEVEL -1) {
            sys_cb.mic_vol++;
        } else {
            sys_cb.mic_vol = USER_KEY_KNOB_LEVEL -1;
        }
    } else {
        if (sys_cb.mic_vol > 0) {
            sys_cb.mic_vol--;
        }
    }
    printf("karaok mic_vol = %d\n", sys_cb.mic_vol);
    bsp_karaok_set_mic_volume();
}

AT(.text.bsp.karaok)
void bsp_karaok_init(void)
{
//    printf("%s %d\n", __func__, func);
    if (sys_cb.karaok_init) {
        return;
    }

    kara_cfg.echo_cache = echo_cache_0;

    sys_clk_req(INDEX_KARAOK, SYS_48M);

    sys_cb.echo_delay = SYS_ECHO_DELAY;
#if (USER_KEY_KNOB_LEVEL == 12)
#if SYS_ECHO_EN
    karaok_vol.echo_delay = echo_delay_12;
    karaok_vol.echo_level_gain = echo_level_gain_12;
#endif // SYS_ECHO_EN
    karaok_vol.dvol_table = karaok_dvol_table_12;
    karaok_vol.micvol_table = karaok_dvol_table_12;
#elif (USER_KEY_KNOB_LEVEL == 16)
#if SYS_ECHO_EN
    karaok_vol.echo_delay = echo_delay_16;
    karaok_vol.echo_level_gain = echo_level_gain_16;
#endif // SYS_ECHO_EN
    karaok_vol.dvol_table = karaok_dvol_table_16;
    karaok_vol.micvol_table = karaok_dvol_table_16;
#endif
    audio_path_init(AUDIO_PATH_KARAOK);
    karaok_init(&kara_cfg);
    plugin_karaok_init();

#if MIC_DNR_EN
    mic_dnr_init(2, 500, 100, 1000);        //10ms检测一次 //连续超过2次大于500就认为有声，连续超过100次低于 1000 就认为无声
#endif
    //karaok_enable_mic_first(65535/10);    //(数字音量最大值为0xFFFF_65535)
    //mic_first_init(2, 700, 30, 1200);     //连续超过2次大于700 就开启mic_first，连续超过 100 次低于 1200 就关闭mic_first
    //karaok_mic_first_music_vol_fade_enable(100, 1000);   //淡入步进100, 淡出步进1000.   (数字音量最大值为0xFFFF_65535)

#if SYS_ECHO_EN
    echo_set_delay(karaok_vol.echo_delay[sys_cb.echo_delay]);
    echo_set_level(karaok_vol.echo_level_gain[sys_cb.echo_level]);
#endif
    karaok_set_mic_volume(karaok_vol.micvol_table[sys_cb.mic_vol]);
    karaok_set_music_volume(karaok_vol.dvol_table[sys_cb.music_vol]);

#if MIC_DETECT_EN
    if (!dev_is_online(DEV_MIC)) {
        karaok_mic_mute();
    }
#endif
    audio_path_start(AUDIO_PATH_KARAOK);

    dac1_fade_in();

#if SYS_MAGIC_VOICE_EN
    karaok_set_mav(sys_cb.magic_type, sys_cb.magic_level);
#endif
    sys_cb.karaok_init = 1;
}

AT(.text.bsp.karaok)
void bsp_karaok_exit(void)
{
    if (sys_cb.karaok_init == 0) {
        return;
    }
    dac1_fade_out();
    dac1_fade_wait();

    karaok_exit();
    audio_path_exit(AUDIO_PATH_KARAOK);
    sys_clk_free(INDEX_KARAOK);

    sys_cb.karaok_init = 0;
}
#endif

bool bt_sco_karaok_is_en(u8 enter)
{
#if SYS_KARAOK_EN
    if (enter) {
        bsp_karaok_exit();
        sys_cb.hfp_karaok_en = BT_HFP_CALL_KARAOK_EN;       //通话是否支持KARAOK
        plugin_hfp_karaok_configure();
        if (sys_cb.hfp_karaok_en) {
            bsp_karaok_init();
            kara_sco_start();
            return true;
        } else {
            return false;
        }
    } else {
        if (sys_cb.hfp_karaok_en) {
            kara_sco_stop();
            bsp_karaok_exit();
            sys_cb.hfp_karaok_en = 0;
            return true;
        } else {
            return false;
        }
    }
#else
    return false;
#endif
}
