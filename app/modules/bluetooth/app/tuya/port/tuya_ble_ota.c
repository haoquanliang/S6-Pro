#include "include.h"
#include "tuya_ble_log.h"
#include "tuya_ble_type.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_api.h"
#include "tuya_ble_ota.h"
#include "api_update.h"
#include "tuya_ble_tws.h"

#if TUYA_BLE_OTA_EN

#define TUYA_FOT_DEBUG_EN                    0

#if TUYA_FOT_DEBUG_EN
#define TUYA_FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define TUYA_FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define TUYA_FOT_DEBUG(...)
#define TUYA_FOT_DEBUG_R(...)
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */
static volatile uint8_t s_ota_state = TUYA_BLE_OTA_REQ;
static volatile uint16_t s_pkg_id AT(.tuya_data);
static uint32_t s_data_len AT(.tuya_data);
static volatile bool s_ota_success;
static u32 s_device_version = TUYA_DEVICE_FVER_NUM;
static tuya_ble_timer_t tuya_ota_timer AT(.tuya_data);
static tuya_ota_file_info_storage_t s_remote_file AT(.tuya_data);
static tuya_ota_file_info_storage_t s_save_file AT(.tuya_data);
/*********************************************************************/

extern void tuya_ota_data_reply(u8 err);

#if TUYA_OTA_TWS_EN
static u8 tws_ota_recv_done = 0;
u8 tuya_tws_ota_data[512 + 8] AT(.fot_data.buf);
u16 tuya_tws_ota_len;

void tuya_ble_tws_ota_set_data(uint8_t *data_ptr, uint16_t size)
{
    memcpy(tuya_tws_ota_data, data_ptr, size);

    tws_ota_recv_done = 1;
}

#endif

u8 is_fot_update_en(void)
{
    return 1;
}

AT(.com_text.tuya.ota)
bool tuya_ble_ota_is_start(void)
{
    if((s_ota_state > TUYA_BLE_OTA_REQ) && (s_ota_state < TUYA_BLE_OTA_END)){
        return true;
    }
    return false;
}

uint32_t tuya_ble_app_ota_init(void)
{
    s_ota_state = TUYA_BLE_OTA_REQ;

    s_ota_success = 0;

    tws_ota_recv_done = 0;

    tuya_tws_ota_len = 0;

    return 0;
}

AT(.text.fot.cache)
void tuya_ble_ota_info_save(void)
{
    tuya_ble_cm_write((u8*)&s_save_file.len, TUYA_CM_FOT_FILE_LEN, 8, 1);
}

void tuya_ble_ota_info_load(void)
{
    tuya_ble_cm_read((u8*)&s_save_file.len, TUYA_CM_FOT_FILE_LEN, 8);
}

static void tuya_ble_ota_sys_reset(tuya_ble_timer_t timer)
{
    WDT_RST();
}

static void tuya_ota_timer_creat_and_start(void)
{
    tuya_ble_timer_create(&tuya_ota_timer, 2000, TUYA_BLE_TIMER_SINGLE_SHOT, tuya_ble_ota_sys_reset);
    tuya_ble_timer_start(tuya_ota_timer);
}

/*********************************************************
FN:
*/
static uint32_t tuya_ota_enter(void)
{
    s_pkg_id = 0;
    s_data_len = 0;
    s_ota_success = false;
    memset(&s_remote_file, 0x00, sizeof(tuya_ota_file_info_storage_t));
    memset(&s_save_file, 0x00, sizeof(tuya_ota_file_info_storage_t));

    reset_sleep_delay();
    reset_pwroff_delay();

    fot_init();

    tuya_ble_ota_info_load();

    TUYA_FOT_DEBUG("save_file_len:%x\n",s_save_file.len);
    TUYA_FOT_DEBUG("save_file_crc32:%x\n",s_save_file.crc32);

    load_code_fota();

    if(bt_nor_is_connected()){
        bt_nor_disconnect();
    }

    ble_update_conn_param(12, 0, 4000);

    set_sys_clk(SYS_120M);

    return 0;
}

static uint32_t tuya_ota_exit_do(void)
{
    if(s_ota_success){
        tuya_ota_timer_creat_and_start();
    }
    s_ota_state = TUYA_BLE_OTA_REQ;

    unlock_code_fota();

    return 0;
}

/*********************************************************
FN:
*/
static uint32_t tuya_ota_exit(void)
{
    if(s_ota_state > TUYA_BLE_OTA_REQ) {
#if TUYA_OTA_TWS_EN
        extern void tuya_ble_tws_ota_exit_sync(void);
        tuya_ble_tws_ota_exit_sync();
#endif
        tuya_ota_exit_do();
    }
    return 0;
}

/*********************************************************
FN:
*/
uint32_t tuya_ble_app_ota_get_ota_state(void)
{
    return s_ota_state;
}

/*********************************************************
FN:
*/
uint32_t tuya_ble_app_ota_disconn_handler(void)
{
    return tuya_ota_exit();
}

/*********************************************************
FN:
*/
AT(.text.fot.cache)
uint32_t tuya_ota_rsp(tuya_ble_ota_response_t* rsp, void* rsp_data, uint16_t data_size)
{
    rsp->p_data = rsp_data;
    rsp->data_len = data_size;
    return tuya_ble_ota_response(rsp);
}

#if TUYA_OTA_TWS_EN
u8 is_fot_tws_support(void)
{
    return 1;
}

void tuya_ble_tws_ota_req_sync(u8 rsp)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_REQ | rsp;
    memcpy(&tuya_tws_ota_data[2],&s_device_version,4);
    tuya_tws_ota_len = 6;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_ota_file_info_sync(u8 rsp)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_FILE_INFO | rsp;
    memcpy(&tuya_tws_ota_data[2],&s_save_file.len,4);
    memcpy(&tuya_tws_ota_data[6],&s_save_file.crc32,4);
    tuya_tws_ota_len = 10;
    tuya_ble_tws_data_sync();
}

void tuya_tws_ota_file_offset_sync(u32 offset, u8 rsp)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_FILE_OFFSET | rsp;
    memcpy(&tuya_tws_ota_data[2],&offset,4);
    tuya_tws_ota_len = 6;
    tuya_ble_tws_data_sync();
}

AT(.text.fot.cache)
u8 tuya_tws_ota_data_sync(u8 *data,u16 len)
{
    static u16 offset = 0;

    if(offset == 0){
        tuya_tws_ota_data[offset++] = TWS_INFO_OTA;
        tuya_tws_ota_data[offset++] = TUYA_BLE_TWS_OTA_DATA;
    }
    memcpy(&tuya_tws_ota_data[offset],data,len);
    offset += len;

    if(offset == 512 + 2){
        tuya_tws_ota_len = offset;
        tuya_ble_tws_data_sync();
        offset = 0;
        return 0;
    }

    return 1;
}

void tuya_tws_ota_data_reply(u8 err)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_DATA | 0x80;
    tuya_tws_ota_data[2] = err;
    tuya_tws_ota_len = 3;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_ota_update_done_sync(void)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_UPDATE_DONE;
    tuya_tws_ota_len = 2;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_ota_err_sync(u8 err)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_ERR;
    tuya_tws_ota_data[2] = err;
    tuya_tws_ota_len = 3;
    tuya_ble_tws_data_sync();
}

void tuya_ble_tws_ota_exit_sync(void)
{
    tuya_tws_ota_data[0] = TWS_INFO_OTA;
    tuya_tws_ota_data[1] = TUYA_BLE_TWS_OTA_EXIT;
    tuya_tws_ota_len = 2;
    tuya_ble_tws_data_sync();
}

AT(.text.fot.cache)
void tuya_ble_tws_ota_data_do(u8 cmd_rsp,uint8_t *data_ptr, u16 size)
{
    if(cmd_rsp){
        u8 remote_err = data_ptr[1];
        if(is_fot_update_success()){
            if(remote_err == FOT_ERR_OK){
                fot_tws_done();
                tuya_ble_tws_ota_update_done_sync();
                tuya_ota_data_reply(0);
                return;
            }else{
                fot_tws_done_sync_err_deal();
            }
        }
        if(fot_get_err() || remote_err){
            if(fot_get_err()){
                tuya_ble_tws_ota_err_sync(fot_get_err());
            }
            tuya_ota_data_reply(4);
            tuya_ota_exit();
        }else{
            tuya_ota_data_reply(0);
        }
    }
}

AT(.text.fot.cache)
void tws_fot_resp(void)
{
    u8 data[2];
    data[0] = TUYA_BLE_OTA_DATA | 0x80;
    data[1] = FOT_ERR_OK;

    tuya_ble_tws_ota_data_do(1,data,2);
}

#endif // TUYA_OTA_TWS_EN

/*********************************************************
FN:
*/
static void tuya_ota_req_reply(void)
{
    tuya_ble_ota_response_t rsp;
    rsp.type = TUYA_BLE_OTA_REQ;

    tuya_ota_req_rsp_t req_rsp;
    memset(&req_rsp, 0x00, sizeof(tuya_ota_req_rsp_t));
    req_rsp.flag = 0x00; //accept ota
    req_rsp.ota_version = 0x03; //OTA Version
    req_rsp.type = 0x00; //firmware info
    req_rsp.version = s_device_version;
    tuya_ble_inverted_array((u8*)&req_rsp.version, sizeof(uint32_t));
    req_rsp.package_maxlen = TUYA_OTA_PKG_LEN;
    tuya_ble_inverted_array((u8*)&req_rsp.package_maxlen, sizeof(uint16_t));

    tuya_ota_rsp(&rsp, &req_rsp, sizeof(tuya_ota_req_rsp_t));
}

static uint32_t tuya_ota_req_handler(uint8_t* cmd, uint16_t cmd_size, tuya_ble_ota_response_t* rsp)
{
    //param check
    if((s_ota_state != TUYA_BLE_OTA_REQ) ||
       (cmd_size != 0x0001) || (*cmd != 0x00) ||
       (bt_get_status() >= BT_STA_INCOMING)){
        TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_REQ- s_ota_state error\n");
        //rsp
        tuya_ota_req_rsp_t req_rsp;
        memset(&req_rsp, 0x00, sizeof(tuya_ota_req_rsp_t));
        req_rsp.flag = 0x01; //refuse ota

        tuya_ota_rsp(rsp, &req_rsp, sizeof(tuya_ota_req_rsp_t));
        tuya_ota_exit();
        return 1;
    }

    {
        tuya_ota_enter();
#if TUYA_OTA_TWS_EN
        tuya_ble_tws_ota_req_sync(0);
#else
        tuya_ota_req_reply();
        s_ota_state = TUYA_BLE_OTA_FILE_INFO;
#endif
    }
    return 0;
}

void tuya_ota_file_info_reply(void)
{
    TUYA_FOT_DEBUG("--->tuya_ota_file_info_reply:%x,%x\n",s_save_file.len,s_save_file.crc32);

    tuya_ble_ota_response_t rsp;
    rsp.type = TUYA_BLE_OTA_FILE_INFO;

    tuya_ota_file_info_rsp_t file_info_rsp;
    file_info_rsp.type = 0;
    file_info_rsp.state = 0x00;
    file_info_rsp.save_file_len = s_save_file.len;
    tuya_ble_inverted_array((u8*)&file_info_rsp.save_file_len, sizeof(uint32_t));
    file_info_rsp.save_file_crc32 = s_save_file.crc32;
    tuya_ble_inverted_array((u8*)&file_info_rsp.save_file_crc32, sizeof(uint32_t));

    tuya_ota_rsp(&rsp, &file_info_rsp, sizeof(tuya_ota_file_info_rsp_t));
}

/*********************************************************
FN:
*/
static uint32_t tuya_ota_file_info_handler(uint8_t* data, uint16_t data_size, tuya_ble_ota_response_t* rsp)
{
    tuya_ota_file_info_t* file_info = (void*)data;

    //param check
    if((s_ota_state != TUYA_BLE_OTA_FILE_INFO) || (file_info->type != 0x00)){
        TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_FILE_INFO- s_ota_state error\n");
        //rsp none
        tuya_ota_exit();
        return 1;
    }

    {
        //file info
        tuya_ble_inverted_array((u8*)&file_info->version, sizeof(uint32_t));
        tuya_ble_inverted_array((u8*)&file_info->file_len, sizeof(uint32_t));
        tuya_ble_inverted_array((u8*)&file_info->crc32, sizeof(uint32_t));

        s_remote_file.len = file_info->file_len;
        s_remote_file.crc32 = file_info->crc32;
        //memcpy(s_remote_file.md5, file_info->md5, TUYA_OTA_FILE_MD5_LEN);

        //rsp
        tuya_ota_file_info_rsp_t file_info_rsp;
        memset(&file_info_rsp, 0x00, sizeof(tuya_ota_file_info_rsp_t));

        if(memcmp(file_info->pid, TUYA_DEVICE_PID, 8)) {
            file_info_rsp.state = 0x01; //pid error
        }
        else if(file_info->version <= s_device_version) {
            file_info_rsp.state = 0x02; //version error
        }
        else if(file_info->file_len > TUYA_OTA_FILE_MAX_LEN) {
            file_info_rsp.state = 0x03; //size error
        } else {
            file_info_rsp.state = 0x00;
            s_ota_state = TUYA_BLE_OTA_FILE_OFFSET_REQ;
        }
#if TUYA_OTA_TWS_EN
        if(file_info_rsp.state == 0x00) {
            tuya_ble_tws_ota_file_info_sync(0);
        }else{
            tuya_ota_rsp(rsp, &file_info_rsp, sizeof(tuya_ota_file_info_rsp_t));
        }
#else
        file_info_rsp.save_file_len = s_save_file.len;
        tuya_ble_inverted_array((u8*)&file_info_rsp.save_file_len, sizeof(uint32_t));
        file_info_rsp.save_file_crc32 = s_save_file.crc32;
        tuya_ble_inverted_array((u8*)&file_info_rsp.save_file_crc32, sizeof(uint32_t));

        tuya_ota_rsp(rsp, &file_info_rsp, sizeof(tuya_ota_file_info_rsp_t));
#endif
        if(file_info_rsp.state != 0x00) {
            TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_FILE_INFO- errorid: %d\n", file_info_rsp.state);
            tuya_ota_exit();
        }
    }
    return 0;
}

void tuya_ota_file_offset_reply(void)
{
    tuya_ble_ota_response_t rsp;
    rsp.type = TUYA_BLE_OTA_FILE_OFFSET_REQ;

    tuya_ota_file_offset_rsp_t file_offset_rsp;
    memset(&file_offset_rsp, 0x00, sizeof(tuya_ota_file_offset_rsp_t));

    file_offset_rsp.offset = s_save_file.len;

    tuya_ble_inverted_array((u8*)&file_offset_rsp.offset, sizeof(uint32_t));

    tuya_ota_rsp(&rsp, &file_offset_rsp, sizeof(tuya_ota_file_offset_rsp_t));
    s_ota_state = TUYA_BLE_OTA_DATA;
}

/*********************************************************
FN:
*/
static uint32_t tuya_ota_file_offset_handler(uint8_t* data, uint16_t data_size, tuya_ble_ota_response_t* rsp)
{
    //param check
    if(s_ota_state != TUYA_BLE_OTA_FILE_OFFSET_REQ){
        TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_FILE_OFFSET_REQ- s_ota_state error\n");
        //rsp none
        tuya_ota_exit();
        return 1;
    }

    //param check
    tuya_ota_file_offset_t* file_offset = (void*)data;
    if(file_offset->type != 0x00){
        TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_FILE_OFFSET_REQ- file_offset->type error\n");
        //rsp none
        tuya_ota_exit();
        return 1;
    }

    {
        tuya_ble_inverted_array((u8*)&file_offset->offset, sizeof(uint32_t));

        //rsp
        tuya_ota_file_offset_rsp_t file_offset_rsp;

        memset(&file_offset_rsp, 0x00, sizeof(tuya_ota_file_offset_rsp_t));

        if(file_offset->offset > 0){
            if(fot_breakpoint_info_read() == true){
                file_offset_rsp.offset = fot_get_curaddr();
                TUYA_FOT_DEBUG("breakpoint_addr:0x%x,0x%x\n",file_offset_rsp.offset,s_save_file.len);
            }
            s_data_len = s_save_file.len;
            s_save_file.len = file_offset_rsp.offset;
        }else{
            TUYA_FOT_DEBUG("--->ota restart\n");
            s_save_file.len = 0;
            s_save_file.crc32 = 0;
            s_data_len = 0;
            fot_init();
        }
#if TUYA_OTA_TWS_EN
        tuya_tws_ota_file_offset_sync(file_offset_rsp.offset,0);
#else
        tuya_ble_inverted_array((u8*)&file_offset_rsp.offset, sizeof(uint32_t));

        tuya_ota_rsp(rsp, &file_offset_rsp, sizeof(tuya_ota_file_offset_rsp_t));
        s_ota_state = TUYA_BLE_OTA_DATA;
#endif
    }
    return 0;
}

AT(.text.fot.cache)
void tuya_ota_data_reply(u8 err)
{
    tuya_ble_ota_response_t rsp;
    rsp.type = TUYA_BLE_OTA_DATA;

    tuya_ota_data_rsp_t ota_data_rsp;
    ota_data_rsp.type = 0x00;
    ota_data_rsp.state = err;
    tuya_ota_rsp(&rsp, &ota_data_rsp, sizeof(tuya_ota_data_rsp_t));
}

/*********************************************************
FN:
*/
AT(.text.fot.cache)
uint32_t tuya_ota_data_handler(uint8_t* data, uint16_t data_size, tuya_ble_ota_response_t* rsp)
{
    tuya_ota_data_t* ota_data = (void*)data;
    //param check
    if(s_ota_state != TUYA_BLE_OTA_DATA){
        TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_DATA- s_ota_state error\n");
        //rsp
        tuya_ota_data_rsp_t ota_data_rsp;
        memset(&ota_data_rsp, 0x00, sizeof(tuya_ota_data_rsp_t));
        ota_data_rsp.state = 0x04; //unknow error

        tuya_ota_rsp(rsp, &ota_data_rsp, sizeof(tuya_ota_data_rsp_t));
        tuya_ota_exit();
        return 1;
    }

    {
        u8 flag;

        tuya_ble_inverted_array((u8*)&ota_data->pkg_id, sizeof(uint16_t));
        tuya_ble_inverted_array((u8*)&ota_data->len, sizeof(uint16_t));
        tuya_ble_inverted_array((u8*)&ota_data->crc16, sizeof(uint16_t));

        //rsp
        tuya_ota_data_rsp_t ota_data_rsp;
        memset(&ota_data_rsp, 0x00, sizeof(tuya_ota_data_rsp_t));
        if(s_pkg_id != ota_data->pkg_id) {
            ota_data_rsp.state = 0x01; //package id error
        }else if(tuya_ble_crc16_compute(ota_data->data, ota_data->len, NULL) != ota_data->crc16) {
            ota_data_rsp.state = 0x03; //crc error
        }else {
#if TUYA_OTA_TWS_EN
            flag = tuya_tws_ota_data_sync(ota_data->data,ota_data->len);
#endif
            fot_write(ota_data->data,fot_get_curaddr(),ota_data->len);

            s_save_file.len += ota_data->len;

            if(s_save_file.len < s_remote_file.len){
                s_ota_state = TUYA_BLE_OTA_DATA;
            }
            else if(s_save_file.len == s_remote_file.len){
                s_ota_state = TUYA_BLE_OTA_END;
            }else{
                ota_data_rsp.state = 0x04;
            }

            if(fot_get_err()){
               ota_data_rsp.state = 0x04;
            }

            TUYA_FOT_DEBUG("pkg_id: %d\n", s_pkg_id);
            s_pkg_id++;

            if(s_save_file.len > s_data_len){
                s_save_file.crc32 = tuya_ble_crc32_compute(ota_data->data, ota_data->len, &s_save_file.crc32);
                if(s_save_file.len >= 512){
                    if(((s_save_file.len - 512) % 0x1000) == 0){
                        tuya_ble_ota_info_save();
                    }
                }
            }
        }
#if TUYA_OTA_TWS_EN
        if(flag || ota_data_rsp.state){
            tuya_ota_data_reply(ota_data_rsp.state);
        }
#else
        tuya_ota_rsp(rsp, &ota_data_rsp, sizeof(tuya_ota_data_rsp_t));
#endif

        if(ota_data_rsp.state != 0x00) {
            TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_DATA- errorid: %d\n", ota_data_rsp.state);
            tuya_ota_exit();
        }
    }
    return 0;
}

/*********************************************************
FN:
*/
static uint32_t tuya_ota_end_handler(uint8_t* data, uint16_t data_size, tuya_ble_ota_response_t* rsp)
{
    //param check
    if((s_ota_state != TUYA_BLE_OTA_END) || (data_size != 0x0001) || (*data != 0x00)){
        TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_END- s_ota_state error\n");
        //rsp
        tuya_ota_end_rsp_t end_rsp;
        memset(&end_rsp, 0x00, sizeof(tuya_ota_end_rsp_t));
        end_rsp.state = 0x03; //unknow error

        tuya_ota_rsp(rsp, &end_rsp, sizeof(tuya_ota_end_rsp_t));
        tuya_ota_exit();
        return 1;
    }

    {
        //rsp
        tuya_ota_end_rsp_t end_rsp;
        memset(&end_rsp, 0x00, sizeof(tuya_ota_end_rsp_t));
        end_rsp.type = 0x00;
        if(s_save_file.len != s_remote_file.len){
            end_rsp.state = 0x01; //total size error
        }
        else if((s_remote_file.crc32 != s_save_file.crc32) || (fot_get_err())){
            end_rsp.state = 0x02; //crc error
            TUYA_FOT_DEBUG("remote_crc:0x%x, save_crc:0x%x, err:%d\n",s_remote_file.crc32,s_save_file.crc32,fot_get_err());
        }
        else{
            end_rsp.state = 0x00;
            s_ota_success = true;
            TUYA_FOT_DEBUG("ota success\n");
        }
        tuya_ota_rsp(rsp, &end_rsp, sizeof(tuya_ota_end_rsp_t));

        s_save_file.len = 0;
        s_save_file.crc32 = 0;
        tuya_ble_ota_info_save();

        if(end_rsp.state != 0x00) {
            TUYA_FOT_DEBUG("Error: TUYA_BLE_OTA_END- errorid: %d\n", end_rsp.state);
        }

        tuya_ota_exit();
    }
    return 0;
}

AT(.text.fot.cache)
void tuya_ble_app_ota_handler(u8 type,u8 *data,u32 len)
{
    TUYA_FOT_DEBUG("ota cmd: %d,len: %d\n",type,len);

    tuya_ble_ota_response_t rsp;
    rsp.type = type;

#if TUYA_FOT_DEBUG_EN
    if(type != TUYA_BLE_OTA_DATA)
    {
        TUYA_FOT_DEBUG("ota cmd: %d,len: %d\n",type,len);
        TUYA_FOT_DEBUG("ota_data: ");
        TUYA_FOT_DEBUG_R(data,len);
    }
#endif

    switch(type){
        case TUYA_BLE_OTA_REQ:{
            tuya_ota_req_handler(data, len, &rsp);
        }
        break;

        case TUYA_BLE_OTA_FILE_INFO:{
            tuya_ota_file_info_handler(data, len, &rsp);
        }
        break;

        case TUYA_BLE_OTA_FILE_OFFSET_REQ:{
            tuya_ota_file_offset_handler(data, len, &rsp);
        }
        break;

        case TUYA_BLE_OTA_DATA:{
            tuya_ota_data_handler(data, len, &rsp);
        }
        break;

        case TUYA_BLE_OTA_END:{
            tuya_ota_end_handler(data, len, &rsp);
        }
        break;

        default:
            break;
    }
}

void tuya_ble_ota_tws_data_proc(uint8_t *data_ptr, u16 size)
{
#if TUYA_OTA_TWS_EN
    TUYA_FOT_DEBUG("tuya_ota_tws_data: ");
    TUYA_FOT_DEBUG_R(data_ptr,size);

    u8 cmd = data_ptr[0] & 0x7f;
    u8 cmd_rsp = (data_ptr[0] & 0x80) ? 1:0;

    if(cmd == TUYA_BLE_TWS_OTA_DATA){
        tuya_ble_tws_ota_data_do(cmd_rsp,data_ptr,size);
        return;
    }

    switch(cmd){
        case TUYA_BLE_TWS_OTA_REQ:{
            u32 remote_version;
            memcpy(&remote_version,&data_ptr[1],4);
            if(remote_version != s_device_version){     //两边的版本号不一致，重头开始升级
                s_save_file.len = 0;
                s_save_file.crc32 = 0;
            }
            if(cmd_rsp){
                tuya_ota_req_reply();
                s_ota_state = TUYA_BLE_OTA_FILE_INFO;
            }else{
                tuya_ota_enter();
                s_ota_state = TUYA_BLE_OTA_FILE_INFO;
                tuya_ble_tws_ota_req_sync(0x80);
            }
        } break;

        case TUYA_BLE_TWS_OTA_FILE_INFO:{
            u32 len,crc32;
            memcpy(&len,&data_ptr[1],4);
            memcpy(&crc32,&data_ptr[5],4);
            if(len < s_save_file.len){
                s_save_file.len = len;
                s_save_file.crc32 = crc32;
            }
            if(cmd_rsp){
                tuya_ota_file_info_reply();
            }else{
                tuya_ble_tws_ota_file_info_sync(0x80);
            }
        } break;

        case TUYA_BLE_TWS_OTA_FILE_OFFSET:{
            u32 remote_offset;
            u32 cur_offset = 0;
            memcpy(&remote_offset,&data_ptr[1],4);
            if(remote_offset> 0){
                if(fot_breakpoint_info_read() == true){
                    cur_offset = fot_get_curaddr();
                    TUYA_FOT_DEBUG("breakpoint_addr:0x%x\n",cur_offset);
                }
                if(cur_offset > remote_offset){
                    cur_offset = remote_offset;
                    param_fot_addr_write((u8*)&cur_offset);
                    fot_breakpoint_info_read();
                }
                s_data_len = s_save_file.len;
                s_save_file.len = cur_offset;
            }else{
                TUYA_FOT_DEBUG("--->ota restart\n");
                s_save_file.len = 0;
                s_save_file.crc32 = 0;
                s_data_len = 0;
                fot_init();
            }
            if(cmd_rsp){
                tuya_ota_file_offset_reply();
            }else{
                tuya_tws_ota_file_offset_sync(cur_offset,0x80);
                s_ota_state = TUYA_BLE_OTA_DATA;
            }
        }  break;

        case TUYA_BLE_TWS_OTA_ERR:
            if(is_fot_update_success()){
                fot_tws_done_sync_err_deal();
            }
            break;

        case TUYA_BLE_TWS_OTA_UPDATE_DONE:
            fot_tws_done();
            s_ota_success = 1;
            s_save_file.len = 0;
            s_save_file.crc32 = 0;
            tuya_ble_ota_info_save();
            tuya_ota_exit_do();
            break;

        case TUYA_BLE_TWS_OTA_EXIT:
            tuya_ota_exit_do();
            break;

        default:
            break;
    }
#endif // TUYA_OTA_TWS_EN
}

AT(.text.fot.cache)
void tuya_ble_ota_process(void)
{
#if TUYA_OTA_TWS_EN
    if(tws_ota_recv_done){
        tws_ota_recv_done = 0;
        fot_write(tuya_tws_ota_data,fot_get_curaddr(),512);
        s_save_file.len += 512;
        if(s_save_file.len > s_data_len){
            s_save_file.crc32 = tuya_ble_crc32_compute((u8* const)&tuya_tws_ota_data, 512, &s_save_file.crc32);
            if(s_save_file.len >= 512){
                if(((s_save_file.len - 512) % 0x1000) == 0){
                    tuya_ble_ota_info_save();
                }
            }
        }
        if(fot_get_err()){
            tuya_tws_ota_data_reply(fot_get_err());
        }else{
            bt_fot_tws_resp();
        }
    }
#endif
}

#else

AT(.com_text.tuya.ota)
bool tuya_ble_ota_is_start(void)
{
    return false;
}

#endif  //LE_TUYA_EN
