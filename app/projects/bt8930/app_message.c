/*
* Copyright (c) 2021 - ruber.wang@qq.com
* All rights, including trade secret rights, reserved.
*/

#include "app_include.h"

#define DEBUG_MESSAGE 1

#if DEBUG_MESSAGE
#define MESSAGE_DEBUG(format, ...)     printf(format, ##__VA_ARGS__)
#else
#define MESSAGE_DEBUG(format, ...)
#endif

#define ENTER_CRITICAL() 
#define EXIT_CRITICAL()

message_mgr_t message_mgr;

static void message_process(message_item_t *messsage);

AT(.text.func.process)
bool message_send(uint16_t msg_id, uint16_t param, uint32_t delay)
{       
    if (message_mgr.number >= MESSAGE_NUM)
    {
        MESSAGE_DEBUG("m:full!\n");
        return false;
    }

    ENTER_CRITICAL();
    delay = sys_cb.SWETZ_tick + delay;
    
    /* Step 1: find the index to insert msg_id */
    /* from left to right , the delay time is getting longer and longer */
    uint8_t i = 0;
    for (i = 0; i < message_mgr.number; i++)
    {
        if (delay < message_mgr.queue[i].ticks_to)
        {
            break;
        }
    }

    /* Step 2: remove the events in the right side */
    if (i != message_mgr.number)
    {
        uint8_t j;

        for (j = 0; j < (message_mgr.number - i); j++)
        {
            message_mgr.queue[message_mgr.number - j] = message_mgr.queue[message_mgr.number - j - 1];
        }
    }

    /* Step 3: insert the msg_id */
    message_mgr.queue[i].ticks_to = delay;
    message_mgr.queue[i].param = param;
    message_mgr.queue[i].msg_id = msg_id;
    message_mgr.number++;

    EXIT_CRITICAL();
    return true;
}

AT(.text.func.process)
uint8_t message_search(uint16_t msg_id)
{
    ENTER_CRITICAL();
    
    uint8_t number = 0;
    uint8_t i;
    
    for (i = 0; i < message_mgr.number; i++)
    {
        if (message_mgr.queue[i].msg_id == msg_id)
        {
            number++;
        }
    }

    EXIT_CRITICAL();
    return number;
}

AT(.text.func.process)
void message_cancel_first(uint16_t msg_id)
{
    ENTER_CRITICAL();
    
    uint8_t i;

    for (i = 0; i < message_mgr.number; i++)
    {
        if (message_mgr.queue[i].msg_id == msg_id)
        {
            break;
        }
    }

    if (i != message_mgr.number)
    {
        uint8_t j = 0;
        for (j = 0; j < (message_mgr.number - i - 1); j++)
        {
            message_mgr.queue[i + j] = message_mgr.queue[i + j + 1];
        }
        message_mgr.number--;
    }

    EXIT_CRITICAL();
}

AT(.text.func.process)
void message_cancel_all(uint16_t msg_id)
{
    ENTER_CRITICAL();
    
    uint8_t number;
    uint8_t i;

    number = message_search(msg_id);

    for (i = 0; i < number; i++)
    {
        message_cancel_first(msg_id);
    }
    EXIT_CRITICAL();
}

AT(.text.func.process)
bool message_is_ready(void)
{
    ENTER_CRITICAL();
    
    uint8_t i;
    bool is_ready = false;
    uint32_t tick_temp = sys_cb.SWETZ_tick;

    for (i = 0; i < message_mgr.number; i++)
    {
        if (message_mgr.queue[i].ticks_to <= tick_temp)
        {
            is_ready = true;
            break;
        }
    }
    EXIT_CRITICAL();
    return is_ready;
}

AT(.text.func.process)
static bool message_dequeue_one_ready(message_item_t *msg)
{
    ENTER_CRITICAL();
    
    uint8_t i;
    bool result = false;
    uint32_t tick_temp = sys_cb.SWETZ_tick;
    
    for (i = 0; i < message_mgr.number; i++)
    {
        if (message_mgr.queue[i].ticks_to <= tick_temp)
        {
            *msg = message_mgr.queue[i];
            result = true;
            break;
        }
    }

    if (i != message_mgr.number)
    {
        uint8_t j = 0;
        for (j = 0; j < (message_mgr.number - i - 1); j++)
        {
            message_mgr.queue[i + j] = message_mgr.queue[i + j + 1];
        }
        message_mgr.number--;
    }

    EXIT_CRITICAL();
    return result;
}

AT(.text.func.process)
bool message_is_empty(void)
{
    ENTER_CRITICAL();
    bool is_empty = (message_mgr.number == 0);
    EXIT_CRITICAL();
    return is_empty;
}

AT(.text.func.process)
void message_init(void)
{
    ENTER_CRITICAL();
    memset(&message_mgr, 0, sizeof(message_mgr));
    EXIT_CRITICAL();
}

AT(.com_text.message)
void message_runnable(void)
{
    
    ENTER_CRITICAL();
    if (message_mgr.number)
    {
        while (message_is_ready())
        {
            message_item_t message;
            
            if (message_dequeue_one_ready(&message))
            {
                message_process(&message);
            }
        }
    }
    EXIT_CRITICAL();
}

AT(.text.func.process)
static void message_process(message_item_t *message)
{
    MESSAGE_DEBUG("m: %d\n", message->msg_id);
    switch (message->msg_id)
    {
        case MSG_ID_TIMEOUT_3S_AFTER_POWER_ON:
        if (func_cb.sta == FUNC_BT)
        {
                printf("22MSG_ID_TIMEOUT_3S_AFTER_POWER_ON\r\n");
        }     
        break;

#if SWETZ_OUTCASE_AFTER_NOT_KEY
        case MSG_ID_TIMEOUT_5S_AFTER_OUT_BOX:
             sys_cb.flag_outcase_5s_kye_null = false; 
             printf("MSG_ID_TIMEOUT_5S_AFTER_OUT_BOX\r\n");  
            break;
#endif

        default:
        break;
    }
}

