#include "include.h"

#if SYS_KARAOK_EN
//{第一级混响幅度[0, 256]， 每级混响衰减系数[0, 256]}
const u16 echo_level_gain_16[16 + 1][2] = {
    {0, 0},
    {45, 20},   {60, 25},   {65, 30},   {65, 51},
    {79, 51},   {79, 72},   {79, 85},   {79, 101},
    {85, 131},  {85, 142},  {86, 147},  {88, 160},
    {88, 163},  {88, 167},  {95, 171},  {170, 150},
};

//{第一级混响幅度[0, 256]， 每级混响衰减系数[0, 256]}
const u16 echo_level_gain_12[12 + 1][2] = {
    {0, 0},
    {45, 20},   {65, 30},   {65, 51},    {79, 51},
    {79, 72},   {79, 85},   {85, 131},   {86, 147},
    {88, 160},  {88, 167},  {95, 171},   {170, 150},
};

void plugin_hfp_karaok_configure(void)
{
    //android用于通话，iphone用于K歌
#if BT_HFP_CALL_KARAOK_EN
    if (bt_is_ios_device()) {
        sys_cb.hfp_karaok_en = 1;
    } else {
        sys_cb.hfp_karaok_en = 0;
    }
#endif
}

//用于karaok初始化
void plugin_karaok_init(void)
{
    sys_cb.echo_delay = SYS_ECHO_DELAY;
#if !USER_KEY_KNOB_EN   //没有旋钮的话就固定一个值
    sys_cb.music_vol = 8;//USER_KEY_KNOB_LEVEL - 1;
    sys_cb.echo_level = USER_KEY_KNOB_LEVEL - 1;
    sys_cb.mic_vol = USER_KEY_KNOB_LEVEL - 1;
#endif
}

#if SYS_MAGIC_VOICE_EN
AT(.rodata.magic)
const u8 magic_table[5][2] = {
    {0, 7},
    {0, 5},
    {0, 2},
    {1, 1},
    {1, 4},
};

void magic_voice_switch(void)
{
    static int magic_sel = 0;

    magic_sel++;
    if (magic_sel > 4) {
        magic_sel = 0;
    }
    if (!magic_sel){
        sys_cb.magic_type = 0;
        sys_cb.magic_level = 0;
    } else {
        sys_cb.magic_type = magic_table[magic_sel - 1][0];
        sys_cb.magic_level = magic_table[magic_sel - 1][1];
    }
    //printf("set mav:%d %d %d\n", magic_sel, sys_cb.magic_type, sys_cb.magic_level);
    karaok_set_mav(sys_cb.magic_type, sys_cb.magic_level);

}

#elif SYS_PITCH_SHIFT_EN
AT(.rodata.magic)
const int pitch_tbl[5] = {0, -31000, -10000, 8000, 16000};    //-32768~32767

void magic_voice_switch(void)
{
    static int magic_sel = 0;
    int freq_shift;
    u32 sample_rate = (DACDIGCON0 & BIT(1)) ?  48000 : 44100;

    magic_sel++;
    if (magic_sel > 5) {
        magic_sel = 1;
    }
    freq_shift = pitch_tbl[magic_sel - 1];
//    printf("freq_shift_switch: %d, %d\n",freq_shift, magic_sel);
    pitch_shift2_init(sample_rate, freq_shift);
}
#endif

#endif // SYS_KARAOK_EN

