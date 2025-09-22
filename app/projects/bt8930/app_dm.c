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

bool app_dm_is_bd_addr_zero(bt_bd_addr_t *addr)//用于判断给定的蓝牙地址是否为零地址（即全0）
{
    bt_bd_addr_t addr_zero;

    memset(&addr_zero, 0, sizeof(bt_bd_addr_t));
    if (memcmp(&addr_zero, addr, sizeof(bt_bd_addr_t)) == 0)
    {
        return true;
    }
    else 
    {
        return false;
    }

}

uint8_t app_dm_get_connected_ag_num(void)//统计并返回当前已连接的音频网关设备的数量
{
    uint8_t i;
    uint8_t num = 0;

    for (i = 0; i < 2; i++)
    {
        if (!app_dm_is_bd_addr_zero(&dm.ag_addr[i]))
        {
            num++;
        }
    }  
    return num;
}

bt_bd_addr_t *app_dm_get_ag_addr_by_index(uint8_t index)//根据指定的索引获取已连接的音频网关设备的蓝牙地址
{
    if (index < 2)
    {
        if (!app_dm_is_bd_addr_zero(&dm.ag_addr[index]))
        {
            return &dm.ag_addr[index];
        }
    }
    return NULL;
}


//检查给定的蓝牙地址 addr 是否存在于已连接的音频网关设备列表中
static bool dm_is_ag_addr_connected(bt_bd_addr_t * addr)
{
    bt_bd_addr_t *temp_addr;
    uint8_t i;

    for (i = 0; i < 2; i++)
    {
        temp_addr = app_dm_get_ag_addr_by_index(i);
        if (temp_addr
        && addr
        && (memcmp(temp_addr, addr, sizeof(bt_bd_addr_t)) == 0)
        )
        {
            return true;
        }
    }
    return false;
}


//0 或 1: 找到的索引位置（有效范围）
// 0xff: 如果地址无效（空指针或零地址）或未找到匹配的地址
static uint8_t dm_get_ag_index_by_addr(bt_bd_addr_t * addr)//根据给定的蓝牙设备地址，查找其在音频网关设备列表中的索引位置
{
    uint8_t i;

    if (!addr || app_dm_is_bd_addr_zero(addr))
    {
        return 0xff;
    }
    
    for (i = 0; i < 2; i++)
    {
        if (memcmp(addr, &dm.ag_addr[i], sizeof(bt_bd_addr_t)) == 0)
        {
            return i;
        }
    }
    return 0xff;
}


// 0 或 1: 找到的空槽位索引
// 0xff: 如果没有找到空槽位（所有位置都被占用）
static uint8_t get_empty_ag_addr_index(void)//查找音频网关设备列表中的第一个空槽位（零地址位置）的索引
{
    uint8_t i;
    bool found = false;
    
    for (i = 0; i < 2; i++)
    {
        if (app_dm_is_bd_addr_zero(&dm.ag_addr[i]))
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        i = 0xFF;
    }
    return i;
}

void app_dm_handle_ag_connect(bt_bd_addr_t* addr)//处理音频网关设备的连接请求，包括验证连接条件、存储设备地址和触发相关事件 addr: 指向要连接的蓝牙设备地址的指针
{

}

void app_dm_handle_ag_disconnect(bt_bd_addr_t* addr)//处理音频网关设备的断开连接事件，包括清除设备地址和触发相关事件  addr: 指向要断开连接的蓝牙设备地址的指针
{

}

void app_dm_sync_info(void)//在 TWS 连接且设备为主设备时，将设备管理信息同步到从设备
{
    if (bt_tws_is_connected() && (!bt_tws_is_slave()))
    {
        u8 sync_data[TWS_SYNC_BUF_SIZE];
        
        memcpy(&sync_data[0], &dm, sizeof(dm_t));
        app_lr_send_notification(LR_NOTIFY_SYNC_DM_INFO, sizeof(dm_t), &sync_data[0]);   //未做
    }
}

dm_t * app_dm_get_info(void)//返回指向设备管理结构体 dm 的指针，允许其他模块访问设备管理信息
{
    return &dm;
}

bool app_dm_is_addr_connected(bt_bd_addr_t * addr)//检查给定的蓝牙设备地址是否已连接到系统中的音频网关设备  addr: 指向要检查的蓝牙设备地址的指针
{
    bt_bd_addr_t *temp_addr;
    uint8_t i;

    if (app_dm_is_bd_addr_zero(addr))
    {
        return false;
    }
    
    for (i = 0; i < 2; i++)
    {
        temp_addr = app_dm_get_ag_addr_by_index(i);
        if (temp_addr
        && addr
        && (memcmp(temp_addr, addr, sizeof(bt_bd_addr_t)) == 0)
        )
        {
            return true;
        }
    }
    return false;
}

bt_bd_addr_t *app_dm_get_next_ag_addr(bt_bd_addr_t * addr)
{
    uint8_t index;

    if (app_dm_get_connected_ag_num() == 2)
    {
        if (app_dm_is_addr_connected(addr))
        {
            index = dm_get_ag_index_by_addr(addr);
            if (index != 0xff)
            {
                if (index == 0)
                {
                    index = 1;
                }
                else
                {
                    index = 0;
                }
                return app_dm_get_ag_addr_by_index(index);
            }
        }
    }
    return NULL;
}

/*
 * return 0x1230 when sw version is "V1.2.3"
 */
u16 app_dm_get_software_version(void)
{

}


