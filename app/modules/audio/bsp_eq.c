#include "include.h"
#include "bsp_eq.h"
#include "bsp_anc.h"

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


#define EQ_CRC_SEED                             0xffff
#define EQ_BAND_NUM                             12
#define CAL_FIX(x)		                        ((int)(x * (1 << 27)))

u8 eq_rx_buf[EQ_BUFFER_LEN];
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
eq_dbg_cb_t eq_dbg_cb;
static u8 eq_tx_buf[12];
const char tbl_eq_version[10] = {'E', 'Q', '1', '*', 6, 0, 12, 4, 5, 0};
const char tbl_drc_version[10] = {'D', 'R', '0', '*', 6, 0, 4, 0, 0, 0};
AT(.com_rodata.anc)
const char tbl_anc_header[3] = "ANC";

uint calc_crc(void *buf, uint len, uint seed);
extern const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN];
void music_set_pt_eq(u8 channel, u8 band_cnt, const u32 *param);
void music_dbb_eq_index_init(u32* coef_l, u32* coef_r);

typedef enum{
    EQ_RES,
    DRC_RES,
}RES_TYPE_SET;

u8 check_sum(u8 *buf, u16 size)
{
    u32 i, sum = 0;
    for (i = 0; i < size; i++) {
        sum += buf[i];
    }
    return (u8)(-sum);
}

void tx_ack(uint8_t *packet, uint16_t len)
{
    delay_5ms(1);   //延时一段时间再ack
    if (eq_dbg_cb.rx_type) {
#if EQ_DBG_IN_UART
        huart_tx(packet, len);
#endif
    } else {
#if EQ_DBG_IN_SPP && BT_SPP_EN
        if (xcfg_cb.eq_dgb_spp_en && (bt_get_status() >= BT_STA_CONNECTED)) {
            bt_spp_tx(SPP_SERVICE_CH0, packet, len);
        }
#endif
    }
    eq_dbg_cb.rx_type = 0;
}

void eq_tx_ack(u8 bank_num, u8 ack, u8 type)
{
    u8 ack_buf[4];
    if (type) {
        ack_buf[0] = 'D';
    } else {
        ack_buf[0] = 'A';
    }
    ack_buf[1] = bank_num;
    ack_buf[2] = ack;
    ack_buf[3] = check_sum(ack_buf, 3);
//    ack_buf[3] = -(ack_buf[0] + ack_buf[1] + ack_buf[2]);
    tx_ack(ack_buf, 4);
}

void eq_tx_version(u8 type)
{
    if (type) {
        memcpy(eq_tx_buf, tbl_drc_version, 10);
    } else {
        memcpy(eq_tx_buf, tbl_eq_version, 10);
    }
#if ANC_EN
    if (xcfg_cb.anc_en) {
        eq_tx_buf[9] |= BIT(0);
    }
#endif
    u16 crc = calc_crc(eq_tx_buf, 10, EQ_CRC_SEED);
    eq_tx_buf[10] = crc;
    eq_tx_buf[11] = crc >> 8;
    tx_ack(eq_tx_buf, 12);
}

#if ANC_EQ_RES2_EN
rmk_cb_t rmk_cb;
u8 res2_eq_buf[MAX_RESBUF_SIZE] AT(.eq_rec_buf);  //载入res2参数的缓存buf
void eq_tx_setparam_res(u8 res)
{
    eq_tx_buf[0] = 'E';
    eq_tx_buf[1] = 'Q';
    eq_tx_buf[2] = 'S';
    eq_tx_buf[3] = 'P';
    eq_tx_buf[4] = res;
    eq_tx_buf[5] = rmk_cb.err;
    tx_ack(eq_tx_buf, 6);
}
#endif

#if ANC_EN
#define FFFB_EQ_NAME_NUM        8
static const char anc_fffb_eq_name[FFFB_EQ_NAME_NUM][13] = {
    "L0_NOS.EQ",
    "L0_MSC.EQ",
    "R0_NOS.EQ",
    "R0_MSC.EQ",
    "TP_L0_NOS.EQ",
    "TP_L0_MSC.EQ",
    "TP_R0_NOS.EQ",
    "TP_R0_MSC.EQ",
};

#define HYBRID_EQ_NAME_NUM      16
static const char anc_hybrid_eq_name[HYBRID_EQ_NAME_NUM][13] = {
    "L0_NOS.EQ",
    "L0_MSC.EQ",
    "L1_NOS.EQ",
    "L1_MSC.EQ",
    "R0_NOS.EQ",
    "R0_MSC.EQ",
    "R1_NOS.EQ",
    "R1_MSC.EQ",
    "TP_L0_NOS.EQ",
    "TP_L0_MSC.EQ",
    "TP_L1_NOS.EQ",
    "TP_L1_MSC.EQ",
    "TP_R0_NOS.EQ",
    "TP_R0_MSC.EQ",
    "TP_R1_NOS.EQ",
    "TP_R1_MSC.EQ",
};

bool anc_eq_name_compare(const char *eq_name)
{
    u32 i, num;
    const char (*anc_name)[13];

    if (xcfg_cb.anc_mode >= MODE_TWS_HYBRID) {
        num = HYBRID_EQ_NAME_NUM;
        anc_name = anc_hybrid_eq_name;
    } else {
        num = FFFB_EQ_NAME_NUM;
        anc_name = anc_fffb_eq_name;
    }
    eq_dbg_cb.anc_packet = 0;
    for (i = 0; i < num; i++) {
        if (strcmp(anc_name[i], eq_name) == 0) {
            eq_dbg_cb.anc_packet = i + 1;
            return true;
        }
    }
    TRACE("eq name fits nothing\n");
    return false;
}

#endif

#if ANC_EN
static bool bsp_eq_tws_sync_param(void)
{
#if BT_TWS_EN
    u8 channel, anc_packet = eq_dbg_cb.anc_packet - 1;
    if (xcfg_cb.anc_mode >= MODE_TWS_HYBRID) {
        channel = (anc_packet / 4) % 2;     //右声道
    } else {
        channel = (anc_packet / 2) % 2;
    }

    if (xcfg_cb.bt_tws_en && (channel == sys_cb.tws_left_channel)) {    //声道不相等，转发
        if(bt_tws_is_connected()) {
            bt_tws_sync_eq_param();
        }
        return true;
    }
#endif
    return false;
}
#endif

#if ANC_EQ_RES2_EN
/*****************************************************************************
 * 功能   : 将flash中res2对应地址的4k字节内容，读取、放到*buf地址
 *****************************************************************************/
void rmk_read_res2(u8 *buf)
{
    u32 base_addr = FLASH_SIZE - 0x6000;
    os_spiflash_read(buf, base_addr, MAX_RESBUF_SIZE);
}

/*****************************************************************************
 * 功能   : 擦除*buf地址的长度为len字节的内容，然后初始化
 *****************************************************************************/
void rmk_rec_res2(u8 *buf, u16 len)
{
    u32 base_addr = FLASH_SIZE - 0x6000;
    os_spiflash_erase(base_addr);
    for (u16 i = 0; i < len; i += 256) {
        os_spiflash_program(buf + i, base_addr + i, 256);
    }
//	  DEBUG
//    os_spiflash_read(res2_test_buf, base_addr, 128);
//    print_r(res2_test_buf, 128);
}

bool rmk_check_single_res_is_valid(u8 *entry)
{
    u32 res_addr = GET_LE32(entry + 24);
    u32 res_len = GET_LE32(entry + 28);
	rmk_cb.err = 0;
    if (entry[0] == 0) {
        rmk_cb.err = RMK_ERR_RES_ENTRY_NAME;
        TRACE("RMK_ERR_RES_ENTRY_NAME");
        return false;
    }
    if (res_addr < RES_BASE || res_addr > RES_END) {
        rmk_cb.err = RMK_ERR_RES_ENTRY_ADDR;
        TRACE("RMK_ERR_RES_ENTRY_ADDR");
        return false;
    }
    if (res_len < 0 || res_len > MAX_RESBUF_SIZE || (res_addr + res_len) > RES_END) {
        rmk_cb.err = RMK_ERR_RES_ENTRY_LEN;
        TRACE("RMK_ERR_RES_ENTRY_LEN");
        return false;
    }
    return true;
}

/*****************************************************************************
 * 功能   : 将flash中res2对应地址的4k字节内容，读取、放到*res指向区域
 *****************************************************************************/
bool rmk_init(u8 *res, u32 len)
{
    u8 *resbuf = res;
    u32 i;
    u32 res_addr = 0;
    u32 res_len = 0;
    memset(&rmk_cb, 0, sizeof(rmk_cb_t));
    rmk_cb.len = len;
    if (len < 32 || len > MAX_RESBUF_SIZE) {
        rmk_cb.err = RMK_ERR_RES_LEN;
        TRACE("rmk_init RMK_ERR_RES_LEN\n");
        return false;
    }
    memcpy(resbuf, res, len);
    if (GET_LE32(resbuf) != 0xD2D4CEC5) {      //ENTRY HEADER
        rmk_cb.err = RMK_ERR_RES_ENTRY_SIG;
        TRACE("RMK_ERR_RES_ENTRY_SIG");
        return false;
    }
    rmk_cb.rescnt = GET_LE32(resbuf + 28);     //0x20
    for (i = 0; i < rmk_cb.rescnt; i++) {
        u8 *entry = resbuf + (i + 1) * 32;
        res_addr = GET_LE32(entry + 24);
        res_len = GET_LE32(entry + 28);
        if(!rmk_check_single_res_is_valid(entry)){
            return false;
        }
    }
    rmk_cb.res2len = res_addr + res_len - RES_BASE;
    return true;
}

/*********************************************************************************
 * 功能   : 若有res2参数，则存放到res2_eq_buf中；否则擦除、重新初始化4k flash段
 *********************************************************************************/
void copy_res2flash(void)
{
    TRACE("res2 copy res2flash\n");
    rmk_read_res2(res2_eq_buf);
    if (rmk_init(res2_eq_buf, MAX_RESBUF_SIZE)) {
        TRACE("reflash res2 4k flash\n");
        return;
    }
    memcpy(res2_eq_buf, (u8*)RES_BUF_RES2_BIN, RES_LEN_RES2_BIN);
    rmk_rec_res2(res2_eq_buf, RES_LEN_RES2_BIN);
}

/*********************************************************************************
 * 功能   : 将eq下载指令的参数，写入res2 对应flash的eq文件中
 *********************************************************************************/
//将EQ文件整合入RES2文件
bool rmk_set_eq(u8 *eqbuf)
{
    TRACE("rmk_set_eq\n");
    rmk_cb.buf = res2_eq_buf;
    u8 *resbuf = rmk_cb.buf;

    int i;
    int eq_offest = 0;
    rmk_read_res2(resbuf);
    if (!(rmk_cb.eqlen && rmk_cb.res2len)) {
        return false;
    }

    for (i = rmk_cb.file_cnt; i < rmk_cb.rescnt; i++) {
        unsigned char *entry = resbuf + (i + 1) * 32;
        int res_addr = GET_LE32(entry + 24);
        int res_len = GET_LE32(entry + 28);
        if(!rmk_check_single_res_is_valid(entry)){
            return false;
        }
        if (i == rmk_cb.file_cnt) {
            u16 temp_len = rmk_cb.eqlen;
            u32 res_raddr = res_addr - RES_BASE;
            rmk_cb.eqlen = (u16)(rmk_cb.eqlen + 31)/32*32;//32byte对齐
            memset(eqbuf + temp_len, 0, rmk_cb.eqlen - temp_len);
            if (!bsp_anc_res_check_crc((u32)eqbuf, (u32)rmk_cb.eqlen)) {
                TRACE("rmk_cb.err RMK_ERR_RES_CRC_ERR\n");
                rmk_cb.err = RMK_ERR_RES_CRC_ERR;
                return false;
            }
            eq_offest = eq_offest + rmk_cb.eqlen - res_len;
            //如果填入的资源比原先的资源小，把资源前移
            if (eq_offest <= 0) {
                memcpy(resbuf + res_raddr + rmk_cb.eqlen, resbuf + res_raddr + res_len, rmk_cb.res2len - res_raddr - res_len);
            } else {
                //如果填入的资源比原先的资源多，把资源后移
                u32 copy_len =rmk_cb.res2len - (res_raddr + res_len);
                u16 j = rmk_cb.res2len + eq_offest-1;
                u16 k = rmk_cb.res2len - 1;
                for(u16 z = copy_len; z > 0; z-- ){
                    resbuf[j--] = resbuf[k--];
                }
            }
            memcpy(resbuf + res_raddr, eqbuf, rmk_cb.eqlen);
            res_len = rmk_cb.eqlen;

        } else {
            res_addr += eq_offest;
        }
        PUT_LE32(entry + 24, res_addr);
        PUT_LE32(entry + 28, res_len);
    }

#if TRACE_EN
    TRACE("\n\n\n");
    TRACE("res2 successfully keep data result:\n");
    print_r(resbuf, 4096);
#endif // TRACE_EN
    rmk_rec_res2(resbuf, MAX_RESBUF_SIZE);
    rmk_cb.res2len = 0;
    rmk_cb.eqlen = 0;
    return true;
}

/*****************************************************************************************
 * 功能   : 收到产测指令EQSFM后，检查res2 4k flash内容是否正常，更新eqlen、file_cnt等参数
 *****************************************************************************************/
bool rmk_save_file_param(u8 *buf)
{
    rmk_cb.buf = res2_eq_buf;
    unsigned char *resbuf = rmk_cb.buf;
    int i;
    rmk_cb.eqlen = 0;
    rmk_cb.file_cnt = 0;
    rmk_read_res2(resbuf);                              //把res2对应flash内容读到resbuf中
    if(!rmk_init(resbuf, MAX_RESBUF_SIZE)){             //刷新4kflash是不是合法的,里面包含默认信息
        return false;
    }

    TRACE("rmk save file param rmk init succeddfully\n");
    for (i = 0; i < rmk_cb.rescnt; i++) {
        unsigned char *entry = resbuf + (i + 1) * 32;
        if (!rmk_check_single_res_is_valid(entry)) {
            return false;
        }
        if (!memcmp(buf, entry, 24)) {                     //字符串和默认信息匹配，确认eq文件序列数和包含滤波器字节数
            rmk_cb.eqlen = (u16)GET_LE32(buf + 24);
            rmk_cb.file_cnt = i;
            return true;
        }
    }
    return false;
}
#endif

void bsp_eq_parse_cmd(void)
{
    RES_TYPE_SET set_type = 0;
#if TRACE_EN
//    print_r(eq_rx_buf, EQ_BUFFER_LEN);
#endif
    if (eq_rx_buf[0] == 'E' && eq_rx_buf[1] == 'Q') {
        set_type = EQ_RES;
    } else if (eq_rx_buf[0] == 'D' && eq_rx_buf[1] == 'R') {
        set_type = DRC_RES;
    } else {
        return;
    }
    if (eq_rx_buf[2] == '?' && eq_rx_buf[3] == '#') {
        eq_tx_version(set_type);
        return;
    } else if (eq_rx_buf[2] == '0' && eq_rx_buf[3] == ':') {
#if ANC_EN
        if (anc_eq_name_compare((const char *)&eq_rx_buf[6])) {
            if (!sys_cb.anc_start || sys_cb.anc_user_mode != 1) {
                bsp_anc_set_mode(1);
            }
            bsp_anc_fade_enable(0);
            bsp_eq_tws_sync_param();
        }
#endif
        eq_tx_ack(0, 0, set_type);
        return;
    } else if (eq_rx_buf[2] == 'S' && (eq_rx_buf[3] == 'P' || eq_rx_buf[3] == 'F')) {
#if ANC_EQ_RES2_EN
        u8 res = 1;
        bt_audio_bypass();
        if ((eq_rx_buf[5] == 'E') && (eq_rx_buf[6] == 'q')) {
            res = rmk_set_eq(eq_rx_buf + 5) ? 0 : 1;
        } else if ((eq_rx_buf[5] == 'F') && (eq_rx_buf[6] == 'M')) {
            res = rmk_save_file_param(eq_rx_buf + 7) ? 0 : 1;
        }
        bt_audio_enable();
        if (!eq_rx_buf[4] && (!res)) {
            eq_rx_buf[4] = 1;
            bt_tws_sync_eq_param();
        }
        eq_tx_setparam_res(res);
#endif
        return ;
    } else if (eq_rx_buf[2] == 'S' && (eq_rx_buf[3] == 'M')) {
#if ANC_EQ_RES2_EN
        u8 mode = eq_rx_buf[8];
        rmk_cb.err = 0;
        if (mode > 2) {
            eq_tx_setparam_res(1);
            return;
        }
        sys_cb.anc_user_mode = mode;
        bsp_anc_set_mode(sys_cb.anc_user_mode);
        if (!eq_rx_buf[4]) {
            eq_rx_buf[4] = 1;
            bt_tws_sync_eq_param();
        }
        eq_tx_setparam_res(0);
        return;
#endif
#if DAC_PT_EN
    } else if (eq_rx_buf[2] == 'S' && (eq_rx_buf[3] == 'D') && (eq_rx_buf[4] == 'A') && (eq_rx_buf[5] == 'C')) {
        u8 band_num = eq_rx_buf[7];                                                 //eq 滤波器数量
        u8 channel  = eq_rx_buf[8];                                                 //左右声道标志位：1->左声道;2->右声道;3->验双声道效果
        u32 crc_rx  = calc_crc(eq_rx_buf, (9 + band_num*4*5), EQ_CRC_SEED);
        TRACE("dac pt rx_buf, band_num [%d], ch [%d], crc [%4x]\n", band_num, channel, crc_rx);
        if (crc_rx == little_endian_read_16(eq_rx_buf, (9 + band_num*4*5))) {      //crc16校验
            eq_tx_ack(band_num, 1, set_type);                                      //校验成功，发送ack1
            TRACE("dac pt rx_buf crc check successfully\n");
        } else {
            eq_tx_ack(band_num, 0, set_type);                                      //校验失败，发送ack0
            TRACE("dac pt rx_buf crc err, correct code :%4x\n", little_endian_read_16(eq_rx_buf, (12 + band_num*4*5)));
            return;
        }
        if (eq_rx_buf[6] == 'T') {                                                  //设置eq参数听效果
            if (channel == 1 || channel == 2) {
                eq_tx_ack(band_num, channel, set_type);
                TRACE("dac pt SDACT cmd ch check successfully\n");
            } else {
                eq_tx_ack(band_num, 0, set_type);
                TRACE("dac pt SDACT cmd ch check err, %d\n", channel);
                return;
            }
            music_set_pt_eq(channel-1, band_num, (u32 *)&eq_rx_buf[9]);
        } else if (eq_rx_buf[6] == 'R') {                                           //烧录eq参数
            u8 error_code = 0;
            if (channel == 1 || channel == 2) {
                u8 dac_ch = channel - 1;
                cm_write8(PARAM_DAC_PT_NUM + dac_ch, band_num);
                cm_write(&eq_rx_buf[9], DAC_PT_PAGE(dac_ch * 120), band_num * 4 * 5);
                cm_sync();
                error_code = channel;
                TRACE("dac pt SDACR cmd cm_write successfully\n");
            } else if (channel == 3) {
                plugin_music_eq();
                error_code = channel;
                TRACE("dac pt SDACR cmd plugin_music_eq\n");
            } else {
                error_code = 4;
                TRACE("dac pt SDACR cmd err\n");
            }
            eq_tx_ack(band_num, error_code, set_type);
        }
        return;
#endif
    }

    u8 band_num = eq_rx_buf[6];
    u32 size = little_endian_read_16(eq_rx_buf, 4);
    u32 crc = calc_crc(eq_rx_buf, size+4, EQ_CRC_SEED);
#if ANC_EN
    if (eq_dbg_cb.anc_packet && bsp_eq_tws_sync_param()) {
        delay_5ms(20);              //wait tws send complete
        eq_tx_ack(band_num, 0, set_type);
        return;
    }
#endif
    if (crc == little_endian_read_16(eq_rx_buf, 4+size)) {
        eq_tx_ack(band_num, 0, set_type);
    } else {
        eq_tx_ack(band_num, 1, set_type);
        return;
    }
    u8 band_cnt = eq_rx_buf[7];
#if ANC_EN
    if (eq_dbg_cb.anc_packet) {
        bsp_anc_dbg_eq_param(eq_dbg_cb.anc_packet, band_cnt, (u32 *)&eq_rx_buf[14]);
    } else
#endif
    {
        if (set_type) {
//            music_set_drc(band_cnt, (u32 *)&eq_rx_buf[10]);
        } else {
#if BT_TWS_EN
            if (!bt_tws_is_slave()) {
                eq_rx_buf[0] = 'E';
                eq_rx_buf[1] = 'Q';
                bt_tws_sync_eq_param();
            }
#endif
            music_set_eq(band_cnt, (u32 *)&eq_rx_buf[14]);
        }
    }

#if (UART0_PRINTF_SEL != PRINTF_NONE)
    u8 k;
    u32 *ptr = (u32 *)&eq_rx_buf[10];
    if (set_type) {
        for (k = 0; k < 10; k++) {
            printf("%08x", *ptr++);
            if (k % 5 == 4) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    } else {
        printf("%08x\n", little_endian_read_32(eq_rx_buf, 14));
        ptr = (u32 *)&eq_rx_buf[18];
        for (k = 0; k < band_cnt*5; k++) {
            printf("%08x", *ptr++);
            if (k % 5 == 4) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
#endif
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
}

#if BT_TWS_EN
uint16_t tws_get_spp_eq_info(uint8_t *buf)
{
    if (buf != NULL) {
        memcpy(buf, eq_rx_buf, EQ_BUFFER_LEN);
    }
//    printf("get_spp_eq: %d\n", EQ_BUFFER_LEN);
    return EQ_BUFFER_LEN;
}

void tws_set_spp_eq_info(uint8_t *buf, uint16_t len)
{
    if (buf != NULL) {
        memcpy(eq_rx_buf, buf, len);
        if (memcmp(eq_rx_buf, tbl_anc_header, 3) == 0) {
            msg_enqueue(EVT_ONLINE_SET_ANC);
        } else {
            msg_enqueue(EVT_ONLINE_SET_EQ);
        }
    }
 //   printf("set_spp_eq: %d\n", len);
}
#endif

AT(.com_huart.text)
u8 bsp_eq_rx_done(u8* rx_buf)
{
    if (memcmp(rx_buf, tbl_anc_header, 3) == 0) {
        msg_enqueue(EVT_ONLINE_SET_ANC);
        eq_dbg_cb.rx_type = 1;
    } else if(((rx_buf[0]=='E')&&(rx_buf[1]=='Q')) || (rx_buf[0] == 'D' && rx_buf[1] == 'R')){
        msg_enqueue(EVT_ONLINE_SET_EQ);
        eq_dbg_cb.rx_type = 1;
    }
#if BT_MUSIC_EFFECT_DBG_EN
    else if ((rx_buf[0]=='C')&&(rx_buf[1]=='A')&&(rx_buf[2]=='R')&&(rx_buf[3]=='_')) {
        msg_enqueue(EVT_ONLINE_SET_EFFECT);
        eq_dbg_cb.rx_type = 1;
    }
#endif
    return eq_dbg_cb.rx_type;
}

void eq_dbg_init(void)
{
    memset(&eq_dbg_cb, 0, sizeof(eq_dbg_cb_t));
}
#endif


AT(.text.music)
void music_set_eq_by_num(u8 num)
{
    if (num > (MUSIC_EQ_TBL_LEN - 1)) {
        return;
    }

#if EQ_APP_EN
    eq_msc_index_init(0);
#endif

#if BT_MUSIC_EFFECT_SPATIAL_AU_EN && BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN
    if (music_spatial_audio_get_fix_eq_sta()) {
        music_set_eq_by_res(RES_BUF_EQ_SPATIAL_AUDIO_EQ, RES_LEN_EQ_SPATIAL_AUDIO_EQ);
        return;
    }
#endif

#if BT_MUSIC_EFFECT_HRTF_RT_EN && BT_MUSIC_EFFECT_HRTF_RT_EQ_EN
    if (music_hrtf_rt_get_fix_eq_sta()) {
        music_set_eq_by_res(RES_BUF_EQ_SPATIAL_AUDIO_EQ, RES_LEN_EQ_SPATIAL_AUDIO_EQ);
        return;
    }
#endif

    music_set_eq_by_res(*music_eq_tbl[num].addr, *music_eq_tbl[num].len);
}

#if EQ_MODE_EN
AT(.text.music)
void sys_set_eq(void)
{
    sys_cb.eq_mode++;
    if (sys_cb.eq_mode > 5) {
        sys_cb.eq_mode = 0;
    }
    music_set_eq_by_num(sys_cb.eq_mode);
    gui_box_show_eq();
}
#endif // EQ_MODE_EN

struct eq_coef_t {
    u32 param0;                         //频点参数，由工具计算
    u32 param1;
    u32 *coef;                          //频段参数，由eq_coef_cal计算
};

#if EQ_APP_EN || DAC_PT_EN || BT_MUSIC_EFFECT_DBB_EN

struct eq_div_t {
    uint32_t total_gain;
    u32 coef[DAC_EQ_NUM_TOTAL][5];              //顺序：APP、产测、动态低音
};

#if EQ_APP_EN
static struct eq_coef_t app_coef[EQ_APP_NUM];   //存放每条计算的参数(APP不分左右声道)
#endif
#if ANC_ALG_ADP_EQ_FF_FB_EN
static struct eq_coef_t adp_eq_coef;
int get_anc_alg_adp_eq_coef(u8 idx);
#endif
static struct eq_div_t eq_div[2];               //存放所有div eq的参数(左右声道)

//根据index设置12条EQ示例，包含高低音
//Q:0.750000
static const int tbl_coef[12][2] = {
    //param0                    param1
    { CAL_FIX(0.0029444916),    CAL_FIX(0.9999902462)},     //Band:0(31Hz)
    { CAL_FIX(0.0059839058),    CAL_FIX(0.9999597162)},     //Band:1(63Hz)
    { CAL_FIX(0.0151961090),    CAL_FIX(0.9997401793)},     //Band:2(160Hz)
    { CAL_FIX(0.0237409561),    CAL_FIX(0.9993657117)},     //Band:3(250Hz)
    { CAL_FIX(0.0379729998),    CAL_FIX(0.9983764898)},     //Band:4(400Hz)
    { CAL_FIX(0.0946628813),    CAL_FIX(0.9898674722)},     //Band:5(1000Hz)
    { CAL_FIX(0.1874074140),    CAL_FIX(0.9596752251)},     //Band:6(2000Hz)
    { CAL_FIX(0.3597005044),    CAL_FIX(0.8419530754)},     //Band:7(4000Hz)
    { CAL_FIX(0.6057018917),    CAL_FIX(0.4177699622)},     //Band:8(8000Hz)
    { CAL_FIX(0.5060881129),    CAL_FIX(-0.6509365173)},    //Band:9(16000Hz)
    { CAL_FIX(0.0284864965),    CAL_FIX(0.9990866674)},     //BASS:300Hz
    { CAL_FIX(0.2763541250),    CAL_FIX(0.9100351062)},     //TREB:3000Hz
};

void eq_index_init(void)
{
    eq_div[0].total_gain = 0x800000;
    eq_div[1].total_gain = 0x800000;
#if EQ_APP_EN
    for (int i = 0; i < EQ_APP_NUM; i++) {
        app_coef[i].param0 = tbl_coef[i][0];
        app_coef[i].param1 = tbl_coef[i][1];
        app_coef[i].coef = eq_div[0].coef[i];                               //只配置左声道
    }
#endif
    music_eq_divband_init(0, DAC_EQ_NUM_TOTAL, (u32 *)eq_div[0].coef); //DACL
    music_eq_divband_init(1, DAC_EQ_NUM_TOTAL, (u32 *)eq_div[1].coef); //DACR

#if DAC_PT_EN
    for (int i = 0; i < 2; i++) {
        u8 dac_pt_num = cm_read8(PARAM_DAC_PT_NUM + i);
        if (dac_pt_num != 0) {
            cm_read(&(eq_div[i].coef[EQ_APP_NUM][0]), DAC_PT_PAGE(120 * i), dac_pt_num * 5 * 4);
            TRACE("dac pt cm_read param do\n");
        }
    }
#endif

#if BT_MUSIC_EFFECT_DBB_EN
    music_dbb_eq_index_init(&(eq_div[0].coef[EQ_APP_NUM+DAC_PT_NUM][0]), &(eq_div[1].coef[EQ_APP_NUM+DAC_PT_NUM][0]));
#endif // BT_MUSIC_EFFECT_DBB_EN

#if ANC_ALG_ADP_EQ_FF_FB_EN
    adp_eq_coef.param0 = get_anc_alg_adp_eq_coef(0);
    adp_eq_coef.param1 = get_anc_alg_adp_eq_coef(1);
    adp_eq_coef.coef = eq_div[0].coef[EQ_APP_NUM+DAC_PT_NUM+BT_MUSIC_EFFECT_DBB_BAND_CNT];
#endif // ANC_ALG_ADP_EQ_FF_FB_EN
}

void eq_msc_index_init(u8 flag)
{
}

#if DAC_PT_EN
//设置产测EQ
void music_set_pt_eq(u8 channel, u8 band_cnt, const u32 *param)
{
    if (band_cnt > DAC_PT_NUM || band_cnt == 0 || param == NULL) {
        TRACE("dac pt SDADT music_set_pt_eq err, band_cnt %d\n", band_cnt);
        return;
    }
    memcpy(&(eq_div[channel].coef[EQ_APP_NUM][0]), param, band_cnt * 5 * 4);
    TRACE("dac pt SDADT music_set_pt_eq, ch %d, band_cnt %d\n", channel, band_cnt);
    music_set_eq_by_num(sys_cb.eq_mode);
}
#endif

#if EQ_APP_EN
//根据EQ号来设置EQ[11:0]
void music_set_eq_for_index(u8 index, int gain)
{
    if (index >= EQ_APP_NUM || gain < -12 || gain > 12) {
        return;
    }
    printf("index:%d gain:%d\n",index,gain);

    eq_coef_cal(&app_coef[index], gain);
    memcpy(eq_div[1].coef[index], eq_div[0].coef[index], 20);           //左右声道保持一致
}

void music_set_eq_for_index_do(void)
{
    music_set_eq_by_num(sys_cb.eq_mode);
}

static const uint32_t eq_total_gain_tbl[14] = {
    0x287a27, 0x2d6a86, 0x32f52d, 0x392cee, 0x4026e7, 0x47facd, 0x50c336, 0x5a9df8,
    0x65ac8c, 0x721483, 0x800000, 0x8f9e4d, 0xa12478, 0xb4ce08,
};

//设置EQ总增益
void music_set_eq_overall_gain(int gain)
{
    if (gain > 3 || gain < -10) {
        return;
    }
    eq_div[1].total_gain = eq_div[0].total_gain = eq_total_gain_tbl[gain+10];
}
#endif

#if ANC_ALG_ADP_EQ_FF_FB_EN
void anc_adp_eq_set_gain(int gain, int gain_fraction)
{
    u8 index = EQ_APP_NUM+DAC_PT_NUM+BT_MUSIC_EFFECT_DBB_BAND_CNT;
    if (gain < 0 || gain > 24) {
        return;
    }
//    printf("gain:%d.%d\n", gain, gain_fraction);

    bass_treble_coef_cal_ext(&adp_eq_coef, gain, gain_fraction, 0);
    memcpy(eq_div[1].coef[index], eq_div[0].coef[index], 20);           //左右声道保持一致
    music_set_eq_by_num(sys_cb.eq_mode);
}
#endif // ANC_ALG_ADP_EQ_FF_FB_EN
#endif

void music_get_eq_total_gain(u32* gain)
{
#if EQ_APP_EN
    if (sys_cb.eq_app_total_gain_en) {
        *gain = eq_div[0].total_gain;
    }
#endif // EQ_APP_EN
#if BT_MUSIC_EFFECT_EN
    music_effect_get_music_eq_total_gain(gain);
#endif // BT_MUSIC_EFFECT_EN
}

void bsp_eq_init(void)
{
    eq_var_init();
#if EQ_APP_EN || DAC_PT_EN || BT_MUSIC_EFFECT_DBB_EN || ANC_ALG_ADP_EQ_FF_FB_EN
    eq_index_init();
#endif
}

