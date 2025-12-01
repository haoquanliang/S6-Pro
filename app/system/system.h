#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "mute.h"
#include "param.h"
#include "device.h"
#include "vbat.h"

#if SWETZ_VBAT_TO_PHONE
#define VOLT_BAT_LEVEL_100 (4259)//
#define VOLT_BAT_LEVEL_95  (4206)
#define VOLT_BAT_LEVEL_90  (4155)
#define VOLT_BAT_LEVEL_85  (4107)
#define VOLT_BAT_LEVEL_80  (4051)
#define VOLT_BAT_LEVEL_75  (4004)
#define VOLT_BAT_LEVEL_70  (3951)
#define VOLT_BAT_LEVEL_65  (3908)
#define VOLT_BAT_LEVEL_60  (3875)
#define VOLT_BAT_LEVEL_55  (3845)
#define VOLT_BAT_LEVEL_50  (3816)
#define VOLT_BAT_LEVEL_45  (3790)

#define VOLT_BAT_LEVEL_40  (3770)
#define VOLT_BAT_LEVEL_35  (3755)
#define VOLT_BAT_LEVEL_30  (3727)
#define VOLT_BAT_LEVEL_25  (3709)
#define VOLT_BAT_LEVEL_20  (3674)
#define VOLT_BAT_LEVEL_15  (3652)
#define VOLT_BAT_LEVEL_10  (3636)
#define VOLT_BAT_LEVEL_5   (3600)



#endif


#define VOL_MAX                         xcfg_cb.vol_max   //�����������

typedef struct {
    u32 rst_reason;
    u8  play_mode;
    u8  db_level;
    u8  vol;
    u8  hfp_vol;
    u8  eq_mode;
    u8  cur_dev;
    u8  lang_id;
    u8  lpwr_warning_cnt;
    u8  lpwr_warning_times;     //�����͵������0xff��ʾһֱ��
    u8  vbat_nor_cnt;
    s8  gain_offset;            //���ڶ�̬�޸�������
    u8  hfp2sys_mul;            //ϵͳ������HFP�����������ֻ�HFP����ֻ��16����
    u8  lpwr_cnt;               //�͵����
    u8 dac_sta_bck;
#if BT_TWS_EN
#if (CHARGE_BOX_TYPE == CBOX_SSW)
    u8 loc_house_state;         //bit0=is_open, bit1=key_state
    u8 rem_house_bat;
    u8 rem_bat;
    u8 rem_house_state;         //bit0=is_open, bit1=key_state
    u8 loc_house_bat;
    u8 loc_bat;
    u8 popup_en;
#endif
    u8 tws_left_channel;        //TWS��������־.
    u8 name_suffix_en;          //TWS�����Ƿ����Ӻ�׺����
    u8 tws_force_channel;       //1: �̶�������, 2:�̶�������
    u8 vusb_force_channel;      //VUSB UART�̶�������
#if USER_INEAR_DETECT_EN
    u8 rem_ear_sta;             //�Զ������״̬��0Ϊ�����1Ϊ����
    u8 loc_ear_sta;				//���������״̬��0Ϊ�����1Ϊ����
#endif // USER_INEAR_DETECT_EN
#endif // BT_TWS_EN
    u8 vusb_uart_flag;          //vusb uart�򿪱�־
    u8 discon_reason;
#if CHARGE_LOW_POWER_EN
    u32 charge_timr0_pr;
#endif
volatile u8  incall_flag;

    u8 fmrx_type;
    u8 inbox_wken_flag;
    u8 outbox_pwron_flag;
    u16 kh_vol_msg;
    u16 vbat;
    u32 sleep_time;
    u32 pwroff_time;
    u32 sleep_delay;
    u32 pwroff_delay;
    u32 sleep_wakeup_time;
    u32 ms_ticks;               //msΪ��λ
    u32 rand_seed;
    u8  tws_res_brk;
    u8  tws_res_wait;
    u8  wav_sysclk_bak;
    u8 wko_pwrkey_en;
    u8 tkey_pwrdwn_en;
    u8 sw_rst_flag;

    u8 sleep_counter;       //sleepmode����ʱ����

volatile u8  cm_times;
volatile u8  loudspeaker_mute;  //����MUTE��־
volatile u8  pwrkey_5s_check;   //�Ƿ���п�������5S��PWRKEY���
volatile u8  pwrkey_5s_flag;    //����5��PWRKEY������־
volatile u8  charge_sta;        //0:���رգ� 1����翪���� 2������
volatile u8  charge_bled_flag;  //charge�������Ƴ�����־
volatile u8  ch_bled_cnt;       //charge����������ʱ�����
volatile u8  micl_en;           //MICL enable working��־
volatile u8  poweron_flag;      //pwrkey������־
volatile u8  pwrdwn_tone_en;    //POWER DOWN�Ƿ񲥷���ʾ��
volatile u8  key2unmute_cnt;    //����������ʱ��mute
volatile u8  pwrdwn_hw_flag;    //ģ��Ӳ���أ��ػ���־

volatile u8  dc_in_filter;      //DC IN filter

#if REC_FAST_PLAY
    u16 rec_num;                //��¼���µ�¼���ļ����
    u32 ftime;
#endif

#if SYS_KARAOK_EN
    u8 echo_level;              //����ȼ�
    u8 echo_delay;              //������
    u8 mic_vol;                 //MIC����
    u8 music_vol;               //MUSIC����
    u8 rec_msc_vol;             //KARAOK¼��ʱMUSIC����������
    u8 magic_type;
    u8 magic_level;
#endif

#if ANC_EN || TINY_TRANSPARENCY_EN
    u8 anc_user_mode;           //ancģʽ, 0:�ر�, 1:ANC, 2:ͨ͸
    u8 ttp_start;
    u8 ttp_sta;
#endif

#if ANC_EN
    u8 anc_start;
    u8 anc_init;
    s8 adjust_val[8];           //mic����ֵ
	u32 mic_gain[4];            //mic��ʼֵ
#endif

#if USER_NTC
    u8  ntc_2_pwrdwn_flag;
#endif

#if ANC_ALG_EN
    volatile u8 anc_alg_en;
    u32 anc_alg_type_bak;
#endif // ANC_ALG_EN
    ///λ������Ҫ������Ҫ���жϸ�ֵ�ı����� �����ʹ��λ�����������ٶ���λ������
    u8  rtc_first_pwron  : 1,   //RTC�Ƿ��һ���ϵ�
        mute             : 1,   //ϵͳMUTE���Ʊ�־
        cm_factory       : 1,   //�Ƿ��һ��FLASH�ϵ�
        cm_vol_change    : 1,   //���������Ƿ���Ҫ���µ�FLASH
        bthid_mode       : 1,   //����HID����ģʽ��־
        port2led_en      : 1,   //1��IO��������
        voice_evt_brk_en : 1,   //������ʾ��ʱ��U�̡�SD����LINEIN�Ȳ����¼��Ƿ�������Ӧ.
        karaok_init      : 1;   //karaok��ʼ����־

    u8  sleep_en             : 1,   //���ڿ����Ƿ����sleep
		lowbat_flag          : 1,
        hfp_karaok_en        : 1,   //ͨ��ģʽ�Ƿ��K�蹦�ܡ�
        eq_app_total_gain_en : 1;   //���ڿ����Ƿ�ʹ��app eq������
    u8 led_scan_en;
#if USER_INEAR_DETECT_EN
    u8 in_ear_en;
#endif
#if BT_MAP_EN
    u8 map_retry;
#endif
#if ABP_EN
    u8 abp_mode;
#endif
#if ASR_EN
    u8 asr_enable;
#endif
#if FUNC_USBDEV_EN
    u8  ude_mic_spr;            //usb mic spr
    u8  ude_mic_start_en;
#endif
#if ANC_ALG_DYVOL_FF_EN
    s8 dyvol_local;
    s8 dyvol_real;
    s8 dyvol_overlap;
#if BT_TWS_EN
    s8 dyvol_remote;
#endif // BT_TWS_EN
#endif // ANC_ALG_DYVOL_FF_EN
#if SWETZ
    volatile u16 sys_5ms_cnt;
    u32 tick_event;
    u8  peer_bat_level;
    u8  local_bat_level; 
    volatile u32 SWETZ_tick;
    volatile u8 flag_local_in_case;
    volatile u8 flag_peer_in_case;
    u8 flag_outcase_5s_kye_null;
    u8 ntc_discharge_out_normal_range;
    u8  recon_tone;
    u16 find_ear_count;
    u8 flag_2nd_ringtone_ongoing;
    u8 find_left_ear_going;
    u8 find_right_ear_going;
    u8 find_left_ear_stop;
    u8 find_right_ear_stop;
    u8 find_me_ear_count;
    u8 find_me_count;
    u8 flag_playing_find_ear;
#endif
#if SWETZ_TEST
    u8 test;
#endif

} sys_cb_t;
extern sys_cb_t sys_cb;

extern volatile int micl2gnd_flag;
extern volatile u32 ticks_50ms;


void sys_init(void);
void sys_update_init(void);
void timer1_irq_init(void);
void bsp_sys_mute(void);
void bsp_sys_unmute(void);
void bsp_clr_mute_sta(void);
#if LOUDSPEAKER_MUTE_EN
void bsp_loudspeaker_mute(void);
void bsp_loudspeaker_unmute(void);
#else
#define bsp_loudspeaker_mute()
#define bsp_loudspeaker_unmute()
#endif
void uart0_mapping_sel(void);
void linein_detect(void);
bool linein_micl_is_online(void);
void get_usb_chk_sta_convert(void);
uint bsp_get_bat_level(void);
void sd_soft_cmd_detect(u32 check_ms);

void cpu_set_sfr(psfr_t sfr, u32 value);
void cpu_clr_sfr(psfr_t sfr, u32 value);

#if SWETZ_VBAT_TO_PHONE
static uint8_t get_bat_level_from_volt_wo_charger(uint16_t volt);
uint8_t app_bat_level_show_for_phone(uint8_t bat_real_level);
uint8_t app_bat_level_show_for_app(uint8_t bat_real_level);
#endif
#if APP_SWITCH_TONE_TYPE
void user_poweron_tone_play(void);
void user_poweroff_tone_play(void);
void user_pairing_tone_play(void);
void user_music_mode_tone_play(void);
void user_game_mode_tone_play(void);
void user_disc_tone_play(void);
void user_connect_tone_play(void);
void user_low_bat_tone_play(void);
#endif



#if SWETZ_NTC
u8 user_ntc_check(void);
#define ntc_gpio_power_supply()     \
    do {                            \
        GPIOEFEN &= ~BIT(4);        \
        GPIOEDE |= BIT(4);          \
        GPIOEDIR &= ~BIT(4);        \
        GPIOESET |= BIT(4);         \
    } while (0)

#define ntc_gpio_power_down()       \
    do {                            \
        GPIOECLR |= BIT(4);         \
    } while (0)    
#endif



#endif // __SYSTEM_H
