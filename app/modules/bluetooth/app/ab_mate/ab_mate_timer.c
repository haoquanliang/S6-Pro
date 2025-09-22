#include "include.h"
#include "ab_mate_timer.h"

#if AB_MATE_APP_EN

#define MAX_TIMER_NUM               6

//定时器
typedef struct
{
    u8                              is_running : 1;
    u8                              is_occupy : 1;
    soft_timer_mode                 time_mode;
    soft_timer_handler_t            p_time_handler;
    u32                             ticks_to_expire;
    u32                             ticks_start;
}soft_timer_t;

soft_timer_t g_soft_timer[MAX_TIMER_NUM] AT(.app.buf.timer);

u32 g_soft_timer_running_flag = 0;

void soft_timer_init(void)
{
    memset(g_soft_timer,0,sizeof(g_soft_timer));
    g_soft_timer_running_flag = 0;
}

static soft_timer_t* acquire_timer(uint32_t timeout_value_ms,soft_timer_mode mode,soft_timer_handler_t timeout_handle)
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

AT(.text.ab_mate.process)
void soft_timer_run(void)
{
    u8 i = 0;
    if(g_soft_timer_running_flag){
        do{
            if(g_soft_timer[i].is_running){
                if (tick_check_expire(g_soft_timer[i].ticks_start,g_soft_timer[i].ticks_to_expire)) {
                    if(g_soft_timer[i].time_mode == TIMER_REPEATED){
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

int soft_timer_create(void** p_timer_id,uint32_t timeout_value_ms, soft_timer_mode mode,soft_timer_handler_t timeout_handler)
{
	soft_timer_t* timer_item = acquire_timer(timeout_value_ms,mode,timeout_handler);

    if(timer_item == NULL){
        return -1;
    }

    *p_timer_id = timer_item;

    return 0;
}

int soft_timer_delete(void* timer_id)
{
	soft_timer_t* timer_item = timer_id;
    int id = release_timer(timer_item);
    if(id == -1){
        return -1;
    }
    return 0;
}


int soft_timer_start(void* timer_id)
{
    soft_timer_t* timer_item = timer_id;
    int32_t idx = find_timer_obj(timer_item);
    if(idx >= 0){
        if(timer_item->is_running == 0){
            timer_item->ticks_start = tick_get();
            timer_item->is_running = 1;
            g_soft_timer_running_flag |= BIT(idx);
        }
        return 0;
    } else {
        return -1;
    }
}


int soft_timer_restart(void* timer_id,uint32_t timeout_value_ms)
{
    soft_timer_t* timer_item = timer_id;
    int32_t idx = find_timer_obj(timer_item);
    if(idx >= 0){
        timer_item->is_running = 0;
        timer_item->ticks_to_expire = timeout_value_ms;
        timer_item->ticks_start = tick_get();
        timer_item->is_running = 1;
        g_soft_timer_running_flag |= BIT(idx);
        return 0;
    } else {
        return -1;
    }
}


int soft_timer_stop(void* timer_id)
{
    soft_timer_t* timer_item = timer_id;
    int32_t idx = find_timer_obj(timer_item);
    if(idx >= 0){
        if(timer_item->is_running){
            timer_item->is_running = 0;
            g_soft_timer_running_flag &= ~BIT(idx);
        }
        return 0;
    } else {
        return -1;
    }
}
#endif
