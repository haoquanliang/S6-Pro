#include "dueros_dma_app.h"
#include "dueros_dma_profile.h"
#include "dueros_dma_extern.h"

#if LE_DUEROS_DMA_EN

#define DUEROS_DMA_DEBUG_EN       1

#if DUEROS_DMA_DEBUG_EN
#define DUEROS_DMA_DEBUG(...)                  printf(__VA_ARGS__)
#define DUEROS_DMA_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define DUEROS_DMA_DEBUG(...)
#define DUEROS_DMA_DEBUG_R(...)
#endif

uint8_t opus_buf[DUEROS_DMA_OPUS_DATA_LEN] AT(.opus.buf.upload);
dueros_dma_app_data_t dueros_dma_app  AT(.buf.dueros_dma);

bool cfg_bt_tws_ble_switch_en = false;    //库里调用，dueros dma 不支持tws ble主从切换

static DUER_DMA_CAPABILITY duer_dma_capability = {
    .device_type = DEVICE_TYPE,
    .manufacturer = MANUFACTURER,
    .support_spp = SUPPORT_SPP,
    .support_a2dp = SUPPORT_A2DP,
    .support_at_command = SUPPORT_AT_COMMAND,
    .support_media = SUPPORT_MEDIA,
    .support_sota = SUPPORT_SOTA,
    .is_earphone = IS_EARPHONE,
    .support_dual_record = SUPPORT_DUAL_RECORD,
    .support_local_voice_wake = SUPPORT_LOCAL_VOICE_WAKE,
    .support_model_beamforming_asr = SUPPORT_MODEL_BEAMFORMING_ASR,
    .support_log = SUPPORT_LOG,
    .support_box_battery = SUPPORT_BOX_BATTERY,
    .ble_role_switch_by_mobile = BLE_ROLE_SWITCH_BY_MOBILE,
    .support_tap_wakeup = SUPPORT_TAP_WAKEUP,
    .support_sign_method = SUPPORT_SIGN_METHOD,
};

//库里调用, 主从切换, 主机同步信息
uint16_t role_switch_get_user_data(uint8_t *data_ptr)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    uint16_t offset = 0;
    dueros_dma_app.role_switch_flag = 1;
    data_ptr[offset++] = dueros_dma_con_type_get();
    data_ptr[offset++] = dueros_dma_client_cfg_get();
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_ROLE_SWITCH_START, NULL, 0, 1);
    return offset;
}

//库里调用, 主从切换, 从机获取信息
uint16_t role_switch_set_user_data(uint8_t *data_ptr, uint16_t len)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    uint16_t offset = 0;
    dueros_dma_app.role_switch_flag = 1;
    dueros_dma_con_type_set(data_ptr[offset++]);
    dueros_dma_client_cfg_set(data_ptr[offset++]);
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_ROLE_SWITCH_START, NULL, 0, 1);
    return offset;
}

////库里调用, 设置左耳地址作为master地址
//void bt_tws_get_public_addr(uint8_t *remote_addr, uint8_t *rand_key)
//{
//    uint8_t channel = 0; //1:左 、 0：右
//    bt_tws_get_channel(&channel);
//    if (channel == 1) {
//        memcpy(remote_addr, xcfg_cb.bt_addr, 6);
//    }
//    DUEROS_DMA_DEBUG("[DMA_APP]:%s, channel:%d\n", __func__, channel);
//    DUEROS_DMA_DEBUG_R(remote_addr, 6);
//}

//库里调用, 唤醒小度语音
void dma_start_speech_to_ai(void)
{
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_KEYWORD_DETECTED, NULL, 0, 0);
}

//库里调用, 停止小度语音
void dma_stop_speech_to_ai(void)
{

}

//库里调用, 同步按键信息设置
void tws_send_get_dma_key_msg(uint8_t *buffer)
{
    buffer[0] = 2; //len
    buffer[1] = dueros_dma_app.key_channel;
    buffer[2] = dueros_dma_app.key_click;
}

//库里调用, 同步按键信息接收
void tws_remote_set_dma_key_msg(uint8_t len, uint8_t *buff)
{
    dueros_dma_app.key_channel = buff[0];
    dueros_dma_app.key_click = buff[1];
    dueros_dma_app.do_flag |= DUEROS_DMA_FLAG_KEY_MSG_CTL;
}

//key_click: DMA_NOTIFY_STATE_DOUBLE_CLICK 或 DMA_NOTIFY_STATE_TRIPLE_CLICK
void dueros_dma_key_msg_send(uint8_t key_click)
{
    bt_tws_get_channel(&dueros_dma_app.key_channel); //1:左 、 0：右
    dueros_dma_app.key_click = key_click;
    if (bt_tws_is_slave()) {
        dueros_dma_key_msg_ctl();
    } else {
        dueros_dma_app.do_flag |= DUEROS_DMA_FLAG_KEY_MSG_CTL;
    }
}

bool dueros_dma_prepare_state_get(void)
{
    if (dueros_dma_client_cfg_get() && (dueros_dma_con_type_get() == DUEROS_DMA_CON_BLE)) {
        return true;
    } else if (dueros_dma_con_type_get() == DUEROS_DMA_CON_SPP){
        return true;
    } else {
        return false;
    }
}

void dueros_dma_ntf_sta_enqueue(uint8_t sta, uint8_t *param_buf, uint8_t param_size, bool priority)
{
    if (priority) {
        dueros_dma_wrap_notify_state(sta, param_buf, param_size);
    } else {
        uint8_t wptr = dueros_dma_app.ntf_sta.wptr & DUEROS_DMA_NTF_STA_MSG_MASK;
        dueros_dma_app.ntf_sta.state_buf[wptr].state = sta;
        dueros_dma_app.ntf_sta.state_buf[wptr].param_size = param_size;
        memcpy(dueros_dma_app.ntf_sta.state_buf[wptr].param_buf, param_buf, param_size);
        dueros_dma_app.ntf_sta.wptr++;
        dueros_dma_app.ntf_sta.num++;
        DUEROS_DMA_DEBUG("[DMA_APP]:%s, wptr:%d, rptr:%d, sta:%d\n", __func__, dueros_dma_app.ntf_sta.wptr, dueros_dma_app.ntf_sta.rptr, sta);
    }
}

bool dueros_dma_ntf_sta_dequeue(uint8_t *sta, uint8_t *param_buf, uint8_t *param_size)
{
    if (dueros_dma_app.ntf_sta.num == 0) {
        // DUEROS_DMA_DEBUG("The number of notification status is empty!\n");
        return false;
    }
    uint8_t rptr = dueros_dma_app.ntf_sta.rptr & DUEROS_DMA_NTF_STA_MSG_MASK;
    *sta = dueros_dma_app.ntf_sta.state_buf[rptr].state;
    *param_size = dueros_dma_app.ntf_sta.state_buf[rptr].param_size;
    memcpy(param_buf, dueros_dma_app.ntf_sta.state_buf[rptr].param_buf, *param_size);
    dueros_dma_app.ntf_sta.rptr++;
    dueros_dma_app.ntf_sta.num--;
    DUEROS_DMA_DEBUG("[DMA_APP]:%s, wptr:%d, rptr:%d, sta:%d\n", __func__, dueros_dma_app.ntf_sta.wptr, dueros_dma_app.ntf_sta.rptr, *sta);
    return true;
}

uint16_t dueros_dma_notify_state_number_get(void)
{
    return dueros_dma_app.ntf_sta.num;
}

void dueros_dma_hfp_at_cmd_set(char *cmd, uint8_t len)
{
    memset(dueros_dma_app.hfp_atcmd, 0, DUEROS_DMA_HFP_AT_CMD_LEN);
    strncpy(dueros_dma_app.hfp_atcmd, cmd, len);
    bt_ctrl_msg(BT_CTL_HFP_AT_CMD);
}

//此函数返回需要发送的ATCMD, 库里调用
char * hfp_get_at_cmd(void)
{
    return dueros_dma_app.hfp_atcmd; //例如，通话过程发起号码键"1"
}

bool dueros_dma_role_switch_flag_get(void)
{
    return dueros_dma_app.role_switch_flag;
}

void dueros_dma_device_type_set(uint8_t dev_type)
{
    dueros_dma_app.dev_info.dev_type = dev_type;
}

uint8_t dueros_dma_device_type_get(void)
{
    return dueros_dma_app.dev_info.dev_type;
}

void dueros_dma_stream_upload_flag_set(bool flag)
{
    dueros_dma_app.stream_upload_flag = flag;
}

uint8_t dueros_dma_stream_upload_flag_get(void)
{
    return dueros_dma_app.stream_upload_flag;
}

void dueros_dma_device_version_set(uint8_t dev_version)
{
    dueros_dma_app.dev_info.dev_version = dev_version;
}

uint8_t dueros_dma_device_version_get(void)
{
    return dueros_dma_app.dev_info.dev_version;
}

void dueros_dma_triad_id_set(char *triad_id, uint8_t from)
{
    uint8_t channel = 0; //1:左 、 0：右
    char triad_id_t[25] = {0};
    bt_tws_get_channel(&channel);
    cm_read(triad_id_t, DUEROS_DMA_PARAM_TRIAD_ID, sizeof(triad_id_t));

    //右耳使用左耳的triad id
    if (((channel == 1) && (from == DUEROS_DMA_TRIAD_FROM_INIT)) || \
        ((channel == 0) && (from == DUEROS_DMA_TRIAD_FROM_LEFT_EAR))) {
        int16_t res = strcmp((char *)triad_id_t, triad_id);
        if (res) {
            cm_write(triad_id, DUEROS_DMA_PARAM_TRIAD_ID, sizeof(triad_id_t));
        }
        strcpy(dueros_dma_app.dev_info.triad_id, triad_id);
    }
    DUEROS_DMA_DEBUG("[DMA_APP]%s, channel:%d, triad_id_t:%s, triad_id:%s\n", __func__, channel, triad_id_t, triad_id);
}

char *dueros_dma_triad_id_get(void)
{
    cm_read(dueros_dma_app.dev_info.triad_id,   DUEROS_DMA_PARAM_TRIAD_ID, 25);
    DUEROS_DMA_DEBUG("[DMA_APP]%s, triad_id:%s\n", __func__, (char *)dueros_dma_app.dev_info.triad_id);
    return (char *)dueros_dma_app.dev_info.triad_id;
}

void dueros_dma_triad_secret_set(char *triad_secret, uint8_t from)
{
    uint8_t channel = 0; //1:左 、 0：右
    char triad_secret_t[17] = {0};
    bt_tws_get_channel(&channel);
    cm_read(triad_secret_t, DUEROS_DMA_PARAM_TRIAD_SECRET, sizeof(triad_secret_t));

    //右耳使用左耳的triad secret
    if (((channel == 1) && (from == DUEROS_DMA_TRIAD_FROM_INIT)) || \
        ((channel == 0) && (from == DUEROS_DMA_TRIAD_FROM_LEFT_EAR))) {
        int16_t res = strcmp((char *)triad_secret_t, triad_secret);
        if (res) {
            cm_write(triad_secret, DUEROS_DMA_PARAM_TRIAD_SECRET, sizeof(triad_secret_t));
        }
        strcpy(dueros_dma_app.dev_info.triad_secret, triad_secret);
    }
    DUEROS_DMA_DEBUG("[DMA_APP]%s, channel:%d, triad_secret_t:%s, triad_secret:%s\n", __func__, channel, triad_secret_t, triad_secret);
}

char *dueros_dma_triad_secret_get(void)
{
    cm_read(dueros_dma_app.dev_info.triad_secret, DUEROS_DMA_PARAM_TRIAD_SECRET, 17);
    DUEROS_DMA_DEBUG("[DMA_APP]%s, triad_secret:%s\n", __func__, (char *)dueros_dma_app.dev_info.triad_secret);
    return (char *)dueros_dma_app.dev_info.triad_secret;
}

void dueros_dma_con_type_set(uint8_t con_type)
{
    dueros_dma_app.con_type = con_type;
}
uint8_t dueros_dma_con_type_get(void)
{
    return dueros_dma_app.con_type;
}

void dueros_dma_client_cfg_set(uint16_t cfg)
{
    dueros_dma_app.client_cfg = cfg;
}

uint16_t dueros_dma_client_cfg_get(void)
{
    return dueros_dma_app.client_cfg;
}

void dueros_dma_wakeup_set(bool wk)
{
    dueros_dma_app.wakeup = wk;
}

bool dueros_dma_app_is_need_wakeup(void)
{
    return dueros_dma_app.wakeup;
}

void dueros_dma_do_flag_set(uint32_t flag)
{
    dueros_dma_app.do_flag |= flag;
}

void dueros_dma_do_flag_clr(uint32_t flag)
{
    dueros_dma_app.do_flag &= ~flag;
}

uint32_t dueros_dma_do_flag_get(void)
{
    return dueros_dma_app.do_flag;
}

bool dueros_dma_opus_enc_flag_get(void)
{
    return dueros_dma_app.opus_enc_flag;
}

void dueros_dma_dev_info_init(void)
{
    DUEROS_DMA_DEBUG("[DMA_APP]%s\n", __func__);
    dueros_dma_device_type_set(DUEROS_DMA_DEVICE_TYPE);
    dueros_dma_device_version_set(DUEROS_DMA_DEVICE_VERSION);
    dueros_dma_triad_id_set(xcfg_cb.dma_triad_id, DUEROS_DMA_TRIAD_FROM_INIT);   //需在xcfg.xm添加“小度AI配置”，请查阅dueros_dma_app.h第一行的readme
    dueros_dma_triad_secret_set(xcfg_cb.dma_secret, DUEROS_DMA_TRIAD_FROM_INIT); //需在xcfg.xm添加“小度AI配置”，请查阅dueros_dma_app.h第一行的readme
}

void dueros_dma_send_packet(uint8_t *packet, uint32_t len)
{
    // DUEROS_DMA_DEBUG("[<---]%s, packet_len:%d\n", __func__, len);
    // DUEROS_DMA_DEBUG_R(packet, len);
    bool finish_flag = 0;
    uint16_t send_len = 0;
    static uint16_t offset = 0;
    uint32_t mtu = 0;
    int32_t res = -1;
    uint8_t con_type = dueros_dma_con_type_get();
    dueros_dma_wrap_get_mobile_mtu(&mtu);

send_data_again:
    if ((len-offset) > mtu) {
        send_len = mtu;
        finish_flag = false;
    } else {
        send_len = len - offset;
        finish_flag = true;
    }

    if (con_type == DUEROS_DMA_CON_SPP) {
        res = bt_spp_tx(SPP_SERVICE_CH1, packet+offset, send_len);
    } else if (con_type == DUEROS_DMA_CON_BLE) {
        res = ble_send_packet(packet+offset, send_len);
    } else {
        offset = 0;
        DUEROS_DMA_DEBUG("!!!dueros dma data send err!!!\n");
        return;
    }

    DUEROS_DMA_DEBUG("[---->] con_type:%d, mtu:%d, len:%d, offset:%d, send_len:%d, res:%d\n", con_type, mtu, len, offset, send_len, res);

    if (finish_flag == false) {
        offset +=  mtu;
        goto send_data_again;
    } else {
        offset = 0;
        // DUEROS_DMA_DEBUG("---dueros dma data send success---\n");
    }
}

bool dueros_dma_app_get_device_capability(void* device_capability)
{
    uint8_t *res = memcpy(device_capability, &duer_dma_capability, sizeof(DUER_DMA_CAPABILITY));
    bool ret = (res != NULL) ? true : false;
    return ret;
}

void dueros_dma_spp_disconnect_callback(void)
{
    if (dueros_dma_app.role_switch_flag == 0) {
        dueros_dma_con_type_set(DUEROS_DMA_CON_NONE);
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_DISCONNECTED, NULL, 0, 0);
    }
}

void dueros_dma_spp_connect_callback(void)
{
    if (dueros_dma_app.role_switch_flag == 0) {
        dueros_dma_con_type_set(DUEROS_DMA_CON_SPP);
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_CONNECTED, NULL, 0, 1);
        dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_CONNECTED, NULL, 0, 0);
    }
}

bool dueros_dma_recv_proc(uint8_t *data,uint8_t len, uint8_t con_type)
{
    DUEROS_DMA_DEBUG("[<----]dueros_dma_recv_proc, con_type:%d\n", con_type);
    DUEROS_DMA_DEBUG_R(data, len);
    dueros_dma_app.wakeup = 1;
    dueros_dma_wrap_recv_mobile_data((const char *)data, len);
    return true;
}

void dueros_dma_app_ble_disconnect_callback(void)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    dueros_dma_con_type_set(DUEROS_DMA_CON_NONE);
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_DISCONNECTED, NULL, 0, 0);
    if(bsp_opus_is_encode()){
        msg_enqueue(EVT_ENCODE_STOP);
    }
}

void dueros_dma_app_ble_connect_callback(void)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    dueros_dma_con_type_set(DUEROS_DMA_CON_BLE);
    // dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_DMA_CONNECTED, NULL, 0, 0);  //订阅后再通知
}

void dueros_dma_app_enter_sleep(void)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    if(ble_get_status() == LE_STA_ADVERTISING){
        ble_set_adv_interval(DUEROS_DMA_ADV_SLEEP_INTERVAL);
    }else if(ble_get_status() == LE_STA_CONNECTION){
        ble_update_conn_param(DUEROS_DMA_CON_SLEEP_INTERVAL, 0, 400);
    }
}

void dueros_dma_app_exit_sleep(void)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    if(ble_get_status() == LE_STA_ADVERTISING){
        ble_set_adv_interval(DUEROS_DMA_ADV_INTERVAL);
    }else if(ble_get_status() == LE_STA_CONNECTION){
        ble_update_conn_param(DUEROS_DMA_CON_INTERVAL, 0, 400);
    }
}

void dueros_dma_app_var_init(void)
{
    memset(&dueros_dma_app, 0, sizeof(dueros_dma_app_data_t));
}

void dueros_dma_bt_disconnect_callback(uint8_t *buf)
{
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_MOBILE_DISCONNECTED, NULL, 0, 0);
}

void dueros_dma_bt_connect_callback(uint8_t *buf)
{
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_MOBILE_CONNECTED, NULL, 0, 0);
}

void dueros_dma_tws_disconnect_callback(void)
{
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_TWS_DISCONNECT, NULL, 0, 0);
}

void dueros_dma_tws_connect_callback(uint8_t *buf)
{
    DUEROS_DMA_DEBUG("[DMA_APP]:%s\n", __func__);
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_TWS_CONNECT, NULL, 0, 0);
}

void dueros_dma_tws_role_change_callback(void)
{
    dueros_dma_app.role_switch_flag = 0;
    dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_ROLE_SWITCH_FINISH, NULL, 0, 0);
}

void dueros_dma_app_bt_evt_notice(uint16_t evt, void *params)
{
    uint8_t *packet = params;
    switch(evt){
        case BT_NOTICE_INIT_FINISH:
            DUEROS_DMA_DEBUG("==>DUEROS_DMA BT_NOTICE_INIT_FINISH\n");
            dueros_dma_ntf_sta_enqueue(DMA_NOTIFY_STATE_BOX_OPEN, NULL, 0, 0); //此处由实际充电仓行为来控制
            break;

        case BT_NOTICE_DISCONNECT:
            DUEROS_DMA_DEBUG("==>DUEROS_DMA BT_NOTICE_DISCONNECT\n");
            dueros_dma_bt_disconnect_callback(NULL);
            break;

        case BT_NOTICE_CONNECTED:
            DUEROS_DMA_DEBUG("==>DUEROS_DMA BT_NOTICE_CONNECTED\n");
            dueros_dma_bt_connect_callback(packet);
            break;

        case BT_NOTICE_TWS_DISCONNECT:
            DUEROS_DMA_DEBUG("==>DUEROS_DMA BT_NOTICE_TWS_DISCONNECT\n");
            dueros_dma_tws_disconnect_callback();
            break;

        case BT_NOTICE_TWS_CONNECTED:
            DUEROS_DMA_DEBUG("==>DUEROS_DMA BT_NOTICE_TWS_CONNECTED\n");
            dueros_dma_tws_connect_callback(packet);
            break;

        case BT_NOTICE_TWS_ROLE_CHANGE:
            DUEROS_DMA_DEBUG("==>DUEROS_DMA BT_NOTICE_TWS_ROLE_CHANGE\n");
            dueros_dma_tws_role_change_callback();
            break;
    }
}

void dueros_dma_tws_user_key_process(uint32_t *opcode)
{
    uint8_t *packet = (uint8_t*)opcode;
    switch (packet[0]) {
        case OPUS_ENC_START:
            DUEROS_DMA_DEBUG("[DMA_APP]TWS USER KEY OPUS_ENC_START, bt_bypass:%d, do_flag:0x%x\n", dueros_dma_app.bt_bypass, dueros_dma_app.do_flag);
            if ((dueros_dma_app.bt_bypass == 0)  || (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_AUDIO_ENABLE)) {
                dueros_dma_app.do_flag |= DUEROS_DMA_FLAG_AUDIO_BYPASS;
            }
            break;

        case OPUS_ENC_STOP:
            DUEROS_DMA_DEBUG("[DMA_APP]TWS USER KEY OPUS_ENC_STOP, bt_bypass:%d, do_flag:0x%x\n", dueros_dma_app.bt_bypass, dueros_dma_app.do_flag);
            if (dueros_dma_app.bt_bypass == 1) {
                dueros_dma_app.do_flag |= DUEROS_DMA_FLAG_AUDIO_ENABLE;
            } else {
                dueros_dma_app.do_flag &= ~DUEROS_DMA_FLAG_AUDIO_BYPASS;
            }
            break;

        default:
            break;
    }
}

uint8_t dueros_dma_flag_process(void)
{
    uint8_t ret = true;
    if (dueros_dma_app.do_flag) {
        DUEROS_DMA_DEBUG("[DMA_APP]%s, bt_bypass:%d, do_flag:0x%x\n", __func__, dueros_dma_app.bt_bypass, dueros_dma_app.do_flag);

        if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_AUDIO_BYPASS) {
            dueros_dma_app.bt_bypass = 1;
            if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_AUDIO_BYPASS) {  //处理判断bt_audio_enable与bt_bypass之间被打断的临界点
                bt_audio_bypass();  //从机 audio bypass
                dueros_dma_app.do_flag &= ~DUEROS_DMA_FLAG_AUDIO_BYPASS;
            } else {
                dueros_dma_app.bt_bypass = 0;
            }
        }

        if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_AUDIO_ENABLE) {
            dueros_dma_app.bt_bypass = 0;
            bt_audio_enable();  //从机 audio en
            dueros_dma_app.do_flag &= ~DUEROS_DMA_FLAG_AUDIO_ENABLE;
        }

        if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_KEY_MSG_CTL) {
            dueros_dma_app.do_flag &= ~DUEROS_DMA_FLAG_KEY_MSG_CTL;
            dueros_dma_ntf_sta_enqueue(dueros_dma_app.key_click, &dueros_dma_app.key_channel, 1, 0);
        }

        if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_VOICE_START) {
            dueros_dma_app.opus_enc_flag = 1;
            if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_VOICE_START) {  //处理判断voice start与opus_enc_flag之间被打断的临界点
                bsp_opus_encode_start();
                dueros_dma_app.do_flag &= ~DUEROS_DMA_FLAG_VOICE_START;
            } else {
                dueros_dma_app.opus_enc_flag = 0;
            }
        }

        if (dueros_dma_app.do_flag & DUEROS_DMA_FLAG_VOICE_STOP) {
            dueros_dma_app.opus_enc_flag = 0;
            bsp_opus_encode_stop();
            dueros_dma_app.do_flag &= ~DUEROS_DMA_FLAG_VOICE_STOP;
        }
    } else {
        ret = false;
    }
    return ret;
}

uint8_t dueros_dma_app_ntf_sta_process(void)
{
    uint8_t ret = true;
    uint8_t state = 0;
    uint8_t param_buf[DUEROS_DMA_NTF_STA_PARAM_BUF_LEN] = {0};
    uint8_t param_size = 0;
    bool res = dueros_dma_ntf_sta_dequeue(&state, param_buf, &param_size);
    if (res) {
        switch (state) {
            case DMA_NOTIFY_STATE_DMA_CONNECTED:
                dueros_dma_tws_dma_conn_sta_sync(dueros_dma_con_type_get(), 1);
                return ret;
                break;

            case DMA_NOTIFY_STATE_DMA_DISCONNECTED:
                dueros_dma_tws_dma_conn_sta_sync(dueros_dma_con_type_get(), 0);
                break;

            case DMA_NOTIFY_STATE_MOBILE_CONNECTED:
                dueros_dma_tws_bt_conn_sta_sync(1);
                break;

            case DMA_NOTIFY_STATE_MOBILE_DISCONNECTED:
                dueros_dma_tws_bt_conn_sta_sync(0);
                break;

            case DMA_NOTIFY_STATE_TWS_CONNECT:
                dueros_dma_tws_tws_conn_info_sync();
                break;

            case DMA_NOTIFY_STATE_TWS_DISCONNECT: {
                if (dueros_dma_app.bt_bypass) {
                    dueros_dma_app.do_flag |= DUEROS_DMA_FLAG_AUDIO_ENABLE;
                }
                break;
            }

            case DMA_NOTIFY_STATE_ROLE_SWITCH_FINISH: {
                if (DUEROS_DMA_CON_BLE == dueros_dma_con_type_get()) {
                    ble_disconnect();
                }
                break;
            }

            default:
                break;
        }
        dueros_dma_wrap_notify_state(state, param_buf, param_size);
    } else {
        ret = false;
    }
    return ret;
}

uint8_t dueros_dma_app_voice_process(void)
{
    uint8_t ret = true;
    if (dueros_dma_stream_upload_flag_get()) {
        uint8_t len = bsp_opus_get_enc_frame(opus_buf, DUEROS_DMA_OPUS_DATA_LEN);
        if(len == DUEROS_DMA_OPUS_DATA_LEN){
            // DUEROS_DMA_DEBUG("send voice!\n");
            dueros_dma_wrap_feed_compressed_data((const char* )opus_buf, DUEROS_DMA_OPUS_DATA_LEN);
        }
    } else {
        ret = false;
    }
    return ret;
}

void dueros_dma_app_process(void)
{
    uint8_t wakeup_det = 0;
	wakeup_det |= dueros_dma_flag_process();
    wakeup_det |= dueros_dma_app_ntf_sta_process();
    wakeup_det |= dueros_dma_app_voice_process();
    if (wakeup_det == 0) {
        dueros_dma_app.wakeup = 0;
    }
}


void dueros_dma_var_init(void)
{
    static bool init_flag = 0;
    dueros_dma_app_var_init();
    if(init_flag == 0){
        init_flag = 1;
        dueros_dma_dev_info_init();
        dueros_dma_wrap_heap_init();
        dueros_dma_wrap_operation_register();
        dueros_dma_wrap_process_register();
        dueros_dma_wrap_protocol_stack_init();
        dueros_dma_wrap_thread_create();
    }
}

void dueros_dma_app_init(void)
{
}

#endif //LE_DUEROS_DMA_EN
