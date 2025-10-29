#include "include.h"
#include "api.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

//下列是可重定义库中的变量
const uint8_t cfg_bt_a2dp_media_discard = 20;            //播放音乐过程中，唤出语音助手后恢复音乐时，部分手机有一点杂音，需要丢几帧音乐数据

//const uint8_t cfg_a2dp_sbc_max_bitpool = 39;            //设置SBC格式最大码率，默认值为53，可选值24~53（建议不低于39）
//const uint32_t cfg_a2dp_aac_max_bitrate = 128000;       //设置AAC格式最大码率，默认值为0（不限制），可选值128000, 192000, 256000,
//uint16_t cfg_bt_music_normal_latency   = 280;           //设置蓝牙普通模式延迟，默认值280ms，可选值100~450
//uint16_t cfg_bt_music_low_latency      = 60;            //设置蓝牙游戏模式延迟，默认值60ms，可选值60~100

#if BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN
const u32 sbc_inbuf_size                = 0x10000;
#endif

#if !BT_A2DP_RECON_EN
void a2dp_recon_hook(void)
{
}
#endif

//将系统音量转换为a2dp_vol
uint8_t a2dp_vol_reverse(uint vol)
{
    uint a2dp_vol = (vol * 1280L /VOL_MAX) /10;

    //32段音量时，同步手机音量由98%->100%调整为97%->100%(98%->100%有时会出现同步不了的情况:耳机音量32了，但手机音量97%)
    if(VOL_MAX==32 && a2dp_vol==0x7C) {
        a2dp_vol=0x7B;
    }

    if(a2dp_vol > 0x7f) {
        a2dp_vol = 0x7f;
    }

    return a2dp_vol;
}

//将a2dp_vol转换为系统音量级数
uint8_t a2dp_vol_conver(uint8_t a2dp_vol)
{
    uint8_t vol_unit = (1280L / VOL_MAX) / 10;
    if (a2dp_vol > 0 && a2dp_vol < vol_unit) {
        a2dp_vol = vol_unit;
    } else if(a2dp_vol >= 127) {
        return VOL_MAX;
    }

    return (a2dp_vol + 1) * VOL_MAX / 128;
}

//将系统音量级数转换为a2dp_vol
uint a2dp_get_vol(void)
{
    return a2dp_vol_reverse(sys_cb.vol);
}

//获取初次连接时蓝牙音量的回调函数
uint8_t a2dp_vol_get_init_cb(uint8_t vol_feat)
{
    if(vol_feat & BIT(0)) {                 //支持音量同步
        return a2dp_get_vol();
    }
    return 0x7f;
}

//按键调节音量的回调函数
uint8_t a2dp_vol_adj_cb(uint8_t a2dp_vol, bool up_flag)
{
    uint8_t vol_set = a2dp_vol_conver(a2dp_vol);

    if(up_flag) {
        vol_set = bsp_volume_inc(vol_set);
    } else {
        vol_set = bsp_volume_dec(vol_set);
    }

    TRACE("adj_vol: %d(%x)\n", sys_cb.vol, a2dp_vol);
    return a2dp_vol_reverse(vol_set);
}

AT(.com_text.a2dp)
void a2dp_tws_sync_close(void)
{
    if (!sbc_is_bypass() && !bsp_res_is_playing()) {
#if SWETZ_FADE_HANGDLE
        dac_fade_out();
#else 
        msg_enqueue(EVT_A2DP_SYNC_CLOSE);
#endif
        
    }
}
