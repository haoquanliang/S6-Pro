#include "include.h"


#if IODM_TEST_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


u8 iodm_ver[] = {1,3};
struct{
    u8 loopback_mic;
    u32 anl_gain;
    u32 dig_gain;
}iodm;
u8 pre_spk_mic = IODM_CFG_MIC_NONE;

AT(.com_rodata.vhouse_tbl)
const char packet_header[]={0x55,0xAA,0xFF};

u8 fcc_param[10];
u32 bt_get_xosc_cap(void);
void bt_set_xosc_cap(u32 cap);
u8 param_bt_xosc_read(void);

AT(.text.iodm)
u8 iodm_cmd_mic_lookback_enter_check(vh_packet_t *packet)
{
    u8 channel = packet->buf[0];
    u8 anl_gain = packet->buf[1];
    u8 dig_gain = packet->buf[2];

    if (((channel == 1) && (!xcfg_cb.mic0_en)) ||
        ((channel == 2) && (!xcfg_cb.mic1_en)) ||
        ((channel == 3) && (!xcfg_cb.mic2_en)) ||
        ((channel == 4) && (!xcfg_cb.mic3_en)) ||
        ((channel == 5) && (!xcfg_cb.mic4_en))) {
        printf("error: mic's configuration error!\n");
        return 1;
    }
    if (anl_gain > 13) {
        return 2;
    }
    if (dig_gain > 39) {
        return 3;
    }
    return IODM_RESULT_OK;
}

AT(.text.iodm)
void iodm_cmd_mic_loopback_exit(void)
{
    dac_fade_out();
    dac_fade_wait();                    //等待淡出完成
    audio_path_exit(AUDIO_PATH_IODM_MIC_TEST);
}

AT(.text.iodm)
void iodm_cmd_mic_loopback_enter(void)
{
#if ANC_EN
    if (xcfg_cb.anc_en && sys_cb.anc_user_mode) {
        bsp_anc_stop();
    }
#endif
    iodm_cmd_mic_loopback_exit();
    audio_path_init(AUDIO_PATH_IODM_MIC_TEST);
    audio_path_start(AUDIO_PATH_IODM_MIC_TEST);
    dac_fade_in();
}

AT(.text.iodm)
void iodm_test_loopback_set_mic_exit(u16 *channel)
{
    if (pre_spk_mic == IODM_CFG_MIC0) {
        *channel = CH_MIC0;
    } else if (pre_spk_mic == IODM_CFG_MIC1) {
        *channel = CH_MIC1;
    } else if (pre_spk_mic == IODM_CFG_MIC2) {
        *channel = CH_MIC2;
    } else if (pre_spk_mic == IODM_CFG_MIC3) {
        *channel = CH_MIC3;
    } else if (pre_spk_mic == IODM_CFG_MIC4) {
        *channel = CH_MIC4;
    }
}

AT(.text.iodm)
void iodm_test_loopback_set_mic_ch(u16 *channel, u32 *anl_gain, u32 *dig_gain)
{
    if (iodm.loopback_mic == 1) {
        *channel = CH_MIC0;
        *anl_gain = ((iodm.anl_gain) | (xcfg_cb.mic1_anl_gain << 6) | (xcfg_cb.mic2_anl_gain << 12) | (xcfg_cb.mic3_anl_gain << 18) | (xcfg_cb.mic4_anl_gain << 24));
        *dig_gain = ((iodm.dig_gain) | (xcfg_cb.bt_mic1_dig_gain << 6) | (xcfg_cb.bt_mic2_dig_gain << 12) | (xcfg_cb.bt_mic3_dig_gain << 18) | (xcfg_cb.bt_mic4_dig_gain << 24));
    } else if (iodm.loopback_mic == 2) {
        *channel = CH_MIC1;
        *anl_gain = ((xcfg_cb.mic0_anl_gain) | (iodm.anl_gain << 6) | (xcfg_cb.mic2_anl_gain << 12) | (xcfg_cb.mic3_anl_gain << 18) | (xcfg_cb.mic4_anl_gain << 24));
        *dig_gain = ((xcfg_cb.bt_mic0_dig_gain) | (iodm.dig_gain << 6) | (xcfg_cb.bt_mic2_dig_gain << 12) | (xcfg_cb.bt_mic3_dig_gain << 18) | (xcfg_cb.bt_mic4_dig_gain << 24));
    } else if (iodm.loopback_mic == 3) {
        *channel = CH_MIC2;
        *anl_gain = ((xcfg_cb.mic0_anl_gain) | (xcfg_cb.mic1_anl_gain << 6) | (iodm.anl_gain << 12) | (xcfg_cb.mic3_anl_gain << 18) | (xcfg_cb.mic4_anl_gain << 24));
        *dig_gain = ((xcfg_cb.bt_mic0_dig_gain) | (xcfg_cb.bt_mic1_dig_gain << 6) | (iodm.dig_gain << 12) | (xcfg_cb.bt_mic3_dig_gain << 18) | (xcfg_cb.bt_mic4_dig_gain << 24));
    } else if (iodm.loopback_mic == 4) {
        *channel = CH_MIC3;
        *anl_gain = ((xcfg_cb.mic0_anl_gain) | (xcfg_cb.mic1_anl_gain << 6) | (xcfg_cb.mic2_anl_gain << 12) | (iodm.anl_gain << 18) | (xcfg_cb.mic4_anl_gain << 24));
        *dig_gain = ((xcfg_cb.bt_mic0_dig_gain) | (xcfg_cb.bt_mic1_dig_gain << 6) | (xcfg_cb.bt_mic2_dig_gain << 12) | (iodm.dig_gain << 18) | (xcfg_cb.bt_mic4_dig_gain << 24));
    } else if (iodm.loopback_mic == 5) {
        *channel = CH_MIC4;
        *anl_gain = ((xcfg_cb.mic0_anl_gain) | (xcfg_cb.mic1_anl_gain << 6) | (xcfg_cb.mic2_anl_gain << 12) | (xcfg_cb.mic3_anl_gain << 18) | (iodm.anl_gain << 24));
        *dig_gain = ((xcfg_cb.bt_mic0_dig_gain) | (xcfg_cb.bt_mic1_dig_gain << 6) | (xcfg_cb.bt_mic2_dig_gain << 12) | (xcfg_cb.bt_mic3_dig_gain << 18) | (iodm.dig_gain << 24));
    }
}

AT(.com_text.iodm)
void iodm_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    sdadc_pcm_2_dac(ptr, samples, ch_mode);
}

uint8_t *bt_rf_get_fcc_param(void)
{
    return fcc_param;
}

AT(.text.iodm)
static void iodm_clr_bt_tws_link_info(void)
{
    bt_clr_master_addr();
    bt_tws_delete_link_info_with_tag(BT_INFO_TAG_IODM, (uint32_t)__builtin_return_address(0));
}

AT(.text.iodm)
static void iodm_clr_bt_all_link_info(void)
{
    bt_clr_master_addr();
    bt_nor_delete_link_info();
    bt_tws_delete_link_info_with_tag(BT_INFO_TAG_IODM, (uint32_t)__builtin_return_address(0));
}

AT(.text.iodm)
static void bt_save_new_name(char *name,u8 len)
{
    cm_write8(PARAM_BT_NAME_LEN, len);
    cm_write(name, PARAM_BT_NAME, len);
    cm_sync();
}


bool bt_get_new_name(char *name)
{
    u8 len = cm_read8(PARAM_BT_NAME_LEN);
    if(len > 32 || len == 0){
        return false;
    }
    memset(name,0x00,32);  //clear
    cm_read(name, PARAM_BT_NAME, len);
    return true;
}

AT(.text.iodm)
static void bt_save_qr_addr(u8 *addr)
{
    iodm_clr_bt_tws_link_info();
    cm_write8(PARAM_QR_ADDR_VALID, 1);
    cm_write(addr, PARAM_QR_ADDR, 6);
    cm_sync();
}

bool bt_get_qr_addr(u8 *addr)
{
    if (cm_read8(PARAM_QR_ADDR_VALID) == 1) {
        cm_read(addr, PARAM_QR_ADDR, 6);
        return true;
    }
    return false;
}

AT(.text.iodm)
static u8 iodm_cmd_set_bt_name(vh_packet_t *packet)
{
    if(packet->length>32) {
        return IODM_RESULT_FAIL;
    }
    bt_save_new_name((char*)packet->buf, packet->length);
    return IODM_RESULT_OK;
}

AT(.text.iodm)
static u8 iodm_cmd_get_bt_name(u8*tx_buf,u8* tx_len)
{

    if (bt_get_new_name((char *)tx_buf)) {
        *tx_len = strlen((char *)tx_buf);
    } else {
        *tx_len = strlen(xcfg_cb.bt_name);
        memcpy(tx_buf, xcfg_cb.bt_name, *tx_len);
    }

    return IODM_RESULT_OK;
}

AT(.text.iodm)
static void iodm_send_data(u8 *buf,u8 len)
{

#if (TEST_INTF_SEL == INTF_HUART)
    huart_tx(buf,len);
#elif (TEST_INTF_SEL == INTF_UART1)
    for(u8 i = 0; i < len; i++){
        uart1_putchar(buf[i]);
    }
#elif (TEST_INTF_SEL == INTF_UART2)
    for(u8 i = 0; i < len; i++){
        uart2_putchar(buf[i]);
    }
#endif

}

#if (TEST_INTF_SEL == INTF_HUART)
AT(.com_text.iodm)
void iodm_packet_huart_recv(u8 *rx_buf)
{
    if (!memcmp(rx_buf,packet_header,3)) {
        if(rx_buf[4] < VH_DATA_LEN){
            if (((rx_buf[3] >= IODM_CMD_DEV_RST) && (rx_buf[3] < IODM_CMD_MAX))
#if  SWETZ_UART_CMD  
            ||(rx_buf[3] == IODM_CMD_CLEAR_PAIR) 
            || (rx_buf[3] > VHOUSE_CMD_CUSTOM_RESV1) && (rx_buf[3] < VHOUSE_CMD_CUSTOM_RESV_END)) {
#else

                || (rx_buf[3] == IODM_CMD_CLEAR_PAIR)) {
#endif                     
                msg_enqueue(EVT_IODM_TEST);
            }
        }
    }
}
#else
AT(.com_text.iodm)
void iodm_packet_recv(void)
{
    if(qtest_get_mode()){
        return ;
    }
    vh_packet_t *packet = &vh_packet;
    if (((packet->cmd >= IODM_CMD_DEV_RST) && (packet->cmd < IODM_CMD_MAX)) ||\
         (packet->cmd == IODM_CMD_CLEAR_PAIR)) {
        msg_enqueue(EVT_IODM_TEST);
    }
}

#endif

AT(.text.iodm)
static u8 iodm_cmd_ack(vh_packet_t *packet)
{

    u8 *buf = (u8*)packet;

    u8 length = 5 + packet->length;
    buf[length] = crc8_maxim(buf, length);
    iodm_send_data(buf, length + 1);
    return true;
}


AT(.text.iodm)
static u8 iodm_cmd_rsp(vh_packet_t *packet, u8 len, u8 result)
{
    packet->header = 0xAA55;
    packet->distinguish = VHOUSE_DISTINGUISH;
    packet->length = len + 1;
    packet->buf[0] = result;
    iodm_cmd_ack(packet);
    return 0;
}

AT(.text.iodm)
void iodm_reveice_data_deal(void)
{

    u8 cmd_rsp_param_len = 0;
    bool ret = true;
    u8 result = IODM_RESULT_OK;
#if (TEST_INTF_SEL == INTF_HUART)
    vh_packet_t *packet = (vh_packet_t*)eq_rx_buf;
#else
    vh_packet_t *packet = &vh_packet;
#endif
    print_r(packet,packet->length + 6);
    u8 *tx_buf = (u8*)packet->buf+1;

    switch (packet->cmd) {
        case IODM_CMD_DEV_RST:
            TRACE("IODM CMD DEVRST");
			cm_write8(PARAM_RST_FLAG, RST_FLAG_MAGIC_VALUE);
            cm_sync();
            iodm_cmd_rsp(packet, cmd_rsp_param_len, result);
            delay_5ms(10);
        	WDT_RST();
            break;

        case IODM_CMD_SET_BT_ADDR:
            TRACE("IODM CMD SET BT ADDR\n");
            bt_save_qr_addr(packet->buf);
            break;

        case IODM_CMD_GET_BT_ADDR:
            TRACE("IODM CMD GET BT ADDR\n");
            cmd_rsp_param_len = sizeof(xcfg_cb.bt_addr);
            if (!bt_get_qr_addr(tx_buf)) {
                bool bt_get_master_addr(u8 *addr);
                if (!bt_get_master_addr(tx_buf)) {
                    memcpy(tx_buf, xcfg_cb.bt_addr, 6);
                }
            }
            break;

        case IODM_CMD_SET_BT_NAME:
            TRACE("IODM CMD SET BT NAME\n");
            result = iodm_cmd_set_bt_name(packet);
            break;

        case IODM_CMD_GET_BT_NAME:
            TRACE("IODM CMD GET BT NAME\n");
            result = iodm_cmd_get_bt_name(tx_buf,&cmd_rsp_param_len);
            break;

        case IODM_CMD_CBT_TEST_ENTER:
            TRACE("IODM CMD CBT TEST ENTER\n");
            if(func_cb.sta != FUNC_BT_DUT){
                func_cb.sta = FUNC_BT_DUT;
                sys_cb.discon_reason = 0;
            }
            break;

        case IODM_CMD_CBT_TEST_EXIT:
            TRACE("IODM CMD CBT TEST EXIT\n");
            if (func_cb.sta != FUNC_BT){
                func_cb.sta = FUNC_BT;
                break;
            }
            break;

        case IODM_CMD_FCC_TEST_ENTER:
            TRACE("IODM CMD FCC TEST ENTER\n");
            if (func_cb.sta != FUNC_BT_IODM){
                func_cb.sta = FUNC_BT_IODM;
                sys_cb.discon_reason = 0;
            }
            break;

        case IODM_CMD_FCC_SET_PARAM:
            TRACE("IODM CMD FCC SET PARAM\n");
            if (func_cb.sta == FUNC_BT_IODM) {
                memcpy(fcc_param, packet->buf, sizeof(fcc_param));
                fcc_param[5] = 7;      //固定power
                //printf("fcc_param:");
                //print_r(fcc_param, 0x0a);
                bt_fcc_test_start();
            } else {
                result = IODM_RESULT_FAIL;
            }
            break;

        case IODM_CMD_FCC_TEST_EXIT:
            TRACE("IODM CMD FCC TEST EXIT\n");
            if (func_cb.sta != FUNC_BT){
                func_cb.sta = FUNC_BT;
                break;
            }
            break;

        case IODM_CMD_SET_XOSC_CAP:
            TRACE("IODM CMD SET XOSC CAP\n");
            u8 xtal = packet->buf[0];
            if (xtal < 63) {
                bt_set_xosc_cap(xtal);   //设置 频偏参数
            } else {
                result = IODM_RESULT_FAIL;
            }
            break;


        case IODM_CMD_GET_XOSC_CAP:
            TRACE("IODM CMD GET XOSC CAP\n");
            cmd_rsp_param_len = 1;
            u8 cap_param = param_bt_xosc_read();
            if (cap_param == 0xff) {
                tx_buf[0] = xcfg_cb.osci_cap;
            } else {
                tx_buf[0] = cap_param;
            }
            break;

        case IODM_CMD_GET_INFO:
            TRACE("IODM CMD GET INFO\n");
            cmd_rsp_param_len = 4;
            tx_buf[0] = 1;
            tx_buf[1] = 0;
            tx_buf[2] = 1;
            tx_buf[3] = VH_DATA_LEN;
            break;

        case IODM_CMD_GET_VER_INFO:
            TRACE("IODM CMD GET VER INFO\n");
            cmd_rsp_param_len = 4;
            memcpy(tx_buf, IODM_HARDWARE_VER, 2);
            memcpy(tx_buf+2, IODM_SOFTWARE_VER, 2);
            break;

        case IODM_CMD_PROTOCOL_VER:
            TRACE("IODM CMD PROTOCOL VER\n");
            cmd_rsp_param_len = 2;
            memcpy(tx_buf,iodm_ver,2);
            break;

        case IODM_CMD_CLEAR_PAIR:
            TRACE("IODM CMD CLEAR PAIR\n");
            iodm_clr_bt_all_link_info();
            break;

        case IODM_CMD_BT_SET_SCAN:
            break;
        case IODM_CMD_MIC_LOOKBACK_ENTER:
            TRACE("IODM CMD_MIC_LOOKBACK\n");
            sys_cb.sleep_delay = -1L;
            iodm.loopback_mic = packet->buf[0];       //当前测试的mic:  1-->mic0      2-->mic1    3-->mic2    4-->mic3    5-->mic4
            iodm.anl_gain = packet->buf[1];
            iodm.dig_gain = packet->buf[2];
            tx_buf[0] = iodm_cmd_mic_lookback_enter_check(packet);
            if (tx_buf[0]) {
                cmd_rsp_param_len = 1;
                result = IODM_RESULT_FAIL;
                TRACE("CMD_MIC_LOOKBACK FALI\n");
                break;
            }
            if (iodm.loopback_mic == 0) {
                iodm_cmd_mic_loopback_exit();          //关闭所有loopback通道
            } else if (iodm.loopback_mic == 1) {
                iodm_cmd_mic_loopback_enter();
                pre_spk_mic = IODM_CFG_MIC0;
            } else if (iodm.loopback_mic == 2) {
                iodm_cmd_mic_loopback_enter();
                pre_spk_mic = IODM_CFG_MIC1;
            } else if (iodm.loopback_mic == 3) {
                iodm_cmd_mic_loopback_enter();
                pre_spk_mic = IODM_CFG_MIC2;
            } else if (iodm.loopback_mic == 4) {
                iodm_cmd_mic_loopback_enter();
                pre_spk_mic = IODM_CFG_MIC3;
            } else if (iodm.loopback_mic == 5) {
                iodm_cmd_mic_loopback_enter();
                pre_spk_mic = IODM_CFG_MIC4;
            } else {
                result = IODM_RESULT_FAIL;
            }
            break;
        case IODM_CMD_MIC_LOOKBACK_EXIT:
            reset_sleep_delay();
            if (iodm.loopback_mic != 0) {
                iodm_cmd_mic_loopback_exit();          //关闭所有loopback通道
            }
            break;
#if SWETZ_UART_CMD
        case VHOUSE_CMD_CUSTOM_GET_VERSION:
                        {
            // cmd_rsp_param_len = 8;
            // tx_buf[1] = SW_VERSION[1]-'0';
            // tx_buf[2] = SW_VERSION[3]-'0';
            // tx_buf[3] = SW_VERSION[5]-'0';
            // tx_buf[4] = 0x00;
            // tx_buf[5] = 0x00;
            // tx_buf[6] = 0x00;
            // tx_buf[7] = VERSIONID;
        }
            break;

        case VHOUSE_CMD_CUSTOM_GET_FIXED_BT_ADDR:
                cmd_rsp_param_len = 6;
                bt_get_fixed_bd_addr(&tx_buf[0]);
        break;

#endif

        default:
            ret = false;
            break;
    }
    if (ret) {
        iodm_cmd_rsp(packet, cmd_rsp_param_len, result);
        TRACE("iodm_reveice_data_deal end\n");
    }
}


#endif
