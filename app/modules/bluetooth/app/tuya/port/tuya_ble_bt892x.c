#include "include.h"
#include "tuya_ble_timer.h"
#include "tuya_ble_port.h"
#include "tuya_ble_type.h"
#include "aes.h"
#include "md5.h"
#include "tuya_ble_utils.h"

#if LE_TUYA_EN


//BLE广播包数据更新
tuya_ble_status_t tuya_ble_gap_advertising_adv_data_update(uint8_t const * p_ad_data, uint8_t ad_len)
{
    u8 res;
    res = ble_set_adv_data(p_ad_data,ad_len);
	return res ? TUYA_BLE_SUCCESS : TUYA_BLE_ERR_INVALID_PARAM;
}

//BLE扫描响应包数据更新
tuya_ble_status_t tuya_ble_gap_advertising_scan_rsp_data_update(uint8_t const *p_sr_data, uint8_t sr_len)
{
    u8 res;
    res = ble_set_scan_rsp_data(p_sr_data,sr_len);
	return res ? TUYA_BLE_SUCCESS : TUYA_BLE_ERR_INVALID_PARAM;
}

//断开BLE连接
tuya_ble_status_t tuya_ble_gap_disconnect(void)
{
    if(ble_is_connect()){
        ble_disconnect();
    }
	return TUYA_BLE_SUCCESS;
}

//BLE GATT发送数据
tuya_ble_status_t tuya_ble_gatt_send_data(const uint8_t *p_data,uint16_t len)
{
    bool result = ble_send_packet((u8*)p_data,len);
    if(result == true){
        return TUYA_BLE_SUCCESS;
    }else{
        return TUYA_BLE_ERR_UNKNOWN;
    }
}

//BLE 设备重启
tuya_ble_status_t tuya_ble_device_reset(void)
{
    WDT_RST();
	return TUYA_BLE_SUCCESS;
}

//获取设备mac地址
tuya_ble_status_t tuya_ble_gap_addr_get(tuya_ble_gap_addr_t *p_addr)
{
    tuya_ble_addr_get(p_addr->addr);
	return TUYA_BLE_SUCCESS;
}

//设置更新设备的mac地址
tuya_ble_status_t tuya_ble_gap_addr_set(tuya_ble_gap_addr_t *p_addr)
{
    uint8_t temp_addr[13];
    tuya_ble_hextoascii(p_addr->addr,6,temp_addr);
    memcpy(TUYA_DEVICE_MAC,temp_addr,12);
	return TUYA_BLE_SUCCESS;
}

//uint32_t g_cpu_ie AT(.tuya_data);
////进入临界区
//void tuya_ble_device_enter_critical(void)
//{
//	g_cpu_ie = PICCON&BIT(0);
//	PICCONCLR = BIT(0);
//}
//
////退出临界区
//void tuya_ble_device_exit_critical(void)
//{
//	PICCON |= g_cpu_ie;
//}


//ms级延时函数
void tuya_ble_device_delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

//us级延时函数
void tuya_ble_device_delay_us(uint32_t us)
{
    delay_us(us);
}

//随机数生成
tuya_ble_status_t tuya_ble_rand_generator(uint8_t* p_buf, uint8_t len)
{
    uint8_t temp,i;
    for(i=0; i<len; i++)
    {
        temp = get_random(0xff);
        p_buf[i] = temp;
    }
	return TUYA_BLE_SUCCESS;
}

//获取unix时间戳
tuya_ble_status_t tuya_ble_rtc_get_timestamp(uint32_t *timestamp,int32_t *timezone)
{
    *timestamp = 0;
    *timezone = 0;
	return TUYA_BLE_SUCCESS;
}

//更新unix时间戳
tuya_ble_status_t tuya_ble_rtc_set_timestamp(uint32_t timestamp,int32_t timezone)
{
	return TUYA_BLE_SUCCESS;
}

//NV初始化
tuya_ble_status_t tuya_ble_nv_init(void)
{
	return TUYA_BLE_SUCCESS;
}

//NV擦除函数
tuya_ble_status_t tuya_ble_nv_erase(uint32_t addr,uint32_t size)
{
    uint32_t erase_pages = size / TUYA_NV_ERASE_MIN_SIZE;
    uint16_t i;

    if((size % TUYA_NV_ERASE_MIN_SIZE) != 0) {
        erase_pages++;
    }

    for(i = 0;i < erase_pages; i++){
        os_spiflash_erase(addr+(i*TUYA_NV_ERASE_MIN_SIZE));
    }
	return TUYA_BLE_SUCCESS;
}

//NV写数据函数
tuya_ble_status_t tuya_ble_nv_write(uint32_t addr,const uint8_t *p_data, uint32_t size)
{
    u8 page_cnt = size / 256;
    u8 remain_len = size % 256;

    while(page_cnt){
       os_spiflash_program((void *)p_data,addr,256);
       p_data += 256;
       addr += 256;
       page_cnt--;
    }
    if(remain_len){
       os_spiflash_program((void *)p_data,addr,remain_len);
    }

	return TUYA_BLE_SUCCESS;
}

//NV读数据函数
tuya_ble_status_t tuya_ble_nv_read(uint32_t addr,uint8_t *p_data, uint32_t size)
{
    os_spiflash_read((void *)p_data,addr,size);
	return TUYA_BLE_SUCCESS;
}

AT(.tuya_text.aes128_cbc)
bool tuya_ble_aes128_ecb_encrypt(uint8_t *key,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    uint16_t length;

    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    length = input_len;

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128); //只用到了AUTH_KEY的128 bit

    while( length > 0 )
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }

    mbedtls_aes_free(&aes_ctx);

    return true;
}

AT(.tuya_text.aes128_cbc)
bool tuya_ble_aes128_ecb_decrypt(uint8_t *key,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    uint16_t length;
    mbedtls_aes_context aes_ctx;
    //
    if(input_len%16)
    {
        return false;
    }

    length = input_len;

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

    while( length > 0 )
    {
        mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, input, output );
        input  += 16;
        output += 16;
        length -= 16;
    }

    mbedtls_aes_free(&aes_ctx);

    return true;
}

AT(.tuya_text.aes128_cbc)
bool tuya_ble_aes128_cbc_encrypt(uint8_t *key,uint8_t *iv,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_aes_context aes_ctx;

    //
    if(input_len%16)
    {
        return false;
    }

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);

    mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_ENCRYPT,input_len,iv,input,output);
    //
    mbedtls_aes_free(&aes_ctx);

    return true;
}

AT(.tuya_text.aes128_cbc)
bool tuya_ble_aes128_cbc_decrypt(uint8_t *key,uint8_t *iv,uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_aes_context aes_ctx;

    if(input_len%16)
    {
        return false;
    }

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

    mbedtls_aes_crypt_cbc(&aes_ctx,MBEDTLS_AES_DECRYPT,input_len,iv,input,output);
    //
    mbedtls_aes_free(&aes_ctx);

    return true;
}

AT(.tuya_text.md5)
bool tuya_ble_md5_crypt(uint8_t *input,uint16_t input_len,uint8_t *output)
{
    mbedtls_md5_context md5_ctx;

    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, input, input_len);
    mbedtls_md5_finish(&md5_ctx, output);
    mbedtls_md5_free(&md5_ctx);


    return true;
}

AT(.tuya_text.hmac_sha1)
bool tuya_ble_hmac_sha1_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input, uint32_t input_len, uint8_t *output)
{
	return true;
}

AT(.tuya_text.hmac_sha256)
bool tuya_ble_hmac_sha256_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input, uint32_t input_len, uint8_t *output)
{
	return true;
}

tuya_ble_status_t tuya_ble_common_uart_send_data(const uint8_t *p_data,uint16_t len)
{
    // huart_putcs(p_data,len);
    return TUYA_BLE_SUCCESS;
}

#endif  //LE_TUYA_EN
