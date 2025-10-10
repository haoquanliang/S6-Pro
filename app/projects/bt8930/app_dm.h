#ifndef __APP_DM_H_
#define __APP_DM_H_

typedef uint8_t bt_bd_addr_t[6];

typedef struct 
{
    bt_bd_addr_t ag_addr[2];  
}dm_t;

void app_dm_init(void);
void app_dm_handle_ag_connect(bt_bd_addr_t* addr);
void app_dm_handle_ag_disconnect(bt_bd_addr_t* addr);
uint8_t app_dm_get_connected_ag_num(void);
void app_dm_sync_info(void);
dm_t * app_dm_get_info(void);
bt_bd_addr_t *app_dm_get_ag_addr_by_index(uint8_t index);
bool app_dm_is_addr_connected(bt_bd_addr_t * addr);
bt_bd_addr_t *app_dm_get_next_ag_addr(bt_bd_addr_t * addr);
bool app_dm_is_bd_addr_zero(bt_bd_addr_t *addr);
u16 app_dm_get_software_version(void);
void print_bd_addr(const uint8_t addr[6]);
void print_bd_addr_sw(void);


#endif
