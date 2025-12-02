#include "include.h"
#include "ab_mate_ota.h"
#include "ab_mate_tws.h"

#if AB_MATE_OTA_EN

#define FOT_DEBUG_EN                    0

#if FOT_DEBUG_EN
#define FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define FOT_DEBUG(...)
#define FOT_DEBUG_R(...)
#endif

extern void unlock_code_fota(void);

#if FOT_SUPPORT_TWS
static void ab_mate_tws_ota_err_sync(u8 err);
#endif

enum{
    FOT_STA_STOP = 0,
    FOT_STA_START_REQ,
    FOT_STA_START_RSP,
    FOT_STA_W4_START,
    FOT_STA_START,
    FOT_STA_PAUSE,
    FOT_STA_W4_TWS_RSP,
    FOT_STA_EXIT,
};

#define FOT_FLASH_SIZE          (FLASH_SIZE/2 - 12*1024)

#define FOT_FLAG_UPDATE_OK      BIT(0)
#define FOT_FLAG_SYS_RESET      BIT(1)
#define FOT_FLAG_CLK_SET        BIT(2)


#define FOT_ADDR_POS            0
#define FOT_DATA_LEN_POS        4
#define DATA_START_POS          8
#define DATA_CONTINUE_POS       0

#define FOT_BLOCK_LEN           512         //max 512

#if FOT_SUPPORT_TWS
extern u8 tws_sync_ota[512+16];
extern u16 tws_sync_ota_len;
static u8 *fot_data AT(.app.buf.fot);
#else
static u8 fot_data[FOT_BLOCK_LEN] AT(.app.buf.fot);
#endif

typedef struct __attribute__((packed)){
    u8 fot_sta;
    u8 fot_flag;
    u8 fot_recv_ok;
    u8 remote_ver[4];
    u32 hash;
    u32 file_size;
    u32 total_len;
    u32 remain_len;
    u16 data_pos;
    u32 addr;
    u32 tick;
	u8 sys_clk;
	u8 type;
#if FOT_SUPPORT_TWS
    u8 tws_recv_ok;
    u8 tws_rsp;
    u8 tws_rem_err;
    u32 tws_rem_addr;
#endif
}fot_s;

fot_s fot_var AT(.app.buf.fot);

#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
//设置是否忽略固件头crc判断,忽略crc判断后支持同版本（同一个固件文件）升级
AT(.text.fot.cache)
bool fot_update_ignore_head_crc(void)
{
    return false;
}
#endif

//fot信息校验，可以比对guid或者版本号限制升级，在app.xm通过setid和setver设置guid和版本号
//返回值:FOT_ERR_OK=支持升级，FOT_ERR_GUID=GUID错误禁止升级，FOT_ERR_VER=版本错误禁止升级
AT(.text.fot.cache)
u8 fot_info_check(fot_info_t *loc_info, fot_info_t *rem_info)
{
    FOT_DEBUG("loc ver:%d guid:", loc_info->version);
    FOT_DEBUG_R(loc_info->guid, 16);
    FOT_DEBUG("rem ver:%d guid:", rem_info->version);
    FOT_DEBUG_R(rem_info->guid, 16);
#if FOT_GUID_CHECK_EN
    if (memcmp(loc_info->guid, rem_info->guid, 16)) {
        printf("fot guid err\n");
        return FOT_ERR_GUID;
    }
#endif
#if FOT_VER_CHECK_EN
    if (rem_info->version <= loc_info->version) {
        printf("fot version err\n");
        return FOT_ERR_VER;
    }
#endif
    return FOT_ERR_OK;
}

AT(.text.fot.cache)
void app_fota_write(void *buf, u32 addr, u32 len)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    fot_write(buf, addr, len);
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    ota_pack_write(buf);
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
        fot_write(buf, addr, len);
    }else if(fot_var.type == AB_FOT_TYPE_PACK){
        ota_pack_write(buf);
    }else{
        if(addr == 0){
            fot_write(buf, addr, len);
            if(fot_get_err() == FOT_ERR_OK){
                fot_var.type = AB_FOT_TYPE_NORMAL;
                param_fot_type_write(&fot_var.type);
                FOT_DEBUG("--->AB_FOT_TYPE_NORMAL\n");
            }else if(fot_get_err() == FOT_ERR_FILE_FORMAT){
                ota_pack_write(buf);
                if(ota_pack_get_err() == FOT_ERR_OK){
                    fot_var.type = AB_FOT_TYPE_PACK;
                    param_fot_type_write(&fot_var.type);
                    FOT_DEBUG("--->AB_FOT_TYPE_PACK\n");
                }
            }
        }
    }
#endif
}

AT(.text.fot.cache)
u8 app_fota_get_err(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    return fot_get_err();
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    return ota_pack_get_err();
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
        return fot_get_err();
    }else if(fot_var.type == AB_FOT_TYPE_PACK){
        return ota_pack_get_err();
    }else{
        return FOT_ERR_FILE_FORMAT;
    }
#endif
}

AT(.text.fot.cache)
void app_fota_verify(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)

#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    ota_pack_verify();
#else
    if(fot_var.type == AB_FOT_TYPE_PACK){
        ota_pack_verify();
    }
#endif
}

bool app_fota_breakpoint_info_read(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    return fot_breakpoint_info_read();
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    return ota_pack_breakpoint_info_read();
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
        return fot_breakpoint_info_read();
    }else if(fot_var.type == AB_FOT_TYPE_PACK){
        return ota_pack_breakpoint_info_read();
    }else{
        return false;
    }
#endif
}

void app_fota_init(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    fot_init();
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    ota_pack_init();
#if SWETZ_OTA_ADDR_CHARGE
    uint32_t bp_addr = 0;
    param_fot_addr_write((u8 *)&bp_addr);
#endif
#else
    fot_init();
    ota_pack_init();
#endif

    load_code_fota();
}

u32 app_fota_get_curaddr(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    return fot_get_curaddr();
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    return ota_pack_get_curaddr();
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
        return fot_get_curaddr();
    }else{
        return ota_pack_get_curaddr();
    }
#endif
}

void app_fota_update_done(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    #if FOT_SUPPORT_TWS
        fot_tws_done();
    #endif
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    ota_pack_done();
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
    #if FOT_SUPPORT_TWS
        fot_tws_done();
    #endif
    }else{
        ota_pack_done();
    }
#endif
}

bool app_fota_is_write_done(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    return is_fot_update_success();
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    return ota_pack_is_write_done();
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
        return is_fot_update_success();
    }else{
        return ota_pack_is_write_done();
    }
#endif
}

void ab_mate_ota_init(void)
{
    memset(&fot_var,0,sizeof(fot_var));

#if FOT_SUPPORT_TWS
    fot_data = &tws_sync_ota[DATA_START_POS + 2];
#endif

    unlock_code_fota();
}

void ab_mate_ota_exit(void)
{
    if(fot_var.sys_clk){
        sys_clk_set(fot_var.sys_clk);
    }
    cfg_bt_2acl_adjust_ble_interval = true;
    if(ble_is_connect()){
        ble_update_conn_param(AB_MATE_CON_INTERVAL, 0, 400);
    }
    memset(&fot_var, 0, sizeof(fot_var));
    unlock_code_fota();
}

bool ab_mate_ota_is_start(void)
{
    return (fot_var.fot_sta == FOT_STA_START);
}

AT(.text.fot.cache)
static void ab_mate_ota_sta_notify(u8 sta)
{
    ab_mate_notify_do(CMD_OTA_STA,&sta,1);
}

void ab_mate_ota_seq_err_notify(void)
{
    ab_mate_ota_sta_notify(FOT_ERR_SEQ);
#if FOT_SUPPORT_TWS
    ab_mate_tws_ota_err_sync(FOT_ERR_SEQ);
#endif
    ab_mate_ota_exit();
}

void fot_update_pause(void)
{
    if(fot_var.fot_sta == FOT_STA_START){
        fot_var.fot_sta = FOT_STA_PAUSE;
        ab_mate_ota_sta_notify(FOT_UPDATE_PAUSE);
    }
}

void fot_update_continue(void)
{
    if(fot_var.fot_sta == FOT_STA_PAUSE){
        ab_mate_ota_sta_notify(FOT_UPDATE_CONTINUE);
    }
}

//-----------------------------------------------------------
//增加TWS同时升级功能
#if FOT_SUPPORT_TWS

u8 is_fot_tws_support(void)
{
    return 1;
}

void app_fota_tws_done_sync_err_deal(void)
{
#if (AB_FOT_TYPE == AB_FOT_TYPE_NORMAL)
    fot_tws_done_sync_err_deal();
#elif (AB_FOT_TYPE == AB_FOT_TYPE_PACK)
    ota_pack_tws_done_sync_err_deal();
#else
    if(fot_var.type == AB_FOT_TYPE_NORMAL){
        fot_tws_done_sync_err_deal();
    }else{
        ota_pack_tws_done_sync_err_deal();
    }
#endif
}

AT(.com_text.fot)
void tws_fot_resp(void)
{
    fot_var.tws_rem_err = FOT_ERR_OK;
    fot_var.tws_rsp = 1;
}

AT(.text.fot.update)
static void ab_mate_tws_ota_err_sync(u8 err)
{
    u8 offset = 0;
    tws_sync_ota[offset++] = TWS_INFO_OTA;
    tws_sync_ota[offset++] = TWS_INFO_OTA_ERR;
    tws_sync_ota[offset++] = err;

    tws_sync_ota_len = offset;
    tws_data_sync_do();
}

AT(.text.fot.update)
static void ab_mate_tws_ota_info_sync(void)
{
    tws_sync_ota[0] = TWS_INFO_OTA;
    tws_sync_ota[1] = TWS_INFO_OTA_REQ;
    memcpy(&tws_sync_ota[2], &fot_var.remote_ver, 4);
    memcpy(&tws_sync_ota[6], &fot_var.hash, 4);
    memcpy(&tws_sync_ota[10], &fot_var.addr, 4);

    tws_sync_ota_len = 14;
    tws_data_sync_do();
}

AT(.text.fot.update)
static void ab_mate_tws_ota_file_addr_sync(void)
{
    tws_sync_ota[0] = TWS_INFO_OTA;
    tws_sync_ota[1] = TWS_INFO_OTA_FILE_ADDR;
    memcpy(&tws_sync_ota[2],&fot_var.addr,4);

    tws_sync_ota_len = 6;
    tws_data_sync_do();
}

AT(.text.fot.update)
static void ab_mate_tws_ota_done_sync(void)
{
    tws_sync_ota[0] = TWS_INFO_OTA;
    tws_sync_ota[1] = TWS_INFO_OTA_UPDATE_DONE;

    tws_sync_ota_len = 2;
    tws_data_sync_do();
}

void ab_mate_tws_ota_req_proc(void)
{
    u32 hash;

    param_fot_hash_read((u8*)&hash);

    app_fota_init();
    if((fot_var.hash != 0xFFFFFFFF) && (hash == fot_var.hash)){
        param_fot_type_read(&fot_var.type);
        if((app_fota_breakpoint_info_read() == true)){
            fot_var.addr = app_fota_get_curaddr();
        }
    }else{
        fot_var.addr = 0;
        fot_var.type = AB_FOT_TYPE_ADAPT;
        param_fot_type_write(&fot_var.type);
        param_fot_addr_write((u8*)&fot_var.addr);
        param_fot_hash_write((u8*)&fot_var.hash);
    }
    if(fot_var.addr > fot_var.tws_rem_addr){
        fot_var.addr = fot_var.tws_rem_addr;
        param_fot_addr_write((u8*)&fot_var.addr);
        app_fota_breakpoint_info_read();
    }
    ab_mate_tws_ota_file_addr_sync();
    fot_var.fot_sta = FOT_STA_W4_START;
}

AT(.text.fot.update)
static void ab_mate_tws_ota_reply_update_request(void)
{
    u32 block_len = 0;
    u8 need_update = 1;

    if(fot_var.addr > fot_var.tws_rem_addr){
        fot_var.addr = fot_var.tws_rem_addr;
        param_fot_addr_write((u8*)&fot_var.addr);
        app_fota_breakpoint_info_read();
    }

    if(bt_get_status() >= BT_STA_INCOMING){
        need_update = 0;
        fot_var.fot_sta = FOT_STA_STOP;
        goto fot_req_reply;
    }

    if(ble_is_connect()){
        u16 interval = 12;
        bool play_sta = bt_is_playing();
        if (bt_get_connected_num() < 2 && !play_sta) {
            cfg_bt_2acl_adjust_ble_interval = false;
        }
        if (play_sta) {
            interval = 48;//音乐状态ble连接间隔调大，避免占用过多音乐和tws带宽
        }
        ble_update_conn_param(interval, 0, 400);
    }

    block_len = FOT_BLOCK_LEN;

    fot_var.fot_sta = FOT_STA_W4_START;

fot_req_reply:
    ab_mate_cmd_send.cmd_head.cmd = CMD_OTA_REQ;
    ab_mate_cmd_send.cmd_head.cmd_type = CMD_TYPE_RESPONSE;
    ab_mate_cmd_send.cmd_head.frame_seq = 0;
    ab_mate_cmd_send.cmd_head.frame_total = 0;
    memcpy(&ab_mate_cmd_send.payload[0],&fot_var.addr,4);
    memcpy(&ab_mate_cmd_send.payload[4],&block_len,4);
    ab_mate_cmd_send.payload[8] = need_update;
    ab_mate_cmd_send.cmd_head.payload_len = 9;
    ab_mate_data_send((u8*)&ab_mate_cmd_send,AB_MATE_HEADER_LEN+ab_mate_cmd_send.cmd_head.payload_len);
}
#endif

#if FOT_SUPPORT_TWS
AT(.text.fot.update)
static void ab_mate_ota_req_proc(void)
{
    u32 hash;

    u32 block_len = 0;
    u8 need_update = 1;

    u32 remote_ver;
    u32 local_ver;

    memcpy(&remote_ver, fot_var.remote_ver, 4);
    memcpy(&local_ver, ab_mate_app.version, 4);

    FOT_DEBUG("-->fot_local_ver:0x%x\n",local_ver);
    FOT_DEBUG("-->fot_remote_ver:0x%x\n",remote_ver);

    if (!bt_tws_is_connected()) {
        printf("OTA ERR:When opening FOT_SUPPORT_TWS,it is necessary to connect TWS!\n");
        ab_mate_ota_sta_notify(FOT_ERR_TWS_DISCONNECT);
        fot_var.fot_sta = FOT_STA_STOP;
        return;
    }

    if( (bt_get_status() >= BT_STA_INCOMING) || (fot_var.file_size > FOT_FLASH_SIZE) || (local_ver >= remote_ver)){
        need_update = 0;
    }

    if(need_update == 0){
        ab_mate_cmd_send.cmd_head.cmd = CMD_OTA_REQ;
        ab_mate_cmd_send.cmd_head.cmd_type = CMD_TYPE_RESPONSE;
        ab_mate_cmd_send.cmd_head.frame_seq = 0;
        ab_mate_cmd_send.cmd_head.frame_total = 0;
        memcpy(&ab_mate_cmd_send.payload[0],&fot_var.addr,4);
        memcpy(&ab_mate_cmd_send.payload[4],&block_len,4);
        ab_mate_cmd_send.payload[8] = 0;
        ab_mate_cmd_send.cmd_head.payload_len = 9;
        ab_mate_data_send((u8*)&ab_mate_cmd_send,AB_MATE_HEADER_LEN+ab_mate_cmd_send.cmd_head.payload_len);
        fot_var.fot_sta = FOT_STA_STOP;
        return;
    }

    FOT_DEBUG("-->hash_val:0x%x\n",fot_var.hash);

    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("-->flash hash val:0x%x\n",hash);

    app_fota_init();

    if((fot_var.hash != 0xFFFFFFFF) && (hash == fot_var.hash)){
        param_fot_type_read(&fot_var.type);
        if((app_fota_breakpoint_info_read() == true)){
            fot_var.addr = app_fota_get_curaddr();
        }
    }else{
        fot_var.addr = 0;
        fot_var.type = AB_FOT_TYPE_ADAPT;
        param_fot_type_write(&fot_var.type);
        param_fot_addr_write((u8*)&fot_var.addr);
        param_fot_hash_write((u8*)&fot_var.hash);
    }

    ab_mate_tws_ota_info_sync();
    fot_var.fot_sta = FOT_STA_W4_TWS_RSP;
}
#else
AT(.text.fot.update)
static void ab_mate_ota_req_proc(void)
{
    u32 hash;
    u32 addr = 0;
    u32 block_len = FOT_BLOCK_LEN;
    u8 need_update = 1;

    if( (bt_get_status() >= BT_STA_INCOMING) || (fot_var.file_size > FOT_FLASH_SIZE)){
        need_update = 0;
        fot_var.fot_sta = FOT_STA_STOP;
        goto fot_req_reply;
    }

    if(ble_is_connect()){
        u16 interval = 12;
        bool play_sta = bt_is_playing();
        if (bt_get_connected_num() < 2 && !play_sta) {
            cfg_bt_2acl_adjust_ble_interval = false;
        }
        if (play_sta) {
            interval = 48;//音乐状态ble连接间隔调大，避免占用过多音乐和tws带宽
        }
        ble_update_conn_param(interval, 0, 400);
    }

    app_fota_init();

    FOT_DEBUG("hash_val:0x%x\n",fot_var.hash);

    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("flash hash val:0x%x\n",hash);

    if((fot_var.hash != 0xFFFFFFFF) && (hash == fot_var.hash)){
        param_fot_type_read(&fot_var.type);
        if(app_fota_breakpoint_info_read() == true){
            addr = app_fota_get_curaddr();
        }
    }else{
        fot_var.type = AB_FOT_TYPE_ADAPT;
        param_fot_type_write(&fot_var.type);
        param_fot_addr_write((u8*)&addr);
        param_fot_hash_write((u8*)&fot_var.hash);
    }

    fot_var.fot_sta = FOT_STA_W4_START;

fot_req_reply:
    ab_mate_cmd_send.cmd_head.cmd = CMD_OTA_REQ;
    ab_mate_cmd_send.cmd_head.cmd_type = CMD_TYPE_RESPONSE;
    ab_mate_cmd_send.cmd_head.frame_seq = 0;
    ab_mate_cmd_send.cmd_head.frame_total = 0;
    memcpy(&ab_mate_cmd_send.payload[0],&addr,4);
    memcpy(&ab_mate_cmd_send.payload[4],&block_len,4);
    ab_mate_cmd_send.payload[8] = need_update;
    ab_mate_cmd_send.cmd_head.payload_len = 9;

    ab_mate_data_send((u8*)&ab_mate_cmd_send,AB_MATE_HEADER_LEN+ab_mate_cmd_send.cmd_head.payload_len);
}
#endif

void ab_mate_ota_disconnect_callback(void)
{
    //printf("--->ab_mate_ota_disconnect_callback\n");

    if((fot_var.fot_flag & FOT_FLAG_UPDATE_OK) == 0){
        if(fot_var.fot_sta){
            fot_var.fot_sta = FOT_STA_EXIT;
        }
    }
}

#if FOT_SUPPORT_TWS
void ab_mate_ota_tws_data_proc(uint8_t *data_ptr, u16 size)
{
    u8 cmd = data_ptr[0];
    size = size - 1;
    u8 *buf = &data_ptr[1];


    switch(cmd){
    case TWS_INFO_OTA_DATA:{
        u32 addr;
        u32 recv_data_len;
        if(fot_var.remain_len){
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
#if FOT_SUPPORT_TWS
            ab_mate_tws_ota_err_sync(FOT_ERR_DATA_LEN);
#endif
            fot_var.fot_sta = FOT_STA_EXIT;
            return;
        }
        if((fot_var.fot_sta != FOT_STA_START) && (fot_var.fot_sta != FOT_STA_PAUSE)){
            fot_var.fot_sta = FOT_STA_START;
            fot_var.fot_flag |= FOT_FLAG_CLK_SET;
        }
        recv_data_len = size-DATA_START_POS;
        memcpy(&addr,&buf[FOT_ADDR_POS],4);
        memcpy(&fot_var.total_len,&buf[FOT_DATA_LEN_POS],4);
        fot_var.remain_len = fot_var.total_len - recv_data_len;
        memcpy(&fot_data[fot_var.data_pos],&buf[DATA_START_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
            fot_var.tws_recv_ok = 1;
            fot_var.data_pos = 0;
        }
    }   break;
    case TWS_INFO_OTA_REQ:{
        memset(&fot_var,0,sizeof(fot_var));
        memcpy(&fot_var.remote_ver, &buf[0], 4);
        memcpy(&fot_var.hash, &buf[4], 4);
        memcpy(&fot_var.tws_rem_addr,&buf[8],4);
        fot_var.fot_sta = FOT_STA_START_REQ;
    }   break;
    case TWS_INFO_OTA_FILE_ADDR:{
        memcpy(&fot_var.tws_rem_addr,&buf[0],4);
        fot_var.fot_sta = FOT_STA_START_RSP;
    }   break;
    case TWS_INFO_OTA_UPDATE_DONE:
        app_fota_update_done();
        fot_var.fot_flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
        fot_var.tick = tick_get();
        break;
    case TWS_INFO_OTA_ERR:
        fot_var.tws_rsp = 1;
        fot_var.tws_rem_err = buf[0];
        if(ab_mate_app.con_sta == 0){
            if(app_fota_is_write_done()){
                app_fota_tws_done_sync_err_deal();
            }
            fot_var.fot_sta = FOT_STA_EXIT;
        }
        break;
    default:
        break;
    }
}
#endif

void ab_mate_ota_proc(u8 cmd,u8 *payload,u8 payload_len)
{
    u32 addr;
    u32 recv_data_len;

    switch(cmd){
    case CMD_OTA_REQ:
        memset(&fot_var,0,sizeof(fot_var));
        memcpy(&fot_var.remote_ver,&payload[0],4);
        memcpy(&fot_var.hash,&payload[4],4);
        memcpy(&fot_var.file_size,&payload[8],4);
        fot_var.fot_sta = FOT_STA_START_REQ;
        break;

    case CMD_OTA_DATA_START:
        if(fot_var.remain_len){
            FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
            ab_mate_ota_sta_notify(FOT_ERR_DATA_LEN);
#if FOT_SUPPORT_TWS
            ab_mate_tws_ota_err_sync(FOT_ERR_DATA_LEN);
#endif
            fot_var.fot_sta = FOT_STA_EXIT;
            return;
        }
        if((fot_var.fot_sta != FOT_STA_START) && (fot_var.fot_sta != FOT_STA_PAUSE)){
            fot_var.fot_sta = FOT_STA_START;
            fot_var.fot_flag |= FOT_FLAG_CLK_SET;
        }
#if FOT_SUPPORT_TWS
        tws_sync_ota[0] = TWS_INFO_OTA;
        tws_sync_ota[1] = TWS_INFO_OTA_DATA;
        memcpy(&tws_sync_ota[2],payload,DATA_START_POS);
#endif
        recv_data_len = payload_len-DATA_START_POS;
        memcpy(&addr,&payload[FOT_ADDR_POS],4);
        memcpy(&fot_var.total_len,&payload[FOT_DATA_LEN_POS],4);
        fot_var.remain_len = fot_var.total_len - recv_data_len;
        memcpy(&fot_data[fot_var.data_pos],&payload[DATA_START_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
#if FOT_SUPPORT_TWS
            tws_sync_ota_len = fot_var.total_len + DATA_START_POS + 2;
            tws_data_sync_do();
#endif
            fot_var.fot_recv_ok = 1;
            fot_var.data_pos = 0;
        }
        break;

    case CMD_OTA_DATA_CONTINUE:
        recv_data_len = payload_len-DATA_CONTINUE_POS;
        if(fot_var.remain_len < recv_data_len){
            FOT_DEBUG("--->len err:%d,%d\n",fot_var.remain_len,recv_data_len);
            ab_mate_ota_sta_notify(FOT_ERR_DATA_LEN);
#if FOT_SUPPORT_TWS
            ab_mate_tws_ota_err_sync(FOT_ERR_DATA_LEN);
#endif
            fot_var.fot_sta = FOT_STA_EXIT;
            return;
        }
        fot_var.remain_len -= recv_data_len;
        memcpy(&fot_data[fot_var.data_pos],&payload[DATA_CONTINUE_POS],recv_data_len);
        fot_var.data_pos += recv_data_len;
        if(fot_var.remain_len == 0){
#if FOT_SUPPORT_TWS
            tws_sync_ota_len = fot_var.total_len + DATA_START_POS + 2;
            tws_data_sync_do();
#endif
            fot_var.fot_recv_ok = 1;
            fot_var.data_pos = 0;
        }
        break;

    default:
        break;
    }
}

AT(.text.fot.cache)
void ab_mate_ota_process_do(void)
{
    switch(fot_var.fot_sta){
    case FOT_STA_START_REQ:
        if(ab_mate_app.con_sta){
            ab_mate_ota_req_proc();
        }else{
#if FOT_SUPPORT_TWS
            ab_mate_tws_ota_req_proc();
#endif
        }
        break;

    case FOT_STA_START_RSP:
#if FOT_SUPPORT_TWS
        ab_mate_tws_ota_reply_update_request();
#endif
        break;

    case FOT_STA_W4_TWS_RSP:
    case FOT_STA_STOP:
    case FOT_STA_W4_START:
        break;

    case FOT_STA_EXIT:
        ab_mate_ota_exit();
        break;

    case FOT_STA_START:
        reset_sleep_delay();
        reset_pwroff_delay();
#if FOT_SUPPORT_TWS
        if(fot_var.tws_recv_ok){
            FOT_DEBUG("--->tws_recv_ok\n");
            fot_var.tws_recv_ok = 0;
            app_fota_write(fot_data, app_fota_get_curaddr(), fot_var.total_len);
            if(app_fota_is_write_done()){
                app_fota_verify();
            }
            if(app_fota_get_err()){
                ab_mate_tws_ota_err_sync(app_fota_get_err());
                fot_var.fot_sta = FOT_STA_EXIT;
            }else{
                bt_fot_tws_resp();
            }
        }
        if(fot_var.fot_recv_ok == 0x01){
            fot_var.fot_recv_ok = 0x02;
            app_fota_write(fot_data, app_fota_get_curaddr(), fot_var.total_len);
        }else if(fot_var.tws_rsp && (fot_var.fot_recv_ok == 0x02)){
            FOT_DEBUG("--->fot_recv_ok\n");
            fot_var.tws_rsp = 0;
            fot_var.fot_recv_ok = 0;
            if(fot_var.tws_rem_err != FOT_ERR_OK){
                FOT_DEBUG("fot_tws_remote_err:0x%x\n",fot_var.tws_rem_err);
                ab_mate_ota_sta_notify(fot_var.tws_rem_err);
                if(app_fota_is_write_done()){
                    app_fota_tws_done_sync_err_deal();
                }
                fot_var.fot_sta = FOT_STA_EXIT;
            }else{
                if(app_fota_is_write_done()){
                    app_fota_verify();
                    if(app_fota_get_err()){
                        ab_mate_tws_ota_err_sync(app_fota_get_err());
                        ab_mate_ota_sta_notify(app_fota_get_err());
                        fot_var.fot_sta = FOT_STA_EXIT;
                    }else{
                        FOT_DEBUG("--->fot update success\n");
                        app_fota_update_done();
                        fot_var.fot_flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
                        fot_var.tick = tick_get();
                        ab_mate_tws_ota_done_sync();
                        ab_mate_ota_sta_notify(FOT_UPDATE_DONE);
                    }
                }else{
                    ab_mate_ota_sta_notify(app_fota_get_err());
                    if(app_fota_get_err()){
                        ab_mate_tws_ota_err_sync(app_fota_get_err());
                        fot_var.fot_sta = FOT_STA_EXIT;
                    }
                }
            }
        }
#else
        if(fot_var.fot_recv_ok){
            //FOT_DEBUG("--->fot_recv_ok\n");
            fot_var.fot_recv_ok = 0;
            app_fota_write(fot_data, app_fota_get_curaddr(), fot_var.total_len);

            if(app_fota_is_write_done()){
                app_fota_verify();
                if(app_fota_get_err()){
                    ab_mate_ota_sta_notify(app_fota_get_err());
                    fot_var.fot_sta = FOT_STA_EXIT;
                }else{
                    FOT_DEBUG("--->fot update success\n");
                    app_fota_update_done();
                    fot_var.fot_flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
                    fot_var.tick = tick_get();
                    ab_mate_ota_sta_notify(FOT_UPDATE_DONE);
                }
            }else{
                ab_mate_ota_sta_notify(app_fota_get_err());
                if(app_fota_get_err()){
                    fot_var.fot_sta = FOT_STA_EXIT;
                }
            }
        }
#endif
        break;

    default:
        break;
    }
}

void ab_mate_ota_flag_do(void)
{
    if(fot_var.fot_flag & FOT_FLAG_CLK_SET){
        fot_var.fot_flag &= ~FOT_FLAG_CLK_SET;
		fot_var.sys_clk = sys_clk_get();
        sys_clk_set(SYS_160M);
    }

    if(fot_var.fot_flag & FOT_FLAG_SYS_RESET){
        if(tick_check_expire(fot_var.tick,3000)){
            fot_var.fot_flag &= ~FOT_FLAG_SYS_RESET;
            FOT_DEBUG("fota update ok,sys reset\n");
            WDT_RST();
        }
    }
}

AT(.text.ab_mate.process)
void ab_mate_ota_process(void)
{
    if(fot_var.fot_sta != FOT_STA_STOP
       && fot_var.fot_sta != FOT_STA_W4_TWS_RSP
       && fot_var.fot_sta != FOT_STA_W4_START) {
        ab_mate_ota_process_do();
    }

    if(fot_var.fot_flag){
        ab_mate_ota_flag_do();
    }

}

#else
bool ab_mate_ota_is_start(void)
{
    return false;
}

#endif
