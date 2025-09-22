#ifndef __TUYA_BLE_OTA_H
#define __TUYA_BLE_OTA_H

/*********************************************************************
 * CONSTANTS
 */
#define TUYA_OTA_TWS_EN          FOT_SUPPORT_TWS
#define TUYA_OTA_PKG_LEN         (256)       //max 256
#define TUYA_OTA_FILE_MD5_LEN    16
#define TUYA_OTA_START_ADDR      (FLASH_SIZE/2)
#define TUYA_OTA_END_ADDR        (FLASH_SIZE - 0x6000 - 0x4000)
#define TUYA_OTA_FILE_MAX_LEN    (TUYA_OTA_END_ADDR - TUYA_OTA_START_ADDR + 0x1000)

typedef enum
{
	TUYA_BLE_TWS_OTA_REQ,
	TUYA_BLE_TWS_OTA_FILE_INFO,
	TUYA_BLE_TWS_OTA_FILE_OFFSET,
	TUYA_BLE_TWS_OTA_DATA,
    TUYA_BLE_TWS_OTA_ERR,
    TUYA_BLE_TWS_OTA_UPDATE_DONE,
    TUYA_BLE_TWS_OTA_EXIT,
}tuya_ble_tws_info_sync_t;

/*********************************************************************
 * STRUCT
 */
typedef struct __attribute__((packed)){
	uint8_t  flag;
	uint8_t  ota_version;
    uint8_t  type;
    uint32_t version;
    uint16_t package_maxlen;
} tuya_ota_req_rsp_t;

typedef struct __attribute__((packed)){
	uint8_t  type;
	uint8_t  pid[8];
    uint32_t version;
    uint8_t  md5[TUYA_OTA_FILE_MD5_LEN];
    uint32_t file_len;
    uint32_t crc32;
} tuya_ota_file_info_t;

typedef struct __attribute__((packed)){
	uint8_t  type;
	uint8_t  state;
    uint32_t save_file_len;
    uint32_t save_file_crc32;
    uint8_t  save_file_md5[TUYA_OTA_FILE_MD5_LEN];
} tuya_ota_file_info_rsp_t;

typedef struct __attribute__((packed)){
	uint8_t  type;
    uint32_t offset;
} tuya_ota_file_offset_t;

typedef struct __attribute__((packed)){
	uint8_t  type;
    uint32_t offset;
} tuya_ota_file_offset_rsp_t;

typedef struct __attribute__((packed)){
	uint8_t  type;
    uint16_t pkg_id;
    uint16_t len;
    uint16_t crc16;
    uint8_t  data[];
} tuya_ota_data_t;

typedef struct __attribute__((packed)){
	uint8_t type;
    uint8_t state;
} tuya_ota_data_rsp_t;

typedef struct __attribute__((packed)){
	uint8_t  type;
    uint8_t state;
} tuya_ota_end_rsp_t;

typedef struct __attribute__((packed)){
	uint32_t len;
    uint32_t crc32;
    //uint8_t  md5[TUYA_OTA_FILE_MD5_LEN];
} tuya_ota_file_info_storage_t;


uint32_t tuya_ble_app_ota_init(void);
void tuya_ble_app_ota_handler(u8 type,u8 *data,u32 len);
bool tuya_ble_ota_is_start(void);
void tuya_ble_ota_tws_data_proc(uint8_t *data_ptr, u16 size);
void tuya_ble_ota_process(void);
void tuya_ble_tws_ota_set_data(uint8_t *data_ptr, uint16_t size);

#endif // __TUYA_BLE_OTA_H
