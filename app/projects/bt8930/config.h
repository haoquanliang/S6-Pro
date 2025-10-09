/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include "config_define.h"

#define DEBUG_SW(...)              printf(__VA_ARGS__)

#define TWS_LR              1
#define TWS_SYNC_BUF_SIZE               (35)
#define SWETZ_SPP_CMD       1       //SPP指令
#define SWETZ_UART_CMD      1       //串口指令
#define SWETZ_SYNC_EVT      0        //事件左右同步执行
#define APP_MESSAGE_RUN     1       


#define STR_PRI                     1//中断打印
#define  SWETZ_TEST                       0

#define SWETZ                       1
#if SWETZ
#define SWETZ_EVT_5S                1
#define SWETZ_ROLE_SWITCH_BY_BAT    0//根据电量切换主从
#define SWETZ_ROLE_SWITCH_BY_INBOX  1//出入仓判断是否需要切换主从

#define SWETZ_DEL_TWS_CON_TONE      1//去掉tws左右通道的提示音
#define SWETZ_SET_SCAN_STATE        1//设置蓝牙状态
#define SWETZ_TONE                  1 
#define NUM_BT_RETURN_CON              10// 上电回连手机次数, 1.28s * n, 不小于5次
#define NUM_TWS_RETURN_CON             5//设置TWS搜索次数, 1.25s * n, 不小于2次
#define SWETZ_CHECK_INBOX           1//出入仓检测
#define SWETZ_L_THREE_P             1//修改三击功能
#define SWETZ_KEY_TONE              1//增加按键声key tone
#define SWETZ_VBAT_TO_PHONE         1//电量表，手机端电量显示

#define SWETZ_SET_VBAT_LOW_WARNING  1//低电报警电量设置
#define VBAT_LOW_WARNING_VAL                (10)
#define BT_PAIR_SLEEP_EN            (1)//不连接手机也可以进入sleep

#define SWETZ_LOW_WARNING_TIME      1//低电提醒
#define LOW_WARNING_TIME            (600)

#define SWETZ_APP                   1//APP功能
#define SWETZ_KEY_RECON             1//单击有配对记录要耳机回连
#define SWETZ_RING_DOUBLE           1 //修改ring下双击功能
#define SWETZ_CALL_DOUBLE           1 //修改call下双击功能
#define SWETZ_RING_TONE             1
#define SWETZ_DEL_RING_SHORT_FUC    1//去掉ring下单击功能
#define SWETZ_DEL_CALL_SHORT_FUC    1//去掉call下单击功能
#define SWETZ_MULT_DEL_TONE         1//双连B手机连接和断开不播提示音
#define SWETZ_CLSOE_INBOX_WALEUP    1//关掉inbox唤醒
#define SWETZ_BT_TO_PAIR            1//重新进配对逻辑
#define SWETZ_INCASE_MUTE           1//入仓mute
#define SWETZ_EVT_1S                1//1s事件
#define SWETZ_LOWVBAT_OUT_SYNC_OFF  1//sys_cb.discon_reason 0不同步 1同步//低电不同步关机
#define SWETZ_OUTCASE_AFTER_NOT_KEY 1//出仓5s不响应按键消息//flag_outcase_5s_kye_null
#define SWETZ_FACTORY_RESET         1//恢复出厂
#define SWETZ_DYN_EQ_DEBUG          0//动态EQ调试
#define SWETZ_NTC                   1//ntc


#endif



/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_MUSIC_EN                   0   //是否打开MUSIC功能
#define FUNC_CLOCK_EN                   0   //是否打开时钟功能
#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_BTHID_EN                   0   //是否打开独立自拍器模式
#define FUNC_BT_DUT_EN                  1   //是否打开蓝牙的独立DUT测试模式
#define FUNC_BT_FCC_EN                  0   //是否打开蓝牙的独立FCC测试模式（IODM已独立模式，不需要打开FCC模式）
#define FUNC_AUX_EN                     0   //是否打开AUX功能
#define FUNC_USBDEV_EN                  0   //是否打开USB DEVICE功能
#define FUNC_SPEAKER_EN                 0   //是否打开Speaker模式
#define FUNC_IDLE_EN                    0   //是否打开IDLE功能


/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define SYS_CLK_SEL                     SYS_24M                 //选择系统时钟
#define BUCK_MODE_EN                    xcfg_cb.buck_mode_en    //是否BUCK MODE
#define POWKEY_10S_RESET                xcfg_cb.powkey_10s_reset
#define SOFT_POWER_ON_OFF               1                       //是否使用软开关机功能
#define PWRKEY_2_HW_PWRON               0                       //用PWRKEY模拟硬开关
#define USB_SD_UPDATE_EN                0                       //是否支持UDISK/SD的离线升级
#define GUI_SELECT                      GUI_NO                  //GUI Display Select
#define UART0_PRINTF_SEL                PRINTF_PB3              //选择UART打印信息输出IO，或关闭打印信息输出
#define SYS_PARAM_RTCRAM                0                       //是否系统参数保存到RTCRAM
#define PWRON_ENTER_BTMODE_EN           0                       //是否上电默认进蓝牙模式
#define SLEEP_DAC_OFF_EN                (is_sleep_dac_off_enable()) //sfunc_sleep是否关闭DAC， 复用MICL检测方案不能关DAC。
#define SYS_INIT_VOLUME                 xcfg_cb.sys_init_vol        //系统默认音量
#define SYS_LIMIT_VOLUME                xcfg_cb.sys_recover_initvol //开机最小音量，避免开机时音量太小误认为没开机
#define SYS_VDDIO_LP_EN                 0                       //休眠模式是否打开切换VDDIO功能（省电，可能会影响VDDIO供电的外设）

#define OFFLINE_LOG_EN                  0                       //是否打开离线日志（打开之前要评估flash空间是否足够）
#define OFFLINE_LOG_SIZE                0x30000                 //离线日志存储区域大小
#define OFFLINE_LOG_START               (FLASH_SIZE - CM_SIZE - OFFLINE_LOG_SIZE) //离线日志存储区域起始位置（注意避让FOTA、自定义KEEP等其他功能）

/*****************************************************************************
 * Module    : SPIFLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      FSIZE_2M                //LQFP48芯片内置1MB，其它封装芯片内置512KB(实际导出prd文件要小于492K)
#define FLASH_CODE_SIZE                 768K                    //程序使用空间大小，code_size <= flash_size - [params(20k) + res2(4k) + res.bin]
#define FLASH_RESERVE_SIZE              0x5000                  //程序保留区空间大小，打开ANC_EQ_RES2_EN需要保留24K
#define FLASH_ERASE_4K                  1                       //是否支持4K擦除
#define FLASH_DUAL_READ                 1                       //是否支持2线模式
#define FLASH_QUAD_READ                 0                       //是否支持4线模式
#define FLASH_SPEED_UP_EN               1                       //SPI FLASH提速。


/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#define MUSIC_UDISK_EN                  0   //是否支持播放UDISK
#define MUSIC_SDCARD_EN                 0   //是否支持播放SDCARD

#define MUSIC_WAV_SUPPORT               1   //是否支持WAV格式解码
#define MUSIC_WMA_SUPPORT               0   //是否支持WMA格式解码
#define MUSIC_APE_SUPPORT               0   //是否支持APE格式解码
#define MUSIC_FLAC_SUPPORT              0   //是否支持FLAC格式解码
#define MUSIC_M4A_SUPPORT               0   //是否支持M4A格式解码
#define MUSIC_SBC_SUPPORT               0   //是否支持SBC格式解码(SD/UDISK的SBC歌曲, 此宏不影响蓝牙音乐)
#define MUSIC_AAC_SUPPORT               0   //仅用于AAC解码测试(SD/UDISK的AAC歌曲, 此宏不影响蓝牙音乐)

#define MUSIC_FOLDER_SELECT_EN          0   //文件夹选择功能
#define MUSIC_AUTO_SWITCH_DEVICE        0   //双设备循环播放
#define MUSIC_BREAKPOINT_EN             0   //音乐断点记忆播放
#define MUSIC_QSKIP_EN                  0   //快进快退功能
#define MUSIC_PLAYMODE_NUM              4   //音乐播放模式总数
#define MUSIC_MODE_RETURN               0   //退出音乐模式之后是否返回原来的模式
#define MUSIC_PLAYDEV_BOX_EN            0   //是否显示“USB”, "SD"界面
#define MUSIC_ID3_TAG_EN                0   //是否获取MP3 ID3信息
#define MUSIC_REC_FILE_FILTER           0   //是否区分录音文件与非录音文件分别播放
#define MUSIC_LRC_EN                    0   //是否支持歌词显示
#define MUSIC_NAVIGATION_EN             0   //音乐文件导航功能(LCD点阵屏功能)
#define MUSIC_ENCRYPT_EN                0   //是否支持加密MP3文件播放(使用MusicEncrypt.exe工具进行MP3加密)

#define MUSIC_ENCRYPT_KEY               12345   //MusicEncrypt.exe工具上填的加密KEY

#define IPHONE_POWER_VAL                50  //苹果充电电流设置
#define IPHONE_POWER_INDEX              190 //苹果充电电流设置


/*****************************************************************************
 * Module    : 蓝牙功能配置
 *****************************************************************************/
#define BT_BQB_RF_EN                    0   //BR/EDR DUT测试模式，为方便测试不自动回连（仅用于BQB RF测试）
#define BT_FCC_TEST_EN                  0   //蓝牙FCC测试使能，默认PB3 波特率1500000通信（仅用于FCC RF测试）
#define BT_BACKSTAGE_EN                 0   //蓝牙后台管理（全模式使用蓝牙，暂不支持BLE后台）
#define BT_BACKSTAGE_PLAY_DETECT_EN     0   //非蓝牙模式下检测到手机蓝牙播放音乐，则切换到蓝牙模式
#define BT_NAME_DEFAULT                 "BT-BOX"     //默认蓝牙名称（不超过31个字符）
#define BT_NAME_WITH_ADDR_EN            0   //蓝牙名称是否附加地址信息（调试用，例如：btbox-***）
#define BT_LINK_INFO_PAGE1_EN           0   //是否使用PAGE1回连信息（打开后可以最多保存8个回连信息）
#define BT_POWER_UP_RECONNECT_TIMES     3   //上电回连次数
#define BT_TIME_OUT_RECONNECT_TIMES     20  //掉线回连次数，5.12s一次
#define BT_SIMPLE_PAIR_EN               1   //是否打开蓝牙简易配对功能（关闭时需要手机端输入PIN码）
#define BT_DISCOVER_CTRL_EN             2   //是否使用按键打开可被发现（需自行添加配对键处理才能被连接配对）
#define BT_PWRKEY_5S_DISCOVER_EN        0   //是否使用长按5S开机进入可被发现(耳机长按开机功能)
#define BT_DISCOVER_TIMEOUT             100 //按键打开可被发现后，多久后仍无连接自动关闭，0不自动关闭，单位100ms
#define BT_ANTI_LOST_EN                 0   //是否打开蓝牙防丢报警
#define BT_DUT_MODE_EN                  0   //正常连接模式，是否使能DUT测试
#define BT_LOCAL_ADDR                   0   //蓝牙是否使用本地地址，0使用配置工具地址
#define BT_LOW_LATENCY_EN               1   //是否打开蓝牙低延时切换功能
#define BT_RF_POWER_BALANCE_EN          0   //优化回连干扰导致的噪声，注意IO是否有冲突

#define BT_2ACL_EN                      1   //是否支持连接两部手机
#define BT_2ACL_AUTO_SWITCH             0   //连接两部手机时是否支持点击播放切换到对应的手机
#define BT_A2DP_EN                      1   //是否打开蓝牙音乐服务
#define BT_HFP_EN                       1   //是否打开蓝牙通话服务
#define BT_HFP_TIME_EN                  0   //是否支持HFP获取时间指令
#define BT_HSP_EN                       0   //是否打开蓝牙HSP通话服务
#define BT_SPP_EN                       1   //是否打开蓝牙串口服务
#define BT_ID3_TAG_EN                   0   //是否打开蓝牙ID3功能
#define BT_HID_EN                       0   //是否打开蓝牙HID服务
#define BT_ATT_EN                       0   //是否打开GATT_OVER_BREDR
#define BT_HID_DOUYIN_EN                0   //是否打开刷抖音功能(对应BT_HID_TYPE = 3,配置文件要开启拍照功能)
#define BT_HID_TYPE                     0   //选择HID服务类型: 0=自拍器(VOL+, 部分Android不能拍照), 1=自拍器(VOL+和ENTER, 影响IOS键盘使用), 2=游戏手柄, 3=抖音功能
#define BT_HID_MANU_EN                  0   //蓝牙HID是否需要手动连接/断开
#define BT_HID_DISCON_DEFAULT_EN        0   //蓝牙HID服务默认不连接，需要手动进行连接。
#define BT_HID_VOL_CTRL_EN              0   //是否支持HID调手机音量功能（需同时打开BT_HID_EN和BT_A2DP_VOL_CTRL_EN）
#define BT_HFP_CALL_PRIVATE_SWITCH_EN   1   //是否打开按键切换私密接听与蓝牙接听功能
#define BT_HFP_CALL_PRIVATE_FORCE_EN    0   //是否强制使用私密接听（仅在手机接听，不通过蓝牙外放）
#define BT_HFP_RECORD_DEVICE_VOL_EN     0   //是否支持分别记录不同连接设备的通话音量
#define BT_HFP_RING_NUMBER_EN           0   //是否支持来电报号
#define BT_HFP_INBAND_RING_EN           1   //是否支持手机来电铃声（部分android不支持，默认用本地RING提示音）
#define BT_HFP_BAT_REPORT_EN            1   //是否支持电量显示
#define BT_HFP_MSBC_EN                  1   //是否打开宽带语音功能
#define BT_A2DP_AAC_AUDIO_EN            1   //是否支持蓝牙AAC音频格式
#define BT_A2DP_LHDC_AUDIO_EN           0   //是否支持蓝牙LHDC音频格式（请与Savitech购买Keypro授权，否则播放一小段时间后无声）
#define BT_A2DP_LDAC_AUDIO_EN           0   //是否支持蓝牙LDAC音频格式（需要加密狗授权，否则播放一小段时间后无声）
#define BT_A2DP_VOL_CTRL_EN             1   //是否支持音量与手机同步，（默认使用AVRCP协议，打开BT_HID_VOL_CTRL_EN后使用HID协议）
#define BT_A2DP_RECORD_DEVICE_VOL_EN    1   //是否支持分别记录不同连接设备的音量，使用设备时恢复当前设备音量
#define BT_A2DP_VOL_REST_EN             0   //是否支持连接不支持同步音量手机时复位音量

#define BT_A2DP_AVRCP_PLAY_STATUS_EN    0   //是否支持手机播放状态同步，可加快播放暂停响应速度
#define BT_A2DP_RECON_EN                0   //是否支持A2DP控制键（播放/暂停、上下曲键）回连
#define BT_A2DP_SUPTO_RESTORE_PLAY_EN   1   //是否支持蓝牙超距回连恢复播放
#define BT_A2DP_EXCEPT_RESTORE_PLAY_EN  0   //是否支持异常复位后回连恢复播放
#define BT_AVDTP_DYN_LATENCY_EN         1   //是否支持根据信号环境动态调整延迟
#define BT_SCO_DBG_EN                   1   //是否打开无线调试通话参数功能
#define BT_PBAP_EN                      0   //是否打开电话簿功能
#define BT_MAP_EN                       0   //是否打开蓝牙短信服务(用于获取设备时间，支持IOS/Android)
#define BT_SNATCH_EN                    0   //是否支持抢连

#define BT_TWS_EN                       1   //是否支持TWS
#define BT_TWS_SCO_EN                   1   //是否支持TWS双路通话
#define BT_TWS_PAIR_MODE                0   //0=通过蓝牙名字配对，1=通过ID配对
#define BT_TWS_PAIR_ID                  0x38393530
#define BT_TWS_PAIR_BONDING_EN          1   //是否支持TWS组队绑定，调用bt_tws_delete_link_info()删除配对信息可解除绑定
#define BT_TWS_MS_SWITCH_EN             1   //是否支TWS主从切换
#define BLE_TWS_MS_SWITCH_EN            1   //是否支BLE主从切换
#define BT_TWS_DBG_EN                   0   //是否支持BT-Assistant工具分析信号质量，需要打开BT_SPP_EN
#define BT_TWS_PUBLIC_ADDR_EN           0   //是否支持TWS配对后生成新地址连接手机（0：使用主耳地址，1：使用新地址，注意0、1程序不兼容不能相互配对）
#define BT_TWS_SLEEP_LED_SYNC_EN        0   //是否打开休眠模式下led灯同步闪烁功能

/*****************************************************************************
 * Module    : BLE功能配置
 *****************************************************************************/
//BLE功能配置
#define LE_BQB_RF_EN                    0   //BLE DUT测试模式，使用串口通信（仅用于BQB RFPHY测试）
#define LE_PAIR_EN                      0   //是否使能BLE的加密配对
#define LE_SM_SC_EN                     0   //是否使能BLE的加密连接，需同时打开 LE_PAIR_EN
#define LE_SM_PAIR_METHOD               NUMERIC_COMPARISON   //使用数字比较配对时, 需要同时开打 LE_PAIR_EN 和 LE_SM_SC_EN
#define LE_CTKD_EN                      0   //是否使能一键双联功能，需同时打开 LE_PAIR_EN、LE_SM_SC_EN
#define LE_ADV_POWERON_EN               1   //上电是否默认打开BLE广播
#define LE_PRIV_EN                      0   //是否开启独立BLE私有链路（可搭配连接充电仓,调试中）

//gatt 配置
#define LE_ATT_NUM                      80  //最大支持多少条gatt属性, att_handle 1 ~ LE_ATT_NUM

#define LE_ADV0_EN                      0   //是否打开通道0广播功能
#define LE_ADV0_CON_EN                  0   //是否打开通道0广播是否可连接
#define LE_WIN10_POPUP                  0   //（调试中）是否打开win10 swift pair快速配对

//google快速配对，需要用到BLE和SPP，注意setting里BLE和SPP开关也需要打开
//支持与AB_Mate_APP同时打开，暂不支持和其他APP一起打开
#define GFPS_EN                         0   //是否打开谷歌快速配对功能

//APP功能相关（APP只能选1个）
#define APP_DEMO_EN                     0   //是否打开demo APP功能，包含ble广播，ble、spp、gatt over br/edr收发示例代码
#define AB_MATE_APP_EN                  1   //是否打开AB-Mate APP控制功能，AB-Mate FOTA通过AB_MATE_OTA_EN打开，TWS只支持连上后一起升级，使用.fot文件格式
#define LE_TUYA_EN                      0   //是否打开涂鸦功能
#define LE_DUEROS_DMA_EN                0   //是否打开DUEROS DMA APP功能，请查阅dueros_dma_app.h第一行的readme(不支持与声加降噪算法同时打开)

#define FOT_EN                          1           //FOTA总开关
//独立FOTA功能配置
//独立FOTA主要用于第三方APP支持.fot文件格式，方便客户APP支持FOTA功能
//独立FOTA需要ble广播才能连接ble进行ota，SDK中配合打开APP_DEMO_EN或者其他有ble广播的APP
//独立FOTA，可通过wiki上的ab-ota-demo APP进行演示（可申请源码）
//AB-Mate APP自身支持FOTA，所以打开AB_MATE_APP_EN后会自动关闭独立FOTA
#define LE_AB_FOT_EN                    1           //是否打开BLE协议的独立FOTA服务，一般IOS使用（需要配合BLE APP使用）
#define BT_AB_FOT_EN                    1           //是否支持SPP协议的独立FOTA服务，一般Android使用（需要配合SPP APP使用）

//FOTA注意事项：
//1)FOTA程序大小需要限制在 (FLASH_SIZE/2) - 24K 以内，若1M flash不够，需要修改上面的FLASH_SIZE的宏改成2M
//2)Downloader的setting里也有设置BLE和SPP，需要留意打开
#define AB_FOT_TYPE                     AB_FOT_TYPE_PACK    //FOTA升级方式选择
#define RES_USERBIN_EN                  0                   //开启后会把res_user目录下的资源打包到userbin，OTA不升级这部分资源(更新这部分资源需要重新烧录)
#define FOT_SUPPORT_TWS                 BT_TWS_EN           //是否支持TWS同步进行FOTA升级功能（打开后只能左右耳一起升级）
#define FOT_GUID_CHECK_EN               0                   //FOTA GUID校验使能（通过app.xm中的setid设置guid，guid校验不过禁止升级）
#define FOT_VER_CHECK_EN                0                   //FOTA 版本号校验使能（通过app.xm中的setver设置版本号，用于限制降级）

//flash 空间分配，根据实际情况调整，默认sdk需要把FLASH_SIZE设置到FSIZE_2M以上才够空间升级
/*
 * 压缩OTA Flash空间分配表，Userbin和Res2为可选区，不使用不占用空间
 *
 * +---------------------+------------------------+---------------------------+------------------------------+
 * | Region Name         | Start Addr             | Size                      | Description                  |
 * +---------------------+------------------------+---------------------------+------------------------------+
 * | Loader1             | 0x00000000             | 4K                        | Loader firmware              |
 * | Loader2             | 0x00001000             | 4K                        | Loader firmware              |
 * | Application         | 0x00002000             | AB_FOT_PACK_START_ADDR-8K | Main application firmware    |
 * | OTA Download        | AB_FOT_PACK_START_ADDR | AB_FOT_PACK_SIZE          | OTA update download area     |
 * | User bin            | USERBIN_START_ADDR     | USERBIN_SIZE              | User bin area(tones)         |
 * | Res2                | FLASH_SIZE-24k         | 4K                        | define by ANC_EQ_RES2_EN     |
 * | User params Data    | FLASH_SIZE-20k         | 20K                       | User params Data             |
 * +---------------------+------------------------+---------------------------+------------------------------+
 */
#if AB_FOT_TYPE == AB_FOT_TYPE_PACK
#if (FLASH_SIZE == FSIZE_1M)
#if RES_USERBIN_EN
#define AB_FOT_PACK_START_ADDR          0xAF000             //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0x32000

#define USERBIN_START_ADDR              0xE1000             //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE)
#define USERBIN_SIZE                    0x19000             //100k
#else
#define AB_FOT_PACK_START_ADDR          0xAF000             //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0x4B000
#endif //RES_USERBIN_EN
#elif (FLASH_SIZE == FSIZE_2M)
#if RES_USERBIN_EN
#define AB_FOT_PACK_START_ADDR          0x100000            //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0xE1000

#define USERBIN_START_ADDR              0x1E1000            //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - USERBIN_SIZE)
#define USERBIN_SIZE                    0x19000             //100k
#else
#define AB_FOT_PACK_START_ADDR          0x12C000            //(FLASH_SIZE - 24K(参数区20k+RES2 4k) - AB_FOT_PACK_SIZE)
#define AB_FOT_PACK_SIZE                0xC3000
#endif // RES_USERBIN_EN
#endif // (FLASH_SIZE == FSIZE_2M) or (FLASH_SIZE == FSIZE_1M)
#endif // AB_FOT_TYPE == AB_FOT_TYPE_PACK


/*****************************************************************************
 * Module    : 蓝牙音乐算法配置
 *****************************************************************************/
#define BT_MUSIC_EFFECT_EN              0                   //蓝牙音乐音效算法处理使能
#define BT_MUSIC_EFFECT_DBG_EN          0                   //蓝牙音乐音效在线调试使能
#define BT_MUSIC_EFFECT_ABT_EN          1                   //是否使用abt文件获取音效参数，abt文件由在线调试工具生成，需要自行替换abt文件或在setting中选择对应算法的资源文件
#define BT_MUSIC_PAUSE_CLK_BACK_EN      0                   //蓝牙音乐暂停播放是否把音乐音效的主频调回去
//动态低音
#define BT_MUSIC_EFFECT_DBB_EN          0                   //动态低音音效使能
#define BT_MUSIC_EFFECT_DBB_BAND_CNT    1                   //动态低音音效的EQ段数
#define BT_MUSIC_EFFECT_DBB_DEF_LEVEL   10                  //动态低音音效默认的等级
//空间音效
#define BT_MUSIC_EFFECT_SPATIAL_AU_EN   0                   //空间音效使能
#define BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN 1                   //空间音效下固定使用spatial_audio.eq
#define BT_MUSIC_EFFECT_SPATIAL_AU_L1   0
#define BT_MUSIC_EFFECT_SPATIAL_AU_DLEN 0                   //空间音效左右声道delay使能
//虚拟低音
#define BT_MUSIC_EFFECT_VBASS_EN        0                   //虚拟低音使能
//动态EQ
#define BT_MUSIC_EFFECT_DYEQ_EN         0                   //动态EQ使能
#define BT_MUSIC_EFFECT_DYEQ_VBASS_EN   0                   //动态EQ和VBASS配合使用，不需要打开 BT_MUSIC_EFFECT_VBASS_EN
//2段DRC
#define BT_MUSIC_EFFECT_XDRC_EN         0                   //2段DRC使能
#define BT_MUSIC_EFFECT_XDRC_EQ_EN      0                   //2段DRC软件EQ使能
#define BT_MUSIC_EFFECT_XDRC_EQ_BAND    4                   //2段DRC软件EQ段数
#define BT_MUSIC_EFFECT_XDRC_DELAY_EN   0                   //2段DRC lookahead使能
#define BT_MUSIC_EFFECT_XDRC_DELAY_CNT  128                 //2段DRC lookahead样点数
//舒适噪声（alpha波、beta波、pink）
#define ABP_EN                          0                   //舒适噪声（alpha波、beta波、pink）使能
#define ABP_MUSIC_DIS_PINK_EN           0                   //播放蓝牙音乐时关闭pink
#define ABP_PLAY_WAKE_EN                0                   //ABP播放时是否保持唤醒不进入休眠。如要进入休眠，则在休眠时会关掉ABP
#define ABP_PLAY_DIS_WAV_EN             0                   //ABP播放时是否不播放wav/piano/tone提示音。如要播放提示音，则会打断ABP播放
//用户自定义
#define BT_MUSIC_EFFECT_USER_EN         0                   //用户自定义音乐音效算法

/*****************************************************************************
 * Module    : 蓝牙通话算法配置
 *****************************************************************************/
//通话丢包修复
#define BT_PLC_EN                       1
#define BT_CALL_MAX_GAIN                xcfg_cb.bt_call_max_gain    //配置通话时DAC最大模拟增益

//通话回声消除算法
#define BT_ECHO_LEVEL                   xcfg_cb.bt_echo_level       //回声消除级别（级别越高，回声衰减越明显，但通话效果越差）(0~15)
#define BT_AEC_DUMP_EN                  0                           //是否打开AEC数据dump功能，dump:算法前 + 下行ref信号

#define BT_AEC_EN                       1       //是否打开硬件AEC算法
#define BT_AEC_FF_MIC_REF_EN            0       //如果aec的ff_mic回声比talk_mic回声大，可使能这功能，用于双mic降噪

//通话MIC端功能配置（上行）
#define ENC_DBG_EN                      1       //是否打开通话产测指令（兼容单、双MIC）

#define BT_SCO_DUMP_EN                  0       //是否打开上行降噪算法数据dump功能（双MIC优先用），dump:算法前主麦 + 算法前副麦 + 算法后
#define BT_EQ_DUMP_EN                   0       //是否打开上行EQ的数据dump功能（单MIC优先用），dump:算法前主麦 + 算法后 + EQ后
#define BT_DUMP_6M_EN                   0       //是否使用6M波特率的dump

#define BT_MIC_DRC_EN                   0       //DRC参数调试在 bt_mic_8k.drc //(msbc)bt_mic_16k.drc

#define BT_TRUMPET_NR_EN                0       //是否打开近端汽车喇叭声降噪（一般不建议打开，对人声高频有影响）
#define BT_TRUMPET_NR_LEVEL             (3)     //近端汽车喇叭声抑制量，范围(0~15, 默认3)

#define BT_SCO_AGC_EN                   0       //是否打开AGC算法

//通话MIC端降噪算法（上行，只能选其一）
#define BT_SCO_NR_EN					0	                        //是否打开自研单麦硬件降噪
#define BT_SCO_NR_LEVEL				    2	                        //降噪量：0~30级（默认0级）

#define BT_SCO_SMIC_AI_EN               0                           //是否打开自研单麦AI降噪算法
#define BT_SCO_SMIC_AI_LEVEL		    xcfg_cb.bt_dnn_level        //降噪量：0~40级（建议范围，默认0级）

#define BT_SCO_SMIC_AI_PLUS_EN          0                           //是否打开自研单麦AI PLUS降噪算法

#define BT_SCO_SMIC_AI_PRO_EN           1                           //是否打开自研单MIC大模型AI降噪算法
#define BT_SCO_SMIC_AI_PRO_LEVEL		xcfg_cb.bt_dnn_level        //降噪量：0~40级（建议范围，默认0级）

#define BT_SCO_AIAEC_DNN_EN             0                           //是否打开自研单麦 + AIAEC降噪算法
#define BT_SCO_AIAEC_DNN_LEVEL          xcfg_cb.bt_dnn_level        //降噪量：0~40级（建议范围，默认0级）

#define BT_SCO_DMIC_AIAEC_EN            0                           //是否打开自研双麦 + AIAEC降噪算法
#define BT_SCO_DMIC_AIAEC_NLMS_EN       1                           //是否打开NLMS
#define BT_SCO_DMIC_AIAEC_NLP_REF       0                           //0代表主mic，1代表副mic

#define BT_SCO_AIAEC_PINK_NOISE_EN      0                           //是否打开自研AIAEC的粉噪处理

#define BT_SCO_FFDM_AI_EN               0                           //是否打开自研耳夹（1+1）单麦小模型降噪算法

#define BT_SCO_FFDM_AI_PRO_EN           0                           //是否打开自研耳夹（1+1）单麦大模型降噪算法

#define BT_SCO_DMIC_AI_EN               0                           //是否打开自研双麦AI降噪算法

#define BT_SCO_LDMIC_AI_EN              0                           //是否打开自研长麦距双麦AI降噪算法

#define BT_SNDP_SMIC_AI_EN              0                           //是否打开声加单麦AI降噪算法

#define BT_SNDP_FBDM_EN                 0                           //是否打开声加双mic（1+1）降噪算法

#define BT_SNDP_DM_AI_EN                0                           //是否打开声加双麦AI降噪算法

//自定义通话降噪、AEC算法
#define BT_SCO_NR_USER_SMIC_EN          0                           //是否打开自定义单麦降噪功能
#define BT_SCO_NR_USER_DMIC_EN          0                           //是否打开自定义双麦降噪功能
#define BT_SCO_AEC_USER_EN              0                           //是否打开自定义AEC功能，需要打开 BT_SCO_NR_USER_SMIC_EN 或 BT_SCO_NR_USER_DMIC_EN

//通话喇叭端算法（下行）
#define BT_SCO_FAR_DUMP_EN              0           //是否打开通话下行数据dump功能，dump:算法前 + 算法后

#define BT_SCO_FAR_NR_EN                1           //是否打开远端降噪算法
#define BT_SCO_FAR_NOISE_LEVEL          6           //强度（0~15dB，越大降噪效果越好，音质越差）

#define BT_SCO_DAC_DRC_EN               0           //是否打开通话下行————DRC，曲线在 eq/call_dac.drc 文件里面

#define BT_SCO_DAC_DNR_EN               0           //是否打开通话下行————动态降噪，注：配合下行DRC使用，不建议单独开启DNR
#define BT_SCO_DAC_DNR_THR              (5)         //设置通话下行动态降噪能量阈值

#define BT_SCO_CALLING_NR_EN            0           //是否打开去电后响铃前喇叭的降噪算法
#define BT_SCO_CALLING_VOICE_POW        100         //设置降噪阈值
#define BT_SCO_CALLING_VOICE_CNT        5           //设置降噪次数


/*****************************************************************************
* Module    : AUX功能配置
******************************************************************************/
#define AUX_CHANNEL_CFG                 0//(CH_AUXL_ADC0 | CH_AUXR_ADC1) //选择LINEIN通路
#define MIC_CHANNEL_CFG                 CH_MIC0                     //选择MIC的通路
#define AUX_SNR_EN                      0                           //AUX模式动态降噪(AUX模拟直通也能用)
#define AUX_ANC_EN                      0                           //AUX模式主动降噪,需要打开ANC_EN, 1为FF mode, 2为FB mode
#define LINEIN_DETECT_EN                0                           //是否打开LINEIN检测
#define LINEIN_2_PWRDOWN_EN             0                           //是否插入Linein后直接软关机（大耳包功能）

///通过配置工具选择检测GPIO
#define LINEIN_DETECT_INIT()            linein_detect_init()
#define LINEIN_IS_ONLINE()              linein_is_online()
#define LINEIN_DETECT_IS_BUSY()         linein_detect_is_busy()


/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                  0                           //使能usb udisk
#define UDE_SPEAKER_EN                  1                           //使能usb speaker audio下行
#define UDE_HID_EN                      1                           //使能usb hid按键
#define UDE_MIC_EN                      1                           //使能usb mic audio上行

#define USB_DET_VER_SEL                 0                           //USB插入检测方式,0-旧方式,1-新方式
#define UAC_VER_SEL                     1                           //0-none, 1-UAC1.0, 2-UAC2.0/UAC3.0
#define CFG_DESC_CFG_TYPE               0                           //UAC2.0/3.0: 0-不兼容I15自定义音频设置,VID可修改; 1-兼容I15自定义音频,VID不可改; 2-不适用; 3-兼容I15自定义音频,24bits mic, VID不可改
#define CFG_USB_MAXPOWER                0x96                        //usb Max Power, unit is 2mA
#define USB_DRIVER_MAX_EN               0                           //USB驱动能力MAX使能

#define USB_USER_CFG_DEV_EN             0                           //USB枚举配置使能,开启后USB枚举经过modules层
#define USB_USER_CFG_HID_EN             0                           //USB枚举配置使能,开启后用户可仅修改Hid report描述符
#define USB_USER_HID_OUT_EN             0                           //USB HID OUT使能,开启后底层会接收主机下发的HID消息
#define USB_USER_CFG_BCD_EN             0                           //USB固件版本使能
#define USB_BCD_DEVICE_NUM              0x0100                      //USB固件版本

#define CFG_GET_CONF_LEN                1                           //0-兼容海贝音乐软件上的usb独占模式音量调节,1-默认
#define USB_BC_EN                       1                           //0-disable, 1-使能USB Battery Charging描述符
#define UDM_VOL_DEFAULT_SEL             0                           //MIC电脑端默认音量: 100%: 0, 90%: 1, 80%: 2, 70%: 3
#define UDA_BALANCE_VOL_EN              1                           //是否使能Window/MacBook USB左右声道均衡调节功能(切换使能PC需要卸载设备)
#define UDA_BALANCE_ID_SEL              0                           //0-MacBook兼容性更好,但不支持3节耳机左右声道均衡,1-支持3节耳机左右声道均衡,但某些MacBook系统异常
#define USB_IPHONE_PREMOTE_EN           0                           //使能后开启iPhone/iPad音乐PP键快进/快退功能(AppleMusic/网易云音乐支持此功能)

#define USB_SPK_SPL_EN                  SPK_SPL_SEL                 //Speaker采样率选择(在win7下,不兼容44.1k,播放会出现杂音)
#define USB_SPK_BITS_EN                 SPK_BITS_SEL                //USB采样位宽选择(16bits/24bits/32bit)
#define USB_SPK_SYNC_MODE_EN            1                           //是否开启USB Speaker下行Sync模式

#define USB_MIC_DCH_EN                  1                           //是否开启USB MIC 2 Channel
#define USB_MIC_SPL_EN                  MIC_SPL_SEL                 //MIC采样率选择
#define USB_MIC_BITS_EN                 MIC_BITS_SEL                //MIC采样位宽选择(16bits/24bits)
#define USB_MIC_NR_EN                   0                           //MIC使用降噪（调试中，目前支持声加单双麦AI和自研长麦杆双麦AI）
#define USB_AEC_EN                      0                           //MIC使用AEC（调试中）

/*****************************************************************************
* Module    : SDADC配置控制
******************************************************************************/
#define SDADC_SOFT_GAIN_EN              0                           //ADC 软件增益使能
#define SDADC_EQ_EN                     0                           //是否打开通话ADC EQ，可用于mic频响补偿 (SDADC_SOFT_GAIN_EN使能有效)，曲线在 eq/sdadcl_16k.eq... 等文件里面
#define SDADC_DRC_EN                    0                           //是否打开ADC DRC v3 (SDADC_SOFT_GAIN_EN使能有效)，曲线在 eq/sdadc.drc 文件里面
#define SDADC_5CH_EN                    0                           //是否SDADC可以支持5路MIC数据，开启后SDADC不支持24bit数据输出

/*****************************************************************************
* Module    : ASR控制
******************************************************************************/
#define ASR_EN                          0                           //是否打开ASR检测
#define VAD_EN                          0                           //是否打开VAD检测
#define ASR_BASE_ADDR                   0XBE000                     //模型存放地址
#define ASR_BASE_LEN                    0X37000                     //模型长度

/*****************************************************************************
* Module    : SDDAC配置控制
******************************************************************************/
#define DAC_CH_SEL                      xcfg_cb.dac_sel             //DAC_MONO ~ DAC_VCMBUF_DUAL
#define DAC_FAST_SETUP_EN               0                           //DAC快速上电，有噪声需要外部功放MUTE
#define DAC_MAX_GAIN                    xcfg_cb.dac_max_gain        //配置DAC最大模拟增益，默认设置为dac_vol_table[VOL_MAX]
#define DAC_24BITS_EN                   xcfg_cb.dac_24bits_en
#define DAC_OUT_SPR                     xcfg_cb.dac_spr_sel         //dac out sample rate
#define DAC_VCM_CAPLESS_EN              xcfg_cb.dac_vcm_less_en     //DAC VCM省电容方案,使用内部VCM
#define DAC_MAXOUT_EN                   xcfg_cb.dac_maxout_en       //DAC大功率模式
#define DAC_PULL_DOWN_DELAY             80                          //控制DAC隔直电容的放电时间, 无电容时可设为0，减少开机时间。
#define DAC_DRC_EN                      1                           //是否使能DRC功能(After EQ0)
#define DAC_OFF_FOR_BT_CONN_EN          xcfg_cb.dac_off_for_conn
#define DAC_DNC_EN                      0
#define DAC_SYNC_DET_EN                 0
#define DAC_EQ_AFTER_VOL_EN             0                           //是否使能DAC为先过音量，再过music EQ

#define DAC_PT_EN                       0                           //是否打开DAC产测校准功能
#define DAC_PT_NUM                      4                           //DAC产测使用EQ条数(最多6条)

/*****************************************************************************
* Module    : EQ和ANC配置
******************************************************************************/
#define EQ_MODE_EN                      1           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define EQ_DBG_IN_UART                  1           //是否使能UART在线调节EQ
#define EQ_DBG_IN_UART_VUSB_EN          0           //打开vusb在线调EQ的功能，vusb使用dma方式，默认波特率1.5M,打开智能仓或快测功能时波特率为9600
#define EQ_DBG_IN_SPP                   1           //是否使能SPP在线调节EQ
#define EQ_DAC_DIV_EN                   0           //是否使能EQ DAC分频功能

#define EQ_APP_EN                       0           //是否打开APP独立调EQ功能
#define EQ_APP_NUM                      6           //APP独立调EQ使用EQ条数

#define ANC_EN                          0           //是否打开ANC功能
#define ANC_EQ_RES2_EN                  0           //是否使用资源2
#define ANC_EQ_RES2_BASE_ADDR           (FLASH_SIZE-0x6000-0x11000000)
#define ANC_EQ_RES2_FIRST_SELECT        0           //优先选择资源2
#define TINY_TRANSPARENCY_EN            0           //是否打开简单通透模式（没有打开ANC_EN时，可以打开简单通透模式）
#define ANC_DCRM_EN                     0           //是否消除降噪\通透下的直流偏置电压，优化功耗以实测为准
#define ANC_MAX_VOL_DIS_FB_EN           0           //是否开启在ANC下大音量关闭FB ANC的功能（与 DAC_DNC_EN 目前不能同时开）
#define ANC_MAX_VOL_DIS_FB_VOL_THR      (VOL_MAX-1) //ANC下大音量关闭FB ANC的音量阈值
#define ANC_MAX_VOL_DIS_FB_DAC_THR      0x74        //ANC下大音量关闭FB ANC的输入信号阈值

#define ANC_DS_DMA_EN                   0           //是否使能ANC DS DMA

#define ANC_ALG_EN                      0           //是否打开ANC算法功能
#define ANC_ALG_STEREO_EN               0           //ANC算法是否立体声左右两路都使能（开发调试中）
#define ANC_ALG_WIND_NOISE_FF_TALK_EN   0           //自研双MIC(FF+TALK)传统特征风噪和能量检测算法
#define ANC_ALG_WIND_NOISE_FF_FB_EN     0           //自研双MIC(FF+FB)传统特征风噪和能量检测算法
#define ANC_ALG_ASM_SIM_FF_EN           0           //自研单MIC(FF)降增噪算法
#define ANC_ALG_HOWLING_FB_EN           0           //自研防啸叫(FB)ANC算法
#define ANC_ALG_HOWLING_FF_EN           0           //自研防啸叫(FF)ANC算法
#define ANC_ALG_AI_WN_FF_EN             0           //自研单MIC(FF)AI风噪检测算法（NPU版本）
#define ANC_ALG_AI_WN_DSP_FF_EN         0           //自研单MIC(FF)AI风噪检测算法（DSP版本）
#define ANC_ALG_AI_WN_MINI_FF_EN        0           //自研单MIC(FF)AI小模型风噪检测算法
#define ANC_ALG_LIMITER_FF_EN           0           //自研单MIC(FF)瞬态噪声检测算法
#define ANC_ALG_DYVOL_FF_EN             0           //自研单MIC(FF)动态音量算法（开发调试中，如需使用请联系工程师）
#define ANC_ALG_MSC_ADP_FB_EN           0           //自研单MIC(FB)自适应音乐补偿算法（开发调试中，如需使用请联系工程师）
#define ANC_ALG_ADP_EQ_FF_FB_EN         0           //自研双MIC(FF+FB)自适应EQ算法
#define ANC_ALG_ASM_FF_EN               0           //自研单MIC(FF)环境自适应ANC算法
#define ANC_ALG_AEM_RT_FF_FB_EN         0           //自研双MIC(FF+FB)半入耳耳道自适应算法（开发调试中，如需使用请联系工程师）
#define ANC_SNDP_SAE_SHIELD_ADAPTER_EN  0
#define ANC_SNDP_SAE_WIND_DETECT_EN     0
#define ANC_ALG_USER_EN                 0           //用户自定义ANC算法
#define ANC_ALG_DUMP_EN                 0           //ANC算法使用bluetrum_voice_record工具dump数据，量产软件务必关闭，仅在调试时使用
#define ANC_ALG_DUMP_FOR_ANC_MODE       0           //是否在ANC模式下开始dump数据，量产软件务必关闭，仅在调试时使用
#define ANC_ALG_DUMP_DATA_TYPE          (ANC_ALG_DUMP_DATA_FF | ANC_ALG_DUMP_DATA_SPK)
#define ANC_ALG_DBG_EN                  0           //ANC算法在线调试功能

#define ANC_TOOL_EN                     0           //是否打开ANC工具功能（开发调试中，请勿打开）
#define ANC_TOOL_SPP_EN                 0
#define ANC_TOOL_LINK_FB_MSC_SIMU_EN    0

/*****************************************************************************
 * Module    : K歌功能配置
 *****************************************************************************/
#define SYS_KARAOK_EN                   0   //是否打开K歌功能
#define SYS_KARAOK_BUFMODE              0   //使用降采样来加倍混响深度
#define BT_HFP_CALL_KARAOK_EN           0   //通话是否支持KARAOK.
#define SYS_ECHO_EN                     1   //是否使能混响功能
#define SYS_ECHO_DELAY                  16  //混响间隔级数
#define SYS_BASS_TREBLE_EN              0   //是否使能高低音调节
#define SYS_MAGIC_VOICE_EN              0   //是否使能魔音功能
#define SYS_HOWLING_EN                  0   //是否使能防啸叫功能
#define SYS_PITCH_SHIFT_EN              0   //是否使能新的魔音算法, 与SYS_MAGIC_VOICE_EN二选一
#define MIC_DNR_EN                      0   //是否使能MIC动态降噪


/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
#define USER_ADKEY                      0           //ADKEY的使用， 0为不使用
#define USER_ADKEY2                     0           //ADKEY2的使用，0为不使用
#define USER_PWRKEY                     1           //PWRKEY的使用，0为不使用
#define USER_IOKEY                      0           //IOKEY的使用， 0为不使用
#define USER_ADKEY_MUX_SDCLK            0           //是否使用复用SDCLK的ADKEY, 共用USER_ADKEY的按键table
#define USER_ADKEY_MUX_LED              0           //是否使用ADKEY与LED复用, 共用USER_ADKEY的流程(ADKEY与BLED配置同一IO)
#define ADKEY_PU10K_EN                  0           //ADKEY是否使用内部10K上拉, 按键数量及阻值见port_key.c

#define USER_TKEY                       1           //TouchKEY的使用，0为不使用
#define USER_TKEY_SOFT_PWR_EN           0           //是否使用TouchKey进行软开关机
#define USER_TKEY_LOWPWR_WAKEUP_DIS     0           //是否电池低电时关掉触摸唤醒, 无保护板的电池需要打开。
#define USER_TKEY_INEAR                 0           //是否使用TouchKey的入耳检测功能
#define USER_TKEY_TEMP_EN               0           //是否使用TouchKey的温度检测功能
#define USER_TKEY_DEBUG_EN              0           //仅调试使用，用于确认TKEY的参数
#define TKEY_IS_PRESS()			        (tkey_is_pressed() && USER_TKEY_SOFT_PWR_EN)

#define USER_TKEY_SLIDE                 0           //是否使用TouchKey的滑动检测功能
#define USER_PB5_ONLY_IS_TKEY           0           //PB5是独立触摸按键，不是滑动按键
#define USER_TKEY_SLIDE_UP              0           //滑动的上键选择，PB5：0, PB0：1, PB1：2, PB2：3, PA7：4
#define USER_TKEY_SLIDE_DOWN            1           //滑动的上键选择，PB5：0, PB0：1, PB1：2, PB2：3, PA7：4

#define USER_KEY_KNOB_EN                0           //旋钮的使用，0为不使用
#define USER_KEY_KNOB_LEVEL             16          //旋钮的级数

#define USER_MULTI_PRESS_EN              1           //按键N击检测使能
#define USER_KEY_DOUBLE                 {KU_PLAY_PWR_USER_DEF, KU_PLAY_USER_DEF}        //支持双击/三击/四击/五击的按键
#define ADKEY_CH                        xcfg_cb.adkey_ch    //ADCCH_PE7
#define ADKEY2_CH                       xcfg_cb.adkey2_ch   //ADCCH_PE6

#define PWRKEY_IS_PRESS()			    (sys_cb.wko_pwrkey_en && pwrkey_is_pressed())
#define DOUBLE_KEY_TIME                 (xcfg_cb.double_key_time)                       //按键双击响应时间（单位50ms）
#define PWRON_PRESS_TIME                (500*xcfg_cb.pwron_press_time)                  //长按PWRKEY多长时间开机？
#define PWROFF_PRESS_TIME               (3+3*xcfg_cb.pwroff_press_time)                 //长按PWRKEY多长时间关机？

/*****************************************************************************
 * Module    : 入耳检测功能配置
 *****************************************************************************/
#define USER_INEAR_DETECT_EN            0           //是否打开入耳检测功能
#define USER_INEAR_DET_OPT              0           //是否使用光感检测入耳, 即判断GPIO的高低电平
#define INEAR_OPT_PORT_INIT()           {GPIOEDE |= BIT(5);\
                                        GPIOEPU  |= BIT(5);\
                                        GPIOEDIR |= BIT(5);}
#if USER_INEAR_DET_OPT
    #define INEAR_IS_ONLINE()           (!(GPIOE & BIT(5)))
#else
    #define INEAR_IS_ONLINE()           tkey_is_inear()
#endif

/*****************************************************************************
 * Module    : LED指示灯配置
 *****************************************************************************/
#define LED_DISP_EN                     1           //是否使用LED指示灯(蓝灯)
#define LED_PWR_EN                      0           //充电及电源指示灯(红灯)
#define LED_LOWBAT_EN                   0                           //电池低电是否闪红灯
#define BLED_CHARGE_FULL                xcfg_cb.charge_full_bled    //充电满是否亮蓝灯
#define BT_RECONN_LED_EN                0//xcfg_cb.bt_reconn_led_en    //蓝牙回连状态是否不同的闪灯方式

#define LED_INIT()                      bled_func.port_init(&bled_gpio)
#define LED_SET_ON()                    bled_func.set_on(&bled_gpio)
#define LED_SET_OFF()                   bled_func.set_off(&bled_gpio)

#define LED_PWR_INIT()                  rled_func.port_init(&rled_gpio)
#define LED_PWR_SET_ON()                rled_func.set_on(&rled_gpio)
#define LED_PWR_SET_OFF()               rled_func.set_off(&rled_gpio)


/*****************************************************************************
 * Module    : Loudspeaker mute及耳机检测配置
 *****************************************************************************/
#define LOUDSPEAKER_MUTE_EN             0           //是否使能功放MUTE
#define LOUDSPEAKER_MUTE_INIT()         loudspeaker_mute_init()
#define LOUDSPEAKER_MUTE_DIS()          loudspeaker_disable()
#define LOUDSPEAKER_MUTE()              loudspeaker_mute()
#define LOUDSPEAKER_UNMUTE()            loudspeaker_unmute()
#define LOUDSPEAKER_UNMUTE_DELAY        6           //UNMUTE延时配置，单位为5ms


/*****************************************************************************
 * Module    : 电量检测及低电
 *****************************************************************************/
#define VBAT_DETECT_EN                  1           //电池电量检测功能
#define VBAT_FILTER_USE_PEAK            0           //电池检测滤波方式: 0=取平均值，1=取峰值(适用于播音乐时电池波动较大的音箱方案).
#define LPWR_WARNING_VBAT               xcfg_cb.lpwr_warning_vbat   //低电提醒电压
#define LPWR_OFF_VBAT                   xcfg_cb.lpwr_off_vbat       //低电关机电压
#define LPWR_REDUCE_VOL_EN              0                           //低电是否降低音量
#define LPWR_WARING_TIMES               0xff                        //报低电次数

/*****************************************************************************
 * Module    : 充电功能选择
 *****************************************************************************/
#define CHARGE_EN                       1           //是否打开充电功能
#define CHARGE_TRICK_EN                 xcfg_cb.charge_trick_en     //是否打开涓流充电功能
#define CHARGE_DC_NOT_PWRON             xcfg_cb.charge_dc_not_pwron //DC插入，是否软开机。 1: DC IN时不能开机
#define CHARGE_VOLT_FOLLOW_EN           xcfg_cb.charge_voltage_follow // 打开电压跟随模式，适用于支持快充的充电仓
#define CHARGE_DC_IN()                  ((RTCCON >> 20) & 0x01)
#define CHARGE_INBOX()                  ((RTCCON >> 22) & 0x01)
#define CHARGE_LOW_POWER_EN             0           //是否打开充电低功耗模式,打开此宏时，不能 INTF_HUART 传输数据

//充电截止电流
#define CHARGE_STOP_CURR                xcfg_cb.charge_stop_curr
//充电截止电压：0:4.2v 1:4.35v 2:4.4v 3:4.45v
#define CHARGE_STOP_VOLT                0
//恒流充电（电池电压大于2.9v）电流
#define CHARGE_CONSTANT_CURR            xcfg_cb.charge_constant_curr
//涓流截止电压：0:2.9v; 1:3v
#define CHARGE_TRICK_STOP_VOLT          1
//涓流充电（电池电压小于2.9v）电流
#define CHARGE_TRICKLE_CURR             xcfg_cb.charge_trickle_curr
//恒压差充电差值选择：0:187.5mV  1:250mV  2:312mV  3:375mV
#define CHARGE_VOLT_FOLLOW_DIFF         3

/*****************************************************************************
 * Module    :  充电仓功能选择
 *****************************************************************************/
#define CHARGE_BOX_EN                   1               //是否打开充电仓功能
#define CHARGE_BOX_TYPE                 CBOX_SSW        //充电仓类型选择，0=普通充电仓，1=昇生微智能充电仓
#define CHARGE_BOX_INTF_SEL             INTF_HUART      //充电仓通信接口选择
#define CHARGE_BOX_DELAY_DISC           3500            //智能充电仓时，延迟断线进入充电（0=不延迟, n=延迟n毫秒）
#define CHARGE_BOX_OUTBOX_FLIT          0               //智能仓通信时，是否使能消抖机制，处理有些仓不能保持维持电压，会检测到一瞬间出仓又入仓
#define CHARGE_BOX_LK_CRC_EN            0               //智能充电仓时，是否使能TWS Linkkey信息校验，注意命令兼容性,VHOUSE_CMD_GET_TWS_BTADDR会加多2byte内容

/*****************************************************************************
* Module    : NTC预警关机功能配置
******************************************************************************/
#define USER_NTC                        1
#define ADCCH_NTC                       ADCCH_PE7


/*****************************************************************************
 * Module    :  产测功能选择
 *****************************************************************************/
#define IODM_TEST_EN                    1               //是否打开小牛测控功能
#define QTEST_EN                        1               //是否打开测试盒快测功能，默认VUSB 通讯测试
#define QTEST_IS_ENABLE()               xcfg_cb.qtest_en    //setting中是否打开了快测功能
#define TEST_INTF_SEL                   INTF_HUART      //产测通信接口选择

/*****************************************************************************
 * Module    : uart升级配置
 *****************************************************************************/
#define UPD_UART_EN                     0           	//是否打开串口直接升级芯片功能
#define UPD_UART_SEL                    INTF_UART1  	//选择串口
#define UPD_UART_BAUD                   115200
#define UPD_UART_TX_IO                  IO_NONE
#define UPD_UART_RX_IO                  IO_NONE
#define UPD_UART_IO_MAP                 (URX1MAP_TX|UTX1MAP_VUSB)

/*****************************************************************************
 * Module    : 其他配置
 *****************************************************************************/
#define RGB_SERIAL_EN                   0           //串行RGB推灯功能
#define PWM_RGB_EN                      0           //PWM RGB三色灯功能
#define ENERGY_LED_EN                   0           //能量灯软件PWM显示,声音越大,点亮的灯越多.
#define TSEN_DETECT_EN                  0			//温度检测

/*****************************************************************************
 * Module    : Sensor配置
 *****************************************************************************/
#define SC7A20_EN						0			//是否使能敲击芯片功能（需要配置I2C）

/*****************************************************************************
 * Module    : SD0配置
 *****************************************************************************/
#define SD0_MAPPING                     SD0MAP_G1   //选择SD0 mapping

///通过配置工具选择检测GPIO
#define SD_DETECT_INIT()                sdcard_detect_init()
#define SD_IS_ONLINE()                  sdcard_is_online()
#define SD_DETECT_IS_BUSY()             sdcard_detect_is_busy()

/*****************************************************************************
 * Module    : I2C配置
 *****************************************************************************/
#define I2C_HW_EN                       0           //是否使能硬件I2C功能
#define I2C_MAPPING                     I2CMAP_PA14PA13 //选择I2C mapping

#define I2C_SW_EN                       0           //是否使能软件I2C功能
#define I2C_MUX_SD_EN                   0           //是否I2C复用SD卡的IO

#if I2C_MUX_SD_EN
#define I2C_SCL_IN()                    SD_CMD_DIR_IN()
#define I2C_SCL_OUT()                   SD_CMD_DIR_OUT()
#define I2C_SCL_H()                     SD_CMD_OUT_H()
#define I2C_SCL_L()                     SD_CMD_OUT_L()
#define I2C_SDA_IN()                    SD_DAT_DIR_IN()
#define I2C_SDA_OUT()                   SD_DAT_DIR_OUT()
#define I2C_SDA_H()                     SD_DAT_OUT_H()
#define I2C_SDA_L()                     SD_DAT_OUT_L()
#define I2C_SDA_IS_H()                  SD_DAT_STA()
#else
#define I2C_SCL_IN()                    GPIOADIR |= BIT(13);
#define I2C_SCL_OUT()                   {GPIOADIR &= ~BIT(13); GPIOADE |= BIT(13);}
#define I2C_SCL_H()                     GPIOASET = BIT(13);
#define I2C_SCL_L()                     GPIOACLR = BIT(13);
#define I2C_SDA_IN()                    {GPIOADIR |= BIT(14); GPIOAPU |= BIT(14);}
#define I2C_SDA_OUT()                   {GPIOADIR &= ~BIT(14); GPIOADE |= BIT(14);}
#define I2C_SDA_H()                     GPIOASET = BIT(14);
#define I2C_SDA_L()                     GPIOACLR = BIT(14);
#define I2C_SDA_IS_H()                  (GPIOA & BIT(14))
#endif // I2C_MUX_SD_EN

#define I2C_SDA_SCL_OUT()               {I2C_SDA_OUT(); I2C_SCL_OUT();}
#define I2C_SDA_SCL_H()                 {I2C_SDA_H(); I2C_SCL_H();}

/*****************************************************************************
 * Module    : IIS配置
 *****************************************************************************/
#define IIS_EN                          0           //是否使能IIS独立模块功能
#define IIS_RX2SCO_EN                   0           //通话时iisrx数据作为上行数据，默认主机输出16K采样率
/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 1            //是否打开提示音功能, 总开关
#define WARNING_MP3_EN                  1            //是否支持mp3提示音（不可以和音乐、通话叠加播放）
#define WARNING_WAV_EN                  1            //是否支持wav提示音（可以和音乐、通话叠加播放）
#define WARNING_WSBC_EN                 0            //是否支持wsbc提示音（可以和音乐、通话叠加播放）
#define WARNING_PIANO_EN                1            //是否支持tone提示音（可以和音乐、通话叠加播放）
#define WARNING_BREAK_EN                0            //是否支持提示音打断功能（主要是打断开机提示音和TWS副耳断开提示音）
#define WARNING_FIXED_VOLUME            0            //提示音是否固定音量，不跟随系统音量调节
#define WARING_MAXVOL_TYPE              RES_TYPE_TONE            //最大音量提示音类型
#define WARNING_VOLUME                  xcfg_cb.warning_volume   //播放提示音的音量级数
#define LANG_SELECT                     LANG_EN_ZH   //提示音语言选择
#define WARNING_SYSVOL_ADJ_EN           0            //是否使能音乐最大音量时播放wav、tone音降低音乐音量
#define WARNING_SYSVOL_ADJ_LEVEL        0x3fff       //音乐降低音量，最大时0x7fff

#define WARNING_POWER_ON                1
#define WARNING_POWER_OFF               1
#define WARNING_FUNC_MUSIC              0
#define WARNING_FUNC_BT                 0
#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                0
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             1
#define WARNING_BT_WAIT_CONNECT         0
#define WARNING_BT_CONNECT              1
#define WARNING_BT_DISCONNECT           1
#define WARNING_BT_INCALL               0
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              1
#define WARNING_MIN_VOLUME              0
#define WARNING_BT_HID_MENU             0            //BT HID MENU手动连接/断开HID Profile提示音
#define WARNING_BTHID_CONN              0            //BTHID模式是否有独立的连接/断开提示音
#define WARNING_BT_PAIR                 1            //BT PAIRING提示音

/*****************************************************************************
 * Module    : 豆包鉴权配置
 *****************************************************************************/
 #define DOUBAO_AUTH_EN                 0            //是否使能豆包鉴权配置

 /*****************************************************************************
 * Module    : OPUS功能配置
 *****************************************************************************/
#define OPUS_ENC_EN                     0            //opus编码使能
#define OPUS_DEC_EN                     0            //opus解码使能

#define UG_VERSION       002
#define SW_VERSION		 2		        //只能使用数字(u16)，例如版本号V1.0.2，ab_mate用102表示，ab_fot用0x102表示
#define HW_VERSION		"V0.0.1"		//只能使用数字0-9,ota需要转码
#include "config_extra.h"

#endif // USER_CONFIG_H
