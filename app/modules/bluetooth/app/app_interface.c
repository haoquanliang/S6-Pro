#include "include.h"
#include "app.h"

void app_ble_connect_callback(void)
{
#if LE_AB_FOT_EN
    fot_ble_connect_callback();
#endif
    app_ctrl_ble_connect_callback();
}

void app_ble_disconnect_callback(void)
{
#if LE_AB_FOT_EN
    fot_ble_disconnect_callback();
#endif
    app_ctrl_ble_disconnect_callback();
}

void app_process(void)
{
#if LE_AB_FOT_EN || BT_AB_FOT_EN
    bsp_fot_process();
#endif
    app_ctrl_process();
}

void app_var_init(void)
{
#if LE_AB_FOT_EN || BT_AB_FOT_EN
    fot_var_init();
#endif
    app_ctrl_var_init();
}

void app_ble_init(void)
{
    app_ctrl_ble_init();
}

void app_init_do(void)
{
    app_ctrl_init_do();
}

u8 app_eq_set(void)
{
    printf("app_eq_set\r\n");
    return app_ctrl_eq_set();
}

void app_enter_sleep(void)
{
    app_ctrl_enter_sleep();
}

void app_exit_sleep(void)
{
    app_ctrl_exit_sleep();
}

bool app_need_wakeup(void)
{
#if LE_AB_FOT_EN || BT_AB_FOT_EN
    if (bsp_fot_is_connect()) {
        return true;
    }
#endif
    return app_ctrl_need_wakeup();
}

void app_bt_evt_notice(uint evt, void *params)
{
    app_ctrl_bt_evt_notice(evt, params);
}

uint16_t app_ble_role_switch_get_data(uint8_t *data_ptr)
{
    return app_ctrl_ble_role_switch_get_data(data_ptr);
}

uint16_t app_ble_role_switch_set_data(uint8_t *data_ptr, uint16_t len)
{
    return app_ctrl_ble_role_switch_set_data(data_ptr, len);
}

void app_sleep_mode_process(void)
{
    app_ctrl_sleep_mode_process();
}

void app_tws_user_key_process(uint32_t *opcode)
{
    app_ctrl_tws_user_key_process(opcode);
}

void app_tws_connect_callback(void)
{
#if LE_AB_FOT_EN || BT_AB_FOT_EN
    fot_tws_connect_callback();
#endif
}

void app_tws_disconnect_callback(void)
{
#if LE_AB_FOT_EN || BT_AB_FOT_EN
    fot_tws_disconnect_callback();
#endif
}

void app_spp_connect_callback(uint8_t ch)
{
    if (ch == SPP_SERVICE_CH0) {
#if BT_AB_FOT_EN
        fot_spp_connect_callback();
#endif
        app_ctrl_spp_connect_callback();
    } else if (ch == SPP_SERVICE_CH1){
        app_ctrl_spp1_connect_callback();
    } else if (ch == SPP_SERVICE_CH2){
        app_ctrl_spp2_connect_callback();
    }
}

void app_spp_disconnect_callback(uint8_t ch)
{
    if (ch == SPP_SERVICE_CH0) {
#if BT_AB_FOT_EN
        fot_spp_disconnect_callback();
#endif
        app_ctrl_spp_disconnect_callback();
    } else if (ch == SPP_SERVICE_CH1){
        app_ctrl_spp1_disconnect_callback();
    } else if (ch == SPP_SERVICE_CH2){
        app_ctrl_spp2_disconnect_callback();
    }
}

bool app_spp_rx_callback(uint8_t ch, u8 *data,u16 len)
{
    if (ch == SPP_SERVICE_CH0) {
#if BT_AB_FOT_EN
        if(fot_app_connect_auth(data, len, FOTA_CON_SPP)){
            fot_recv_proc(data, len);
            return true;
        }
#endif
        if (app_test_cmd_process(data, len, 0)) {
            return true;
        }
        return app_ctrl_spp_rx_callback(data, len);
    } else if (ch == SPP_SERVICE_CH1){
        return app_ctrl_spp1_rx_callback(data, len);
    } else if (ch == SPP_SERVICE_CH2){
        return app_ctrl_spp2_rx_callback(data, len);
    }

    return true;
}
