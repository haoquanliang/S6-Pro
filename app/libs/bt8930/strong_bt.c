/**********************************************************************
*
*   strong_bt.c
*   定义库里面bt部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


#if (!BT_FCC_TEST_EN && !FUNC_BT_FCC_EN) || !LE_EN
uint8_t vs_ble_test(void const *cmd) {
    return 0x11;
}

#if !LE_BQB_RF_EN
uint8_t ble_test_start(void* params) {
    return 0x0c;
}
uint8_t ble_test_stop(void) {
    return 0x0c;
}
#endif
#endif

#if BT_FCC_TEST_EN || FUNC_BT_FCC_EN
#if LE_EN
uint8_t vs_ble_test_do(void const *cmd);
uint8_t vs_ble_test(void const *cmd) {
    return vs_ble_test_do(cmd);
}
#endif
#else
AT(.com_text.bt21.isr.test.fcc)
bool bt_acl_test_rx_end(uint8_t index, void *par) {
    return false;
}
#if !IODM_TEST_EN
uint8_t vs_fcc_test_cmd(void const *param) {
    return 0x11;
}
#endif

#if !LE_BQB_RF_EN
void hci_h3c_init(void) {}
void bt_uart_init(void){}
AT(.com_text.stack.uart_isr)
bool bt_uart_isr(void) {
    return false;
}
#endif
#endif

#if !BT_HID_EN
void hid_device_init(void) {}
int bt_hid_is_connected(void) { return 0;}
void hid_establish_service_level_connection(void* bd_addr) {}
void hid_release_service_level_connection(void* bd_addr) {}
void *get_hid_device_context_for_bd_addr(void *bd_addr) {return NULL;}
void remove_hid_device_context(void * hid_device) {}
void btstack_hid_api(uint param) {}
const void *btstack_hid_tbl[0];
void btstack_hid_send(void) {}
bool bt_hid_send(void *buf, uint len, bool auto_release) { return false;}
bool bt_hid_send_key(uint type, uint keycode) {return false;}
bool bt_hid_is_send_complete(void) { return true;}
void hid_report_set(void *buf, uint len, bool auto_release) {}
bool bsp_bt_hid_vol_change(u16 keycode) {return 0;}
bool bsp_bt_hid_photo(u16 keycode) {return 0;}
void bsp_bt_hid_tog_conn(void) {}
#endif

#if !BT_HID_DOUYIN_EN
void btstack_hid_douyin(uint keycode){}
#endif

#if !BT_HID_EN || !BT_TWS_MS_SWITCH_EN
uint8_t tws_get_hid_service_report(uint8_t *data_ptr) {return 0;}
uint8_t tws_set_hid_service_report(uint8_t *data_ptr, uint8_t len) {return 0;}
uint8_t tws_get_hid_service(uint8_t *data_ptr){return 0;}
uint8_t tws_set_hid_service(uint8_t *data_ptr, uint16_t conhdl, u8 address[], uint8_t len){return 0;}
#endif


#if !BT_2ACL_EN
void avdtp_ready_switch_play(void) {};
#endif

#if !BT_AVDTP_DYN_LATENCY_EN
void a2dp_set_latency_ms(uint32_t a2dp_latency_ms) {}
void a2dp_latency_check(void) {}
uint16_t a2dp_calc_latency(int8_t rssi, uint16_t rxpkt_ok, uint8_t rxpkt_fail, uint8_t rxpkt_total) {return 0;}
AT(.com_text.sbc.play)
void a2dp_switch_latency(void) {}
#endif

#if !BT_A2DP_EXCEPT_RESTORE_PLAY_EN
void noload_set_play_state(uint8_t play_state){}
uint16_t noload_get_play_state(uint8_t play_state){return 0;}
void noload_clear_play_state(uint8_t play_state){}
void noload_reset_play_state() {}
#endif

WEAK uint8_t bt_get_connected_num(void) {return 0;}

#if !BT_HFP_REC_EN
AT(.com_text.bt_rec)
void bt_sco_rec_mix_do(u8 *buf, u32 samples) {}
void bt_sco_rec_fill_far_buf(u16 *buf, u16 samples) {}
#endif


#if !BT_SPP_EN
void spp_establish_service_level_connection(void *bd_addr) {}
void spp_release_service_level_connection(void *bd_addr) {}
void remove_spp_connection_context(void * spp_connection) {}
void * provide_spp_connection_context_for_bd_addr(void *bd_addr) { return NULL; }
void spp_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {}
void spp_service_flush(void) {}
void * get_spp_connection_context_for_bd_addr(void *bd_addr) { return NULL; }
void spp_init_var(void) {}
#endif

#if !BT_SPP_EN
void spp_txpkt_init(void) {}
AT(.com_text.spp.send_req)
void btstack_spp_send_req(void) {}
int spp_send_packet_channel0(void *context, void *buf) {return -1;}
AT(.com_text.spp.event_send)
void spp_event_send(uint16_t cid) {}
#endif

#if !(BT_PBAP_EN || BT_MAP_EN)
void goep_client_init() {}
uint8_t tws_get_goep_service(uint8_t *data_ptr) { return 0; }
uint8_t tws_set_goep_service(uint8_t *data_ptr, uint16_t conhdl, void *address, uint8_t len) { return 0; }
void goep_client_remove_connection(void) {}
#endif

#if !BT_MAP_EN
void map_client_init(void) {}
void btstack_map(uint param) {}
void bt_get_time(char *ptr){}
uint8_t tws_set_map_service(uint8_t *data_ptr, uint16_t conhdl, void *address, uint8_t len) { return 0; }
uint8_t tws_get_map_service(uint8_t *data_ptr) { return 0; }
void map_client_remove_connection(void) {}
int bt_map_is_connected(void) { return 0; }
#endif

#if !BT_PBAP_EN
void pbap_client_init(void) {}
void pbap_establish_service_level_connection(void* addr) {}
const void *btstack_pbap_tbl[0];
void btstack_pbap_api(uint param) {}
bool bt_pbap_is_connected(void) {return false;}
#endif

