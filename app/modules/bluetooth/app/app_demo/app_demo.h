#ifndef __APP_DEMO_H
#define __APP_DEMO_H

#define APP_DEMO_CTKD_EN         LE_SM_SC_EN             //连接BLE后自动连接经典蓝牙功能

int app_demo_ble_notify(u8 *buf, u8 len);
int app_demo_latt_notify(u8 *buf, u8 len);
int app_demo_spp_send(u8 *buf, u8 len);
bool app_demo_spp_rx_callback(u8 *data, u16 len);
void app_demo_gatt_app_init(void);
#endif
