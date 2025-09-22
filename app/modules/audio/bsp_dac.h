#ifndef _BSP_DAC_H
#define _BSP_DAC_H

//数字音量
#define MAX_DIG_VAL         32767
#define DIG_N0DB            32767     //(int)(MAX_DIG_VAL / 1.000000)           //0db
#define DIG_N1DB            29203     //(int)(MAX_DIG_VAL / 1.122018)           //-1db
#define DIG_N2DB            26027     //(int)(MAX_DIG_VAL / 1.258925)
#define DIG_N3DB            23197     //(int)(MAX_DIG_VAL / 1.412538)
#define DIG_N4DB            20674     //(int)(MAX_DIG_VAL / 1.584893)
#define DIG_N5DB            18426     //(int)(MAX_DIG_VAL / 1.778279)
#define DIG_N6DB            16422     //(int)(MAX_DIG_VAL / 1.995262)
#define DIG_N7DB            14636     //(int)(MAX_DIG_VAL / 2.238721)
#define DIG_N8DB            13044     //(int)(MAX_DIG_VAL / 2.511886)
#define DIG_N9DB            11626     //(int)(MAX_DIG_VAL / 2.818383)
#define DIG_N10DB           10361     //(int)(MAX_DIG_VAL / 3.162278)
#define DIG_N11DB           9234      //(int)(MAX_DIG_VAL / 3.548134)
#define DIG_N12DB           8230      //(int)(MAX_DIG_VAL / 3.981072)
#define DIG_N13DB           7335      //(int)(MAX_DIG_VAL / 4.466836)
#define DIG_N14DB           6537      //(int)(MAX_DIG_VAL / 5.011872)
#define DIG_N15DB           5826      //(int)(MAX_DIG_VAL / 5.623413)
#define DIG_N16DB           5193      //(int)(MAX_DIG_VAL / 6.309573)
#define DIG_N17DB           4628      //(int)(MAX_DIG_VAL / 7.079458)
#define DIG_N18DB           4125      //(int)(MAX_DIG_VAL / 7.943282)
#define DIG_N19DB           3676      //(int)(MAX_DIG_VAL / 8.912509)
#define DIG_N20DB           3276      //(int)(MAX_DIG_VAL / 10.000000)
#define DIG_N21DB           2920      //(int)(MAX_DIG_VAL / 11.220185)
#define DIG_N22DB           2602      //(int)(MAX_DIG_VAL / 12.589254)
#define DIG_N23DB           2319      //(int)(MAX_DIG_VAL / 14.125375)
#define DIG_N24DB           2067      //(int)(MAX_DIG_VAL / 15.848932)
#define DIG_N25DB           1842      //(int)(MAX_DIG_VAL / 17.782794)
#define DIG_N26DB           1642      //(int)(MAX_DIG_VAL / 19.952623)
#define DIG_N27DB           1463      //(int)(MAX_DIG_VAL / 22.387211)
#define DIG_N28DB           1304      //(int)(MAX_DIG_VAL / 25.118864)
#define DIG_N29DB           1162      //(int)(MAX_DIG_VAL / 28.183829)
#define DIG_N30DB           1036      //(int)(MAX_DIG_VAL / 31.622777)
#define DIG_N31DB           923       //(int)(MAX_DIG_VAL / 35.481339)
#define DIG_N32DB           823       //(int)(MAX_DIG_VAL / 39.810717)
#define DIG_N33DB           733       //(int)(MAX_DIG_VAL / 44.668359)
#define DIG_N34DB           653       //(int)(MAX_DIG_VAL / 50.118723)
#define DIG_N35DB           582       //(int)(MAX_DIG_VAL / 56.234133)
#define DIG_N36DB           519       //(int)(MAX_DIG_VAL / 63.095734)
#define DIG_N37DB           462       //(int)(MAX_DIG_VAL / 70.794578)
#define DIG_N38DB           412       //(int)(MAX_DIG_VAL / 79.432823)
#define DIG_N39DB           367       //(int)(MAX_DIG_VAL / 89.125094)
#define DIG_N40DB           327       //(int)(MAX_DIG_VAL / 100.000000)
#define DIG_N41DB           292       //(int)(MAX_DIG_VAL / 112.201845)
#define DIG_N42DB           260       //(int)(MAX_DIG_VAL / 125.892541)
#define DIG_N43DB           231       //(int)(MAX_DIG_VAL / 141.253754)
#define DIG_N44DB           206       //(int)(MAX_DIG_VAL / 158.489319)
#define DIG_N45DB           184       //(int)(MAX_DIG_VAL / 177.827941)
#define DIG_N46DB           164       //(int)(MAX_DIG_VAL / 199.526231)
#define DIG_N47DB           146       //(int)(MAX_DIG_VAL / 223.872114)
#define DIG_N48DB           130       //(int)(MAX_DIG_VAL / 251.188643)
#define DIG_N49DB           116       //(int)(MAX_DIG_VAL / 281.838293)
#define DIG_N50DB           103       //(int)(MAX_DIG_VAL / 316.227766)
#define DIG_N51DB           92        //(int)(MAX_DIG_VAL / 354.813389)
#define DIG_N52DB           82        //(int)(MAX_DIG_VAL / 398.107171)
#define DIG_N53DB           73        //(int)(MAX_DIG_VAL / 446.683592)
#define DIG_N54DB           65        //(int)(MAX_DIG_VAL / 501.187234)
#define DIG_N55DB           58        //(int)(MAX_DIG_VAL / 562.341325)
#define DIG_N56DB           51        //(int)(MAX_DIG_VAL / 630.957344)
#define DIG_N57DB           46        //(int)(MAX_DIG_VAL / 707.945784)
#define DIG_N58DB           41        //(int)(MAX_DIG_VAL / 794.328235)
#define DIG_N59DB           36        //(int)(MAX_DIG_VAL / 891.250938)
#define DIG_N60DB           0

//模拟音量
#define ANL_N54DB       0x00        //0 0000
#define ANL_N51DB       0x01        //0 0001
#define ANL_N48DB       0x02        //0 0010
#define ANL_N45DB       0x03        //0 0011
#define ANL_N42DB       0x04        //0 0100
#define ANL_N39DB       0x05        //0 0101
#define ANL_N36DB       0x06        //0 0110
#define ANL_N33DB       0x07        //0 0111
#define ANL_N30DB       0x08        //0 1000
#define ANL_N27DB       0x09        //0 1001
#define ANL_N24DB       0x0A        //0 1010
#define ANL_N21DB       0x0B        //0 1011
#define ANL_N18DB       0x0C        //0 1100
#define ANL_N15DB       0x0D        //0 1101
#define ANL_N12DB       0x0E        //0 1110
#define ANL_N11DB       0x0F        //0 1111
#define ANL_N10DB       0x10        //1 0000
#define ANL_N9DB        0x11        //1 0001
#define ANL_N8DB        0x12        //1 0010
#define ANL_N7DB        0x13        //1 0011
#define ANL_N6DB        0x14        //1 0100
#define ANL_N5DB        0x15        //1 0101
#define ANL_N4DB        0x16        //1 0110
#define ANL_N3DB        0x17        //1 0111
#define ANL_N2DB        0x18        //1 1000
#define ANL_N1DB        0x19        //1 1001
#define ANL_N0DB        0x1A        //1 1010
#define ANL_P1DB        0x1B        //1 1011
#define ANL_P2DB        0x1C        //1 1100
#define ANL_P3DB        0x1D        //1 1101
#define ANL_P4DB        0x1E        //1 1110
#define ANL_P5DB        0x1F        //1 1111

extern const uint32_t tbl_sample_rate[10];
extern const u16 dac_dvol_tbl_db[61];
extern uint8_t cfg_dac_out_spr;

u8 bsp_volume_inc(u8 vol);
u8 bsp_volume_dec(u8 vol);
void bsp_change_volume(u8 vol);
void bsp_change_volume_db(u8 level);
bool bsp_set_volume(u8 vol);
void dac_init(void);
void dac_set_anl_offset(u8 bt_call_flag);
void dac_dnr_init(u8 voice_cnt, u16 voice_pow, u8 silence_cnt, u16 silence_pow);
void dac_dnr_set_sta(u8 sta);
u8 dac_dnr_get_sta(void);
bool bsp_dac_off_for_bt_conn(void);
u16 bsp_volume_convert(u8 vol);
u32 bsp_dac_get_gain_level(u8 vol);
void bsp_set_dac_dvol_tbl(u8* buf);

void bsp_set_auphy_spr(u8 auphy, u8 spr);
void bsp_auphy_adc_mode(u8 auphy, u8 mode);

#endif // _BSP_DAC_H
