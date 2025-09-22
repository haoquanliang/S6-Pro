#include "include.h"
#include "tuya_ble_timer.h"
#include "tuya_ble_type.h"

#if LE_TUYA_EN

#define SOFT_TIMER_PRINT_EN       0

#if SOFT_TIMER_PRINT_EN
#define soft_timer_printf(...)               printf(__VA_ARGS__)
#define soft_timer_print_r(...)              print_r(__VA_ARGS__)
#else
#define soft_timer_printf(...)
#define soft_timer_print_r(...)
#endif

#define MAX_TIMER_NUM               8

//定时器
typedef struct
{
    u8                              is_running : 1;
    u8                              is_occupy : 1;
    tuya_ble_timer_mode             time_mode;
    tuya_ble_timer_handler_t        p_time_handler;
    u32                             ticks_to_expire;
    u32                             ticks_start;
}soft_timer_t;

soft_timer_t g_soft_timer[MAX_TIMER_NUM] AT(.tuya_data);
AT(.tuya_data)
u16 g_soft_timer_running_flag = 0;

void soft_timer_init(void)
{
    memset(g_soft_timer,0,sizeof(g_soft_timer));
    g_soft_timer_running_flag = 0;
}

static soft_timer_t* acquire_timer(uint32_t timeout_value_ms,tuya_ble_timer_mode mode,tuya_ble_timer_handler_t timeout_handle)
{
    int8_t i;
    for(i = 0; i < MAX_TIMER_NUM; i++){
        if(g_soft_timer[i].is_occupy == 0){
            g_soft_timer[i].is_occupy = 1;
            g_soft_timer[i].is_running = 0;
            g_soft_timer[i].time_mode = mode;
            g_soft_timer[i].ticks_to_expire = timeout_value_ms;
            g_soft_timer[i].p_time_handler = timeout_handle;
            return &g_soft_timer[i];
        }
    }
    return NULL;
}

static int32_t release_timer(void* timer_id)
{
    uint8_t i;
    for(i = 0; i < MAX_TIMER_NUM; i++){
        if(timer_id == &g_soft_timer[i]){
            g_soft_timer[i].is_occupy = 0;
            g_soft_timer[i].is_running = 0;
            g_soft_timer_running_flag &= ~BIT(i);
            return i;
        }
    }
    return -1;
}

static int32_t find_timer_obj(void* timer_id)
{
    uint8_t i;
    for(i = 0; i < MAX_TIMER_NUM; i++){
        if(timer_id == &g_soft_timer[i]){
            return i;
        }
    }
    return -1;
}

//放到main线程循环
void soft_timer_run(void)
{
    u8 i = 0;
    if(g_soft_timer_running_flag){
        do{
            if(g_soft_timer[i].is_running){
                if (tick_check_expire(g_soft_timer[i].ticks_start,g_soft_timer[i].ticks_to_expire)) {
                    soft_timer_printf("soft time handle:%d\n",i);
                    if(g_soft_timer[i].time_mode == TUYA_BLE_TIMER_REPEATED){
                        g_soft_timer[i].ticks_start = tick_get();
                    } else {
                        g_soft_timer[i].is_running = 0;
                        g_soft_timer_running_flag &= ~BIT(i);
                    }
                    if(g_soft_timer[i].p_time_handler){
                        g_soft_timer[i].p_time_handler(&g_soft_timer[i]);
                    }
                }
            }
            i++;
        }while(i < MAX_TIMER_NUM);
    }
}

//创建一个定时器
tuya_ble_status_t tuya_ble_timer_create(void** p_timer_id,uint32_t timeout_value_ms, tuya_ble_timer_mode mode,tuya_ble_timer_handler_t timeout_handler)
{
	soft_timer_t* timer_item = acquire_timer(timeout_value_ms,mode,timeout_handler);

    if(timer_item == NULL){
        return TUYA_BLE_ERR_NO_MEM;
    }

    *p_timer_id = timer_item;

    return TUYA_BLE_SUCCESS;
}

//删除一个定时器
tuya_ble_status_t tuya_ble_timer_delete(void* timer_id)
{
	soft_timer_t* timer_item = timer_id;
    int id = release_timer(timer_item);
    if(id == -1){
        return TUYA_BLE_ERR_NOT_FOUND;
    }
    return TUYA_BLE_SUCCESS;
}

//启动一个定时器
tuya_ble_status_t tuya_ble_timer_start(void* timer_id)
{
    soft_timer_t* timer_item = timer_id;
    int32_t idx = find_timer_obj(timer_item);
    if(idx >= 0){
        if(timer_item->is_running == 0){
            timer_item->ticks_start = tick_get();
            timer_item->is_running = 1;
            g_soft_timer_running_flag |= BIT(idx);
        }
        return TUYA_BLE_SUCCESS;
    } else {
        soft_timer_printf("no find timer\n");
        return TUYA_BLE_ERR_NOT_FOUND;
    }
}

//以新的定时时间重新启动一个定时器
tuya_ble_status_t tuya_ble_timer_restart(void* timer_id,uint32_t timeout_value_ms)
{
    soft_timer_t* timer_item = timer_id;
    int32_t idx = find_timer_obj(timer_item);
    if(idx >= 0){
        timer_item->is_running = 0;
        timer_item->ticks_to_expire = timeout_value_ms;
        timer_item->ticks_start = tick_get();
        timer_item->is_running = 1;
        g_soft_timer_running_flag |= BIT(idx);
        return TUYA_BLE_SUCCESS;
    } else {
        soft_timer_printf("no find timer\n");
        return TUYA_BLE_ERR_NOT_FOUND;
    }
}

//停止一个定时器
tuya_ble_status_t tuya_ble_timer_stop(void* timer_id)
{
    soft_timer_t* timer_item = timer_id;
    int32_t idx = find_timer_obj(timer_item);
    if(idx >= 0){
        if(timer_item->is_running){
            timer_item->is_running = 0;
            g_soft_timer_running_flag &= ~BIT(idx);
        }
        return TUYA_BLE_SUCCESS;
    } else {
        soft_timer_printf("no find timer\n");
        return TUYA_BLE_ERR_NOT_FOUND;
    }
}

#if 0
/*timer test*/
soft_timer_t* p_test;
soft_timer_t* p_test_2;

void tuya_timer_test_callback(void)
{
    printf("--->test:%d\n",tick_get());
}

void tuya_ble_timer_test(void)
{
    tuya_ble_timer_create((void**)&p_test,50,TUYA_BLE_TIMER_SINGLE_SHOT,(tuya_ble_timer_handler_t)tuya_timer_test_callback);
    tuya_ble_timer_start(p_test);
    tuya_ble_timer_create((void**)&p_test_2,1000,TUYA_BLE_TIMER_REPEATED,(tuya_ble_timer_handler_t)tuya_timer_test_callback);
    tuya_ble_timer_start(p_test_2);
}
#endif

#endif
