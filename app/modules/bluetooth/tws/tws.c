#include "include.h"
#include "api.h"
#include "app_include.h"
#if BT_TWS_EN

extern const char *bt_get_local_name(void);

#define BT_TWS_FEATS        ((TWS_FEAT_MS_SWITCH*BT_TWS_MS_SWITCH_EN) | \
                             ((TWS_FEAT_TSCO|TWS_FEAT_TSCO_RING)*BT_TWS_SCO_EN) | \
                             (TWS_FEAT_CODEC_AAC*BT_A2DP_AAC_AUDIO_EN) | \
                             (TWS_FEAT_CODEC_MSBC*BT_HFP_MSBC_EN) | \
                             (TWS_FEAT_CODEC_HIRES*(BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN)) )
//bit[7:6]: pair mode
//0: 使用蓝牙名称配对
//1: 使用蓝牙ID配对
//bit[5]: MS bonding
//0: 主从不组队绑定
//1: 主从组队绑定
//bit[3:0]: pair operation
//0: 使用内置自动配对，上电先回连TWS，再回连手机
//1: 使用api操作，不自动配对，上电不回连
//2: 使用按键调用内置配对，上电先回连TWS，再回连手机
#define TWS_PAIR_MODE       ((BT_TWS_PAIR_MODE<<6))


uint8_t  cfg_bt_tws_pair_mode   = TWS_PAIR_MODE;
uint16_t cfg_bt_tws_feat        = BT_TWS_FEATS;
#if SWETZ
uint8_t  cfg_bt_nor_connect_times           = NUM_BT_RETURN_CON;    //设置上电回连手机次数, 1.28s * n, 不小于5次  
uint8_t  cfg_bt_tws_search_slave_times      = NUM_TWS_RETURN_CON;    //设置TWS搜索次数, 1.25s * n, 不小于2次      
#else
uint8_t  cfg_bt_nor_connect_times           = 10;    //设置上电回连手机次数, 1.28s * n, 不小于5次
uint8_t  cfg_bt_tws_search_slave_times      = 3;    //设置TWS搜索次数, 1.25s * n, 不小于2次
#endif


uint8_t  cfg_bt_tws_connect_times           = 4;    //设置上电回连TWS次数, 1.28s * n, 不小于4次
uint16_t cfg_bt_tws_sup_to_connect_times    = 100;  //设置TWS断线回连次数, (1.28s + 1) * n, 设置(-1)为一直回连
uint8_t cfg_bt_tws_limit_pair_time          = 0;    //设置TWS上电、出仓配对限制时间, 单位80ms, 0为不限制（L、R规定时间内上电、出仓才能配对）
uint8_t  cfg_bt_tws_tick_freq = 1;


//pair_mode选择ID配对时，用该函数获取ID
uint32_t bt_tws_get_pair_id(void)
{
    return BT_TWS_PAIR_ID;
}

bool bt_tws_name_suffix_replace(char *buf)
{
    if (sys_cb.name_suffix_en) {
        if(buf[0] == 'L') {
            buf[0] = 'R';
            return true;
        } else if(buf[0] == 'R') {
            buf[0] = 'L';
            return true;
        }
    }
    return false;
}

void bt_tws_set_discon(uint8_t reason)
{
    if(reason != 0 && reason != 0xff) {
        sys_cb.pwrdwn_tone_en = 1;
        sys_cb.discon_reason = 1;   //同步关机
        func_cb.sta = FUNC_PWROFF;
    }
}

//tws搜索配对时，获取channel避免左左（或右右）配对
bool bt_tws_get_channel_cfg(uint8_t *channel)
{
    if(xcfg_cb.bt_tws_lr_mode == 9) {
        *channel = sys_cb.tws_left_channel;     //GPIOx有接地为左声道
        return true;
    } else if(10 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为左声道
        *channel = 1;
        return true;
    } else if(11 == xcfg_cb.bt_tws_lr_mode) {   //配置选择为右声道
        *channel = 0;
        return true;
    }

    return false;
}

bool bt_tws_get_channel(uint8_t *channel)
{
    if (bt_tws_get_channel_cfg(channel)) {
        return true;
    } else if (sys_cb.vusb_force_channel) {
        *channel = sys_cb.vusb_force_channel - 1;
        return true;
    }

    return false;
}

#if (CHARGE_BOX_TYPE == CBOX_SSW) || QTEST_EN
void bt_tws_channel_read(void)
{
    u8 channel;
    if (!bt_tws_get_channel_cfg(&channel)) {
        param_tws_channel_read();
    }
}

u8 bt_tws_get_tws_channel(void)
{
    uint8_t channel;
    if (!bt_tws_get_channel_cfg(&channel)) {
        return NO_DISTRIBUTION;
    }
    if (channel) {
        return LEFT_CHANNEL;
    }
    return RIGHT_CHANNEL;
}

u8 bt_tws_get_tws_role(void)
{
    u8 ms_role = 0;
   if (sys_cb.tws_force_channel == 1) {//1 固定为左声道
        if(xcfg_cb.bt_tws_lr_mode == 2) {
            ms_role = 1;
        }
    } else if (sys_cb.tws_force_channel == 2) {
         if(xcfg_cb.bt_tws_lr_mode != 2) {
            ms_role = 1;
         }
    }
    return ms_role;
}

bool bt_tws_channel_check(u8 channel)
{
    u8 tws_channel = bt_tws_get_tws_channel();
    if (tws_channel != NO_DISTRIBUTION) {
        if (tws_channel == LEFT_CHANNEL) {
            tws_channel = LEFT_CHANNEL_USER;
        } else {
            tws_channel = RIGHT_CHANNEL_USER;
        }
#if (CHARGE_BOX_TYPE == CBOX_SSW)
        if ((channel != tws_channel)&&(!qtest_get_mode())) {
            return false;
        }
#endif
    } else if ((channel == LEFT_CHANNEL_USER) || (channel == RIGHT_CHANNEL_USER)) {
        if (channel == RIGHT_CHANNEL_USER) {
            sys_cb.tws_force_channel = 1;
        } else {
            sys_cb.tws_force_channel = 2;
        }
        param_tws_channel_write(sys_cb.tws_force_channel);         //自动分配的方案通过电池仓自动固定左右声道
    }else if ((channel == NOTFIX_LCHANEL_USER) || (channel == NOTFIX_RCHANEL_USER)) {
        if (channel == NOTFIX_RCHANEL_USER) {
            sys_cb.tws_force_channel = 1;
        } else {
            sys_cb.tws_force_channel = 2;
        }
    }
    return true;
}

#endif

#if USER_INEAR_DETECT_EN
void bt_update_ear_sta(void)
{
    bt_tws_sync_setting();          //通知对方
}

//api, 设置耳机状态
void bt_set_ear_sta(bool ear_sta)
{
    sys_cb.loc_ear_sta = ear_sta;       //0=戴入，1=取下
    bt_update_ear_sta();
}
#endif // USER_INEAR_DETECT_EN

//tws一些同步信息，例如EQ, 语言等，param最大是12byte
bool bt_tws_get_info(uint8_t *param)
{
    u8 offset = 0;
#if LANG_SELECT == LANG_EN_ZH
    param[offset] = sys_cb.lang_id & 0x0f;  //bit0~3
#endif
#if EQ_MODE_EN
    param[offset] |= (u8)sys_cb.eq_mode<<4;
#endif
    offset++;
    param[offset++] = sys_cb.hfp_vol;      //同步初始通话音量
#if USER_INEAR_DETECT_EN
    param[offset++] |= sys_cb.loc_ear_sta; //同步入耳状态
#endif // USER_INEAR_DETECT_EN
#if ABP_EN
    param[offset++] = sys_cb.abp_mode;
#endif // ABP_EN
#if ASR_EN
    param[offset++] = sys_cb.asr_enable;
#endif
#if ANC_ALG_DYVOL_FF_EN
    param[offset++] = sys_cb.dyvol_local;
    param[offset++] = sys_cb.dyvol_real;
    param[offset++] = sys_cb.dyvol_overlap;
#endif // ANC_ALG_DYVOL_FF_EN
    return true;
}

void bt_tws_set_info(uint8_t *param)
{
    uint8_t flag = 0,tmp = 0, offset = 0;
#if LANG_SELECT == LANG_EN_ZH
    tmp = (param[offset] & 0x0f);
    if(xcfg_cb.lang_id >= 2 && tmp < 2 && tmp != sys_cb.lang_id) {  //bit0~3
        sys_cb.lang_id = tmp;
        msg_enqueue(EVT_BT_SET_LANG_ID);
    }
#endif
#if EQ_MODE_EN
    tmp = (param[offset] & 0xf0)>>4;
    if(tmp < 6 && tmp != sys_cb.eq_mode) {  //bit4~7
        sys_cb.eq_mode = tmp;
        msg_enqueue(EVT_BT_SET_EQ);
    }
#endif
    offset++;
    sys_cb.hfp_vol = param[offset++];      //同步初始通话音量
#if USER_INEAR_DETECT_EN
    tmp = param[offset++];
    if(tmp != sys_cb.rem_ear_sta) {
        sys_cb.rem_ear_sta = tmp;	//同步入耳状态
    }
#endif // USER_INEAR_DETECT_EN
#if ABP_EN
    tmp = param[offset++];
    if (tmp != sys_cb.abp_mode) {
        sys_cb.abp_mode = tmp;
        bt_tws_req_alarm_abp(sys_cb.abp_mode);
    }
#endif // ABP_EN
#if ASR_EN
    sys_cb.asr_enable = param[offset++];
#endif
#if ANC_ALG_DYVOL_FF_EN
    sys_cb.dyvol_remote = param[offset++];
    if (!bt_tws_is_slave()) {
        bsp_anc_alg_dyvol_set_vol();
    } else {
        sys_cb.dyvol_real = param[offset++];
        sys_cb.dyvol_overlap = param[offset++];
#if ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
        bsp_anc_alg_dyvol_set_local_gain();
#endif // ANC_ALG_DYVOL_FF_LOCAL_VOL_EN
    }
#endif // ANC_ALG_DYVOL_FF_EN
    tmp = flag;      //避免编译警告
}
//tws同步用户自定义的一些数据,通过 bt_tws_sync_custom_data() 进行回调
uint16_t tws_get_custom_data(uint8_t *buf)
{
    uint16_t len = 0;
#if TWS_LR
    len = app_lr_tws_get_data(buf);
    if (len)
    {
        bt_tws_sync_custom_data();
        return len;
    }
#endif     
#if GFPS_EN
    if(len == 0){
        len = gfps_tws_get_data(buf);
    }
#endif
    if(len){
        bt_tws_sync_custom_data();
    }

    return len;
}

uint16_t tws_set_custom_data(uint8_t *data_ptr, uint16_t size)
{
    //tag:data_ptr[0]
#if TWS_LR
    sys_cb.tick_event = tick_get();
    if (app_lr_tws_set_data(data_ptr, size))
    {
        return 0;
    }
#endif     
#if GFPS_EN
    if(data_ptr[0] == TWS_SYNC_TAG_GFP){
        gfps_tws_set_data(&data_ptr[1], size - 1);
    }
#endif

    return 0;
}

void bt_tws_role_change(u32 param)
{
//    u8 new_role = param;
//    u8 adv_en = (param>>8);
//    if(new_role == 0 && adv_en) {
//    } else {
//    }
}

void tws_user_key_process(uint32_t *opcode)
{
//*opcode
    app_tws_user_key_process(opcode);
}

#if BT_TWS_PUBLIC_ADDR_EN
void bt_tws_get_public_addr(uint8_t *remote_addr, uint8_t *rand_key)
{
    uint8_t local_addr[6];
#if LE_DUEROS_DMA_EN
    uint8_t channel = 0; //1:左 、 0：右
    bt_tws_get_channel(&channel);
    if (channel == 1) {
        memcpy(remote_addr, xcfg_cb.bt_addr, 6);
    }
    return;
#endif
#if IODM_TEST_EN
    if(!bt_get_qr_addr(local_addr))
#endif
    {
        bt_get_local_bd_addr(local_addr);
    }

    u8 header_flag = 1;
    for (int i = 0; i < 6; i++) {
        remote_addr[i] ^= local_addr[i];
        if (remote_addr[i] == 0 && header_flag) {
            remote_addr[i] = local_addr[i];
        } else {
            header_flag = 0;
            remote_addr[i] ^= rand_key[i];
//            printf("    0x%02x, //%d\n", rand_key[i], i);
        }
    }
}
#endif

#if BT_TWS_DBG_EN
u8 tws_dbg_ind[24] = {
    'T', 'W', 'S',      //SPP-TAG
    0x14,               //[0]   len
    0xff,               //[1]   data_type
    0x42, 0x06,         //[2:3] comany_id
    0x00, 0x00, 0x00,   //[4:6] reserved
    0x00,               //[7]   headset_type
    0xff,               //[8]   rssi_left_tws
    0xfe,               //[9]   rssi_right_tws
    0x01,               //[10]  rssi_left_phone
    0x02,               //[11]  rssi_right_phone
    0xff,               //[12]  per_left_tws
    0xff,               //[13]  per_right_tws
    0xff,               //[14]  per_left_phone
    0xff,               //[15]  per_right_phone
    0x00,               //[16]  battery_left
    0x00,               //[17]  battery_right
    0x00,               //[18]  battery_house
    0x00,               //[19]  unknow
    0x00,               //[20]  color
};

void bt_tws_report_dgb(void)
{
    u8 *ind = tws_dbg_ind + 3;
    s8 *rssi = (s8 *)(ind + 8);

    ind[19] = sys_cb.tws_left_channel;   //0=right, 1=left
    if(bt_tws_get_link_rssi(rssi, sys_cb.tws_left_channel)) {
        bt_spp_tx(SPP_SERVICE_CH0, tws_dbg_ind, 24);
//        ble_set_adv_data(tws_dbg_ind+3, 21);
//        ble_adv_en();
    }
}
#endif // BT_TWS_DBG_EN
#endif // BT_TWS_EN
