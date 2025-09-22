#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : SDK版本配置
 *****************************************************************************/
#ifndef SDK_VERSION
    #define SDK_VERSION                 0x016       //V016
#endif

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#undef  SYS_ADJ_DIGVOL_EN
#define SYS_ADJ_DIGVOL_EN               1

#ifndef SYS_MODE_BREAKPOINT_EN
#define SYS_MODE_BREAKPOINT_EN          0
#endif // SYS_MODE_BREAKPOINT_EN

#ifndef ENERGY_LED_EN
#define ENERGY_LED_EN                   0
#endif

#ifndef PLUGIN_SYS_INIT_FINISH_CALLBACK
#define PLUGIN_SYS_INIT_FINISH_CALLBACK 0
#endif

#ifndef PLUGIN_FUNC_IDLE_ENTER_CHECK
#define PLUGIN_FUNC_IDLE_ENTER_CHECK    0
#endif


#ifndef FLASH_SPEED_UP_EN
#define FLASH_SPEED_UP_EN               1
#endif

#ifndef MUSIC_AAC_SUPPORT
#define MUSIC_AAC_SUPPORT               0
#endif // MUSIC_AAC_SUPPORT

#ifndef USER_NTC
#define USER_NTC                        0
#endif


#if !EQ_DBG_IN_UART
#undef EQ_DBG_IN_UART_VUSB_EN
#define EQ_DBG_IN_UART_VUSB_EN          0
#endif

/*****************************************************************************
 * Module    :  充电仓功能选择
 *****************************************************************************/
#if !CHARGE_EN
    #undef CHARGE_BOX_EN
    #define CHARGE_BOX_EN         0
#endif

#if !CHARGE_BOX_EN
    #undef CHARGE_BOX_TYPE
    #define CHARGE_BOX_TYPE         CBOX_NONE
#endif

#if (CHARGE_BOX_TYPE != CBOX_SSW)
    #undef CHARGE_BOX_INTF_SEL
    #define CHARGE_BOX_INTF_SEL     INTF_NONE
#endif

#if ((CHARGE_BOX_TYPE == CBOX_SSW) && !BT_TWS_EN)
    #undef CHARGE_BOX_TYPE
    #define CHARGE_BOX_TYPE         CBOX_NONE
#endif

/*****************************************************************************
 * Module    :  产测功能选择
 *****************************************************************************/
#if !IODM_TEST_EN && !QTEST_EN
    #undef TEST_INTF_SEL
    #define TEST_INTF_SEL               INTF_NONE
#endif

#if IODM_TEST_EN
#undef FUNC_BT_DUT_EN

#define FUNC_BT_DUT_EN            1
#endif

#if IODM_TEST_EN || FUNC_BT_FCC_EN || FUNC_BT_DUT_EN || QTEST_EN
#define TEST_MODE_BT_INFO         1
#endif

#if !DAC_PT_EN
#undef DAC_PT_NUM
#define DAC_PT_NUM                      0
#endif

/*****************************************************************************
 * Module    :  UART功能选择（待整理）
 *****************************************************************************/
#if BT_AEC_DUMP_EN || BT_AEC_FRE_DUMP_EN || BT_SCO_DUMP_EN || BT_EQ_DUMP_EN || BT_SCO_FAR_DUMP_EN
    #define BT_SCO_DUMP_TX_EN           1
#else
    #define BT_SCO_DUMP_TX_EN           0
#endif

#if (CHARGE_BOX_INTF_SEL == INTF_UART1) || (TEST_INTF_SEL == INTF_UART1) || (UPD_UART_EN && UPD_UART_SEL == 1)
    #define UART1_EN                    1
#elif (CHARGE_BOX_INTF_SEL == INTF_UART2) || (TEST_INTF_SEL == INTF_UART2) || (UPD_UART_EN && UPD_UART_SEL == 2)
    #define UART2_EN                    1
#endif

#if (CHARGE_BOX_INTF_SEL == INTF_HUART) || (TEST_INTF_SEL == INTF_HUART) || EQ_DBG_IN_UART || BT_SCO_DUMP_TX_EN || QTEST_EN || ANC_TOOL_EN/*(强制打开HUART)*/
    #define HUART_EN                    1
#endif

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#if !FUNC_MUSIC_EN
#undef  MUSIC_UDISK_EN
#undef  MUSIC_SDCARD_EN

#undef  MUSIC_WAV_SUPPORT
#undef  MUSIC_WMA_SUPPORT
#undef  MUSIC_APE_SUPPORT
#undef  MUSIC_FLAC_SUPPORT
#undef  MUSIC_SBC_SUPPORT
#undef  MUSIC_M4A_SUPPORT
#undef  MUSIC_AAC_SUPPORT
#undef  MUSIC_ID3_TAG_EN

#define MUSIC_UDISK_EN              0
#define MUSIC_SDCARD_EN             0
#define MUSIC_WAV_SUPPORT           0
#define MUSIC_WMA_SUPPORT           0
#define MUSIC_APE_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define MUSIC_M4A_SUPPORT           0
#define MUSIC_SBC_SUPPORT           0
#define MUSIC_ID3_TAG_EN            0
#define MUSIC_AAC_SUPPORT           0
#endif // FUNC_MUSIC_EN

#if !CHARGE_EN
#undef  CHARGE_TRICK_EN
#undef  CHARGE_DC_IN
#undef  CHARGE_DC_NOT_PWRON
#define CHARGE_TRICK_EN             0
#define CHARGE_DC_NOT_PWRON         0
#define CHARGE_DC_IN()              0
#endif

#if !LINEIN_DETECT_EN
#undef LINEIN_DETECT_INIT
#undef LINEIN_IS_ONLINE
#undef LINEIN_DETECT_IS_BUSY

#define LINEIN_DETECT_INIT()
#define LINEIN_IS_ONLINE()          0
#define LINEIN_DETECT_IS_BUSY()        0
#endif

#ifndef PWROFF_PRESS_TIME
#define PWROFF_PRESS_TIME           3               //1:大约1秒    3: 1.5s,  6: 2s,  9: 2.5s, 12: 3s
#endif // PWROFF_PRESS_TIME

#ifndef PWRON_PRESS_TIME
#define PWRON_PRESS_TIME            1500           //1.5s
#endif // PWRON_PRESS_TIME

#ifndef DOUBLE_KEY_TIME
#define DOUBLE_KEY_TIME             1
#endif // DOUBLE_KEY_TIME

#ifndef SYS_LIMIT_VOLUME
#define SYS_LIMIT_VOLUME            5
#endif // SYS_LIMIT_VOLUME

#ifndef LINEIN_2_PWRDOWN_TONE_EN
#define LINEIN_2_PWRDOWN_TONE_EN    0   //插入Linein直接关机，是否播放关机提示音。
#endif // LINEIN_2_PWRDOWN_TONE_EN

#ifndef LPWR_WARING_TIMES
#define LPWR_WARING_TIMES           0xff
#endif // LPWR_WARING_TIMES

#if !BUZZER_EN
#undef BUZZER_INIT
#define BUZZER_INIT()
#undef BUZZER_ON
#define BUZZER_ON()
#undef BUZZER_OFF
#define BUZZER_OFF()
#endif

#ifndef LED_LOWBAT_EN
#define LED_LOWBAT_EN              0
#endif // RLED_LOWBAT_EN

#ifndef RLED_LOWBAT_FOLLOW_EN
#define RLED_LOWBAT_FOLLOW_EN       0
#endif // RLED_LOWBAT_FOLLOW_EN

#ifndef SLEEP_DAC_OFF_EN
#define SLEEP_DAC_OFF_EN            1
#endif // SLEEP_DAC_OFF_EN

#ifndef PWRON_FRIST_BAT_EN
#define PWRON_FRIST_BAT_EN          1
#endif // PWRON_FRIST_BAT_EN

#if !LED_DISP_EN
#undef LED_LOWBAT_EN
#undef RLED_LOWBAT_FOLLOW_EN
#undef BLED_FFT_EN
#undef BLED_LOW2ON_EN
#undef BLED_CHARGE_FULL_EN
#undef BT_RECONN_LED_EN
#define LED_LOWBAT_EN              0
#define RLED_LOWBAT_FOLLOW_EN       0
#define BLED_FFT_EN                 0
#define BLED_LOW2ON_EN              0
#define BLED_CHARGE_FULL_EN         0
#define BT_RECONN_LED_EN            0
#endif

#if !LED_DISP_EN
#undef LED_INIT
#define LED_INIT()
#undef LED_SET_ON
#define LED_SET_ON()
#undef LED_SET_OFF
#define LED_SET_OFF()
#endif

#if !LED_PWR_EN
#undef LED_PWR_INIT
#define LED_PWR_INIT()
#undef LED_PWR_SET_ON
#define LED_PWR_SET_ON()
#undef LED_PWR_SET_OFF
#define LED_PWR_SET_OFF()
#endif

#undef GUI_LCD_EN
#define GUI_LCD_EN                      0
#undef  MUSIC_NAVIGATION_EN
#define MUSIC_NAVIGATION_EN             0


#if !LOUDSPEAKER_MUTE_EN
#undef LOUDSPEAKER_MUTE_INIT
#define LOUDSPEAKER_MUTE_INIT()
#undef LOUDSPEAKER_MUTE_DIS
#define LOUDSPEAKER_MUTE_DIS()
#undef LOUDSPEAKER_MUTE
#define LOUDSPEAKER_MUTE()
#undef LOUDSPEAKER_UNMUTE
#define LOUDSPEAKER_UNMUTE()
#undef LOUDSPEAKER_UNMUTE_DELAY
#define LOUDSPEAKER_UNMUTE_DELAY        0
#endif

#ifndef IR_INPUT_NUM_MAX
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999
#endif // IR_INPUT_NUM_MAX

#ifndef FMRX_THRESHOLD_VAL
#define FMRX_THRESHOLD_VAL              128
#endif // FMRX_THRESHOLD_VAL

#ifndef FMRX_SEEK_DISP_CH_EN
#define FMRX_SEEK_DISP_CH_EN            0
#endif // FMRX_SEEK_DISP_CH_EN

#ifndef SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN               0
#undef SD_IS_SOFT_DETECT
#define SD_IS_SOFT_DETECT()             0
#endif // SD_SOFT_DETECT_EN

#if USER_ADKEY_MUX_LED
#undef  USER_ADKEY
#undef  USER_ADKEY_MUX_SDCLK
#undef  ADKEY_PU10K_EN

#define USER_ADKEY                      1
#define USER_ADKEY_MUX_SDCLK            0
#define ADKEY_PU10K_EN                  0
#endif // USER_ADKEY_MUX_LED

#if ((SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G4)) && MUSIC_UDISK_EN && MUSIC_SDCARD_EN
#define SD_USB_MUX_IO_EN				0
#else
#define SD_USB_MUX_IO_EN				0
#endif

#ifndef DAC_DRC_EN
#define DAC_DRC_EN                      0
#endif

#if !USER_PWRKEY
#undef PWRKEY_IS_PRESS
#define PWRKEY_IS_PRESS()               0
#endif

#if !USER_TKEY
#undef  USER_TKEY_SOFT_PWR_EN
#undef  USER_TKEY_INEAR
#undef  USER_TKEY_DEBUG_EN
#undef  TKEY_IS_PRESS
#undef  USER_TKEY_TEMP_EN
#define USER_TKEY_SOFT_PWR_EN           0
#define USER_TKEY_INEAR                 0
#define USER_TKEY_DEBUG_EN              0
#define TKEY_IS_PRESS()                 0
#define USER_TKEY_TEMP_EN               0
#else
#undef  USER_PWRKEY
#define USER_PWRKEY                     0
#if USER_TKEY_SLIDE
#define USER_TKEY_MULT                  1           //使用硬件检测的多路触摸，要求USER_TKEY也要打开
#endif
#endif

#if USER_TKEY_INEAR
#undef  USER_TKEY_TEMP_EN
#define USER_TKEY_TEMP_EN               1
#endif // USER_TKEY_INEAR

#if !USER_TKEY_SOFT_PWR_EN
#undef USER_TKEY_LOWPWR_WAKEUP_DIS
#define USER_TKEY_LOWPWR_WAKEUP_DIS     0
#endif

#if !USER_INEAR_DETECT_EN
#undef USER_TKEY_INEAR
#undef USER_INEAR_DET_OPT
#undef INEAR_IS_ONLINE
#define USER_TKEY_INEAR                 0
#define USER_INEAR_DET_OPT              0
#define INEAR_IS_ONLINE()               0
#endif

#if !USER_INEAR_DET_OPT
#undef INEAR_OPT_PORT_INIT
#define INEAR_OPT_PORT_INIT()
#endif

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#if !FUNC_REC_EN
#undef  FMRX_REC_EN
#undef  AUX_REC_EN
#undef  MIC_REC_EN
#undef  REC_TYPE_SEL
#undef  REC_AUTO_PLAY
#undef  REC_FAST_PLAY
#undef  BT_REC_EN
#undef  BT_HFP_REC_EN
#undef  KARAOK_REC_EN
#undef  REC_STOP_MUTE_1S

#define FMRX_REC_EN                 0
#define AUX_REC_EN                  0
#define MIC_REC_EN                  0
#define REC_AUTO_PLAY               0
#define REC_FAST_PLAY               0
#define BT_REC_EN                   0
#define BT_HFP_REC_EN               0
#define KARAOK_REC_EN               0
#define REC_TYPE_SEL                REC_NO
#define REC_STOP_MUTE_1S            0
#endif //FUNC_REC_EN


#if FMRX_INSIDE_EN
#undef  FMRX_QN8035_EN
#define FMRX_QN8035_EN              0
//#else
//#undef  I2C_SW_EN
//#define I2C_SW_EN                   1
#endif

#if !I2C_SW_EN
#undef  I2C_MUX_SD_EN
#define I2C_MUX_SD_EN               0
#endif


/*****************************************************************************
 * Module    : karaok相关配置
 *****************************************************************************/
#if SYS_KARAOK_EN
#undef FUNC_SPEAKER_EN
#undef BT_HFP_REC_EN

#define FUNC_SPEAKER_EN             0
#define BT_HFP_REC_EN               0   //Karaok不支持通话录音

#else
#undef SYS_ECHO_EN
#undef HIFI4_ECHO_EN
#undef HIFI4_REVERB_EN
#undef SYS_MAGIC_VOICE_EN
#undef HIFI4_PITCH_SHIFT_EN
#undef HIFI4_HOWLING_EN
#undef SYS_HOWLING_EN
#undef KARAOK_REC_EN

#define SYS_ECHO_EN                 0
#define HIFI4_ECHO_EN               0
#define HIFI4_REVERB_EN             0
#define SYS_MAGIC_VOICE_EN          0
#define HIFI4_PITCH_SHIFT_EN        0
#define HIFI4_HOWLING_EN            0
#define SYS_HOWLING_EN              0
#define KARAOK_REC_EN               0
#endif // SYS_KARAOK_EN

#if SYS_HOWLING_EN
#undef SYS_MAGIC_VOICE_EN
#define SYS_MAGIC_VOICE_EN          1
#endif


/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/
#if (APP_DEMO_EN && AB_MATE_APP_EN)
#error "APP: please don't open APP_DEMO_EN & AB_MATE_APP_EN at the same time\n"
#endif

#if APP_DEMO_EN || AB_MATE_APP_EN || LE_BQB_RF_EN || GFPS_EN || LE_PRIV_EN || LE_DUEROS_DMA_EN || LE_TUYA_EN
    #define LE_EN                   1
    #define BT_DUAL_MODE_EN         1
#else
    #define LE_EN                   0
    #define BT_DUAL_MODE_EN         0
#endif

#if GFPS_EN
    #define LE_ADDRESS_TYPE         3       //GAP_RANDOM_ADDRESS_RESOLVABLE
#else
    #define LE_ADDRESS_TYPE         0       //GAP_RANDOM_ADDRESS_TYPE_OFF
#endif

#if !LE_EN
#undef LE_AB_FOT_EN
#undef LE_PAIR_EN
#undef LE_SM_SC_EN
#define LE_AB_FOT_EN                0
#define LE_PAIR_EN                  0
#define LE_SM_SC_EN                 0
#endif

#if GFPS_EN
#undef LE_ADV_POWERON_EN
#define LE_ADV_POWERON_EN          0
#endif

#if LE_DUEROS_DMA_EN
    #undef OPUS_ENC_EN
    #define OPUS_ENC_EN     1

#if BT_TWS_EN
    #undef BT_TWS_PUBLIC_ADDR_EN
    #define BT_TWS_PUBLIC_ADDR_EN     1
#endif

#endif

#ifndef BT_A2DP_RECON_EN
#define BT_A2DP_RECON_EN            0
#endif

#ifndef BT_PAIR_SLEEP_EN
#define BT_PAIR_SLEEP_EN            0
#endif // BT_PAIR_SLEEP_EN

#if BT_BACKSTAGE_EN
#undef MUSIC_WMA_SUPPORT
#undef MUSIC_FLAC_SUPPORT
#undef FMRX_THRESHOLD_FZ

#define MUSIC_WMA_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define FMRX_THRESHOLD_FZ           0
#endif

#ifndef BT_HFP_RINGS_BEFORE_NUMBER
#define BT_HFP_RINGS_BEFORE_NUMBER  0
#endif

#ifndef BT_A2DP_AAC_AUDIO_EN
#define BT_A2DP_AAC_AUDIO_EN        0
#endif // BT_A2DP_AAC_AUDIO_EN

#if !BT_A2DP_EN
#undef  BT_A2DP_AAC_AUDIO_EN
#define BT_A2DP_AAC_AUDIO_EN        0
#endif

#if BT_FCC_TEST_EN || LE_BQB_RF_EN    //FCC 默认PB3 (USB_DP) 波特率1500000通信, 关闭用到PB3的程序
#undef FUNC_USBDEV_EN
#undef MUSIC_UDISK_EN
#define FUNC_USBDEV_EN             0
#define MUSIC_UDISK_EN             0
//#if (UART0_PRINTF_SEL == PRINTF_PB3)
//#undef UART0_PRINTF_SEL
//#define UART0_PRINTF_SEL  PRINTF_NONE
//#endif
#endif

#if !BT_TWS_EN
#undef BT_TWS_SCO_EN
#define BT_TWS_SCO_EN               0
#undef BT_TWS_MS_SWITCH_EN
#define BT_TWS_MS_SWITCH_EN         0
#undef FOT_SUPPORT_TWS
#define FOT_SUPPORT_TWS               0
#undef BT_TWS_DBG_EN
#define BT_TWS_DBG_EN               0
#endif

#if BT_TWS_DBG_EN
    #if !BT_SPP_EN
    #error "TWS_DBG: please open BT_TWS_DBG_EN & BT_SPP_EN at the same time\n"
    #endif
#endif // BT_TWS_DBG_EN

#if AB_MATE_APP_EN
#undef EQ_APP_EN
#undef BT_A2DP_AVRCP_PLAY_STATUS_EN
#define EQ_APP_EN                   1
#define BT_A2DP_AVRCP_PLAY_STATUS_EN    1

#undef LE_AB_FOT_EN
#undef BT_AB_FOT_EN
#define LE_AB_FOT_EN                0
#define BT_AB_FOT_EN                0
#endif

#if LE_DUEROS_DMA_EN
#undef LE_AB_FOT_EN
#undef BT_AB_FOT_EN
#define LE_AB_FOT_EN    0
#define BT_AB_FOT_EN    0
#endif

#if !FOT_EN
#undef LE_AB_FOT_EN
#undef BT_AB_FOT_EN
#define LE_AB_FOT_EN                0
#define BT_AB_FOT_EN                0
#endif

#if (FOT_EN && (AB_FOT_TYPE == AB_FOT_TYPE_PACK))
//限制FOTA flash分区必须按照4k对齐
#if ((AB_FOT_PACK_START_ADDR & 0xFFF) != 0 || (AB_FOT_PACK_SIZE & 0xFFF) != 0)
#error "FOTA: please align the AB_FOT_PACK_START_ADDR & AB_FOT_PACK_SIZE according to 4k\n"
#endif
#if (RES_USERBIN_EN && ((USERBIN_START_ADDR & 0xFFF) != 0 || (USERBIN_SIZE & 0xFFF) != 0))
#error "FOTA: please align the USERBIN_START_ADDR & USERBIN_SIZE according to 4k\n"
#endif
#endif

#if BT_AB_FOT_EN
#if !BT_SPP_EN
#error "FOTA: please open BT_AB_FOT_EN & BT_SPP_EN at the same time\n"
#endif
#endif // BT_AB_FOT_EN

#if BT_HID_VOL_CTRL_EN
#undef  BT_HID_EN
#undef  BT_A2DP_VOL_CTRL_EN

#define BT_HID_EN                    1
#define BT_A2DP_VOL_CTRL_EN          1
#endif // BT_HID_VOL_CTRL_EN

#if !EQ_APP_EN
#undef EQ_APP_NUM
#define EQ_APP_NUM                      0
#endif

#if BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN
#undef DAC_OUT_SPR
#define DAC_OUT_SPR                     DAC_OUT_96K
#define BT_A2DP_VENDOR_AUDIO_EN         1
#endif

/*****************************************************************************
 * Module    : 蓝牙音乐算法配置
 *****************************************************************************/
#if (!BT_MUSIC_EFFECT_EN) &&            \
    (BT_MUSIC_EFFECT_DBB_EN ||          \
     BT_MUSIC_EFFECT_SPATIAL_AU_EN ||   \
     BT_MUSIC_EFFECT_USER_EN ||         \
     BT_MUSIC_EFFECT_VBASS_EN ||        \
     BT_MUSIC_EFFECT_DYEQ_EN ||         \
     BT_MUSIC_EFFECT_XDRC_EN ||         \
     ABP_EN ||                          \
     BT_MUSIC_EFFECT_HRTF_RT_EN         \
     )
#error "EFFECT: please set the macro BT_MUSIC_EFFECT_EN!"
#endif

#if !BT_MUSIC_EFFECT_DBB_EN
#undef BT_MUSIC_EFFECT_DBB_BAND_CNT
#define BT_MUSIC_EFFECT_DBB_BAND_CNT    0
#endif

#if BT_MUSIC_EFFECT_DYEQ_VBASS_EN
#undef BT_MUSIC_EFFECT_VBASS_EN
#define BT_MUSIC_EFFECT_VBASS_EN        0
#endif

#if !BT_MUSIC_EFFECT_XDRC_EN
#undef BT_MUSIC_EFFECT_XDRC_EQ_EN
#undef BT_MUSIC_EFFECT_XDRC_DELAY_EN
#define BT_MUSIC_EFFECT_XDRC_EQ_EN      0
#define BT_MUSIC_EFFECT_XDRC_DELAY_EN   0
#endif

#if !ABP_EN
#undef ABP_MUSIC_DIS_PINK_EN
#undef ABP_PLAY_WAKE_EN
#undef ABP_PLAY_DIS_WAV_EN
#define ABP_MUSIC_DIS_PINK_EN           0
#define ABP_PLAY_WAKE_EN                0
#define ABP_PLAY_DIS_WAV_EN             0
#endif

#if BT_MUSIC_EFFECT_VBASS_EN || BT_MUSIC_EFFECT_DYEQ_EN || BT_MUSIC_EFFECT_XDRC_EN
#define BT_MUSIC_EFFECT_TWS_ALG_EN      1
#endif

#if BT_MUSIC_EFFECT_DBB_EN || BT_MUSIC_EFFECT_DYEQ_EN || BT_MUSIC_EFFECT_XDRC_EN
#define BT_MUSIC_EFFECT_SOFT_VOL_EN     1
#else
#define BT_MUSIC_EFFECT_SOFT_VOL_EN     0
#endif

#if BT_MUSIC_EFFECT_XDRC_EN || BT_MUSIC_EFFECT_DYEQ_EN
#undef BT_MUSIC_EFFECT_ABT_EN
#define BT_MUSIC_EFFECT_ABT_EN          1
#endif

#if !BT_MUSIC_EFFECT_SPATIAL_AU_EN
#undef BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN
#undef BT_MUSIC_EFFECT_SPATIAL_AU_L1
#undef BT_MUSIC_EFFECT_SPATIAL_AU_DLEN
#define BT_MUSIC_EFFECT_SPATIAL_AUEQ_EN 0
#define BT_MUSIC_EFFECT_SPATIAL_AU_L1   0
#define BT_MUSIC_EFFECT_SPATIAL_AU_DLEN 0
#endif

#if !BT_MUSIC_EFFECT_HRTF_RT_EN
#undef BT_MUSIC_EFFECT_HRTF_RT_RST_EN
#undef BT_MUSIC_EFFECT_HRTF_RT_EQ_EN
#define BT_MUSIC_EFFECT_HRTF_RT_RST_EN  0
#define BT_MUSIC_EFFECT_HRTF_RT_EQ_EN   0
#endif

#if !BT_TWS_EN && BT_MUSIC_EFFECT_TWS_ALG_EN
#error "EFFECT: music effect only support TWS mode now."
#endif

#if BT_MUSIC_EFFECT_SPATIAL_AU_EN && BT_MUSIC_EFFECT_HRTF_RT_EN
#error "EFFECT: please select only one spatial audio."
#endif

/*****************************************************************************
 * Module    : 通话算法相关配置
 *****************************************************************************/
#define BT_SCO_SMIC_EN                  0
#define BT_SCO_DMIC_EN                  0

#if (BT_SCO_NR_EN || BT_SNDP_SMIC_AI_EN || BT_SCO_SMIC_AI_EN || BT_SCO_SMIC_AI_PLUS_EN || BT_SCO_SMIC_AI_PRO_EN || BT_SCO_AIAEC_DNN_EN || BT_SCO_NR_USER_SMIC_EN)
#undef BT_SCO_SMIC_EN
#define BT_SCO_SMIC_EN                  1
#undef BT_AEC_FF_MIC_REF_EN
#define BT_AEC_FF_MIC_REF_EN            0
#endif

#if BT_SCO_DMIC_AIAEC_EN
#undef BT_AEC_EN
#undef BT_AEC_NLP_BYPASS_EN
#undef BT_AEC_FF_MIC_REF_EN
    #if BT_SCO_DMIC_AIAEC_NLMS_EN
    #define BT_AEC_EN                       1
    #else
    #define BT_AEC_EN                       0
    #endif
    #define BT_AEC_NLP_BYPASS_EN            1
    #define BT_AEC_FF_MIC_REF_EN            BT_SCO_DMIC_AIAEC_NLP_REF
#else
    #define BT_AEC_NLP_BYPASS_EN            0
#endif

#if (BT_SNDP_DMIC_EN || BT_SCO_DMIC_AI_EN || BT_SCO_LDMIC_AI_EN || BT_SCO_FFDM_AI_EN || BT_SCO_FFDM_AI_PRO_EN || BT_SCO_DMIC_AIAEC_EN || BT_SCO_NR_USER_DMIC_EN || BT_SNDP_FBDM_EN || BT_SNDP_DM_AI_EN)
#undef BT_SCO_DMIC_EN
#define BT_SCO_DMIC_EN                  1
#endif

#if !BT_SCO_DMIC_EN && !BT_SCO_SMIC_EN
#undef ENC_DBG_EN
#define ENC_DBG_EN                      0
#endif

#if BT_SCO_SMIC_EN && BT_SCO_DMIC_EN
#error "NR err: please choose only one of the NR algorithms at the same time!\n"
#endif

#if (BT_SCO_NR_EN + BT_SNDP_SMIC_AI_EN + BT_SCO_SMIC_AI_EN + BT_SCO_SMIC_AI_PLUS_EN + BT_SCO_SMIC_AI_PRO_EN + BT_SCO_FFDM_AI_EN + BT_SCO_FFDM_AI_PRO_EN + BT_SCO_AIAEC_DNN_EN + BT_SCO_DMIC_AIAEC_EN) > 1
#error "NR err: please select only one nr algorithm in BT_SCO_NR_EN, BT_SNDP_SMIC_AI_EN, BT_SCO_SMIC_AI_EN, BT_SCO_SMIC_AI_PLUS_EN, BT_SCO_FFDM_AI_EN, BT_SCO_FFDM_AI_PRO_EN, BT_SCO_SMIC_AI_PRO_EN\n"
#endif

#if (BT_SCO_DUMP_EN || BT_AEC_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_EQ_DUMP_EN) && ((CHARGE_BOX_TYPE == CBOX_SSW) || QTEST_EN)
#warning "DUMP warning: please select only one nr algorithm in BT_XXX_DUMP_EN, VUSB_XXX_EN\n"
#endif

#if (BT_SCO_DUMP_EN + BT_AEC_DUMP_EN + BT_SCO_FAR_DUMP_EN + BT_EQ_DUMP_EN) > 1
#error "DUMP err: please select only one dump in BT_AEC_DUMP_EN, BT_SCO_DUMP_EN, BT_SCO_FAR_DUMP_EN, BT_EQ_DUMP_EN\n"
#endif

#if !FUNC_USBDEV_EN
#undef USB_MIC_NR_EN
#define USB_MIC_NR_EN                   0
#endif

#if USB_MIC_NR_EN
#define SRC_EN                          1
#else
#define SRC_EN                          0
#endif
/*****************************************************************************
 * Module    : ANC相关配置
 *****************************************************************************/
#if ANC_EN
#undef SYS_ADJ_DIGVOL_EN
#undef TINY_TRANSPARENCY_EN

#define SYS_ADJ_DIGVOL_EN           1       //DAC固定模拟增益调数字增益
#define TINY_TRANSPARENCY_EN        0       //开ANC不能打开小通透

#if !(BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN)
#undef DAC_OUT_SPR
#define DAC_OUT_SPR                 DAC_OUT_48K
#endif
#endif

#if TINY_TRANSPARENCY_EN
#undef SYS_ADJ_DIGVOL_EN

#define SYS_ADJ_DIGVOL_EN           1       //DAC固定模拟增益调数字增益
#endif

#if !ANC_EN
#undef ANC_EQ_RES2_EN
#undef ANC_MAX_VOL_DIS_FB_EN
#define ANC_EQ_RES2_EN              0        //如果ANC关闭的情况下，不能开启优先使用资源2
#define ANC_MAX_VOL_DIS_FB_EN       0
#endif

#if ANC_EQ_RES2_EN
#undef FLASH_RESERVE_SIZE
#define FLASH_RESERVE_SIZE          0x6000
#endif

#if ANC_ALG_EN
#if ANC_ALG_HOWLING_FB_EN ||            \
    ANC_ALG_FIT_DETECT_FF_FB_EN ||      \
    ANC_ALG_HOWLING_FF_EN ||            \
    ANC_ALG_WIND_NOISE_FF_FB_EN ||      \
    ANC_ALG_LIMITER_FF_EN ||            \
    ANC_ALG_DUMP_EN ||                  \
    ANC_ALG_MSC_ADP_FB_EN ||            \
    ANC_ALG_ADP_EQ_FF_FB_EN ||          \
    ANC_ALG_AEM_RT_FF_FB_EN
#undef ANC_DS_DMA_EN
#define ANC_DS_DMA_EN                   1
#endif
#if ANC_ALG_AI_WN_FF_EN
#define ANC_ALG_AI_NPU_EN               1
#else
#define ANC_ALG_AI_NPU_EN               0
#endif
#if ANC_ALG_STEREO_EN
#undef SDADC_5CH_EN
#define SDADC_5CH_EN                    1
#endif // ANC_ALG_STEREO_EN
#else
#undef ANC_ALG_WIND_NOISE_FF_TALK_EN
#undef ANC_ALG_WIND_NOISE_FF_FB_EN
#undef ANC_ALG_ASM_SIM_FF_EN
#undef ANC_ALG_HOWLING_FB_EN
#undef ANC_ALG_FIT_DETECT_FF_FB_EN
#undef ANC_ALG_HOWLING_FF_EN
#undef ANC_ALG_AI_WN_FF_EN
#undef ANC_ALG_AI_WN_DSP_FF_EN
#undef ANC_ALG_AI_WN_MINI_FF_EN
#undef ANC_ALG_LIMITER_FF_EN
#undef ANC_ALG_DYVOL_FF_EN
#undef ANC_ALG_MSC_ADP_FB_EN
#undef ANC_ALG_ADP_EQ_FF_FB_EN
#undef ANC_ALG_ASM_FF_EN
#undef ANC_ALG_AEM_RT_FF_FB_EN
#undef ANC_SNDP_SAE_SHIELD_ADAPTER_EN
#undef ANC_SNDP_SAE_WIND_DETECT_EN
#undef ANC_ALG_USER_EN
#undef ANC_ALG_DUMP_EN
#undef ANC_ALG_DBG_EN
#define ANC_ALG_WIND_NOISE_FF_TALK_EN   0
#define ANC_ALG_WIND_NOISE_FF_FB_EN     0
#define ANC_ALG_ASM_SIM_FF_EN           0
#define ANC_ALG_HOWLING_FB_EN           0
#define ANC_ALG_FIT_DETECT_FF_FB_EN     0
#define ANC_ALG_HOWLING_FF_EN           0
#define ANC_ALG_AI_WN_FF_EN				0
#define ANC_ALG_AI_WN_DSP_FF_EN         0
#define ANC_ALG_AI_WN_MINI_FF_EN        0
#define ANC_ALG_LIMITER_FF_EN           0
#define ANC_ALG_DYVOL_FF_EN             0
#define ANC_ALG_MSC_ADP_FB_EN           0
#define ANC_ALG_ADP_EQ_FF_FB_EN         0
#define ANC_ALG_ASM_FF_EN               0
#define ANC_ALG_AEM_RT_FF_FB_EN         0
#define ANC_SNDP_SAE_SHIELD_ADAPTER_EN  0
#define ANC_SNDP_SAE_WIND_DETECT_EN     0
#define ANC_ALG_USER_EN                 0
#define ANC_ALG_DUMP_EN                 0
#define ANC_ALG_DBG_EN                  0
#endif // ANC_ALG_EN

#if ANC_ALG_ADP_EQ_FF_FB_EN
#undef DAC_EQ_AFTER_VOL_EN
#define DAC_EQ_AFTER_VOL_EN             1
#define ANC_ALG_ADP_EQ_NUM              1
#else
#define ANC_ALG_ADP_EQ_NUM              0
#endif

#if ANC_ALG_HOWLING_FB_EN && ANC_ALG_HOWLING_FF_EN
#error "ANC ALG: please select only one howling algorithm in ANC_ALG_HOWLING_FB_EN and ANC_ALG_HOWLING_FF_EN"
#endif

#if (ANC_ALG_WIND_NOISE_FF_TALK_EN + ANC_ALG_AI_WN_FF_EN + ANC_ALG_WIND_NOISE_FF_FB_EN + ANC_ALG_AI_WN_DSP_FF_EN + ANC_ALG_AI_WN_MINI_FF_EN) > 1
#error "ANC ALG: please select only one wind noise detect algorithm"
#endif

#if !ANC_ALG_DUMP_EN && ANC_ALG_DUMP_FOR_ANC_MODE
#error "ANC ALG: please set the macro ANC_ALG_DUMP_EN"
#endif

#if ANC_TOOL_EN

#undef ANC_DS_DMA_EN
#define ANC_DS_DMA_EN                   1

#if !ANC_EN
#error "ANC TOOL: please set the macro ANC_EN"
#endif

#if ANC_TOOL_SPP_EN && (!BT_SPP_EN)
#error "ANC TOOL: please set the macro BT_SPP_EN"
#endif

#else
#undef ANC_TOOL_SPP_EN
#undef ANC_TOOL_LINK_FB_MSC_SIMU_EN
#define ANC_TOOL_SPP_EN                 1
#define ANC_TOOL_LINK_FB_MSC_SIMU_EN    1
#endif // ANC_TOOL_EN

/*****************************************************************************
 * Module    : uart0 printf 功能自动配置(自动关闭SD卡，USB)
 *****************************************************************************/
#if (UART0_PRINTF_SEL == PRINTF_PB2)
    #if (SD0_MAPPING == SD0MAP_G2)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PA7)
    #if (SD0_MAPPING == SD0MAP_G1)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    #undef  FUNC_USBDEV_EN
    #undef  MUSIC_UDISK_EN
    #define FUNC_USBDEV_EN          0
    #define MUSIC_UDISK_EN          0
#elif (UART0_PRINTF_SEL == PRINTF_PE13)
    #if (SD0_MAPPING == SD0MAP_G4)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_NONE)
    //关闭所以打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif

#if !MUSIC_SDCARD_EN
#undef SD_DETECT_INIT
#undef SD_IS_ONLINE
#undef SD_DETECT_IS_BUSY

#define SD_DETECT_INIT()
#define SD_IS_ONLINE()              0
#define SD_DETECT_IS_BUSY()            0
#endif


/*****************************************************************************
 * Module    : usb device 功能配置
 *****************************************************************************/
#if !FUNC_USBDEV_EN
    #undef  UDE_STORAGE_EN
    #undef  UDE_SPEAKER_EN
    #undef  UDE_HID_EN
    #undef  UDE_MIC_EN
    #undef  UDE_ENUM_TYPE

    #define UDE_STORAGE_EN              0
    #define UDE_SPEAKER_EN              0
    #define UDE_HID_EN                  0
    #define UDE_MIC_EN                  0
    #define UDE_ENUM_TYPE               0
#else
    #define UDE_ENUM_TYPE               (UDE_STORAGE_EN*0x01 + UDE_SPEAKER_EN*0x02 + UDE_HID_EN*0x04 + UDE_MIC_EN*0x08)
#endif

/*****************************************************************************
 * Module    : iis 功能配置
 *****************************************************************************/
#if I2S_EN

#if I2S_MODE_SEL
#undef  I2S_DMA_EN
#define I2S_DMA_EN                      1       //slave模式必须打开DMA功能
#endif

#endif // I2S_EN

#if !IIS_EN
#undef  IIS_RX2SCO_EN
#define IIS_RX2SCO_EN                   0
#endif
/*****************************************************************************
 * Module    : 提示音配置
 *****************************************************************************/
#if (!WARNING_TONE_EN)
#undef WARNING_POWER_ON
#undef WARNING_POWER_OFF
#undef WARNING_FUNC_MUSIC
#undef WARNING_FUNC_BT
#undef WARNING_FUNC_CLOCK
#undef WARNING_FUNC_FMRX
#undef WARNING_FUNC_AUX
#undef WARNING_FUNC_USBDEV
#undef WARNING_FUNC_SPEAKER
#undef WARNING_LOW_BATTERY
#undef WARNING_BT_CONNECT
#undef WARNING_BT_DISCONNECT
#undef WARNING_BT_INCALL
#undef WARNING_USB_SD
#undef WARNING_BT_HID_MENU
#undef WARNING_BTHID_CONN
#undef WARNING_MAX_VOLUME
#undef WARNING_BT_PAIR

#define WARNING_POWER_ON               0
#define WARNING_POWER_OFF              0
#define WARNING_FUNC_MUSIC             0
#define WARNING_FUNC_BT                0
#define WARNING_FUNC_CLOCK             0
#define WARNING_FUNC_FMRX              0
#define WARNING_FUNC_AUX               0
#define WARNING_FUNC_USBDEV            0
#define WARNING_FUNC_SPEAKER           0
#define WARNING_LOW_BATTERY            0
#define WARNING_BT_CONNECT             0
#define WARNING_BT_DISCONNECT          0
#define WARNING_BT_INCALL              0
#define WARNING_USB_SD                 0
#define WARNING_BT_HID_MENU            0
#define WARNING_BTHID_CONN             0
#define WARNING_MAX_VOLUME             0
#define WARNING_BT_PAIR                0
#endif

#if ((!MUSIC_UDISK_EN) && (!MUSIC_SDCARD_EN))
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               0
#endif

#if UPD_UART_EN
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               1
#endif

#if SD_USB_MUX_IO_EN == 1
	#undef FUNC_USBDEV_EN
	#define FUNC_USBDEV_EN 			   0
#endif

#ifndef UPD_FILENAME
#define UPD_FILENAME                   "fw5000.upd"
#endif

#if ((SD0_MAPPING == SD0MAP_G1) || (SD0_MAPPING == SD0MAP_G4) || (SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G6))
#define SDCLK_AD_CH                    ADCCH_PA6
#elif (SD0_MAPPING == SD0MAP_G2)
#define SDCLK_AD_CH                    ADCCH_PB1
#else
#define SDCLK_AD_CH                    ADCCH_PE6
#endif
#define USB_SUPPORT_EN                 (MUSIC_UDISK_EN | FUNC_USBDEV_EN)
#define SD_SUPPORT_EN                  (MUSIC_SDCARD_EN)

#if MUSIC_SDCARD_EN
#define SDCMD_MUX_DETECT_EN            1
#define SDCLK_MUX_DETECT_EN            1
#else
#undef  USER_ADKEY_MUX_SDCLK
#undef  I2C_MUX_SD_EN

#define I2C_MUX_SD_EN                  0
#define SDCMD_MUX_DETECT_EN            0
#define SDCLK_MUX_DETECT_EN            0
#define USER_ADKEY_MUX_SDCLK           0
#endif // MUSIC_SDCARD_EN

#if !SD_SUPPORT_EN
#undef  UDE_STORAGE_EN
#define UDE_STORAGE_EN                 0
#undef  SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN              0
#endif


/*****************************************************************************
* Module    : 有冲突或功能上不能同时打开的宏
*****************************************************************************/
#if (SYS_KARAOK_EN && BT_BACKSTAGE_EN)
#error "SYS_KARAOK_EN and BT_BACKSTAGE_EN (include BT_APP_EN) can't open at sametime"
#endif

#define DAC_EQ_NUM_TOTAL               (DAC_PT_NUM + EQ_APP_NUM + BT_MUSIC_EFFECT_DBB_BAND_CNT + ANC_ALG_ADP_EQ_NUM)

#if (DAC_EQ_NUM_TOTAL > 20)
#error "error: (DAC_PT_NUM + EQ_APP_NUM + BT_MUSIC_EFFECT_DBB_BAND_CNT) > 20"
#endif

#if ANC_ALG_AI_NPU_EN && (BT_MUSIC_EFFECT_SPATIAL_AU_EN || BT_MUSIC_EFFECT_USER_EN || BT_MUSIC_EFFECT_VBASS_EN ||BT_MUSIC_EFFECT_DYEQ_EN || BT_MUSIC_EFFECT_XDRC_EN)
#error "Algorithm RAM exist conflict. Please close ANC AI algorithm or music effect algorithm."
#endif

/*****************************************************************************
* Module    : 计算FunctionKey
*****************************************************************************/
#if BT_TWS_EN
    #define FUNCKEY_TWS                 0xe0938553
#else
    #define FUNCKEY_TWS                 0
#endif
#if BT_SNDP_SMIC_AI_EN || BT_SNDP_DM_AI_EN || BT_SNDP_FBDM_EN
    #define FUNCKEY_SNDP                0xf3c19711
#else
    #define FUNCKEY_SNDP                0
#endif
#if BT_A2DP_LHDC_AUDIO_EN || BT_A2DP_LDAC_AUDIO_EN
    #define FUNCKEY_LHDC                0xf8789d28
#else
    #define FUNCKEY_LHDC                0
#endif
#endif // __CONFIG_EXTRA_H__

