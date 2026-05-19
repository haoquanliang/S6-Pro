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

//�����ǿ��ض�����еı���
const uint8_t cfg_bt_a2dp_media_discard = 20;            //�������ֹ����У������������ֺ�ָ�����ʱ�������ֻ���һ����������Ҫ����֡��������
#if SWETZ
const uint8_t cfg_a2dp_sbc_max_bitpool = 39;            //����SBC��ʽ������ʣ�Ĭ��ֵΪ53����ѡֵ24~53�����鲻����39��
const uint32_t cfg_a2dp_aac_max_bitrate = 192000;       //����AAC��ʽ������ʣ�Ĭ��ֵΪ0�������ƣ�����ѡֵ128000, 192000, 256000,
uint16_t cfg_bt_music_normal_latency   = 180; //230          //����������ͨģʽ�ӳ٣�Ĭ��ֵ280ms����ѡֵ100~450
uint16_t cfg_bt_music_low_latency      = 60;            //����������Ϸģʽ�ӳ٣�Ĭ��ֵ60ms����ѡֵ60~100
#else
const uint8_t cfg_a2dp_sbc_max_bitpool = 39;            //����SBC��ʽ������ʣ�Ĭ��ֵΪ53����ѡֵ24~53�����鲻����39��
const uint32_t cfg_a2dp_aac_max_bitrate = 128000;       //����AAC��ʽ������ʣ�Ĭ��ֵΪ0�������ƣ�����ѡֵ128000, 192000, 256000,
uint16_t cfg_bt_music_normal_latency   = 280;           //����������ͨģʽ�ӳ٣�Ĭ��ֵ280ms����ѡֵ100~450
uint16_t cfg_bt_music_low_latency      = 60;            //����������Ϸģʽ�ӳ٣�Ĭ��ֵ60ms����ѡֵ60~100
#endif
#if FUNC_1TO2_SWTICH_MUSIC
uint8_t cfg_silence_chk_cnt = 20;
#endif

#if BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN
const u32 sbc_inbuf_size                = 0x10000;
#endif

#if !BT_A2DP_RECON_EN
void a2dp_recon_hook(void)
{
}
#endif

//��ϵͳ����ת��Ϊa2dp_vol
uint8_t a2dp_vol_reverse(uint vol)
{
    uint a2dp_vol = (vol * 1280L /VOL_MAX) /10;

    //32������ʱ��ͬ���ֻ�������98%->100%����Ϊ97%->100%(98%->100%��ʱ�����ͬ�����˵����:��������32�ˣ����ֻ�����97%)
    if(VOL_MAX==32 && a2dp_vol==0x7C) {
        a2dp_vol=0x7B;
    }

    if(a2dp_vol > 0x7f) {
        a2dp_vol = 0x7f;
    }

    return a2dp_vol;
}

//��a2dp_volת��Ϊϵͳ��������
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

//��ϵͳ��������ת��Ϊa2dp_vol
uint a2dp_get_vol(void)
{
    return a2dp_vol_reverse(sys_cb.vol);
}

//��ȡ��������ʱ���������Ļص�����
uint8_t a2dp_vol_get_init_cb(uint8_t vol_feat)
{
    if(vol_feat & BIT(0)) {                 //֧������ͬ��
        return a2dp_get_vol();
    }
    return 0x7f;
}

//�������������Ļص�����
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
