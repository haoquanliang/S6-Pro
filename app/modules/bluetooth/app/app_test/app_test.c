/*
 *  app_test.c
 *  APP_TEST服务或者SPP在线调试、测试相关
 */
#include "include.h"

extern u8 eq_rx_buf[EQ_BUFFER_LEN];
void sco_audio_set_param(u8 type, u16 value);

#if BT_SCO_DBG_EN
void app_test_send_aec_param(u8 type)
{
    u8 buf[11], size = 11;
    buf[0] = 'a';
    buf[1] = 'e';
    buf[2] = 'c';
    buf[3] = (u8)xcfg_cb.mic4_anl_gain;
    buf[4] = (u8)xcfg_cb.bt_mic4_dig_gain;
    buf[5] = (u8)xcfg_cb.bt_sco_post_gain;
    buf[6] = 0;
    buf[7] = 0;
    buf[8] = (u8)xcfg_cb.bt_echo_level;
    buf[9] = 0;
    buf[10] = 0;

    bt_spp_tx(SPP_SERVICE_CH0, buf, size);
}

void app_test_send_aec_ack(u8 type)
{
    u8 buf[6];
    buf[0] = 'a';
    buf[1] = 'e';
    buf[2] = 'c';
    buf[3] = ' ';
    buf[4] = 'o';
    buf[5] = 'k';

    bt_spp_tx(SPP_SERVICE_CH0, buf, 6);
}
#endif

bool app_test_cmd_process(u8 *ptr, u16 size, u8 type)
{
//    printf("SPP RX:");
//    print_r(ptr, size);
#if EQ_DBG_IN_SPP
    if (xcfg_cb.huart_en || xcfg_cb.eq_dgb_spp_en || ANC_EN) {
        eq_spp_cb_t *p = &eq_dbg_cb.eq_spp_cb;
        if ((ptr[0] == 'E' && ptr[1] == 'Q') || (ptr[0] == 'D' && ptr[1] == 'R')) {   //EQ和DRC消息
            if ((ptr[2] == '?')
#if ANC_EQ_RES2_EN
            || ((ptr[2] == 'S') && ((ptr[3] == 'P') || (ptr[3] == 'F')))
#endif
#if DAC_PT_EN
            || (ptr[2] == 'S')
#endif
            ) {
                memcpy(eq_rx_buf, ptr, size);
                msg_enqueue(EVT_ONLINE_SET_EQ);
                return true;
            }
            u32 rx_size = little_endian_read_16(ptr, 4) + 6;
            memcpy(eq_rx_buf, ptr, size);
            p->rx_size = rx_size;
            if (size < rx_size) {
                p->remain = 1;
                p->remian_ptr = size;
            } else {
                p->remain = 0;
                p->remian_ptr = 0;
                msg_enqueue(EVT_ONLINE_SET_EQ);
            }
            return true;
        }
#if BT_MUSIC_EFFECT_DBG_EN
        else if (ptr[0] == 'C' && ptr[1] == 'A' && ptr[2] == 'R' && ptr[3] == '_') {
            memcpy(eq_rx_buf, ptr, size);
            msg_enqueue(EVT_ONLINE_SET_EFFECT);
            return true;
        }
#endif
        if (p->remain) {
            memcpy(&eq_rx_buf[p->remian_ptr], ptr, size);
            p->remian_ptr += size;
            if (p->rx_size == p->remian_ptr) {
                msg_enqueue(EVT_ONLINE_SET_EQ);
                memset(p, 0, sizeof(eq_spp_cb_t));
            }
            return true;
        }
    }
#endif

#if ANC_EN
    if (memcmp(ptr, "ANC", 3) == 0) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_ANC);
        return true;
    }
#endif

#if ENC_DBG_EN
    if (memcmp(ptr, "ENC", 3) == 0 || memcmp(ptr, "SMIC", 4) == 0) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_ENC);
        return true;
    }
#endif

#if BT_SCO_DBG_EN
    if (memcmp(ptr, "aec", 3) == 0) {
        u8 cnt, i;
        u8 args;

        if (ptr[3] == '?') {
            app_test_send_aec_param(type);
            return true;
        }
        cnt = 3;
        args = 6;
        if (size == 16) {
            args = 9;
        }
        for (i = 0; i < args; i++) {
            u16 value;
            if (i == AEC_PARAM_NOISE) {    //第4参数为2Byte
                value = ptr[cnt++];
                value |= ptr[cnt++] << 8;
            } else {
                value = ptr[cnt++];
            }
            sco_audio_set_param(i, value);
        }
        app_test_send_aec_ack(type);
        return true;
    }
#endif

#if ANC_ALG_DBG_EN
    if (bsp_anc_alg_dbg_rx_done(ptr, 1)) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_ANC_ALG);
        return true;
    }
#endif // ANC_ALG_DBG_EN

#if ANC_TOOL_EN
    if (bsp_anc_tool_rx_done(ptr, 1)) {
        memcpy(eq_rx_buf, ptr, size);
        msg_enqueue(EVT_ONLINE_SET_ANC_TOOL);
        return true;
    }
#endif // ANC_TOOL_EN
#if SWETZ_SPP_CMD
    if ((size > 3) && (memcmp(ptr, "AT+", 3) == 0)) 
    {
        memcpy(eq_rx_buf, ptr + 3, size -3);
        eq_rx_buf[size - 3] = '\0';
        msg_enqueue(EVT_SPP_AT_CMD);
        return true;
    }    
#endif
    return false;
}

