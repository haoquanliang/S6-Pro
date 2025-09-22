#ifndef __APP_FOTA_H
#define __APP_FOTA_H

typedef enum{
    FOTA_CON_NONE = 0,
    FOTA_CON_SPP,
    FOTA_CON_BLE,
}FOTA_CON_TYPE;

void fot_var_init(void);
void bsp_fot_init(void);
void fot_recv_proc(u8 *buf, u16 len);
void bsp_fot_process(void);
void fot_update_pause(void);
void fot_update_continue(void);
void fot_tws_connect_callback(void);
void fot_tws_disconnect_callback(void);
u8 is_fot_start(void);
bool bsp_fot_is_connect(void);
u8 fot_app_connect_auth(uint8_t *packet, uint16_t size, FOTA_CON_TYPE type);
void fot_spp_connect_callback(void);
void fot_spp_disconnect_callback(void);
void fot_ble_disconnect_callback(void);
void fot_ble_connect_callback(void);

#endif // __APP_FOTA_H
