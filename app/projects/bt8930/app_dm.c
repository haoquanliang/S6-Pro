#include "include.h"

#include "app_lr.h"

#include "app_dm.h"

#if DEBUG_DM
#define DM_DEBUG(...)              printf(__VA_ARGS__)
#else
#define DM_DEBUG(...)
#endif

static dm_t dm;

void app_dm_init(void)
{
    memset(&dm, 0, sizeof(dm_t));
}

bool app_dm_is_bd_addr_zero(bt_bd_addr_t *addr)
{

}

uint8_t app_dm_get_connected_ag_num(void)
{

}

bt_bd_addr_t *app_dm_get_ag_addr_by_index(uint8_t index)
{

}

static bool dm_is_ag_addr_connected(bt_bd_addr_t * addr)
{

}

static uint8_t dm_get_ag_index_by_addr(bt_bd_addr_t * addr)
{

}

static uint8_t get_empty_ag_addr_index(void)
{

}

void app_dm_handle_ag_connect(bt_bd_addr_t* addr)
{

}

void app_dm_handle_ag_disconnect(bt_bd_addr_t* addr)
{

}

void app_dm_sync_info(void)
{

}

dm_t * app_dm_get_info(void)
{

}

bool app_dm_is_addr_connected(bt_bd_addr_t * addr)
{

}

bt_bd_addr_t *app_dm_get_next_ag_addr(bt_bd_addr_t * addr)
{

}

/*
 * return 0x1230 when sw version is "V1.2.3"
 */
u16 app_dm_get_software_version(void)
{

}


