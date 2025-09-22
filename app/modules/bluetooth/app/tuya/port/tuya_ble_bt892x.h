#ifndef TUYA_BLE_BT892X_H_
#define TUYA_BLE_BT892X_H_


#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_config.h"


#if (TUYA_BLE_LOG_ENABLE || TUYA_APP_LOG_ENABLE)

#define TUYA_BLE_PRINTF(...)        printf("\n");printf(__VA_ARGS__)
#define TUYA_BLE_HEXDUMP(...)       print_r(__VA_ARGS__)


#else

#define TUYA_BLE_PRINTF(...)
#define TUYA_BLE_HEXDUMP(...)

#endif

#define tuya_ble_device_enter_critical()    uint32_t tuya_cpu_ie = PICCON&BIT(0); PICCONCLR = BIT(0)
#define tuya_ble_device_exit_critical()     PICCON |= tuya_cpu_ie

#endif
