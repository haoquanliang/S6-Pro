#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

void smic_test_set_en(bool smic_en);
void dmic_test_set_channel(u8 ch);
void tx_ack(uint8_t *packet, uint16_t len);

u32 getcfg_dmic_ch(void)
{
    u32 mic_xcfg_cfg = (xcfg_cb.bt_mmic_cfg) | (xcfg_cb.bt_smic_cfg << 4);
    return mic_xcfg_cfg;
}

#if ENC_DBG_EN
static const char tbl_enc_header[3] = "ENC";
static const char tbl_smic_header[4] = "SMIC";

static void bsp_enc_dbg_ack(u8 channel, u8 errcode)
{
    u8 ack[4];
    memset(ack, 0, 4);
    if (errcode > 0) {
        ack[0] = 'N';
    } else {
        ack[0] = 'Y';
    }
    ack[1] = channel;
    ack[2] = errcode;
    ack[3] = 0xff;

#if TRACE_EN
    printf("ENC TX:");
    print_r(ack, 4);
#endif

    tx_ack(ack, 4);
}

void bsp_enc_parse_cmd(void)
{
    u8 *ptr = eq_rx_buf;
#if TRACE_EN
    printf("ENC RX:");
    print_r(ptr, 10);
#endif
    if (memcmp((char *)ptr, tbl_smic_header, 4) == 0) {
        u8 smic_type  = ptr[6];
        u8 ctlcmd   = ptr[7];
        if (smic_type > '2' || smic_type == '0') {
            bsp_enc_dbg_ack(smic_type, 2);                  //error code 2:channel error
            return;
        }

        if (ctlcmd == 'N') {
            smic_test_set_en(1);                            //smic dbg
            bsp_enc_dbg_ack(smic_type, 0);                  //sucess
        } else if (ctlcmd == 'Y') {
            smic_test_set_en(0);                            //smic dbg close
            bsp_enc_dbg_ack(smic_type, 0);                  //sucess
        } else {
            bsp_enc_dbg_ack(smic_type, 3);                  //error code 3:ctlcmd error
        }
    } else if (memcmp((char *)ptr, tbl_enc_header, 3) == 0) {
        u8 channel  = ptr[5];
        u8 ctlcmd   = ptr[6];
        //printf("channel = %c; ctlcmd = %c\n",channel,ctlcmd);
        if (channel > '3' || channel == '0') {
            bsp_enc_dbg_ack(channel, 2);                    //error code 2:channel error
            return;
        }
        if (ctlcmd == '1' || ctlcmd == '2') {
            dmic_test_set_channel(channel);
            bsp_enc_dbg_ack(channel, 0);                    //sucess
        } else if (ctlcmd == '3') {                         //设置主MIC端口
            xcfg_cb.bt_mmic_cfg = ptr[7] - '0';
            TRACE("ENC Test change mmic port:%x\n", xcfg_cb.bt_mmic_cfg);
            bsp_enc_dbg_ack(channel, 0);                    //sucess
        } else {
            bsp_enc_dbg_ack(channel, 3);                    //error code 3:ctlcmd error
        }
    }
}
#endif
