#include "include.h"
#include "api.h"
#include "hfhs.h"

char redial_buf[2][32];

void bt_redial_init(void)
{
    memset(redial_buf, 0, 64);
}

void bt_reset_redial_number(uint8_t index)
{
    memset(redial_buf[index], 0, 32);
}

void bt_update_redial_number(uint8_t index, char *buf, u32 len)
{
    if (len < 32) {
        memset(redial_buf[index], 0, 32);
        memcpy(redial_buf[index], buf, len);
    }
}

const char *bt_get_last_call_number(uint8_t index)
{
    if (redial_buf[index][0]) {
        return (const char *)redial_buf[index];
    } else {
        return NULL;
    }
}

const char *hfp_get_last_call_number(uint8_t index) {
    return bt_get_last_call_number(index);
}

#if AB_MATE_CALL_EN
void hfp_hf_emit_curr_calls(u8 idx, u8 type, const char * number, u32 len)
{
    bt_update_redial_number(idx, (char *)number, len);
}

AT(.text.func.btring)
bool hfp_is_list_curr_calls(void) {
    return true;
}
#endif

uint hfp_get_mic_gain(void)
{
    return 9;
}

uint hfp_get_spk_gain(void)
{
    return (sys_cb.hfp_vol>15)? 15 : sys_cb.hfp_vol;
}

uint hfp_get_bat_level(void)
{
#if VBAT_DETECT_EN
    //计算方法：level = (实测电压 - 关机电压) / ((满电电压 - 关机电压) / 10)
    u16 bat_off = LPWR_OFF_VBAT * 100 + 2700;
    if (bat_off > sys_cb.vbat) {
        return 0;
    }
#if SWETZ_VBAT_TO_PHONE
    u8 bat_level = bsp_get_bat_level();
         {
            if(sys_cb.local_bat_level != bat_level){
                    sys_cb.local_bat_level = bat_level;
                    app_lr_send_notification(LR_NOTIFY_BATTERY_LEVEL, 1, &sys_cb.local_bat_level);
                    
            }
        }
        if(sys_cb.local_bat_level > sys_cb.peer_bat_level && (bt_tws_is_connected())){
                bat_level = sys_cb.peer_bat_level;
        }else {
                bat_level = sys_cb.local_bat_level;
        }
        

    return app_bat_level_show_for_phone(bat_level);
#else 
    uint bat_level = (sys_cb.vbat - bat_off) / ((4200 - bat_off) / 10);
    if (bat_level) {
        bat_level--;
    }
//    printf("bat level: %d %d\n", sys_cb.vbat, bat_level);
    return bat_level;

#endif


#else
    return 9;
#endif
}

uint hfp_get_bat_level_ex(void)
{
    uint level_bat;
    level_bat = hfp_get_bat_level();
    return (level_bat+1) * 10;
}

bool hfp_is_list_curr_calls(void) {  //test: voip需要这里要返回true去查clcc
    return true;
}

/************************使用示例************************/
////按键发起回拨号码
//bt_call_redial_number();
//delay_5ms(10); //延迟一下，等它发送完毕
//
////此函数返回需要回拨的号码，例如“10086”
//char * hfp_get_outgoing_number(void) {
//    return "10086";
//}

////发送自定义ATCMD
//bt_hfp_send_at_cmd();
//delay_5ms(10); //延迟一下，等它发送完毕
//
// //此函数返回需要发送的ATCMD
//char * hfp_get_at_cmd(void) {
//    return "AT+VTS=1"; //例如，通话过程发起号码键"1"
//    return "ATD10086;"; //也可以，发起回拨号码"10086"
//    return "AT+CCLK?\r";//获取IOS手机时间（安卓暂不支持），获取回调函数hfp_get_time
//}

//获取IOS手机时间（安卓暂不支持）,需要先发送"AT+CCLK?\r"AT命令
//void hfp_notice_network_time(u8 *buf, u16 len)
//{
//    char cache[12];
//    memcpy(cache, buf, 12);
//    printf("%s\n", cache);
//}

//---------------------------------AT_CMD---------------------------------
char *at_cmd_pt;        //AT命令指针

#if !LE_DUEROS_DMA_EN
//蓝牙库获取AT命令
char * hfp_get_at_cmd(void) {
    return at_cmd_pt;
}
#endif

void bt_hfp_send_at_cmd_with_str(char *cmd_str)
{
    at_cmd_pt = cmd_str;
    bt_hfp_send_at_cmd();
    delay_5ms(10); //延迟一下，等它发送完毕
}
//------------------------------------------------------------------------

///AT指令拨号
#if AB_MATE_CUSTOM_CMD_CALL_EN
char hfp_cmd_call[16] = "ATD10086;";

void hfp_at_dial(char *number)
{
    if (strlen(number) > 11) {
        number[11] = '\0';
    }
    sprintf(hfp_cmd_call, "ATD%s;", number);
    bt_hfp_send_at_cmd_with_str(hfp_cmd_call);
}
#endif // AB_MATE_CUSTOM_CMD_CALL_EN

///AT指令获取IOS手机时间
#if BT_HFP_TIME_EN
char hfp_cmd_cclk[] = "AT+CCLK?\r";
u32 hfp_start_tick;
u8 hfp_time_data[7] = {0};

void hfp_at_kick(void)
{
    memset(hfp_time_data, 0, 7);
    hfp_start_tick = tick_get();

    printf("hfp_at_kick\n");
    bt_hfp_send_at_cmd_with_str(hfp_cmd_cclk);
}

bool hfp_at_time_get(u8 *buf, u8 len)
{
    if (hfp_time_data[0] == 0) {
        printf("hfp time empty!\n");
        return false;
    }
    if (tick_check_expire(hfp_start_tick, 30000)) {
        printf("hfp time outdated!\n");   //超过30s，数据过时
        return false;
    }
    memcpy(buf, hfp_time_data, min(len, 7));
    return true;
}

int nibble_for_char(char c);
void hfp_get_time(char *ptr)
{
    my_printf("-->(hfp)set time\n");
    u16 year = 2000 + nibble_for_char(ptr[0])*10 + nibble_for_char(ptr[1]);
    u8 month = nibble_for_char(ptr[2])*10 + nibble_for_char(ptr[3]);
    u8 day = nibble_for_char(ptr[4])*10 + nibble_for_char(ptr[5]);
    u8 hour = nibble_for_char(ptr[6])*10 + nibble_for_char(ptr[7]);
    u8 min = nibble_for_char(ptr[8])*10 + nibble_for_char(ptr[9]);
    u8 sec = nibble_for_char(ptr[10])*10 + nibble_for_char(ptr[11]);
    if((year >= 2000) && (year <= 3000) && (month <= 12) && (day <= 31) && (hour <= 24) && (min <= 60) && sec <= 60){
        printf("IOS get time:\ndate:%04d.%02d.%02d time:%02d:%02d:%02d\n", year, month, day, hour, min, sec);
        memcpy(&hfp_time_data[0], &year, 2);
        hfp_time_data[2] = month;
        hfp_time_data[3] = day;
        hfp_time_data[4] = hour;
        hfp_time_data[5] = min;
        hfp_time_data[6] = sec;
        msg_enqueue(EVT_HFP_TIME_DONE);
    } else {
//        printf("time error:04d.%02d.%02d-%02d:%02d:%02d\n", year, month, day, hour, min, sec);
    }
}

void hfp_notice_network_time(u8 *buf, u16 len)
{
//    char cache[16] = "";
//    memcpy(cache, buf, min(len, 15));
//    printf("hfp_notice_network_time:%s\n", cache);

    if (bt_is_ios_device()) {
        hfp_get_time((char *)buf);
    }
}
#endif //BT_HFP_TIME_EN
