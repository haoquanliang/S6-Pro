#include "include.h"


#if ANC_TOOL_EN


#define TRACE_EN                            1

#if TRACE_EN
#define TRACE(...)                          printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif // TRACE_EN

#define ANC_TOOL_CRC_SEED                   0xFFFF
//子协议ID
#define ANC_TOOL_SPROF_ID_CONNECT           0
#define ANC_TOOL_SPROF_ID_CONTROL           1
#define ANC_TOOL_SPROF_ID_AUDIO             2


//数据类型（跟dump协议一致）
enum {
    ANC_DATA_FF_0_MIC = 0x0,
    ANC_DATA_FB_0_MIC,
    ANC_DATA_FF_1_MIC,
    ANC_DATA_FB_1_MIC,
    ANC_DATA_HYBRID_ANC_0_OUT,
    ANC_DATA_HYBRID_ANC_1_OUT,
    ANC_DATA_SPK_L,
    ANC_DATA_SPK_R,
    ANC_DATA_MUSIC_L,
    ANC_DATA_MUSIC_R,
    ANC_DATA_TALK_MIC,

    ANC_DATA_MAX,
};

//位宽
enum {
    ANC_TOOL_PCM_BITS_16 = 0x0,
    ANC_TOOL_PCM_BITS_24,
};

//采样率
enum {
    ANC_TOOL_PCM_SPR_4000 = 0x0,
    ANC_TOOL_PCM_SPR_8000,
    ANC_TOOL_PCM_SPR_11025,
    ANC_TOOL_PCM_SPR_12000,
    ANC_TOOL_PCM_SPR_16000,
    ANC_TOOL_PCM_SPR_22050,
    ANC_TOOL_PCM_SPR_24000,
    ANC_TOOL_PCM_SPR_32000,
    ANC_TOOL_PCM_SPR_36000,
    ANC_TOOL_PCM_SPR_44100,
    ANC_TOOL_PCM_SPR_48000,
    ANC_TOOL_PCM_SPR_88200,
    ANC_TOOL_PCM_SPR_96000,
    ANC_TOOL_PCM_SPR_176400,
    ANC_TOOL_PCM_SPR_192000,
    ANC_TOOL_PCM_SPR_352800,
    ANC_TOOL_PCM_SPR_384000,
};

//控制指令
enum {
    ANC_TOOL_CMD_START_STREAM = 0x0001,
    ANC_TOOL_CMD_STOP_STREAM,
};

//指令ACK
enum {
    ANC_TOOL_CMD_ACK_OK = 0x0,
    ANC_TOOL_CMD_ACK_UNDEFINED_ID,
    ANC_TOOL_CMD_ACK_CMD_ERROR,
    ANC_TOOL_CMD_ACK_SPROF_ID_ERROR,
    ANC_TOOL_CMD_ACK_CRC_ERROR,
};

enum {
#if ANC_TOOL_LINK_FB_MSC_SIMU_EN
    ANC_TOOL_LINK_FB_MSC_SIMU_IDX,
#endif // ANC_TOOL_LINK_FB_MSC_SIMU_EN

    ANC_TOOL_LINK_MAX,
};

enum {
    ANC_TOOL_DATA_FUNCTION_FB_MSC_SIMU = 0x01,
};

enum {
    ANC_TOOL_ANC_MODE_OFF,
    ANC_TOOL_ANC_MODE_ON,
};

typedef struct {
    u32 data_types;
    u32 samples;
    u32 pcm_spr;
    u8 pcm_bits;
    u8 pcm_nch;
    u8 path_type;   //0:HUART，1:SPP
    u16 dma_start_delay;
} anc_tool_cb_t;

typedef struct {
    u32 data_types;
    u32 samples;
    u32 support_bits;
    u32 support_spr;
    u8 path_type;   //0:HUART，1:SPP
    u8 data_function;
    u8 anc_mode;
    u16 dma_start_delay;
} anc_tool_link_cfg_cb_t;



volatile u8 anc_tool_dump_sta;
u8 anc_tool_path_type;
u8 anc_tool_data_function;
u16 anc_tool_tick_process_period = 0;
u32 anc_tool_tick = 0;
u32 anc_tool_todo_flag = 0;
u8 anc_tool_tx_buf[12] AT(.anc_tool_buf.dump_buf);
anc_tool_cb_t anc_tool_cb AT(.anc_tool_buf.cb);
#if ANC_DS_DMA_EN
static anc_dma_cfg_cb anc_dma_cfg AT(.anc_tool_buf.cfg);
#endif // ANC_DS_DMA_EN
u8 anc_dump_buf[96 * 1024] AT(.anc_tool_buf.dump_buf);
u32 anc_dump_ptr AT(.anc_tool_buf.ptr);


AT(.rodata.anc_tool)
const anc_tool_link_cfg_cb_t anc_tool_link_cfg_tbl[ANC_TOOL_LINK_MAX] = {
#if ANC_TOOL_LINK_FB_MSC_SIMU_EN
    [ANC_TOOL_LINK_FB_MSC_SIMU_IDX] = {
        .data_types = BIT(ANC_DATA_FB_0_MIC) | BIT(ANC_DATA_SPK_L),
        .samples = 256,
        .support_bits = BIT(ANC_TOOL_PCM_BITS_16),
        .support_spr = BIT(ANC_TOOL_PCM_SPR_16000),
        .path_type = 1,
        .data_function = ANC_TOOL_DATA_FUNCTION_FB_MSC_SIMU,
        .anc_mode = ANC_TOOL_ANC_MODE_OFF,
        .dma_start_delay = 40,
    },
#endif // ANC_TOOL_LINK_FB_MSC_SIMU_EN
};

#if ANC_DS_DMA_EN
AT(.rodata.anc_tool)
const u8 anc_dma_channel_tbl[] = {
    [ANC_DATA_FF_0_MIC] = ANC_DS_DATA_ADC0_ANC_DOUT,
    [ANC_DATA_FB_0_MIC] = ANC_DS_DATA_ADC1_ANC_DOUT,
    [ANC_DATA_FF_1_MIC] = ANC_DS_DATA_ADC2_ANC_DOUT,
    [ANC_DATA_FB_1_MIC] = ANC_DS_DATA_ADC3_ANC_DOUT,
    [ANC_DATA_HYBRID_ANC_0_OUT] = ANC_DS_DATA_ANC0_FF_DOUT,
    [ANC_DATA_HYBRID_ANC_1_OUT] = ANC_DS_DATA_ANC1_FF_DOUT,
    [ANC_DATA_SPK_L] = ANC_DS_DATA_DACL_SPK,
    [ANC_DATA_SPK_R] = ANC_DS_DATA_DACR_SPK,
    [ANC_DATA_MUSIC_L] = ANC_DS_DATA_DACL_MUSIC,
    [ANC_DATA_MUSIC_R] = ANC_DS_DATA_DACR_MUSIC,
};
#endif // ANC_DS_DMA_EN

extern u32 __anc_tool_code_vma, __anc_tool_code_lma, __anc_tool_code_size;

uint calc_crc(void *buf, uint len, uint seed);
unsigned int s_bcnt(unsigned int rs1);
void audio_anc_tool_dump_process(void* buf, u32 samples);
int audio_anc_tool_dump_init(u16 data_type, u8 pcm_bits, u8 pcm_spr, u8 path_type);
void anc_add_to_music_enable(bool enable);


#define ANC_TOOL_TODO_FLAG_SET(x)               {anc_tool_todo_flag |= BIT(x);}
#define ANC_TOOL_TODO_FLAG_CLR(x)               {anc_tool_todo_flag &= ~BIT(x);}


///加载ANC TOOL的代码段
void load_anc_tool_code(void)
{
    memcpy(&__anc_tool_code_vma, &__anc_tool_code_lma, (u32)&__anc_tool_code_size);
    TRACE("load_anc_tool_code: %x, %x, %x\n", (u32)&__anc_tool_code_vma, (u32)&__anc_tool_code_lma, (u32)&__anc_tool_code_size);
}

#if ANC_TOOL_SPP_EN
///SPP TX数据
AT(.anc_tool_text.dump.spp)
void spp_uni_dump_tx_do(void* buf, uint32_t len)
{
//    printf("SPP TX: %d\n", len);
//    print_r(buf, len);
    bt_spp_tx(SPP_SERVICE_CH0, buf, len);
}

///SPP建立连接的回调函数
void anc_tool_spp_connect_callback(void)
{
    TRACE("anc_tool_spp_connect_callback\n");
}

///SPP断开连接的回调函数
void anc_tool_spp_disconnect_callback(void)
{
    TRACE("anc_tool_spp_disconnect_callback\n");
    ANC_TOOL_TODO_FLAG_SET(ANC_TOOL_TODO_SPP_DISCONNECT);
}
#endif // ANC_TOOL_SPP_EN

#if ANC_DS_DMA_EN
///ANC DMA数据process
AT(.anc_tool_text.anc_dma.process)
void anc_tool_anc_dma_process(u8* ptr, u32 samples, int ch_mode)
{
    if (anc_tool_dump_sta) {
        if (anc_tool_cb.dma_start_delay) {
            anc_tool_cb.dma_start_delay--;
            return;
        }
//        printf("ptr %08x samples %d ch_mode %d\n", ptr, samples, ch_mode);
        if (anc_dump_ptr < (24 * 1024)) {
            u8 nch = 1;
            if (ch_mode & BIT(0)) {
                nch = 2;
            }
            u8 pcm_bytes = 2;
            s16* dst = (s16*)&anc_dump_buf[anc_dump_ptr * nch * pcm_bytes];
            memcpy(dst, ptr, samples * nch * sizeof(s16));
            anc_dump_ptr += samples;
        } else if (anc_dump_ptr == (24 * 1024)) {
            ANC_TOOL_TODO_FLAG_SET(ANC_TOOL_TODO_FB_MSC_SIMU_DATA_FULL);
            anc_dump_ptr++;
        }
    }
}

static u16 data_types_to_anc_dma_channel(u32 data_types)
{
    u8 nch = 0;
    u16 dma_channel = 0;
    for (int i = 0; i < ANC_DATA_MAX; i++) {
        if (data_types & BIT(i)) {
            if (i != ANC_DATA_TALK_MIC) {
                dma_channel |= (anc_dma_channel_tbl[i] << (nch * 8));
                nch++;
                if (nch >= 2) {
                    break;
                }
            }
        }
    }
    TRACE("%s %x\n", __func__, dma_channel);
    return dma_channel;
}
#endif // ANC_DS_DMA_EN

static u8 anc_tool_pcm_spr_to_audio_spr(u8 anc_tool_pcm_spr)
{
#define ANC_TOOL_PCM_SPR_CASE(x)          \
        case ANC_TOOL_PCM_SPR_##x:        \
        spr = SPR_##x;                    \
        break;

    u8 spr = SPR_16000; //默认16K
    switch (anc_tool_pcm_spr) {
        ANC_TOOL_PCM_SPR_CASE(4000);
        ANC_TOOL_PCM_SPR_CASE(8000);
        ANC_TOOL_PCM_SPR_CASE(11025);
        ANC_TOOL_PCM_SPR_CASE(12000);
        ANC_TOOL_PCM_SPR_CASE(16000);
        ANC_TOOL_PCM_SPR_CASE(22050);
        ANC_TOOL_PCM_SPR_CASE(24000);
        ANC_TOOL_PCM_SPR_CASE(32000);
        ANC_TOOL_PCM_SPR_CASE(44100);
        ANC_TOOL_PCM_SPR_CASE(48000);
        ANC_TOOL_PCM_SPR_CASE(88200);
        ANC_TOOL_PCM_SPR_CASE(96000);
        ANC_TOOL_PCM_SPR_CASE(176400);
        ANC_TOOL_PCM_SPR_CASE(192000);
        ANC_TOOL_PCM_SPR_CASE(352800);
        ANC_TOOL_PCM_SPR_CASE(384000);
    };
    TRACE("%s %d\n", __func__, spr);
    return spr;
}

///启动dump数据链路
void bsp_anc_tool_dump_start(u32 link, u8 pcm_spr, u8 pcm_bits)
{
    if (anc_tool_dump_sta) {
        return;
    }

    const anc_tool_link_cfg_cb_t* p = &anc_tool_link_cfg_tbl[link];

    load_anc_tool_code();

    anc_dump_ptr = 0;
    memset(&anc_tool_cb, 0, sizeof(anc_tool_cb_t));

    sys_clk_req(INDEX_ANC, SYS_120M);

    audio_anc_tool_dump_init(p->data_types, pcm_bits, pcm_spr, 1);

#if ANC_EN
    bsp_anc_set_mode(1);
#endif // ANC_EN

    if (p->anc_mode == ANC_TOOL_ANC_MODE_OFF) {
        anc_add_to_music_enable(0);
    }

#if ANC_DS_DMA_EN
    memset(&anc_dma_cfg, 0, sizeof(anc_dma_cfg_cb));
    anc_dma_cfg.bits_mode = pcm_bits ? 0 : 1;
    anc_dma_cfg.callback = anc_tool_anc_dma_process;
    anc_dma_cfg.samples = p->samples;
    anc_dma_cfg.sample_rate = anc_tool_pcm_spr_to_audio_spr(pcm_spr);
    anc_dma_cfg.channel = data_types_to_anc_dma_channel(p->data_types);
    anc_dma_cfg.dig_gain = 0;
    anc_dma_cfg.resv = 0;
    anc_dma_start(&anc_dma_cfg, 1);
#endif // ANC_DS_DMA_EN

    anc_tool_cb.data_types = p->data_types;
    anc_tool_cb.pcm_nch = s_bcnt(p->data_types);
    anc_tool_cb.pcm_bits = pcm_bits;
    anc_tool_cb.pcm_spr = pcm_spr;
    anc_tool_cb.samples = p->samples;
    anc_tool_cb.path_type = p->path_type;
    anc_tool_cb.dma_start_delay = p->dma_start_delay;

    anc_tool_dump_sta = 1;

    TRACE("data_types:       %d\n", anc_tool_cb.data_types);
    TRACE("pcm_nch:          %d\n", anc_tool_cb.pcm_nch);
    TRACE("pcm_bits:         %d\n", anc_tool_cb.pcm_bits);
    TRACE("pcm_spr:          %d\n", anc_tool_cb.pcm_spr);
    TRACE("samples:          %d\n", anc_tool_cb.samples);
    TRACE("path_type:        %d\n", anc_tool_cb.path_type);
    TRACE("dma_start_delay:  %d\n", anc_tool_cb.dma_start_delay);
    TRACE("%s\n", __func__);
}

///关闭dump数据链路
void bsp_anc_tool_dump_stop(void)
{
    if (anc_tool_dump_sta == 0) {
        return;
    }

#if ANC_DS_DMA_EN
    anc_dma_exit();
#endif // ANC_DS_DMA_EN

#if ANC_EN
    bsp_anc_set_mode(0);
#endif // ANC_EN

    sys_clk_free(INDEX_ANC);

    anc_tool_dump_sta = 0;

    TRACE("%s\n", __func__);
}

///ANC TOOL主线程todo process
NO_INLINE void anc_tool_todo_process_do(void)
{
    if (anc_tool_todo_flag & BIT(ANC_TOOL_TODO_SPP_DISCONNECT)) {
        ANC_TOOL_TODO_FLAG_CLR(ANC_TOOL_TODO_SPP_DISCONNECT);
        bsp_anc_tool_dump_stop();
        anc_tool_data_function = 0;
        anc_tool_tick = 0;
        anc_tool_tick_process_period = 0;
    }
#if ANC_TOOL_LINK_FB_MSC_SIMU_EN
    if (anc_tool_todo_flag & BIT(ANC_TOOL_TODO_FB_MSC_SIMU_DATA_FULL)) {
        ANC_TOOL_TODO_FLAG_CLR(ANC_TOOL_TODO_FB_MSC_SIMU_DATA_FULL);
        bsp_anc_tool_dump_stop();
        anc_dump_ptr = 0;
        anc_tool_tick_process_period = 20;
    }
#endif // ANC_TOOL_LINK_FB_MSC_SIMU_EN
}

///ANC TOOL主线程tick process
NO_INLINE void anc_tool_tick_process_do(void)
{
//    printf("anc_tool_tick %d\n", anc_tool_tick);
#if ANC_TOOL_LINK_FB_MSC_SIMU_EN
    if (anc_tool_data_function == ANC_TOOL_DATA_FUNCTION_FB_MSC_SIMU) {
        u8 nch = anc_tool_cb.pcm_nch;
        u32 samples = 32;
        u8 pcm_bytes = 2;
        if (anc_tool_cb.pcm_bits == ANC_TOOL_PCM_BITS_24) {
            pcm_bytes = 3;
        }
        s16* ptr = (s16*)&anc_dump_buf[anc_dump_ptr * nch * pcm_bytes];
        audio_anc_tool_dump_process(ptr, samples);
        anc_dump_ptr += samples;
    	if (anc_dump_ptr >= (24 * 1024)) {
            anc_tool_tick = 0;
            anc_tool_tick_process_period = 0;
            printf("ANC TOOL: FB MSC data upload end.\n");
        }
//        printf("anc_dump_ptr %d\n", anc_dump_ptr);
    }
#endif // ANC_TOOL_LINK_FB_MSC_SIMU_EN
}

///ANC TOOL主线程process
AT(.com_text.anc_tool)
void anc_tool_process(void)
{
    if (anc_tool_todo_flag) {
        anc_tool_todo_process_do();
    }
    if (anc_tool_tick_process_period) {
        if (tick_check_expire(anc_tool_tick, anc_tool_tick_process_period)) {
            anc_tool_tick_process_do();
            anc_tool_tick = tick_get();
        }
    }
}

///判断是否是上位机指令
AT(.com_text.anc_tool)
bool bsp_anc_tool_rx_done(u8* rx_buf, u8 type)
{
    if ((rx_buf[0] == 'B') && (rx_buf[1] == 'T') && (rx_buf[2] == 'A') && (rx_buf[3] == 'N') && (rx_buf[4] == 'C')) {
        anc_tool_path_type = type;
        return true;
    }
    return false;
}

static void tx_ack(u8* packet, u16 len)
{
    if (anc_tool_path_type == 0) {
        huart_tx(packet, len);
    } else if (anc_tool_path_type == 1) {
#if ANC_TOOL_SPP_EN
        if (bt_get_status() >= BT_STA_CONNECTED) {
//            printf("SPP TX ACK:\n");
//            print_r(packet, len);
            bt_spp_tx(SPP_SERVICE_CH0, packet, len);
        }
#endif // ANC_TOOL_SPP_EN
    }
}

///上位机指令回复ACK
void bsp_anc_tool_cmd_tx_ack(u16 cmd, u8 ack)
{
    anc_tool_tx_buf[0] = 'B';
    anc_tool_tx_buf[1] = 'T';
    anc_tool_tx_buf[2] = 'A';
    anc_tool_tx_buf[3] = 'N';
    anc_tool_tx_buf[4] = 'C';
    anc_tool_tx_buf[5] = 12;
    anc_tool_tx_buf[6] = ANC_TOOL_SPROF_ID_CONTROL;
    anc_tool_tx_buf[7] = (u8)((cmd >> 0) & 0xFF);
    anc_tool_tx_buf[8] = (u8)((cmd >> 8) & 0xFF);
    anc_tool_tx_buf[9] = ack;

    u16 crc_cal = calc_crc(anc_tool_tx_buf, 10, ANC_TOOL_CRC_SEED);
    anc_tool_tx_buf[10] = (u8)((crc_cal >> 0) & 0xFF);
    anc_tool_tx_buf[11] = (u8)((crc_cal >> 8) & 0xFF);
    tx_ack(anc_tool_tx_buf, 12);
}

///上位机指令解析
void bsp_anc_tool_parse_cmd(void)
{
//    printf("bsp_anc_tool_parse_cmd:\n");
//    print_r(eq_rx_buf, EQ_BUFFER_LEN);

    u8* rx_buf = eq_rx_buf;

    if ((rx_buf[0] != 'B') || (rx_buf[1] != 'T') || (rx_buf[2] != 'A') || (rx_buf[3] != 'N') || (rx_buf[4] != 'C')) {
        return;
    }

    //长度
    u8 length = rx_buf[5];
//    printf("length %d\n", length);
    if (length == 0) {
        return;
    }

    //CRC
    u16 crc_rx = little_endian_read_16(rx_buf, length - 2);
    u16 crc_cal = calc_crc(rx_buf, length - 2, ANC_TOOL_CRC_SEED);
//    printf("crc_rx %x crc_cal %x\n", crc_rx, crc_cal);
    if (crc_rx != crc_cal) {
        printf("bsp_anc_tool_parse_cmd CRC ERROR!\n");
        return;
    }

    //子协议
    u8 sprof = rx_buf[6];
    if (sprof == ANC_TOOL_SPROF_ID_CONNECT) {

    } else if (sprof == ANC_TOOL_SPROF_ID_CONTROL) {
        u16 cmd_id = little_endian_read_16(rx_buf, 7);
        u8* cmd_buf = (u8*)&rx_buf[9];
//        printf("cmd %x\n", cmd_id);
        if (cmd_id == ANC_TOOL_CMD_START_STREAM) {
            if (!anc_tool_dump_sta) {       //未开始DUMP才能进行启动
                u8 data_function = cmd_buf[0];
//                u16 data_length = little_endian_read_16(cmd_buf, 1);
//                u32 data_type = little_endian_read_32(cmd_buf, 3);
                u8 pcm_bits = (cmd_buf[7] >> 4) & 0xF;
                u8 pcm_spr = cmd_buf[8];
//                printf("data_function %x data_length %x\n", data_function, data_length);
//                printf("data_type %x pcm_bits %x pcm_spr %x\n", data_type, pcm_bits, pcm_spr);
                bool res = false;
                int idx = 0;
                for (; idx < ANC_TOOL_LINK_MAX; idx++) {
                    if (anc_tool_link_cfg_tbl[idx].data_function == data_function) {
                        res = true;
                        anc_tool_data_function = data_function;
                        break;
                    }
                }
                if (res) {
                    bsp_anc_tool_cmd_tx_ack(ANC_TOOL_CMD_START_STREAM, ANC_TOOL_CMD_ACK_OK);
                    bsp_anc_tool_dump_start(idx, pcm_spr, pcm_bits);
                } else {
                    bsp_anc_tool_cmd_tx_ack(ANC_TOOL_CMD_START_STREAM, ANC_TOOL_CMD_ACK_CMD_ERROR);
                }
            }
        }
    }
}

#endif // ANC_TOOL_EN
