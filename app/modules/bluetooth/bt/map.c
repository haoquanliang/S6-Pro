#include "include.h"
#include "map.h"

#if BT_MAP_EN

u32 map_start_tick;
u8 map_time_data[7] = {0};

void bt_map_start(void);
int nibble_for_char(char c);

void bt_map_kick(void)
{
    memset(map_time_data, 0, 7);
    map_start_tick = tick_get();

    bt_map_start();
}

bool bt_map_time_get(u8 *buf, u8 len)
{
    if (map_time_data[0] == 0) {
        printf("map data empty!\n");
        return false;
    }
    if (tick_check_expire(map_start_tick, 30000)) {
        printf("map data outdated!\n");   //超过30s，数据过时
        return false;
    }
    memcpy(buf, map_time_data, min(len, 7));
    return true;
}

void bt_get_time(char *ptr)
{
    my_printf("-->set time\n");
//#if UART0_PRINTF_SEL
    bool ios_flag = bt_is_ios_device();
    u16 year = ios_flag? 2000 + nibble_for_char(ptr[2])*10 + nibble_for_char(ptr[3]) : nibble_for_char(ptr[0])*1000 + nibble_for_char(ptr[1])*100 + nibble_for_char(ptr[2])*10 + nibble_for_char(ptr[3]);
    u8 month = nibble_for_char(ptr[4])*10 + nibble_for_char(ptr[5]);
    u8 day = nibble_for_char(ptr[6])*10 + nibble_for_char(ptr[7]);
    u8 hour = nibble_for_char(ptr[9 - ios_flag])*10 + nibble_for_char(ptr[10 - ios_flag]);
    u8 min = nibble_for_char(ptr[11 - ios_flag])*10 + nibble_for_char(ptr[12 - ios_flag]);
    u8 sec = nibble_for_char(ptr[13 - ios_flag])*10 + nibble_for_char(ptr[14 - ios_flag]);
    if ((year >= 2000) && (year <= 3000) && (month <= 12) && (day <= 31) && (hour <= 24) && (min <= 60) && sec <= 60) {
        ios_flag ? printf("IOS get time:\n") : printf("Android get time:\n");
        printf("date:%04d.%02d.%02d time:%02d:%02d:%02d\n",year,month,day,hour,min,sec);
        memcpy(&map_time_data[0], &year, 2);
        map_time_data[2] = month;
        map_time_data[3] = day;
        map_time_data[4] = hour;
        map_time_data[5] = min;
        map_time_data[6] = sec;
        if (!ios_flag) {
            sys_cb.map_retry = 0;
        }
        msg_enqueue(EVT_BT_MAP_DONE);
    } else {
//        if(!ios_flag){
//            sys_cb.map_retry = 1;
//			bt_map_start();
//        }
    }
//#endif
}

void bt_map_data_callback(uint8_t *packet)     //获取时间例程
{
    char *ptr = (char *)(packet+13);
    bt_get_time(ptr);
}
#endif

