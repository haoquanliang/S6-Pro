#ifndef __APP_INCLUDE_H_
#define __APP_INCLUDE_H_

#include "include.h"

#include "app_lr.h"
#include "app_role_switch.h"
#include "app_ringbuf.h"
#include "app_message.h"
#include "app_ble.h"
#include "app_dm.h"


#if SWETZ_BT_TO_PAIR
extern volatile uint8_t hfp_index;
#endif
extern uint btstack_get_call_indicate_for_index(uint8_t index);

#endif
