#include "include.h"

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#define EQ_CRC_SEED                             0xffff

uint calc_crc(void *buf, uint len, uint seed);
void tx_ack(uint8_t *packet, uint16_t len);
u8 check_sum(u8 *buf, u16 size);

#if ANC_EN
static void bsp_anc_dbg_ack_inquiry(void)
{
    u8 ack[4];
    memset(ack, 0, 4);
    ack[0] = 'A';
    ack[1] = 'N';
    ack[2] = 'C';
    ack[3] = '*';
    tx_ack(ack, 4);
}

static void bsp_anc_dbg_ack(u8 error_code)
{
    u8 ack[4];
    memset(ack, 0, 4);
    ack[0] = 'Y';
    ack[2] = error_code;
    ack[3] = check_sum(ack, 3);
    tx_ack(ack, 4);
}

void bsp_anc_parse_cmd(void)
{
    u8 *ptr = eq_rx_buf;
#if TRACE_EN
    //print_r(eq_rx_buf, EQ_BUFFER_LEN);
#endif
    if (ptr[3] == '?') {
        bsp_anc_dbg_ack_inquiry();
        return;
    }
    u16 size = ptr[4] + 3;
    u16 crc = calc_crc(ptr, size, 0xffff);
    if (crc != little_endian_read_16(ptr, size)) {
        bsp_anc_dbg_ack(1);                 //error code 1:crc error
        return;
    }
    u8 channel = ptr[5];
    u8 bit_ctl = ptr[6];
    s8 adjust0 = (s8)ptr[8];              //ch0 mute sta
    s8 adjust1 = (s8)ptr[9];

    bool anc_on = (bool)(bit_ctl & BIT(0));   //判断是否打开或关闭anc
    bool save_flag = (bool)(bit_ctl & BIT(1));//判断参数是否烧录
    bool eq_tool = (bool)(bit_ctl & BIT(4));  //判断是eq工具还是产测协议
    bool tp_flag = (bool)(bit_ctl & BIT(5));  //判断是anc模式测试，还是通透模式测试

    if (channel > 3 || channel == 0) {      //L-1,R-2,L+R-3
        bsp_anc_dbg_ack(2);                 //error code 2:param error
        return;
    }
    if (!xcfg_cb.anc_en) {
        printf("anc close\n");
        bsp_anc_dbg_ack(3);                 //error code 3:anc close
        return;
    }

    if (!eq_tool && !anc_on) {
        bsp_anc_stop();
        bsp_anc_exit();
        bsp_anc_dbg_ack(0);                     //error code 0:succeed
        TRACE("anc parse cmd anc exit\n");
        return;
    } else {
        if (!tp_flag) {
            if ((!sys_cb.anc_start) || (sys_cb.anc_user_mode != 1)) {
                bsp_anc_set_mode(1);
            }
        } else {
            if ((!sys_cb.anc_start) || (sys_cb.anc_user_mode != 2)) {
                bsp_anc_set_mode(2);
            }
        }
		TRACE("anc parse cmd anc start\n");
    }

    bsp_anc_fade_enable(0);

    do {
#if BT_TWS_EN
        u8 ch = channel - 1;
        if (xcfg_cb.bt_tws_en) {
            if (ch == 2 || (ch == sys_cb.tws_left_channel)) {   //声道不相等，转发
                if (bt_tws_is_connected()) {
                    if (ch == 2) {                              //把这个声道去掉，重新算下CRC
                        ptr[5] = (sys_cb.tws_left_channel == 1) ? 2 : 1;
                        u16 crc = calc_crc(ptr, 14, EQ_CRC_SEED);
                        ptr[14] = crc;
                        ptr[15] = crc >> 8;
                    }
                    bt_tws_sync_eq_param();
                }
                if (ch != 2) {                                  //如果收到单声道的命令，转发后跳出
                    break;
                }
            }
            if (xcfg_cb.anc_mode == MODE_TWS_FFFB) {            //TWS FFFB模式只调MICL
                channel = 1;
            }
        }
#endif
        bool ch0_mute = false;
        bool ch1_mute = false;
        if (xcfg_cb.anc_mode >= MODE_TWS_HYBRID) {
            ch0_mute = !anc_on || (bit_ctl & BIT(2));
            ch1_mute = !anc_on || (bit_ctl & BIT(3));
            TRACE("channel %d, ch0 mute %d, %d, %d, %d\n", channel, ch0_mute, ch1_mute, ch2_mute, ch3_mute);
            if (xcfg_cb.anc_mode == MODE_HYBRID) {              //hybrid mode, ch0~ch3
                if (channel & 0x01) {                           //mute ch0 ch1
                    bsp_anc_mic_mute(0, ch0_mute);
                    bsp_anc_mic_mute(1, ch1_mute);
                    if (!eq_tool) {                             //产测流程才会调整adjust value
                        if (ch0_mute == 0) bsp_anc_gain_adjust(0, adjust0);                //烧录gain adjust0值
                        if (ch1_mute == 0) bsp_anc_gain_adjust(1, adjust1);                //烧录gain adjust1值
                    }
                    if (!tp_flag) {
                        bsp_param_write((u8*)&adjust0, PARAM_ANC_MIC0_VAL, 1);
                        bsp_param_write((u8*)&adjust1, PARAM_ANC_MIC1_VAL, 1);
                    } else {
                        bsp_param_write((u8*)&adjust0, PARAM_ANC_TP_MIC0_VAL, 1);
                        bsp_param_write((u8*)&adjust1, PARAM_ANC_TP_MIC1_VAL, 1);
                    }
                }
                if (channel & 0x02) {                           //mute ch2 ch3
                    bsp_anc_mic_mute(2, ch0_mute);
                    bsp_anc_mic_mute(3, ch1_mute);
                    if (!eq_tool) {
                        if (ch0_mute == 0) bsp_anc_gain_adjust(2, adjust0);                //烧录gain adjust0值
                        if (ch1_mute == 0) bsp_anc_gain_adjust(3, adjust1);                //烧录gain adjust1值
                    }
                    if (!tp_flag) {
                        bsp_param_write((u8*)&adjust0, PARAM_ANC_MIC2_VAL, 1);
                        bsp_param_write((u8*)&adjust1, PARAM_ANC_MIC3_VAL, 1);
                    } else {
                        bsp_param_write((u8*)&adjust0, PARAM_ANC_TP_MIC2_VAL, 1);
                        bsp_param_write((u8*)&adjust1, PARAM_ANC_TP_MIC3_VAL, 1);
                    }
                }
            } else {                                            //tws hybrid mode, ch0、ch1
                bsp_anc_mic_mute(0, ch0_mute);
                bsp_anc_mic_mute(1, ch1_mute);
                if (!eq_tool) {                                 //产测流程才会调整adjust value
                    if (ch0_mute == 0) bsp_anc_gain_adjust(0, adjust0);                //烧录gain adjust0值
                    if (ch1_mute == 0) bsp_anc_gain_adjust(1, adjust1);                //烧录gain adjust1值
                }
                if (save_flag) {
                    if (!tp_flag) {
                        bsp_param_write((u8*)&adjust0, PARAM_ANC_MIC0_VAL, 1);
                        bsp_param_write((u8*)&adjust1, PARAM_ANC_MIC1_VAL, 1);
                    } else {
                        bsp_param_write((u8*)&adjust0, PARAM_ANC_TP_MIC0_VAL, 1);
                        bsp_param_write((u8*)&adjust1, PARAM_ANC_TP_MIC1_VAL, 1);
                    }
                }
            }

            if (eq_tool) {                                      //eq工具设置模拟增益
                if (xcfg_cb.anc_mode == MODE_HYBRID) {
                    if (channel & 0x01) {
                        bsp_anc_set_mic_gain(0, 1, ptr[10]);
                        bsp_anc_set_mic_gain(1, 1, ptr[11]);
                    }
                    if (channel & 0x02) {
                        bsp_anc_set_mic_gain(2, 1, ptr[10]);
                        bsp_anc_set_mic_gain(3, 1, ptr[11]);
                    }
                } else {
                    bsp_anc_set_mic_gain(0, 1, ptr[10]);
                    bsp_anc_set_mic_gain(1, 1, ptr[11]);
                }
            }
        } else {
            bool ch_mute = !anc_on || (bit_ctl & BIT(2));
            if (channel & 0x01) {
                bsp_anc_mic_mute(0, ch_mute);
                if (!eq_tool) {
                    bsp_anc_gain_adjust(0, adjust0);
                }
                if (save_flag) {
                    if (!tp_flag) {
                        bsp_param_write((u8*)&sys_cb.adjust_val[0], PARAM_ANC_MIC0_VAL, 1);
                    } else {
                        bsp_param_write((u8*)&sys_cb.adjust_val[0], PARAM_ANC_TP_MIC0_VAL, 1);
                    }
                }
            }
            if (channel & 0x02) {
                bsp_anc_mic_mute(1, ch_mute);
                if (!eq_tool) {
                    bsp_anc_gain_adjust(1, adjust0);
                }
                if (save_flag) {
                    if (!tp_flag) {
                        bsp_param_write((u8*)&sys_cb.adjust_val[1], PARAM_ANC_MIC1_VAL, 1);
                    } else {
                        bsp_param_write((u8*)&sys_cb.adjust_val[1], PARAM_ANC_TP_MIC1_VAL, 1);
                    }
                }
            }
            //调整again，研发调试用
            if (eq_tool) {
                if (channel & 0x01) {
                    bsp_anc_set_mic_gain(0, 1, ptr[10]);
                }
                if (channel & 0x02) {
                    bsp_anc_set_mic_gain(1, 1, ptr[10]);
                }
            }
        }
        if (save_flag) {
            bsp_param_sync();
        }
    } while (0);

    bsp_anc_dbg_ack(0);                     //error code 0:succeed
}
#endif
