#include "dueros_dma_extern.h"
#include "dueros_dma_app.h"
#include "api_cm.h"
#include "dueros_dma_profile.h"
#include "api_btstack.h"
#include "xcfg.h"
#include "dueros_dma_sha256.h"
#include "dueros_dma_tws.h"

#if LE_DUEROS_DMA_EN

#define DMA_WRAPPER_DEBUG_EN       1

#if DMA_WRAPPER_DEBUG_EN
#define DMA_WRAPPER_DEBUG(...)                  printf(__VA_ARGS__)
#define DMA_WRAPPER_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define DMA_WRAPPER_DEBUG(...)
#define DMA_WRAPPER_DEBUG_R(...)
#endif

#define HEAP_SIZE           (14*1024)
uint16_t cfg_sys_heap_size = HEAP_SIZE;
u8 mem_heap[HEAP_SIZE] AT(.dueros_dma_queue_memory);

static sha256_ctx_t dueros_dma_wrap_ctx;
DUER_DMA_OPER duer_dma_oper;
uint8_t dueros_dma_wrap_malloc_memory_pool[DMA_MALLOC_MEMORY_POOL_SIZE] AT(.dueros_dma_malloc_memory);
uint8_t dueros_dma_wrap_queue_memory_pool[DMA_QUEUE_MEMORY_POOL_SIZE] AT(.dueros_dma_queue_memory);

const char number_transforam_table[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

/*********************************************system operation*********************************************/

/**
* @brief 断言
*/
void dma_assert(int cond, ...)
{
    va_list param;
    va_start(param, cond);
    char *ptr = NULL;
    ptr = ptr;
    if(cond == false){
        DMA_WRAPPER_DEBUG("dma assert err!\n");
        ptr = va_arg(param, char*);
        DMA_WRAPPER_DEBUG("%s\n", ptr);
    }
    va_end(param);
}

/**
* @brief 获取DMA设备能力集
*
* @param[out] device_capability      设备能力集
*
* @return 是否获取到设备能力集
*     @retval false 获取能力集失败
*     @retval true 获取能力集成功
*/
bool dueros_dma_wrap_get_device_capability(DUER_DMA_CAPABILITY* device_capability)
{
   return dueros_dma_app_get_device_capability(device_capability);
}

/**
* @brief 获取DMA设备版本号
*
* @param[out] fw_rev_0      4位固件版本号第一位
* @param[out] fw_rev_1      4位固件版本号第二位
* @param[out] fw_rev_2      4位固件版本号第三位
* @param[out] fw_rev_3      4位固件版本号第四位
*
* @return 是否获取DMA设备版本号
*     @retval false 获取版本号失败
*     @retval true 获取版本号成功
* @note 使用小度APP的DMA及OTA功能要求设备版本号必须4位，格式为xx.xx.xx.xx \n
* 每位取值范围为0-99,同一产品禁止不同固件使用相同版本号
// */
bool dueros_dma_wrap_get_firmeware_version(uint8_t* fw_rev_0, uint8_t* fw_rev_1,
                                       uint8_t* fw_rev_2, uint8_t* fw_rev_3)
{
    if(fw_rev_0 != NULL){
        *fw_rev_0 = (DUEROS_DMA_FW_VERSION[0]-'0')*10;
        *fw_rev_0 += DUEROS_DMA_FW_VERSION[1]-'0';
    }

    if(fw_rev_1 != NULL){
        *fw_rev_1 = (DUEROS_DMA_FW_VERSION[3]-'0')*10;
        *fw_rev_1 += DUEROS_DMA_FW_VERSION[4]-'0';
    }

    if(fw_rev_2 != NULL){
        *fw_rev_2 = (DUEROS_DMA_FW_VERSION[6]-'0')*10;
        *fw_rev_2 += DUEROS_DMA_FW_VERSION[7]-'0';
    }

    if(fw_rev_3 != NULL){
        *fw_rev_3 = (DUEROS_DMA_FW_VERSION[9]-'0')*10;
        *fw_rev_3 += DUEROS_DMA_FW_VERSION[10]-'0';
    }
   return true;
}

/**
* @brief DMA协议栈动态内存初始化
*
* @note 固定4KByte内存
*/
void dueros_dma_wrap_heap_init(void)
{
   dma_mm_init((uint32_t)&dueros_dma_wrap_malloc_memory_pool[0], (uint32_t)&dueros_dma_wrap_malloc_memory_pool[DMA_MALLOC_MEMORY_POOL_SIZE-1]);
}

/**
* @brief DMA协议栈内存申请
*
* @param[in] size    内存申请大小
*
* @return 是否申请到内存
*     @retval NULL 表示内存申请失败
*     @retval 非NULL 表示内存申请成功
* @note 该内存申请用于DMA协议栈，内存申请峰值不超过4KB \n
* 该接口应保证DMA设备在任何使用模式下均可申请内存，线程安全
*/
void* dueros_dma_wrap_heap_malloc(size_t size)
{
   return dma_malloc(size);
}

/**
* @brief DMA协议栈内存释放
*
* @param[in] ptr    释放内存地址
*
* @note 该接口应保证DMA设备在任何使用模式下均可释放内存，线程安全
*/
void dueros_dma_wrap_heap_free(void* ptr)
{
   dma_free(ptr);
}

/**
* @brief DMA协议栈线程创建
*
* @note 优先级介于main和btstack之间，堆栈大小2048Byte，阻塞型
*/
void dueros_dma_wrap_thread_create(void)
{
   os_dueros_dma_thread_create();
}

/**
* @brief DMA协议栈等待信号量
*
* @param[in] timeout_ms    等待信号量超时时间，单位毫秒，0xFFFFFFFF表示wait for ever
*
* @return 是否等到信号
*     @retval false 等待信号量超时
*     @retval true 等待信号量成功
*
*/
bool dueros_dma_wrap_sem_wait(uint32_t timeout_ms)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]:%s\n", __func__);
   int32_t res = os_dueros_dma_thread_sem_wait(timeout_ms);
   bool ret = (res == 0) ? true : false;
   return ret;
}

/**
* @brief DMA协议栈释放信号量
*
* @return 是否释放信号成功
*     @retval false 释放信号量失败
*     @retval true 释放信号量成功
*
*/
bool dueros_dma_wrap_sem_signal(void)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]:%s\n", __func__);
   int32_t res = os_dueros_dma_thread_sem_signal();
   bool ret = (res == 0) ? true : false;
   return ret;
}

/**
* @brief DMA协议栈互斥锁加锁
*
* @param[in] mutex_id    互斥锁id
*
* @return 互斥锁加锁是否成功
*     @retval false 互斥锁加锁失败
*     @retval true 互斥锁加锁成功
*
*/
bool dueros_dma_wrap_mutex_lock(DMA_MUTEX_ID mutex_id)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]:%s\n", __func__);
   int32_t res = os_dueros_dma_thread_mutex_lock(mutex_id);
   bool ret = (res == 0) ? true : false;
   return ret;
}

/**
* @brief DMA协议栈互斥锁去锁
*
* @param[in] mutex_id    互斥锁id
*
* @return 互斥锁去锁是否成功
*     @retval false 互斥锁加锁失败
*     @retval true 互斥锁加锁成功
*
*/
bool dueros_dma_wrap_mutex_unlock(int32_t mutex_id)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]:%s\n", __func__);
   int32_t res = os_dueros_dma_thread_mutex_unlock(mutex_id);
   bool ret = (res == 0) ? true : false;
   return ret;
}

/**
* @brief 获取设备端的DMA自定义用户配置信息
*
* @param[out] dma_wrap_userdata_config     设备端存储的DMA自定义用户配置信息
*
* @return 获取用户配置信息是否成功
*     @retval false 获取设备端的DMA自定义用户配置信息失败
*     @retval true 获取设备端的DMA自定义用户配置信息成功
*
* @note 该接口应保证DMA设备在任何使用模式下均可获取，线程安全 \n
* 该配置信息须保持在非掉电易失的存储空间
*/
bool dueros_dma_wrap_get_userdata_config(DMA_USER_DATA_CONFIG* dma_userdata_config)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   cm_read(&dma_userdata_config->dma_config_data[0],   DUEROS_DMA_PARAM_PAGE0(0), 250);
   cm_read(&dma_userdata_config->dma_config_data[250], DUEROS_DMA_PARAM_PAGE1(0), 250);
   cm_read(&dma_userdata_config->dma_config_data[500], DUEROS_DMA_PARAM_PAGE2(0), 12);
   return true;
}

/**
* @brief 写入设备端的DMA自定义用户配置信息
*
* @param[in] dma_wrap_userdata_config     设备端存储的DMA自定义用户配置信息
*
* @return 写入用户配置信息是否成功
*     @retval false 写入设备端的DMA自定义用户配置信息失败
*     @retval true 写入设备端的DMA自定义用户配置信息成功
*
* @note 该接口应保证DMA设备在任何使用模式下均可写入，线程安全 \n
* 该配置信息须保持在非掉电易失的存储空间
*/
bool dueros_dma_wrap_set_userdata_config(const DMA_USER_DATA_CONFIG* dma_userdata_config)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   cm_write((void *)&dma_userdata_config->dma_config_data[0],   DUEROS_DMA_PARAM_PAGE0(0), 250);
   cm_write((void *)&dma_userdata_config->dma_config_data[250], DUEROS_DMA_PARAM_PAGE1(0), 250);
   cm_write((void *)&dma_userdata_config->dma_config_data[500], DUEROS_DMA_PARAM_PAGE2(0), 12);
   cm_sync();
   return true;
}

/**
* @brief 随机数
*
* @return 无符号32bit随机数
*
*/
uint32_t dueros_dma_wrap_get_rand(void)
{
   return sys_get_rand_key();
}

/**
* @brief 获取DMA设备是否处于OTA升级状态
*
* @return 当前是否处于OTA状态
*     @retval false 当前非OTA升级状态
*     @retval true 当前为OTA升级状态
* @note TWS类设备两侧调用该接口须有相同的返回值 \n
*/
bool dueros_dma_wrap_get_ota_state(void)
{
   return false;
}

/**
* @brief 获取DMA设备是否升级成功
*
* @return 是否升级成功
*     @retval 0 未进行升级
*     @retval 1 升级成功
*     @retval 2 升级失败
* @note TWS类设备两侧调用该接口须有相同的返回值 \n
*/
uint8_t dueros_dma_wrap_get_upgrade_state(void)
{
   return 0;
}

/**
* @brief 获取DMA设备连接的手机类型
*
* @return 手机类型
*     @retval 0 未连接手机
*     @retval 1 连接iOS手机
*     @retval 2 连接Android手机
* @note TWS类设备两侧调用该接口须有相同的返回值 \n
*/
int32_t dueros_dma_wrap_get_mobile_connect_type(void)
{
   int32_t ret = 0;
   if (bt_nor_is_connected()) {
      if (bt_is_ios_device()) {
         ret = 1;
      } else {
         ret = 2;
      }
   } else {
      ret = 0;
   }
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s, ret:%d\n", __func__, ret);
   return ret;
}

/*********************************************BLE control*********************************************/

/**
* @brief 设置DMA设备BLE广播包
*
* @param[in] adv_data   DMA广播的广播包，长度不超过31字节
* @param[in] adv_data_len   DMA广播的广播包长度
* @param[in] scan_response    DMA广播的响应包，长度不超过31字节
* @param[in] scan_response_len   DMA广播的响应包长度
* @param[in] ibeacon_adv_data    iBeacon广播的广播包，长度不超过31字节
* @param[in] ibeacon_adv_data_len    iBeacon广播包长度

* @return 广播包是否设置成功
*     @retval false 设置DMA设备BLE广播包失败
*     @retval true 设置DMA设备BLE广播包成功
*
* @note TWS类设备仅在Master使能有效，当ibeacon_adv_data非空时， 开启BLE广播后 \n
* 须进行DMA和iBeacon的交错广播，交错格式为ABABAB……，两者的间隔不应大于100ms
*/
bool dueros_dma_wrap_set_ble_advertise_data(const char* adv_data, uint8_t adv_data_len,
                                       const char* scan_response, uint8_t scan_response_len,
                                       const char* ibeacon_adv_data, uint8_t ibeacon_adv_data_len)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s, ibeacon_adv_data_len:%d\n", __func__, ibeacon_adv_data_len);
   ble_update_scan_data((uint8_t *)scan_response, scan_response_len);
   ble_update_adv_data((uint8_t *)adv_data, adv_data_len);
   return true;
}

/**
* @brief 设置DMA设备是否使能BLE广播
*
* @param[in] on    BLE广播的使能标志
*     @arg true 开启广播
*     @arg false 关闭广播
*
* @return 开关BLE广播是否成功
*     @retval false 开关BLE广播失败
*     @retval true 开关BLE广播成功
*
* @note TWS类设备仅在Master开启广播有效 \n
*/
bool dueros_dma_wrap_set_ble_advertise_enable(bool on)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, on:%d\n", __func__, on);
   if (on) {
      ble_adv_en();
   } else {
      ble_adv_dis();
   }
   return true;
}

/**
* @brief 获取DMA设备的BT MAC地址
*
* @param[in] addr_type    BTMAC地址类型
*     @arg DMA_BT_ADDR_DEFAULT 出厂烧录的原始MAC地址
*     @arg DMA_BT_ADDR_IN_USED 当前对外体现的MAC地址
*
* @param[out] bt_address    BTMAC地址，6字节数组
*
* @return 获取MAC地址是否成功
*     @retval false 获取BT MAC地址失败
*     @retval true 获取BT MAC地址成功
*
* @note TWS类设备当前对外提现的MAC地址和出厂烧录的原始MAC地址可能不一致 \n
* 当入参为DMA_BT_ADDR_IN_USED，必须获取到手机扫描到的BT MAC地址
*/
//
//
//
bool dueros_dma_wrap_get_bt_address(DMA_BT_ADDR_TYPE addr_type, uint8_t* bt_address)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, addr_type:%d\n", __func__, addr_type);
   bt_get_local_bd_addr(bt_address);
   DMA_WRAPPER_DEBUG_R(bt_address, 6);
   return true;
}

/**
* @brief 获取DMA设备的蓝牙名称
*
* @param[out] bt_local_name    蓝牙名称，数组长度须大于产品蓝牙名称长度
*
* @return 获取蓝牙名称是否成功
*     @retval false 获取蓝牙名称失败
*     @retval true 获取蓝牙名称成功
*
* @note TWS类设备当前对外提现的蓝牙名称和出厂烧录的蓝牙名称可能不一致 \n
* 必须获取到手机扫描到的蓝牙名称
*/
bool dueros_dma_wrap_get_bt_local_name(char* bt_local_name)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   const char *local_name = bt_get_local_name();
   strcpy(bt_local_name, local_name);
   return true;
}

/**
* @brief 获取DMA设备的BLE名称
*
* @param[out] ble_local_name    BLE名称，数组长度须大于产品BLE名称长度
*
* @return 获取BLE名称是否成功
*     @retval false 获取BLE名称失败
*     @retval true 获取BLE名称成功
*
* @note TWS类设备当前对外提现的BLE名称和出厂烧录的BLE名称可能不一致 \n
* 必须获取到手机可连接的BLE名称
*/
bool dueros_dma_wrap_get_ble_local_name(char* ble_local_name)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   uint8_t len = strlen(xcfg_cb.le_name);
   if (len > 0) {
      memcpy(ble_local_name, xcfg_cb.le_name, len);
   }
   return true;
}

/**
* @brief 获取当前连接手机的BT MAC地址
*
* @param[out] bt_address    BTMAC地址，6字节数组
*
* @return 获取手机MAC地址是否成功
*     @retval false 获取BT MAC地址失败
*     @retval true 获取BT MAC地址成功
*/
bool dueros_dma_wrap_get_mobile_bt_address(uint8_t* bt_address)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n",__func__);
   bool ret = bt_nor_get_link_info(bt_address);
   return ret;
}

/**
* @brief 获取BT MAC地址是否存在于已配对列表
*
* @param[in] bt_address    BTMAC地址，6字节数组
*
* @return MAC地址是否已配对过
*     @retval false 获取BT MAC地址不存在于已配对列表
*     @retval true 获取BT MAC地址存在于已配对列表
*/
bool dueros_dma_wrap_get_linkkey_exist_state(const uint8_t* bt_address)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n",__func__);
   uint8_t address[6];
   for (uint8_t i = 0; i < cfg_bt_max_acl_link; i++) {
      if (bt_nor_get_link_info_addr(address, i) == true) {
         if (memcmp(bt_address, address, 6) == 0x00) {
               return true;
         }
      }
   }
   return false;
}

/**
* @brief 获取DMA设备的Serial Number
*
* @param[in] sn_type    SN类型
*     @arg DMA_SN_DEFAULT 出厂烧录的原始SN
*     @arg DMA_SN_IN_USED 当前对外体现的MAC地址对应的SN
*
* @param[out] serial_number    设备SN，数组长度须大于产品SN长度
*
* @return 获取SN是否成功
*     @retval false 获取SN失败
*     @retval true 获取SN成功
*
* @note TWS类设备当前对外提现的MAC地址和出厂烧录的原始MAC地址可能不一致 \n
* 当入参为DMA_BT_ADDR_IN_USED，必须获取到手机扫描到的BT MAC地址
*/
bool dueros_dma_wrap_get_serial_number(DMA_SN_TYPE sn_type, uint8_t* serial_number)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, sn_type:%d\n", __func__, sn_type);
   uint8_t adress_bt[6];
   uint8_t len = 0;

   bt_get_local_bd_addr(adress_bt);

   //SN生成算法
   memcpy(serial_number, "TRUM", 4);
   len += 4;
   for(uint8_t i = 0; i < 6; i++){
      serial_number[len] = number_transforam_table[(adress_bt[i]>>4)&0x0F];
      len += 1;
      serial_number[len] = number_transforam_table[adress_bt[i]&0x0F];
      len += 1;
   }
   serial_number[len] = '\0';
   DMA_WRAPPER_DEBUG("SN:%s\n", serial_number);
   return true;
}

/**
* @brief 获取DMA设备是否在通话状态
*
* @return 当前DMA是否在通话状态
*     @retval false 当前DMA设备不在通话状态
*     @retval true 当前DMA设备在通话状态
*
* @note 是否在通话状态，以建立SCO链路为准 \n
* TWS类设备两侧调用该接口须有相同的返回值
*/
bool dueros_dma_wrap_get_sco_state(void)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n",__func__);
   return sco_is_connected();
}

/**
* @brief 获取DMA设备是否在回连手机
*
* @return 设备是否处于回连状态
*     @retval false 当前DMA设备未在回连手机
*     @retval true 当前DMA设备在回连手机
*
* @note TWS类设备两侧调用该接口须有相同的返回值
*
*/
bool dueros_dma_wrap_get_reconnect_state(void)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n",__func__);
   if (bt_get_status() == BT_STA_CONNECTING) {
      return true;
   } else {
      return false;
   }
}

/*********************************************core dma protocol*********************************************/

/**
* @brief 设置DMA设备MIC及编码状态
*
* @param[in] cmd    设置MIC状态命令字
*     @arg DMA_VOICE_STREAM_STOP    关闭MIC及编码器
*     @arg DMA_VOICE_STREAM_START    开启MIC及编码器
*
* @param[in] codec_type    编码器类型
*     @arg DMA_VOICE_STREAM_NONE    无编码器
*     @arg DMA_VOICE_STREAM_OPUS    opus编码器
*     @arg DMA_VOICE_STREAM_BEAMFORMING    波束编码器
* @return 开关MIC和编码器是否成功
*     @retval false 设置MIC及编码器失败
*     @retval true 设置MIC及编码器成功
*
*/
bool dueros_dma_wrap_set_voice_mic_stream_state(DMA_VOICE_STREAM_CTRL_TYPE cmd, DMA_VOICE_STREAM_CODEC_TYPE codec_type)
{
   bool res = true;
   bool opus_enc_flag = dueros_dma_opus_enc_flag_get();
   uint32_t do_flag = dueros_dma_do_flag_get();
   DMA_WRAPPER_DEBUG("[DMA_APP]%s cmd:%d, codec_type:%d, opus_enc_flag:%d, do_flag:0x%x\n", __func__, cmd, codec_type, opus_enc_flag, do_flag);
   if ((cmd == DMA_VOICE_STREAM_START) && (codec_type == DMA_VOICE_STREAM_OPUS)) {
      if ((opus_enc_flag == 0)  || (do_flag & DUEROS_DMA_FLAG_VOICE_STOP)) {
         dueros_dma_do_flag_set(DUEROS_DMA_FLAG_VOICE_START);
         delay_5ms(20);      //wait do start
      }
   } else if (cmd == DMA_VOICE_STREAM_STOP) {
      if (opus_enc_flag == 1) {
         dueros_dma_do_flag_set(DUEROS_DMA_FLAG_VOICE_STOP);
          delay_5ms(20);      //wait do stop
      } else {
         dueros_dma_do_flag_clr(DUEROS_DMA_FLAG_VOICE_START);
         delay_5ms(20);      //wait do start
      }
   } else {
      res = false;
   }
   return res;
}

/**
* @brief 设置DMA设备是否使能上传音频
*
* @param[in] on  DMA设备上传音频的使能标志
*     @arg true 开启DMA设备上传音频
*     @arg false 关闭DMA设备上传音频
*
* @return 使能上行音频是否成功
*     @retval false DMA设备音频上传设置失败
*     @retval true DMA设备音频上传设置成功
*
* @note 该接口用于控制DMA设备开启或停止上传音频，不依赖是否已经开启MIC和编码器 \n
* TWS类设备Master设备调用该接口表示向手机端上传音频 \n
* Slave设备调用该接口表示向Master设备上传音频
*
*/
bool dueros_dma_wrap_set_stream_upload_enable(bool on)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s on:%d\n", __func__, on);
   dueros_dma_stream_upload_flag_set(on);
   return true;
}

/**
* @brief 获取DMA设备是否在上传音频状态
*
* @return 当前设备是否处于上传音频状态
*     @retval false DMA设备不在上传音频状态
*     @retval true DMA设备在上传音频状态
*
* @note 该接口表示DMA设备已进入上传音频状态，不依赖是否已经实际上传了音频数据 \n
* TWS类设备两侧调用该接口须返回当前设备是否实际进入上传音频状态，其中 \n
* Slave设备的上传音频意为向Master设备上传音频
*
*/
bool dueros_dma_wrap_get_stream_upload_state(void)
{
   return dueros_dma_stream_upload_flag_get();
}

/**
* @brief 获取DMA设备与手机协商的MTU
*
* @param[out] mtu   DMA设备上行音频的MTU
*
* @return 获取手机MTU是否成功
*     @retval false 获取DMA设备与手机协商的MTU失败
*     @retval true 获取DMA设备与手机协商的MTU成功
*
* @note TWS类设备Master设备调用该接口获取和手机之间的MTU \n
* Slave设备不调用该接口
*
*/
bool dueros_dma_wrap_get_mobile_mtu(uint32_t* mtu)
{
   if (dueros_dma_con_type_get() == DUEROS_DMA_CON_SPP) {
      *mtu = get_spp_mtu_size();
   } else if (dueros_dma_con_type_get() == DUEROS_DMA_CON_BLE) {
      *mtu = ble_get_gatt_mtu();
   } else {
      *mtu = 20;  //mtu default 20
      DMA_WRAPPER_DEBUG("!!!dma get mobile mtu err!!!\n");
      return false;
   }
   *mtu = (*mtu > DUEROS_DMA_MTU_MAX_LEN) ? DUEROS_DMA_MTU_MAX_LEN : *mtu;
   // DMA_WRAPPER_DEBUG("LOCAL MTU:%d\n", *mtu);
   return true;
}

/**
* @brief 获取TWS设备之间的MTU
*
* @param[out] mtu   TWS设备间的MTU
*
* @return 获取对端MTU是否成功
*     @retval false 获取TWS设备之间的MTU失败
*     @retval true 获取TWS设备之间的MTU成功
*
* @note TWS类设备两侧调用该接口须有相同的返回值
*
*/
bool dueros_dma_wrap_get_peer_mtu(uint32_t* mtu)
{
   dueros_dma_wrap_get_mobile_mtu(mtu);
   DMA_WRAPPER_DEBUG("PEER MTU:%d\n", *mtu);
   return true;
}

/**
* @brief 设置DMA设备是否使能语音唤醒
*
* @param[in] on  语音唤醒的使能标志
*     @arg true 开启语音唤醒
*     @arg false 关闭语音唤醒
*
* @note TWS类设备仅在Master使能有效，当未开启MIC时使能语音唤醒，应在此接口内开启MIC
*/
bool dueros_dma_wrap_set_wakeup_enable(bool on)
{
   return true;
}


/*********************************************dma process*********************************************/

/**
* @brief 获取DMA设备的签名校验
*
* @param[in] input_data      待签名校验的原始数据
*
* @param[in] len      数据长度
*
* @param[out] output_string      字符串格式的签名校验结果
*
* @return 签名校验是否成功
*     @retval false 签名校验失败
*     @retval true 签名校验成功
*
*/
bool dueros_dma_wrap_get_check_summary(const void* input_data, uint32_t len, uint8_t* output_string)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n",__func__);
   sha256_init(&dueros_dma_wrap_ctx);
   sha256_update(&dueros_dma_wrap_ctx, input_data, len);
   sha256_final(&dueros_dma_wrap_ctx, output_string);
   return true;
}


/*********************************************parse cmd interface*********************************************/

/**
* @brief 获取当前是否处于可发送DMA数据状态
*
* @return 当前是否处于可发送数据状态
*     @retval false 当前未处于可发送状态
*     @retval true 当前未处于可发送状态
*
*/
bool dueros_dma_wrap_get_prepare_state(void)
{
   bool res = dueros_dma_prepare_state_get();
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s, res:%d\n", __func__, res);
   return res;
}

/**
* @brief 处理DMA命令
*
* @param[in] cmd      DMA命令类型
*
* @param[in] param_buf    DMA命令参数
*
* @param[in] param_size      DMA命令参数长度
*
* @return 处理DMA操作是否成功
*     @retval false 处理DMA命令失败
*     @retval true 处理DMA命令成功
*
*/
bool dueros_dma_wrap_process_cmd(DMA_OPERATION_CMD cmd, void* param_buf, uint32_t  param_size)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, cmd:%d\n", __func__, cmd);
   switch (cmd) {
      case DMA_OPERATION_NO_CMD:
         break;

      case DMA_AUDIO_PLAY:
         bt_music_play();
         break;

      case DMA_AUDIO_PAUSE:
         bt_music_pause();
         break;

      case DMA_AUDIO_GET_PLAY_PAUSE:
         *(bool *)param_buf = bt_is_playing();
         break;

      case DMA_AUDIO_PLAY_BACKWARD:
         bt_music_prev();
         break;

      case DMA_AUDIO_PLAY_FORWARD:
         bt_music_next();
         break;

      case DMA_SEND_CMD:
         dueros_dma_send_packet(param_buf, param_size);
         break;

      case DMA_SEND_DATA:
         dueros_dma_send_packet(param_buf, param_size);
         break;

      case DMA_SEND_ATCMD:
         DMA_WRAPPER_DEBUG("AT: %s\n", param_buf);
         dueros_dma_hfp_at_cmd_set(param_buf, param_size);
         break;

      case DMA_SEND_VOICE_COMMAND:
         bt_hfp_siri_switch();
         break;

      default:
         break;
   }
   return true;
}


/*********************************************tws option*********************************************/

/**
* @brief 获取TWS设备的组对状态
*
* @return TWS是否已组对
*     @retval false 获取TWS设备的组对状态失败或TWS设备未组对
*     @retval true TWS设备的组对成功
*
* @note TWS类设备两侧调用该接口须有相同的返回值
*/
bool dueros_dma_wrap_get_peer_connect_state(void)
{
   bool res = bt_tws_is_connected();
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, res:%d\n", __func__, res);
   return res;
}

/**
* @brief 发送自定义消息到TWS设备对端
*
* @param[in] param_buf      自定义消息数据
*
* @param[in] param_size     自定义消息数据长度
*
* @return 发送自定义消息到对端是否成功
*     @retval false 发送消息失败
*     @retval true 发送消息成功
*/
bool dueros_dma_wrap_send_custom_info_to_peer(uint8_t* param_buf, uint16_t param_size)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   dueros_dma_tws_tws_dma_data_sync(param_buf, param_size);
   return true;
}

/**
* @brief 获取TWS设备的角色
*
* @param[out] role_type      TWS设备的角色
*     @arg DMA_TWS_UNKNOWN 未知类型
*     @arg DMA_TWS_MASTER   主耳Master
*     @arg DMA_TWS_SLAVE    从耳Slave
*
* @return 获取TWS设备的角色是否成功
*     @retval false 获取TWS设备的角色失败
*     @retval true 获取TWS设备的角色成功
*
* @note TWS类设备两侧建立连接后调用该接口，须获取到非UNKNOWN的角色类型
*/
bool dueros_dma_wrap_get_tws_role(DMA_TWS_ROLE_TYPE* role_type)
{
   if (bt_tws_is_slave()) {
      *role_type = DMA_TWS_SLAVE;
   } else {
      *role_type = DMA_TWS_MASTER;
   }

   //以下处理主机进仓role switch期间的主从问题
   if (!bt_nor_is_connected() && dueros_dma_role_switch_flag_get()){
      *role_type = DMA_TWS_SLAVE;
   }
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s, role_type:%d, bt_nor_is_connected:%d, role_sw_flag:%d\n", __func__, *role_type, bt_nor_is_connected(), dueros_dma_role_switch_flag_get());
   return true;
}

/**
* @brief 获取TWS设备的左右信息
*
* @param[out] side_type    TWS设备的左右信息
*     @arg DMA_SIDE_INVALID 无效类型
*     @arg DMA_SIDE_LEFT  左侧设备
*     @arg DMA_SIDE_RIGHT    右侧设备
*
* @return 获取TWS设备的左右信息是否成功
*     @retval false 获取TWS设备的左右信息失败
*     @retval true 获取TWS设备的左右信息成功
*
* @note TWS类设备两侧调用该接口，须获取到非INVALID的左右类型
*/
bool dueros_dma_wrap_get_tws_side(DMA_TWS_SIDE_TYPE* side_type)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   uint8_t channel = 0; //1:左 、 0：右
   bool res = bt_tws_get_channel(&channel);
   *side_type = (channel == 1) ? DMA_SIDE_LEFT : DMA_SIDE_RIGHT;
   DMA_WRAPPER_DEBUG("dma_get_tws_side  channel:%d, res:%d\n", channel, res);
   return res;
}

/**
* @brief 设置是否运行进行主从切换
*
* @param[in] on 主从切换的使能标志
*     @arg true 允许主从切换
*     @arg false 不允许主从切换
*
* @return 获取主从切换使能标志是否成功
*     @retval false 使能主从切换失败
*     @retval true 使能主从切换成功
*
* @note 该接口仅对应DMA_NOTIFY_STATE_ROLE_SWITCH_REQUEST时控制是否发起主从切换 \n
* 不影响非DMA连接时的主从切换，不是主从切换的总开关
*/
bool dueros_dma_wrap_set_role_switch_enable(bool on)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   return false;
}

/**
* @brief 获取盒仓的开关状态信息
*
* @param[out] side_type    TWS设备的左右信息
*     @arg DMA_BOX_STATE_UNKNOWN 无效类型
*     @arg DMA_BOX_STATE_OPEN    盒仓开盖
*     @arg DMA_BOX_STATE_CLOSE   盒仓关盖
*
* @return 获取盒仓的开关状态是否成功
*     @retval false 获取盒仓的开关状态信息失败
*     @retval true 获取盒仓的开关状态信息成功W
*/
bool dueros_dma_wrap_get_box_state(DMA_BOX_STATE_TYPE* box_state)
{
   //此处由实际方案提供仓状态
   *box_state = DMA_BOX_STATE_OPEN;
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, box_state:%d\n", __func__, *box_state);
   return true;
}

/**
* @brief 获取当前设备入耳状态信息
*
* @return 获取设备是否已入耳
*     @retval false 设备未入耳
*     @retval true 设备已入耳
*/
bool dueros_dma_wrap_get_wearing_state(void)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   //此处由实际方案提供入耳状态
   return true;
}

/**
* @brief 获取设备电量
*
* @return 设备电量 0~100
*
*/
uint8_t dueros_dma_wrap_get_battery_level(void)
{
   uint8_t level = bsp_get_bat_level();
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, bat_level:%d\n", __func__, level);
   return level;
}

/**
* @brief 获取盒仓电量
*
* @return 盒仓电量 0~100
*
* @note 该接口应返回当前设备最后从盒仓获取到的电量。如不支持获取盒仓电量，请直接返回0
*/
uint8_t dueros_dma_wrap_get_box_battery_level(void)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   //此处由实际方案提供仓电量
   return 100;
}

/**
* @brief 获取三元组数据
*
* @param[out] triad_id    三元组id，不超过32字节数组
* @param[out] triad_secret    三元组密钥，不超过32字节数组
*
* @return 是否获取到三元组
*
*/
bool dueros_dma_wrap_get_triad_info(char* triad_id, char* triad_secret)
{
   strcpy(triad_id, dueros_dma_triad_id_get());
   strcpy(triad_secret, dueros_dma_triad_secret_get());
   return true;
}

/**
* @brief 播放小度自定义提示音
*
* @param[in] tts_id    提示音ID
* @param[in] both_side    是否两边同时播放
*
* @return 是否成功播放小度自定义提示音
*
*/
bool dueros_dma_wrap_play_local_tts(DMA_TTS_ID tts_id, bool both_side)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s  tts_id:%d, both_side:%d\n", __func__, tts_id, both_side);
   switch (tts_id) {
      case DMA_TTS_WAITING_DMA_CONNECTING_0_TIMES_NEVER_CONNECTED:
         break;

      case DMA_TTS_WAITING_DMA_CONNECTING_6_TIMES_NEVER_CONNECTED:
         break;

      case DMA_TTS_WAITING_DMA_CONNECTING_12_TIMES_NEVER_CONNECTED:
         break;

      case DMA_TTS_WAITING_DMA_CONNECTING_18or24or50n_TIMES_NEVER_CONNECTED:
         break;

      case DMA_TTS_WAITING_DMA_CONNECTING_MORETHAN_50_TIMES_AFTER_LASTCONNECT:
         break;

      case DMA_TTS_USE_XIAODU_APP:
         break;

      case DMA_TTS_KEYWORD_DETECTED:
         break;

      case DMA_TTS_WAITING_FOR_DMA_CONNECT:
         break;

      case DMA_TTS_WAITING_FOR_DMA_CONNECT_IGNORE:
         break;

      case DMA_TTS_OPEN_APP_FAIL:
         break;

      default:
         break;
   }

   return true;
}

/**
* @brief 注册小度dma线程处理函数
*
* @note 该接口在库里创建线程时调用
*
*/
AT(.com_text.dma.api)
void dueros_dma_wrap_process_register(void)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   os_dueros_dma_thread_process_register(dma_process);
}

/**
* @brief 向DMA协议栈注册接口
*/
void dueros_dma_wrap_operation_register(void)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   dma_register_operation(&duer_dma_oper);
}

/**
* @brief 初始化DMA协议栈
*
* @note 以下参数除vendor id外，请联系小度获取
*/
void dueros_dma_wrap_protocol_stack_init(void)
{
   uint8_t dev_type = dueros_dma_device_type_get();
   uint8_t dev_version = dueros_dma_device_version_get();
   DMA_ERROR_CODE res = dma_protocol_init((const uint8_t *)DUEROS_DMA_CLIENT_ID, (const uint8_t *)DUEROS_DMA_CLIENT_SECRET, (const uint8_t *)DUEROS_DMA_VERDOR_ID, dev_type, dev_version, &dueros_dma_wrap_queue_memory_pool[0]);
   DMA_WRAPPER_DEBUG("[DMA_APP]%s  res:%d\n", __func__, res);
}

/**
* @brief 向DMA协议栈发送从手机端经DMA链路发送来的数据
*
* @param[in] input_data  收集发来的数据
* @param[in] date_len  数据长度
*
* @return 是否成功将数据发送到DMA协议栈
*
*/
DMA_ERROR_CODE dueros_dma_wrap_recv_mobile_data(const char* input_data, uint32_t date_len)
{
   // DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   DMA_ERROR_CODE ret = dma_recv_mobile_data(input_data, date_len);
   return ret;
}

/**
* @brief 向DMA协议栈发送从对耳接受到的数据
*
* @param[in] input_data  对耳发来的数据
* @param[in] date_len  数据长度
*
* @return 是否成功将数据发送到DMA协议栈
*
*/
DMA_ERROR_CODE dueros_dma_wrap_recv_peer_data(const char* input_data, uint32_t date_len)
{
   DMA_ERROR_CODE ret = dma_recv_peer_data(input_data, date_len);
   DMA_WRAPPER_DEBUG("[DMA_APP]%s, ret:%d\n", __func__, ret);
   return ret;
}

/**
* @brief 向DMA协议栈发送经过压缩编码的音频数据
*
* @param[in] input_data  经过压缩编码的音频数据
* @param[in] date_len  数据长度
*
* @return 是否成功将数据发送到DMA协议栈
*
*/
DMA_ERROR_CODE dueros_dma_wrap_feed_compressed_data(const char* input_data, uint32_t date_len)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   DMA_ERROR_CODE ret = dma_feed_compressed_data(input_data, date_len);
   return ret;
}

/**
* @brief 向DMA协议栈发送当前系统状态
*
* @param[in] state  系统状态
* @param[in] param_buf    DMA命令参数
* @param[in] param_size      DMA命令参数长度
*
* @return 是否成功通知DMA协议栈当前状态
*
*/
DMA_ERROR_CODE dueros_dma_wrap_notify_state(DMA_NOTIFY_STATE state, void* param_buf, uint32_t  param_size)
{
   DMA_WRAPPER_DEBUG("[DMA_APP]%s\n", __func__);
   DMA_ERROR_CODE ret = dma_notify_state(state, param_buf, param_size);
   return ret;
}

DUER_DMA_OPER duer_dma_oper = {
   .get_device_capability       =    dueros_dma_wrap_get_device_capability,
   .get_firmeware_version       =    dueros_dma_wrap_get_firmeware_version,
   .dma_heap_malloc             =    dueros_dma_wrap_heap_malloc,
   .dma_heap_free               =    dueros_dma_wrap_heap_free,
   .dma_sem_wait                =    dueros_dma_wrap_sem_wait,
   .dma_sem_signal              =    dueros_dma_wrap_sem_signal,
   .dma_mutex_lock              =    dueros_dma_wrap_mutex_lock,
   .dma_mutex_unlock            =    dueros_dma_wrap_mutex_unlock,
   .dma_get_userdata_config     =    dueros_dma_wrap_get_userdata_config,
   .dma_set_userdata_config     =    dueros_dma_wrap_set_userdata_config,
   .dma_rand                    =    dueros_dma_wrap_get_rand,
   .get_ota_state               =    dueros_dma_wrap_get_ota_state,
   .get_upgrade_state           =    dueros_dma_wrap_get_upgrade_state,
   .get_mobile_connect_type     =    dueros_dma_wrap_get_mobile_connect_type,
   .set_ble_advertise_data      =    dueros_dma_wrap_set_ble_advertise_data,
   .set_ble_advertise_enable    =    dueros_dma_wrap_set_ble_advertise_enable,
   .get_bt_address              =    dueros_dma_wrap_get_bt_address,
   .get_bt_local_name           =    dueros_dma_wrap_get_bt_local_name,
   .get_ble_local_name          =    dueros_dma_wrap_get_ble_local_name,
   .get_mobile_bt_address       =    dueros_dma_wrap_get_mobile_bt_address,
   .get_linkkey_exist_state     =    dueros_dma_wrap_get_linkkey_exist_state,
   .get_serial_number           =    dueros_dma_wrap_get_serial_number,
   .get_sco_state               =    dueros_dma_wrap_get_sco_state,
   .get_reconnect_state         =    dueros_dma_wrap_get_reconnect_state,
   .set_voice_mic_stream_state  =    dueros_dma_wrap_set_voice_mic_stream_state,
   .set_stream_upload_enable    =    dueros_dma_wrap_set_stream_upload_enable,
   .get_stream_upload_state     =    dueros_dma_wrap_get_stream_upload_state,
   .get_mobile_mtu              =    dueros_dma_wrap_get_mobile_mtu,
   .get_peer_mtu                =    dueros_dma_wrap_get_peer_mtu,
   .set_wakeup_enable           =    dueros_dma_wrap_set_wakeup_enable,
   .get_check_summary           =    dueros_dma_wrap_get_check_summary,
   .get_prepare_state           =    dueros_dma_wrap_get_prepare_state,
   .dma_process_cmd             =    dueros_dma_wrap_process_cmd,
   .get_peer_connect_state      =    dueros_dma_wrap_get_peer_connect_state,
   .send_custom_info_to_peer    =    dueros_dma_wrap_send_custom_info_to_peer,
   .get_tws_role                =    dueros_dma_wrap_get_tws_role,
   .get_tws_side                =    dueros_dma_wrap_get_tws_side,
   .set_role_switch_enable      =    dueros_dma_wrap_set_role_switch_enable,
   .get_box_state               =    dueros_dma_wrap_get_box_state,
   .get_wearing_state           =    dueros_dma_wrap_get_wearing_state,
   .get_battery_level           =    dueros_dma_wrap_get_battery_level,
   .get_box_battery_level       =    dueros_dma_wrap_get_box_battery_level,
   .get_triad_info              =    dueros_dma_wrap_get_triad_info,
   .play_local_tts              =    dueros_dma_wrap_play_local_tts,
};

#endif































