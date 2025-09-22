#include "include.h"
#include "bsp_anc_param_table.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

uint calc_crc(void *buf, uint len, uint seed);
void bsp_param_read(u8 *buf, u32 addr, uint len);
void bsp_param_write(u8 *buf, u32 addr, uint len);
extern uint os_spiflash_read(void *buf, u32 addr, uint len);
#if ANC_EQ_RES2_EN
static u8 anc_temp_buf[EQ_BUFFER_LEN] AT(.anc_res2_buf);
#endif

#if ANC_EN
/****************************** ANC通路按照下表固定 **************************************
 * MODE_TWS_FFFB：    输入：mic2(FF/FB)              输出DACL
 * MODE_FFFB：        输入：mic2、3(FF/FB)           输出DACL/R (mic2->dacl，mic3->dacr)
 * MODE_TWS_HYBRID：  输入：mic2、3(Hybrid)          输出DACL
 * MODE_HYBRID：      输入：mic0、1、2、3(Hybrid)    输出DACL/R (mic01->dacl，mic23->dacr)
 *****************************************************************************************/
static anc_channel_t channel_0 = {
    .mic_ch = MIC2,
    .msc_dly = 0,
    .bypass = 0,
    .drc.drc_en = 0,
    .drc.filter_en = 0,
    .drc.drc_after_eq = 0,
};

static anc_channel_t channel_1 = {
    .mic_ch = MIC3,
    .msc_dly = 0,
    .bypass = 0,
    .drc.drc_en = 0,
    .drc.filter_en = 0,
    .drc.drc_after_eq = 0,
};

static anc_channel_t channel_2 = {
    .mic_ch = MIC0,
    .msc_dly = 0,
    .bypass = 0,
    .drc.drc_en = 0,
    .drc.filter_en = 0,
    .drc.drc_after_eq = 0,
};

static anc_channel_t channel_3 = {
    .mic_ch = MIC1,
    .msc_dly = 0,
    .bypass = 0,
    .drc.drc_en = 0,
    .drc.filter_en = 0,
    .drc.drc_after_eq = 0,
};

struct anc_cfg_t anc_cfg = {
    .spr = SPR_384000,
    .ch[0] = &channel_0,
    .ch[1] = &channel_1,
    .ch[2] = &channel_2,
    .ch[3] = &channel_3,
    .fade_in_step = 12,
    .fade_out_step = 12,
    .change_en = 0,
    .dc_rm = ANC_DCRM_EN,
    .fade_en = 1,
    .order = 1,
};

static u32 anc_dbg_cache[8][51] AT(.anc_dbg_buf);
#if ANC_MAX_VOL_DIS_FB_EN
static u32 anc_max_vol_dis_fb_tick = 0;
static u8 anc_max_vol_process_sta = 0;
static u8 anc_max_vol_dac_vol_sta = 0;
static u8 anc_max_vol_change_cnt = 0;
#endif // ANC_MAX_VOL_DIS_FB_EN

#define EQ_GAIN_STEP        2       //0.2dB

AT(.text.update)
void bsp_get_anctrim(u8 trimbuf[2])
{
    bsp_param_read(&trimbuf[0], PARAM_ANC_MIC0_VAL, 1);
    bsp_param_read(&trimbuf[1], PARAM_ANC_MIC1_VAL, 1);
}

//计算校准增益
static u32 bsp_anc_gain_adjust_do(u8 ch, s8 value)
{
    s32 gain, abs_gain, adjust_val = 0;
    gain = (s32)sys_cb.mic_gain[ch];//初始gain值
    if (value != 0) {
        if (value >= 60 || value <= -60) {
            s8 value1 = (value >= 60) ? 59 : -59;
            s8 value2 = (value >= 60) ? (value - 59) : (value + 59);
            adjust_val = (s32)anc_pow10_cal(value1 * EQ_GAIN_STEP);
            s32 adjust_val2 = (s32)anc_pow10_cal(value2 * EQ_GAIN_STEP);
            //gain = ((s64)gain * adjust_val) >> ANC_FIX_BIT;
            //gain = ((s64)gain * adjust_val2) >> ANC_FIX_BIT;
            gain = (s32)muls_shift23(gain, adjust_val);
            gain = (s32)muls_shift23(gain, adjust_val2);
            TRACE("adjust_val %d %d\n", adjust_val, adjust_val2);
        } else {
            adjust_val = (s32)anc_pow10_cal(value * EQ_GAIN_STEP);
            //gain = ((s64)gain * adjust_val) >> ANC_FIX_BIT;
            gain = (s32)muls_shift23(gain, adjust_val);
        }
        abs_gain = gain > 0? gain : -gain;
        if (abs_gain > EQ_GAIN_MAX || abs_gain < EQ_GAIN_MIN) {
            TRACE("adjust gain overflow %d\n", abs_gain);
            return sys_cb.mic_gain[ch];
        }
        TRACE("anc ch[%d] adjust value:%d adjust_val:%d prev_gain:%d  gain:%d\n", ch, value, adjust_val, sys_cb.mic_gain[ch], gain);
    }
    return (u32)gain;
}

//设置校准增益
void bsp_anc_gain_adjust(u8 ch, s8 value)
{
    if (anc_cfg.ch[ch] == NULL) {
        return;
    }
    sys_cb.adjust_val[ch] = value;
    anc_cfg.ch[ch]->nos_gain = bsp_anc_gain_adjust_do(ch, value);
    anc_set_param();                //更新参数
}

bool bsp_anc_res_check_crc(u32 addr, u32 len)
{
    u8 *ptr = (u8 *)addr;
    u8 offset = 0;
    if (ptr[0] == 'E' && ptr[1] == 'q' && ptr[2] == 0x01) {
        offset = 4;
    }
    u8 band_cnt = ptr[3+offset];
    if (band_cnt > 10) {        //最多支持10条降噪eq
        return false;
    }
    u32 res_crc = ptr[band_cnt*27+11+offset] << 8 | ptr[band_cnt*27+10+offset];
    u32 cal_crc = calc_crc(ptr, band_cnt*27+10+offset, 0xffff);
    //TRACE("res_crc %x, cal_crc %x\n", res_crc, cal_crc);
    if (res_crc == cal_crc) {
        return true;
    } else {
        return false;
    }
}

static void bsp_anc_set_nos_param(u8 ch, u8 band_cnt, const u32 *param)
{
    if (anc_cfg.ch[ch] == NULL) {
        return;
    }
    anc_cfg.ch[ch]->nos_band = band_cnt;
    if (band_cnt == 0 || param == NULL) {
        return;
    }
    TRACE("bsp_anc_set_nos_param dbg mode (%d), band %d\n", ch, band_cnt);
    anc_cfg.ch[ch]->nos_gain = *param++;
    anc_cfg.ch[ch]->nos_param = param;
}

static void bsp_anc_set_msc_param(u8 ch, u8 band_cnt, const u32 *param)
{
    if (anc_cfg.ch[ch] == NULL) {
        return;
    }
    anc_cfg.ch[ch]->msc_band = band_cnt;
    if (band_cnt == 0 || param == NULL) {
        return;
    }
    TRACE("bsp_anc_set_msc_param dbg mode (%d), band %d\n", ch, band_cnt);
    anc_cfg.ch[ch]->msc_gain = *param++;
    anc_cfg.ch[ch]->msc_param = param;
}

#if ANC_EQ_RES2_EN
enum res2_sta {
    RES_LEN_VAL        =   BIT(0),       //原来的anc资源文件是否有效
    RES2_LEN_VAL       =   BIT(1),       //资源文件中是否预留res2文件
};

static bool bsp_anc_load_res2_addr(u32 *addr, u32 len, u32 addr2, u32 len2_addr)
{
    u8 res_valid = 0;
    u32 res2_len = 0;
    u32 res2_addr = 0;

    if (len != 0) {
        res_valid |= RES_LEN_VAL;
    }
    addr2 += ANC_EQ_RES2_BASE_ADDR;
    len2_addr += ANC_EQ_RES2_BASE_ADDR;
    os_spiflash_read(&res2_len, len2_addr, 4);
    if (res2_len) {
        res_valid |= RES2_LEN_VAL;
    }
    if (res_valid == 0) {
        TRACE("res2 len is zero, len2_addr %x\n", len2_addr);
        return false;
    }
    if (res_valid & RES2_LEN_VAL) {
        if ((res_valid & RES_LEN_VAL) == 0 || ANC_EQ_RES2_FIRST_SELECT) {
            os_spiflash_read(&res2_addr, addr2, 4);
            os_spiflash_read(anc_temp_buf, res2_addr + ANC_EQ_RES2_BASE_ADDR, res2_len);
            *addr = (u32)anc_temp_buf;
            TRACE("anc_load_res2: drc_res2_addr %x  res2_len%x addr %x ", res2_addr + ANC_EQ_RES2_BASE_ADDR, res2_len, addr);
        }
    }
    return true;
}
#endif

//从资源文件中获取drc参数
static u8 *bsp_anc_get_drc_addr_by_res(u32 addr, u32 len, u32 addr2, u32 len2_addr)
{
#if ANC_EQ_RES2_EN
    TRACE("bsp_anc_get_drc_addr_by_res len2_addr %x\n", len2_addr);
    if (!bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr)) {
        return NULL;
    }
#endif
    u8 *ptr = (u8 *)addr;
    if (ptr[2] != 0x02) {           //DRC V3
        return NULL;
    }
    u8 drc_cnt = ptr[7];
    u32 res_crc = ptr[drc_cnt * 17 + 41] << 8 | ptr[drc_cnt * 17 + 40];
    u32 cal_crc = calc_crc(ptr, drc_cnt * 17 + 40, 0xffff);

    if (res_crc != cal_crc) {
        return NULL;
    }
    return &ptr[8];
}

//从资源文件中获取drc eq参数
static u32 *bsp_anc_get_drc_eq_addr_by_res(u8 ch, u32 addr, u32 len, u32 addr2, u32 len2_addr)
{
#if ANC_EQ_RES2_EN
    TRACE("bsp_anc_get_drc_eq_addr_by_res len2_addr %x\n", len2_addr);
    bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr);
#endif
    if (!bsp_anc_res_check_crc(addr, len)) {
        return NULL;
    }
    u8 *ptr = (u8 *)addr;
    anc_cfg.ch[ch]->drc.filter_band = ptr[7];
    return (u32 *)&ptr[8];
}

//从资源文件中获取nos参数
static int bsp_anc_set_nos_param_by_res(u8 ch, u32 addr, u32 len, u32 addr2, u32 len2_addr, u8 tp)
{
#if ANC_EQ_RES2_EN
    TRACE("bsp_anc_set_nos_param_by_res ch %d, len2_addr %x\n", ch, len2_addr);
    if (!bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr)) {
        return -1;
    }
#endif
    if (!bsp_anc_res_check_crc(addr, len)) {
        return -2;
    }
    u8 *ptr = (u8 *)addr;
    u8 band_cnt = ptr[7];
    u32 *param = (u32 *)&ptr[8];
    if (anc_cfg.ch[ch] == NULL) {
        return -3;
    }
    bsp_anc_set_nos_param(ch, band_cnt, param);
    sys_cb.mic_gain[ch] = anc_cfg.ch[ch]->nos_gain;                             //保存资源文件中的gain值
    if (xcfg_cb.anc_gain_en) {	                                                //写入量产测试的Gain
        if(sys_cb.adjust_val[ch] != 0) {
            anc_cfg.ch[ch]->nos_gain = bsp_anc_gain_adjust_do(ch, sys_cb.adjust_val[ch + tp*4]);
            TRACE("ch %d reset mic dgain:%d %x\n", ch, sys_cb.adjust_val[ch], anc_cfg.ch[ch]->nos_gain);
        }
    }
    return 0;
}

//从资源文件中获取msc参数
static int bsp_anc_set_msc_param_by_res(u8 ch, u32 addr, u32 len, u32 addr2, u32 len2_addr)
{
#if ANC_EQ_RES2_EN
    TRACE("bsp_anc_set_msc_param_by_res ch %d, len2_addr %x\n", ch, len2_addr);
    if (!bsp_anc_load_res2_addr((u32 *)&addr, len, addr2, len2_addr)) {
        return -1;
    }
#endif
    if (!bsp_anc_res_check_crc(addr, len)) {
        return -2;
    }
    u8 *ptr = (u8 *)addr;
    u8 band_cnt = ptr[7];
    u32 *param = (u32 *)&ptr[8];
    if (anc_cfg.ch[ch] == NULL) {
        return -3;
    }
    bsp_anc_set_msc_param(ch, band_cnt, param);
    return 0;
}

void bsp_anc_set_mic_gain(u8 ch, u8 anl, u8 gain)
{
    TRACE("anc ch[%d], set mic anl %d, gain %d\n", ch, anl, gain);
    anc_set_mic_gain(ch, anl, gain);
}

void bsp_anc_mic_mute(u8 ch, u8 mute)
{
    TRACE("anc ch[%d] mute_set:%x\n", ch, mute);
    anc_mic_mute(ch, mute);
}

void bsp_anc_dig_mic_mute(u8 ch, u8 mute)
{
    TRACE("anc ch[%d] mute_set:%x\n", ch, mute);
    anc_dig_mic_mute(ch, mute);
}

void bsp_anc_start(void)
{
    dac_fade_in();
    if (sys_cb.anc_start == 0) {
        sys_cb.anc_start = 1;
        anc_start();
    }
}

void bsp_anc_stop(void)
{
    if (sys_cb.anc_start) {
        sys_cb.anc_start = 0;
        anc_stop();
    }
}

#if ANC_ALG_EN
void bsp_anc_set_mode_alg_do(u8 mode)
{
#if ANC_ALG_HOWLING_FB_EN
    if (mode == 2) {        //通透下开启
        bsp_anc_alg_howling_fb_start();
    } else {
        bsp_anc_alg_howling_fb_stop();
    }
#endif // ANC_ALG_HOWLING_FB_EN
#if ANC_ALG_HOWLING_FF_EN
    if (mode) {
        bsp_anc_alg_howling_ff_start();
    } else {
        bsp_anc_alg_howling_ff_stop();
    }
#endif // ANC_ALG_HOWLING_FF_EN
#if ANC_ALG_LIMITER_FF_EN
    if (mode) {
        bsp_anc_alg_limiter_ff_start();
    } else {
        bsp_anc_alg_limiter_ff_stop();
    }
#endif // ANC_ALG_LIMITER_FF_EN
#if ANC_ALG_DUMP_FOR_ANC_MODE
    if (mode) {
        if (sys_cb.anc_alg_en == 1) {
            bsp_anc_alg_stop();
        }
        bsp_anc_alg_start(ANC_ALG_DUMP_TYPE);
    } else {
        bsp_anc_alg_stop();
    }
#endif // ANC_ALG_DUMP_FOR_ANC_MODE
}
#endif // ANC_ALG_EN

void bsp_anc_set_param(u8 tp)
{
    int i;
    const anc_param *anc_tbl;
    const anc_param *drc_tbl;
    const anc_param *drc_eq_tbl;
#if ANC_EQ_RES2_EN
    const anc_exparam *anc_tbl2;
    const anc_exparam *drc_tbl2;
    const anc_exparam *drc_eq_tbl2;
#endif
    TRACE("ANC MODE ==> %d\n", xcfg_cb.anc_mode);
    if (anc_cfg.anc_mode >= MODE_TWS_HYBRID) {
        u8 channel = 0;
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en){
            channel = !sys_cb.tws_left_channel;
        }
#endif
        u8 tbl_offset = 0;
        if (anc_cfg.anc_mode == MODE_TWS_HYBRID) {            //TWS HYBRID 区分左右声道，位移为4
            tbl_offset = 4;
        } else {                                              //4mic HYBRID L0\L1\R0\R1都要配，位移为2
            channel = 0;                                      //HYBRID MODE声道设为0,保证正确读取资源文件
            tbl_offset = 2;
        }
        anc_tbl = &anc_hybrid_eq_tbl[tp][tbl_offset * channel];
        drc_tbl = &anc_hybrid_drc_tbl[tp][tbl_offset/2 * channel];
        drc_eq_tbl = &anc_hybrid_drc_eq_tbl[tp][tbl_offset/2 * channel];
#if ANC_EQ_RES2_EN
        anc_tbl2 = &anc_hybrid_eq_tbl2[tp][tbl_offset * channel];
        drc_tbl2 = &anc_hybrid_drc_tbl2[tp][tbl_offset/2 * channel];
        drc_eq_tbl2 = &anc_hybrid_drc_eq_tbl2[tp][tbl_offset/2 * channel];
#endif
    } else {
        u8 eq_num = 0;
        u8 drc_num = 0;
#if BT_TWS_EN
        if (anc_cfg.anc_mode == MODE_TWS_FFFB && xcfg_cb.bt_tws_en) {
            if (!sys_cb.tws_left_channel) {                     //TWS下FF/FB模式右声道取R的曲线给MICL
                eq_num = 2;
                drc_num = 1;
            }
        }
#endif

#if FUNC_AUX_EN && AUX_ANC_EN
        if ((func_cb.sta == FUNC_AUX) && (AUX_ANC_EN == 2)) {   //AUX模式下开启FB mode
            eq_num = 2;
            drc_num = 1;
        }
#endif
        anc_tbl = &anc_fffb_eq_tbl[4 * tp + eq_num];
        drc_tbl = &anc_fffb_drc_tbl[2 * tp + drc_num];
        drc_eq_tbl = &anc_fffb_drc_eq_tbl[2 * tp + drc_num];
#if ANC_EQ_RES2_EN
        anc_tbl2 = &anc_fffb_eq_tbl2[4 * tp + eq_num];
        drc_tbl2 = &anc_fffb_drc_tbl2[2 * tp + drc_num];
        drc_eq_tbl2 = &anc_fffb_drc_eq_tbl2[2 * tp + drc_num];
#endif
    }
    anc_set_tansparency_mode(tp);

    for (i = 0; i < 4; i++) {
        if (anc_cfg.ch[i] == NULL) {
            continue;
        }
        u32 *drc_addr, *drc_eq_addr;
#if ANC_EQ_RES2_EN
        bsp_anc_set_nos_param_by_res(i, *anc_tbl[2*i].addr, *anc_tbl[2*i].len, anc_tbl2[2*i].addr, anc_tbl2[2*i].len, tp);
        bsp_anc_set_msc_param_by_res(i, *anc_tbl[2*i+1].addr, *anc_tbl[2*i+1].len, anc_tbl2[2*i+1].addr, anc_tbl2[2*i+1].len);
        drc_addr = (u32 *)bsp_anc_get_drc_addr_by_res(*drc_tbl[i].addr, *drc_tbl[i].len, drc_tbl2[i].addr, drc_tbl2[i].len);
        drc_eq_addr = (u32 *)bsp_anc_get_drc_eq_addr_by_res(i, *drc_eq_tbl[i].addr, *drc_eq_tbl[i].len, drc_eq_tbl2[i].addr, drc_eq_tbl2[i].len);
#else
        bsp_anc_set_nos_param_by_res(i, *anc_tbl[2*i].addr, *anc_tbl[2*i].len, 0, 0, tp);
        bsp_anc_set_msc_param_by_res(i, *anc_tbl[2*i+1].addr, *anc_tbl[2*i+1].len, 0, 0);
        drc_addr = (u32 *)bsp_anc_get_drc_addr_by_res(*drc_tbl[i].addr, *drc_tbl[i].len, 0, 0);
        drc_eq_addr = (u32 *)bsp_anc_get_drc_eq_addr_by_res(i, *drc_eq_tbl[i].addr, *drc_eq_tbl[i].len, 0, 0);
#endif
        if (drc_addr != NULL) {
            anc_cfg.ch[i]->drc.drc_coef = drc_addr;
        }
        if (drc_eq_addr != NULL) {
            anc_cfg.ch[i]->drc.filter_coef = drc_eq_addr;
        }
    }
    anc_set_param();            //更新参数
}

//mode: 0[off], 1[anc], 2[tansparency]
void bsp_anc_set_mode(u8 mode)
{
    if (!xcfg_cb.anc_en) {
        return;
    }
#if  DAC_DNC_EN
    if (mode){           //anc生效时，不开启dnc检测
        dac_dnc_stop();
    }
#endif
    sys_cb.anc_user_mode = mode;
    if (mode == 0) {
        bsp_anc_stop();
        bsp_anc_exit();
    } else {
        bsp_anc_init();
        bsp_anc_set_param(mode - 1);
        bsp_anc_start();
    }
#if ANC_ALG_EN
    #if ASR_EN
    if (sys_cb.asr_enable) {
        bsp_asr_stop();
        sys_cb.asr_enable = 0;
    }
    #endif
    bsp_anc_set_mode_alg_do(mode);
#endif // ANC_ALG_EN
#if  DAC_DNC_EN
    if (!mode && !sco_is_connected()){              //关闭anc时，开启dac检测
        dac_dnc_start();
    }
#endif
    TRACE("anc user mode:%d\n", mode);
}

//无线/uart调anc参数接口
void bsp_anc_dbg_eq_param(u8 packet, u8 band_cnt, u32 *eq_buf)
{
    if (!xcfg_cb.anc_en) {
        return;
    }

    TRACE("anc dbg:%d %d\n", packet, band_cnt);
    if (packet > 8) {
        anc_set_tansparency_mode(1);
        packet -= 8;
    } else {
        anc_set_tansparency_mode(0);
    }
#if BT_TWS_EN
    if (xcfg_cb.anc_mode == MODE_TWS_FFFB && xcfg_cb.bt_tws_en) {   //TWS下FF/FB模式只调试MICL
        while (packet > 2) {
            packet -= 2;
        }
    }
#endif
    u8 eq_sel = packet = packet - 1;
    memcpy(anc_dbg_cache[packet], eq_buf, ((band_cnt * 5) + 1) * 4);

    u8 ch_get = 0;
    if (anc_cfg.anc_mode == MODE_HYBRID) {
        ch_get = eq_sel / 2;
    } else {
        if (eq_sel == 0 || eq_sel == 1 || eq_sel == 4 || eq_sel == 5) {             //L0_NOS.EQ, R0_NOS.EQ, L0_MSC.EQ, R0MSC.EQ
            ch_get = 0;
        } else if (eq_sel == 2 || eq_sel == 3 || eq_sel == 6 || eq_sel == 7) {      //L1_NOS.EQ, R1_NOS.EQ, L1_MSC.EQ, R1_MSC.EQ
            ch_get = 1;
        }
    }
    if (packet % 2) {
        bsp_anc_set_msc_param(ch_get, band_cnt, anc_dbg_cache[packet]);
    } else {
        bsp_anc_set_nos_param(ch_get, band_cnt, anc_dbg_cache[packet]);
    }
    anc_set_param();            //更新参数
}

void bsp_anc_fade_enable(u8 enable)
{
    anc_cfg.fade_en = enable;
}

void bsp_anc_init(void)
{
    int i, cnt = 0;
    if (!xcfg_cb.anc_en) {
        TRACE("'xcfg_cb.anc_en' define 0\n");
        return;
    }
    if (sys_cb.anc_init) {
        return;
    }

    //初始化参数
    for (i = 0; i < 4; i++) {
#if ANC_EQ_RES2_EN
        bsp_anc_set_nos_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len, anc_fffb_eq_tbl2[cnt].addr, anc_fffb_eq_tbl2[cnt].len, 0);
        cnt++;
        bsp_anc_set_msc_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len, anc_fffb_eq_tbl2[cnt].addr, anc_fffb_eq_tbl2[cnt].len);
        cnt++;
#else
        bsp_anc_set_nos_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len, 0, 0, 0);
        cnt++;
        bsp_anc_set_msc_param_by_res(i, *anc_fffb_eq_tbl[cnt].addr, *anc_fffb_eq_tbl[cnt].len, 0, 0);
        cnt++;
#endif
    }

#if FUNC_AUX_EN && AUX_ANC_EN
    if (func_cb.sta == FUNC_AUX) {
        anc_cfg.anc_mode = MODE_FFFB;
        u8 aux_anc_type = AUX_ANC_EN;
        if (aux_anc_type == 1) {                    //FF mode
            anc_cfg.ch[0]->mic_ch = MIC0;
            anc_cfg.ch[1]->mic_ch = MIC2;
            bsp_param_read((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC0_VAL, 1);
            bsp_param_read((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC2_VAL, 1);
            bsp_param_read((u8*)&sys_cb.adjust_val[4], PARAM_ANC_TP_MIC0_VAL, 1);
            bsp_param_read((u8*)&sys_cb.adjust_val[5], PARAM_ANC_TP_MIC2_VAL, 1);
        } else {                                    //FB mode
            anc_cfg.ch[0]->mic_ch = MIC1;
            anc_cfg.ch[1]->mic_ch = MIC3;
            bsp_param_read((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC1_VAL, 1);
            bsp_param_read((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC3_VAL, 1);
            bsp_param_read((u8*)&sys_cb.adjust_val[4], PARAM_ANC_TP_MIC1_VAL, 1);
            bsp_param_read((u8*)&sys_cb.adjust_val[5], PARAM_ANC_TP_MIC3_VAL, 1);
        }
        anc_cfg.ch[2] = NULL;
        anc_cfg.ch[3] = NULL;
    } else
#endif
    {
        anc_cfg.anc_mode = xcfg_cb.anc_mode;
        if (anc_cfg.anc_mode != MODE_HYBRID) {      //如果不是立体声HYBRID，就只使用2个adc
            anc_cfg.ch[2] = NULL;
            anc_cfg.ch[3] = NULL;
        } else {
            anc_cfg.ch[0]->mic_ch = MIC0;
            anc_cfg.ch[1]->mic_ch = MIC1;
            anc_cfg.ch[2]->mic_ch = MIC2;
            anc_cfg.ch[3]->mic_ch = MIC3;
        }
        if (anc_cfg.anc_mode == MODE_TWS_FFFB) {    //如果是TWS的FFFB，只保留1个adc
            anc_cfg.ch[1] = NULL;
        }
        bsp_param_read((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC0_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC1_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[2], PARAM_ANC_MIC2_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[3], PARAM_ANC_MIC3_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[4], PARAM_ANC_TP_MIC0_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[5], PARAM_ANC_TP_MIC1_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[6], PARAM_ANC_TP_MIC2_VAL, 1);
        bsp_param_read((u8*)&sys_cb.adjust_val[7], PARAM_ANC_TP_MIC3_VAL, 1);
}
    anc_init(&anc_cfg);
    sys_cb.anc_init = 1;
    TRACE("anc init\n");
    TRACE("anc mode:%d\n", anc_cfg.anc_mode);
    TRACE("anc adjust mic gain:%d %d\n", sys_cb.adjust_val[0], sys_cb.adjust_val[1]);
#if TRACE_EN
    for (u8 i=0; i<8; i++) {
        TRACE("adjust_val[%d]: %d\n", i, sys_cb.adjust_val[i]);
    }
#endif
}

void bsp_anc_exit(void)
{
    anc_exit();
    sys_cb.anc_init = 0;
    sys_cb.anc_start = 0;
}

#if ANC_MAX_VOL_DIS_FB_EN
u8 bsp_anc_max_vol_dis_fb_get_sta(void)
{
    return anc_max_vol_dac_vol_sta;
}

void bsp_anc_max_vol_set_vol(u8 max_vol)
{
    s16 fb_gain = 0x7FFF;
#if ANC_ALG_EN
    fb_gain = anc_alg_get_gain(1);
#endif // ANC_ALG_EN

    if (max_vol) {
        fb_gain = 0x0000;
    }

    anc_vol_set(1, (u16)fb_gain, 0, 0, 12);
//    anc_vol_set(3, (u16)fb_gain, 0, 0, 12);

    anc_max_vol_dac_vol_sta = max_vol;

    TRACE("bsp_anc_max_vol_set_vol %d\n", max_vol);
}

AT(.com_text.anc.max_vol_dis_fb.dac_det)
u8 bsp_anc_max_vol_dac_det_process(u32 pow)
{
    u8 change = 0;
    u8 sta = anc_max_vol_dac_vol_sta;

    if (DAC_24BITS_EN) {
        pow = pow >> 8;
    }

//    printf("pow 0x%x\n", pow);

    if (anc_max_vol_dac_vol_sta) {
        if (pow < ANC_MAX_VOL_DIS_FB_DAC_THR) {
            change = 1;
        }
    } else {
        if (pow >= ANC_MAX_VOL_DIS_FB_DAC_THR) {
            change = 1;
        }
    }

    if (change) {
        anc_max_vol_change_cnt++;
    } else {
        anc_max_vol_change_cnt = 0;
    }

    if (anc_max_vol_change_cnt >= 25) {
        sta = (anc_max_vol_dac_vol_sta ? 0 : 1);
    }

    return sta;
}

AT(.com_text.anc.max_vol_dis_fb)
void bsp_anc_max_vol_dis_fb_process(void)
{
    u8 dac_vol_sta = 0;

    if ((anc_max_vol_process_sta) && (sys_cb.anc_user_mode) && (sys_cb.vol >= ANC_MAX_VOL_DIS_FB_VOL_THR)) {
        if (!tick_check_expire(anc_max_vol_dis_fb_tick, 20)) {
            return;
        }
        anc_max_vol_dis_fb_tick = tick_get();

        u32 pow = dac_pcm_pow_calc();
        dac_vol_sta = bsp_anc_max_vol_dac_det_process(pow);
    }

    if (dac_vol_sta != anc_max_vol_dac_vol_sta) {
        bsp_anc_max_vol_set_vol(dac_vol_sta);
        anc_max_vol_change_cnt = 0;
    }
}

void bsp_anc_max_vol_dac_det_start(void)
{
    anc_max_vol_process_sta = 1;
    anc_max_vol_dac_vol_sta = 0;
    anc_max_vol_change_cnt = 0;
}

void bsp_anc_max_vol_dac_det_stop(void)
{
    anc_max_vol_process_sta = 0;
    anc_max_vol_dac_vol_sta = 0;
    anc_max_vol_change_cnt = 0;
}
#endif // ANC_MAX_VOL_DIS_FB_EN

#endif // ANC_EN

#if TINY_TRANSPARENCY_EN
void bsp_ttp_start(void)
{
    if (sys_cb.ttp_start) return;

    u32 ttp_cfg = 0;
#ifdef RES_BUF_EQ_MIC_TTP_EQ
    if (mic_set_eq_by_res(RES_BUF_EQ_MIC_TTP_EQ, RES_LEN_EQ_MIC_TTP_EQ)) {
        mic_set_post_gain(mic_gain_tbl[0]);
        ttp_cfg |= BIT(0);
    }
#endif
    ttp_init(ttp_cfg);

    audio_path_init(AUDIO_PATH_TTP);
    audio_path_start(AUDIO_PATH_TTP);
    sys_cb.ttp_start = 1;
}

void bsp_ttp_stop(void)
{
    if (!sys_cb.ttp_start) return;
    sys_cb.ttp_start = 0;
    audio_path_exit(AUDIO_PATH_TTP);
}

//mode: 0[off], 1[off], 2[tansparency]
void bsp_anc_set_mode(u8 mode)
{
    sys_cb.anc_user_mode = mode;
    if (sys_cb.anc_user_mode == 2) {
        bsp_ttp_start();
    } else {
        bsp_ttp_stop();
    }
    TRACE("ttp user mode:%d\n", mode);
}

void bsp_ttp_init(void)
{

}

void bsp_ttp_exit(void)
{
    ttp_exit();
}
#endif

#if ANC_ALG_AEM_RT_FF_FB_EN
void bsp_anc_aem_set_fb_param(void)
{
    u8 channel = 0;
#if BT_TWS_EN
    if (xcfg_cb.bt_tws_en) {
        channel = !sys_cb.tws_left_channel;
    }
#endif
    bsp_anc_set_nos_param_by_res(1, *anc_aem_fb_eq_tbl[2*channel+0].addr, *anc_aem_fb_eq_tbl[2*channel+0].len, 0, 0, 0);
    bsp_anc_set_msc_param_by_res(1, *anc_aem_fb_eq_tbl[2*channel+1].addr, *anc_aem_fb_eq_tbl[2*channel+1].len, 0, 0);
    anc_set_param();
}

void bsp_anc_aem_set_ff_param(u8 idx_num, bool change_en, bool cvt_flag)
{
    u8 channel = 0;
#if BT_TWS_EN
    if (xcfg_cb.bt_tws_en) {
        channel = !sys_cb.tws_left_channel;
    }
#endif
    idx_num -= 1;
    bsp_anc_set_nos_param_by_res(0, *anc_aem_ff_eq_tbl[2*idx_num+channel].addr, *anc_aem_ff_eq_tbl[2*idx_num+channel].len, 0, 0, 0);
    if (!cvt_flag) {
        anc_cfg.change_en = change_en;
        anc_set_param();
        anc_cfg.change_en = 0;
    }
}

void bsp_anc_aem_set_ff_cvt(s16 ff_gain)
{
    anc_cfg.ch[0]->nos_gain = ((s64)((s32)anc_cfg.ch[0]->nos_gain)*anc_pow10_cal(ff_gain)) >> 23;

    anc_cfg.change_en = 1;
    anc_set_param();
    anc_cfg.change_en = 0;
}
#endif

