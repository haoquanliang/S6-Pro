/**********************************************************************
*
*   strong_le.c
*   定义库里面le部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"


void strong_le_include(void) {}
#if !LE_EN && !BT_ATT_EN
void ble_txpkt_init(void) {}
void btstack_gatt_init(void) {}
void ble_gatt_init_att(void **cb_info_table_p, uint16_t cb_info_tb_size) {}
bool ble_gatt_init_att_info(uint16_t att_handle, ble_gatt_characteristic_cb_info_t *att_cb_info) {return false;}
#endif

#if !BT_ATT_EN
void latt_establish_service_level_connection(uint8_t * bd_addr) {}
void latt_release_service_level_connection(uint8_t * bd_addr) {}
void latt_send_kick(void) {}
int latt_tx_notify(uint16_t att_handle, const uint8_t *value, uint16_t value_len) {return -1;}
void * provide_latt_connection_context_for_bd_addr(void * bd_addr) {return NULL;}
void remove_latt_connection_context(void * latt_con) {}
int latt_send_notify_packet(void *context, struct txbuf_tag *buf) {return -1;}
void latt_event_server_send(uint16_t cid) {}
void btstack_latt_send(void) {}
void btstack_latt_connect(void) {}
void btstack_latt_disconnect(void) {}
void latt_init_do(void) {}
#endif

#if !LE_EN
AT(.com_text.stack.ble.mtu)
u16 ble_get_gatt_mtu(void) { return 0; }
int ble_tx_notify_do(void *att, u8 idx, u16 handle, u8* buf, u8 len) {return 0x0c;}
int ble_tx_notify(u16 handle, u8* buf, u8 len) {return 0x0c;}
int ble_tx_indication(u16 handle, u8* buf, u8 len) {return 0x0c;}
void ble_update_conn_param(u16 interval, u16 latency, u16 timeout) {}
void ble_set_adv_interval(u16 interval) {}
void *get_app_ble_att_connect(void) {return NULL;}

void btstack_ble_send_req(void) {}
int ble_send_notify_packet(void *context, void *buf) {return -1;}
void att_event_server_send(void) {}
//void hci_run_le_connection(void) {}
bool ble_event_cmd_complete(uint8_t *packet, int size) { return false; }
void ble_event_meta(uint8_t *packet, int size) {}
void btstack_ble_init(void) {}
void btstack_ble_update_conn_param(void) {}
void btstack_ble_set_adv_interval(void) {}
void btstack_ble_set_adv_data(void) {}
void btstack_ble_set_scan_rsp_data(void) {}
void btstack_ble_disconnect(void) {}
void btstack_ble_send_sm_req(void) {}
bool ble_delete_bonding_for_addr(u8 address_type, u8 *address){return false;}
void ble_delete_bonding_all(void){}

typedef uint8_t (*bb_msg_func_t)(uint16_t index, void const *param);
typedef uint8_t (*bb_cmd_func_t)(uint16_t cmd, void const *param);
typedef uint8_t (*ll_cntl_func_t)(uint8_t opcode);

struct ll_cntl_pdu_info
{
    ll_cntl_func_t  func;
    const char*     desc;
    uint16_t        length;
    uint8_t         flag;
};

struct bb_msg_info
{
    uint16_t index;
    bb_msg_func_t func;
};

struct bb_cmd_info
{
    uint16_t index;
    bb_cmd_func_t func;
};

AT(.rodata.le.ll_proc)
const struct bb_cmd_info ll_hci_cmd_tbl[1] = {0};
AT(.rodata.le.ll_proc)
const struct bb_msg_info ll_msg_tbl[1] = {0};
AT(.rodata.le.ll_cntl)
const struct ll_cntl_pdu_info ll_cntl_pdu_tbl[1] = {0};
AT(.rodata.le.ll_proc)
const struct bb_msg_info mgr_hci_cmd_tbl[1] = {0};
void ll_init(uint8_t init_type) {}
uint8_t ll_start(uint8_t index, void *param) {return -1;}
void ll_stop(uint8_t index) {}
void ll_cntl_state_set(uint8_t index, uint8_t txrx, uint8_t state) {}
void ll_proc_timer_set(uint8_t index, uint8_t type, bool enable) {}
void ll_proc_timer_set_state(uint8_t index, uint8_t type, bool enable) {}
void ll_cntl_send(uint8_t index, void *pdu, ll_cntl_func_t tx_func) {}
void ll_cntl_tx_check(uint8_t index) {}
bool ble_adv_end_con_ind(void const *param) { return false; }
AT(.com_text.bb.ble.chs)
void ble_channel_assess(uint8_t channel, bool rx_ok, uint32_t ts) {}
AT(.com_text.bb.ble.end)
void ble_con_rx_end(uint8_t index, bool rx_ok, uint16_t rxchass) {}
AT(.com_text.bb.ble.isr)
void ble_pwr_ctrl_hook(uint8_t index) {}
AT(.com_text.bb.ble.adj)
void ble_sched_prio_time_adj(void *item) {}
void aes_init(uint8_t init_type) {};
void aes_result_handler(uint8_t status, uint8_t* result) {};
void ble_ecpy(uint8_t *key, uint8_t *enc_data) {}
void mgr_clk_acc_set(uint8_t aid, bool clk_acc) {}
void mgr_reset(void) {}
void mgr_1st_reset(void) {}
int ble_adv_end_ind_do(uint16_t const mid, void const *param, uint16_t const did, uint16_t const sid) {return 0;}
int ble_scan_end_ind_do(uint16_t const mid, void const *param, uint16_t const did, uint16_t const sid) {return 0;}

void ble_adv_rand_addr_update(uint8_t index, void *addr) {}
void ble_adv_scan_rsp_data_update(uint8_t index, uint8_t len, uint16_t buffer) {}
void ble_adv_adv_data_update(uint8_t index, uint8_t len, uint16_t buffer) {}
void ble_adv_restart(uint8_t index, uint16_t dura, uint8_t max_ext) {}
void * get_ble_link_ctrl_context_for_bd_addr(uint8_t *bd_addr) {return NULL;}
void * alloc_ble_link_ctrl_context_for_bd_addr(uint8_t *bd_addr, uint16_t provide_flag) {return NULL;}
void * provide_ble_link_ctrl_context_for_bd_addr(uint8_t *bd_addr, uint16_t provide_flag) {return NULL;}
void remove_ble_link_ctrl_context(void * link_ctrl, uint16_t provide_flag) {}
void *get_ble_link_ctrl_for_index(uint8_t index) {return NULL;}
bool ble_link_provide_check(uint8_t index, uint16_t provide_flag) {return false;}
void *get_ble_link_ctrl_for_handle(uint16_t con_handle) {return NULL;}

#endif

#if ((!BT_TWS_EN) || (!LE_EN))
uint16_t tws_get_ble_service(uint8_t *data_ptr){return 0;}
uint16_t tws_ble_get_adv_info(uint8_t *data_ptr){return 0;}
void tws_ble_set_adv_info(uint8_t *data_ptr, uint16_t size){}
void tws_set_ble_service(uint8_t *data_ptr, uint16_t size){}
void tws_send_ble_service_cfm(void) {}
void tws_send_ble_service_continue_cfm(void) {}
uint16_t tws_get_ble_link(uint8_t *data_ptr) { return 0; }
uint16_t tws_set_ble_link(uint8_t *data_ptr, uint16_t size) { return 0; }
bool tws_ble_get_link_info_bb(uint16_t *ble_conhdl, uint8_t *bdaddr) {return false;}
uint8_t tws_get_ble_service_bb(uint8_t *data_ptr) {return 0;}
void tws_send_ble_service_cfm_bb(void) {}
void tws_send_ble_service_continue_cfm_bb(void) {}
void tws_ble_switch_pend(void) {}
uint16_t tws_get_ble_service_continue_len(void) {return 0;}
void tws_ble_restore_tx(void) {}
void tws_ble_con_stop(void) {}
void tws_set_ble_service_continue(uint8_t *data_ptr, uint16_t size) {}
bool tws_ble_switch_save_pend(uint8_t type, uint8_t flag, uint32_t buf_ptr, uint16_t len) { return false; }
uint16_t tws_get_ble_service_continue(uint8_t *data_ptr) { return 0;}
int ll_ble_switch_to_func(uint16_t id, void *param, uint16_t did, uint16_t sid) {return 0;}
uint8_t tws_ble_switch_is_active(void) {return 0;}
void tws_le_init(bool reset) {}
void tws_ble_switch(void) {}
bool hci_acl_data_hook(u16 const did, void *param) {return false;}
bool ble_acl_data_rx_hook(u16 const did, void const *param) {return false;}
void hct_flush_le_acl_rx_pend(uint8_t index) {}
AT(.com_text.bb.ble.sw_nb)
uint8_t ble_get_con_nb(void) {return 0;}
int lc_op_vs_le_switch_func(u16 const mid, void const *param, u16 const dest_id, u16 const sid) {return 0;}
AT(.text.stk.tws.send)
bool tws_ble_l2cap_send_req(void) {return false;}
#endif

#if !LE_PAIR_EN
#if (LE_ADDRESS_TYPE == GAP_RANDOM_ADDRESS_TYPE_OFF)
void gap_random_address_set(void) {}
void gap_random_address_set_mode(void) {}
void le_device_db_init(void){}
int gap_authorization_state(uint16_t con_handle){return 3;}
void sm_get_rand_addr(uint8_t * addr) {}
#endif
void sm_just_works_confirm(void) {}
void sm_request_pairing(void) {}
void sm_send_security_request(void) {}
int sm_authorization_state(void) { return 0; }
int sm_authenticated(void) { return 0; }
int sm_encryption_key_size(void) { return 0; }
void sm_add_event_handler(void) {}
void sm_cmac_signed_write_start(void){ }
int sm_cmac_ready(void) { return 0; }
uint16_t sm_set_setup_context(uint8_t *ptr){ return 0;}
uint16_t sm_get_setup_context(uint8_t *ptr){ return 0;}
uint16_t tws_get_sm_db_info(uint8_t *ptr){ return 0;}
uint16_t tws_set_sm_db_info(uint8_t *ptr, uint16_t len){ return 0;}
#endif

#if !LE_ADV0_EN
void ble_adv0_init(uint8_t init_type) {}
uint8_t adv0_data[1];
void btstack_adv0_ctrl(uint opcode) {}
bool ble_adv0_end_ind(uint8_t id) { return false;}
void ble_adv0_set_en(bool adv_en) {}
void ble_adv0_update_adv_data(const uint8_t *adv_ptr, u32 size) {}
#endif

#if !LE_PRIV_EN
void btstack_priv_con_ctrl(uint parms) {}
int ble_priv_tx_notify(u16 att_handle, u8* buf, u8 len) {return 0;}
void ble_priv_adv_en(u16 opcode) {}
bool ble_priv_is_connect(void) {return false;}
void ble_priv_con_discon(void) {}
uint8_t priv_adv_data[1];
void ble_priv_adv_init(uint8_t init_type) {};
void ble_priv_adv_set_en(bool adv_en) {}
bool ble_is_priv_adv_id(u8 id)
{
    return false;
}
bool ble_priv_adv_get_acc_addr(u8 act_id, u32 *acc_addr)
{
    return false;
}
void ble_priv_adv_update_adv_data(const uint8_t *adv_ptr, u32 size) {}
void ble_priv_adv_set_interval(u16 interval) {}
#endif
