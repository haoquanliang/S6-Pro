#include "include.h"

#if LE_EN || BT_ATT_EN

static ble_gatt_characteristic_cb_info_t *characteristic_cb_info[LE_ATT_NUM] AT(.ble_cache.att);
static uint8_t  gatts_profile_table[LE_ATT_NUM * 10] AT(.ble_cache.att);
void priv_ble_gatts_service_init(void);
#if LE_EN
bool cfg_ble_sm_sc_en          = LE_SM_SC_EN;
bool cfg_ble_ctkd_en           = LE_CTKD_EN;
bool cfg_bt_ble_adv            = LE_ADV_POWERON_EN;
u8  le_pair_method             = LE_SM_PAIR_METHOD;
u32 cfg_sm_passkey             = 795612;            // Passkey between 000,000 ~ 999,999
u8  numeric_cmp_confirm        = 1;                 // Numeric Comparion Confirm (0: No ,1: Yes)

//可重定义该函数修改ble地址
#if LE_SM_SC_EN
void ble_get_local_bd_addr(u8 *addr)
{
    bt_get_local_bd_addr(addr);
}
#endif

#if APP_BLE_ADDR_CHANGE
const u8 static_ble_addr[6] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45};
void ble_get_local_bd_addr(u8 *addr)
{
    bt_get_local_bd_addr(addr);
#if !LE_SM_SC_EN
    addr[2] = addr[2] + 40;
#endif     
    // memcpy(addr, static_ble_addr, 6);
    //bt_get_local_bd_addr(addr);
}

#endif

#if LE_PAIR_EN
//可重定义该函数选择数字比较配对的结果
u8 ble_sm_numeric_comparison_confirm(uint32_t passkey)
{
    printf("Passkey Display:%06d, Confirm Yes or No:%d\n",passkey,numeric_cmp_confirm);
    return numeric_cmp_confirm;
}

//可重定义该函数修改 ble 配对 Passkey
u32 ble_sm_passkey_get(void)
{
    printf("ble_sm_passkey_get\n");
    cfg_sm_passkey = cfg_sm_passkey & 0xfffff;
    if (cfg_sm_passkey >= 999999){
        cfg_sm_passkey = cfg_sm_passkey - 999999;
    }
    printf("sm passkey display:%06d\n",cfg_sm_passkey);
    return cfg_sm_passkey;
}
#endif

//可重定义该函数修改ble地址类型
u8 ble_get_local_addr_mode(void)
{
    return LE_ADDRESS_TYPE;
}


void ble_disconnect_callback(void)
{
    printf("--->ble_disconnect_callback\n");
#if GFPS_EN
    gfps_ble_disconnected_callback();
#endif
    app_ble_disconnect_callback();
}

void ble_connect_callback(void)
{
    printf("--->ble_connect_callback\n");
#if GFPS_EN
    gfps_ble_connected_callback();
#endif
    app_ble_connect_callback();
}

//主从切换时获取需要同步给新主机的数据，返回数据长度
uint16_t role_switch_get_user_ble_data(uint8_t *data_ptr)
{
    printf("role_switch_get_user_ble_data\n");

    uint16_t len = app_ble_role_switch_get_data(data_ptr);

    return len;
}
//主从切换时接收到旧主机传递的数据
uint16_t role_switch_set_user_ble_data(uint8_t *data_ptr, uint16_t len)
{

    printf("role_switch_set_user_ble_data data\n");

    app_ble_role_switch_set_data(data_ptr,len);

    return len;
}

uint ble_get_bat_level(void)
{
    return bsp_get_bat_level();
}

#if LE_SM_SC_EN
void ble_get_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_read: %04x,%04x, %08lx\n", addr, size, BLE_CM_PAGE(addr));

    cm_read(buf, BLE_CM_PAGE(addr), size);

    //print_r(buf, size);
}

void ble_put_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_write: %04x,%04x, %08lx\n", addr, size, BLE_CM_PAGE(addr));
    //print_r(buf, size);

    cm_write(buf, BLE_CM_PAGE(addr), size);
}

void ble_sync_link_info(void)
{
    cm_sync();
}
#endif
#endif // LE_EN

void ble_sm_param_init(void)
{
    uint8_t    le_security   = 0;
    uint8_t    auth_req      = 0;
    uint8_t    io_capability = IO_CAPABILITY_NO_INPUT_NO_OUTPUT; // default :Just Work

    // setup SM
#if LE_PAIR_EN
        auth_req = SM_AUTHREQ_BONDING | SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_CT2;

        if(cfg_ble_sm_sc_en){
            auth_req        |=  SM_AUTHREQ_SECURE_CONNECTION;     // Enable LE Secure Connections pairing (LESC) ,otherwise ues Legacy pairing.
        }else{
            le_security  = 1;                                     // Send an sm request when ble connecting.
        }

        // Setup IO Capability.
        switch(le_pair_method){
            case JUST_WORK:
                io_capability = IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
                break;

            case PASSKEY_ENTRY:
                io_capability  = IO_CAPABILITY_DISPLAY_ONLY;
                break;

            case NUMERIC_COMPARISON:
                io_capability  = IO_CAPABILITY_DISPLAY_YES_NO;   // Numeric Comparison must enable LESC.
                break;
        }
#endif
        // printf("le_security:%d ,io_capability:%d, auth_req:0x%x\n",le_security,io_capability,auth_req);
        le_sm_init(le_security, io_capability, auth_req);
    
}


void ble_init_att(void)
{
    ble_sm_param_init();
    ble_gatts_init(gatts_profile_table, sizeof(gatts_profile_table),
                    characteristic_cb_info,
                    LE_ATT_NUM);
    app_ble_init();
    priv_ble_gatts_service_init();

#if LE_AB_FOT_EN
    ab_fot_gatt_app_init();
#endif
}

#endif // LE_EN || BT_ATT_EN
