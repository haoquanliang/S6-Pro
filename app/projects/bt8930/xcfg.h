//该头文件由软件自动生成，请勿随意修改！
#ifndef _XCFG_H
#define _XCFG_H

#define XCFG_EN             1

typedef struct __attribute__((packed)) _xcfg_cb_t {
    u8 lang_id;                                 //提示音语言选择: 0:英文, 1:中文, 2:中英文(出厂默认英文), 3:中英文(出厂默认中文)
    u32 func_music_en                    : 1;   //SD/U盘播放功能
    u32 func_aux_en                      : 1;   //AUX输入功能
    u32 powkey_10s_reset                 : 1;   //POWKEY 10s复位系统
    u32 spk_mute_en                      : 1;   //功放MUTE功能
    u32 spk_mute_io_sel                  : 5;   //功放MUTE控制IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5/WKO: 14, PE0: 17, PE1: 18, PE2: 19, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26, PF2: 27, PF3: 28
    u32 high_mute                        : 1;   //高MUTE
    u8 loudspeaker_unmute_delay;                //功放MUTE延时(单位5ms)
    u8 ampabd_type;                             //功放AB/D控制模式: 0:独立IO电平控制, 1:mute脉冲控制
    u32 ampabd_io_sel                    : 5;   //功放AB/D控制IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5/WKO: 14, PE0: 17, PE1: 18, PE2: 19, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26, PF2: 27, PF3: 28
    u32 earphone_det_iosel               : 6;   //耳机检测IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5/WKO: 14, PE0: 17, PE1: 18, PE2: 19, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26, PF2: 27, PF3: 28, 复用SDCLK检测: 60, 复用SDCMD检测: 61, 复用PWRKEY检测: 62
    u16 sys_sleep_time;                         //自动休眠时间: 不休眠: 0, 10秒钟后: 10, 20秒钟后: 20, 30秒钟后: 30, 45秒钟后: 45, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u16 sys_off_time;                           //自动关机时间: 不关机: 0, 30秒钟后: 30, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u32 lowpower_warning_en              : 1;   //低电提示
    u8 lpwr_warning_vbat;                       //低电语音提示电压: 0:2.8V, 1:2.9V, 2:3.0V, 3:3.1V, 4:3.2V, 5:3.3V, 6:3.4V, 7:3.5V, 8:3.6V, 9:3.7V
    u8 lpwr_off_vbat;                           //低电关机电压: 0:不关机, 1:2.8V, 2:2.9V, 3:3.0V, 4:3.1V, 5:3.2V, 6:3.3V, 7:3.4V, 8:3.5V, 9:3.6V, 10:3.7V
    u8 lpwr_warning_period;                     //低电语音播报周期(秒)
    u8 osc_both_cap;                            //OSC基础电容: 0:0PF, 1:6PF
    u8 uosci_cap;                               //自定义OSCI电容
    u8 uosco_cap;                               //自定义OSCO电容
    u32 ft_osc_cap_en                    : 1;   //优先使用产测电容值
    u32 pwron_press_time                 : 3;   //软开机长按时间选择: 0.1秒: 0, 0.5秒: 1, 1秒: 2, 1.5秒: 3, 2秒: 4, 2.5秒: 5, 3秒: 6, 3.5秒: 7
    u32 pwroff_press_time                : 3;   //软关机长按时间选择: 1.5秒: 0, 2秒: 1, 2.5秒: 2, 3秒: 3, 3.5秒: 4, 4秒: 5, 4.5秒: 6, 5秒: 7
    u32 eq_dgb_spp_en                    : 1;   //EQ调试（蓝牙串口）
    u32 huart_en                         : 1;   //HUART调试(EQ/FCC)
    u32 huart_sel                        : 4;   //HUART串口选择: PA7: 0, PB2: 1, PB3: 2, PE7: 3, PF0: 4, PA6: 5, PB1: 6, PB4: 7, PE6: 8, PF1: 9, VUSB: 10
    u32 uart1_en                         : 1;   //UART1调试(仓/IODM)
    u32 uart1_sel                        : 2;   //UART1串口选择: PA7: 0, PF0: 1, VUSB: 2
    u32 uart2_en                         : 1;   //UART2调试(仓/IODM)
    u32 uart2_sel                        : 2;   //UART2串口选择: PE7: 0, PB2: 1, VUSB: 2
    u32 ntc_en_bt                        : 1;   //是否使能蓝汛默认NTC
    u32 ntc_user_inner_pu                : 1;   //NTC使用内部10K上拉
    u8 ntc_thd_val;                             //NTC阈值
    u32 dac_sel                          : 4;   //DAC声道选择: VCMBUF单声道: 2, VCMBUF双声道: 3, 差分单声道: 4, 差分双声道: 5
    u32 dac_spr_sel                      : 2;   //DAC输出采样率: 44.1KHz: 0, 48KHz: 1
    u32 dac_24bits_en                    : 1;   //DAC使能24bit
    u32 dac_maxout_en                    : 1;   //DAC扩展模式
    u32 dac_vcm_less_en                  : 1;   //省VCM方案
    u32 dac_off_for_conn                 : 1;   //蓝牙连接时关闭DAC
    u8 dac_max_gain;                            //音乐衰减增益: 0:0DB, 1:-1DB, 2:-2DB, 3:-3DB, 4:-4DB, 5:-5DB, 6:-6DB, 7:-7DB, 8:-8DB, 9:-9DB
    u8 bt_call_max_gain;                        //通话衰减增益: 0:0DB, 1:-1DB, 2:-2DB, 3:-3DB, 4:-4DB, 5:-5DB, 6:-6DB, 7:-7DB, 8:-8DB, 9:-9DB
    u8 vol_max;                                 //音量级数: 0:16级音量, 1:32级音量
    u8 sys_init_vol;                            //开机默认音量
    u8 sys_recover_initvol;                     //恢复默认音量阈值
    u8 warning_volume;                          //提示音播放音量
    u32 buck_mode_en                     : 1;   //BUCK MODE
    u32 vddcore_buck_en                  : 1;   //双BUCK模式
    u32 vddbt_capless_en                 : 1;   //VDDBT省电容
    u32 vddio_sel                        : 4;   //VDDIO电压: None: 0, 2.9V: 5, 3.0V: 6, 3.1V: 7, 3.2V: 8, 3.3V: 9, 3.4V: 10, 3.5V: 11, 3.6V: 12
    u32 charge_en                        : 1;   //充电使能
    u32 charge_trick_en                  : 1;   //涓流充电使能
    u32 charge_dc_not_pwron              : 1;   //插入DC禁止软开机
    u32 charge_voltage_follow            : 1;   //充电电压跟随模式
    u8 charge_stop_curr;                        //充电截止电流: 0:0mA, 1:2.5mA, 2:5mA, 3:7.5mA, 4:10mA, 5:12.5mA, 6:15mA, 7:17.5mA, 8:20mA, 9:22.5mA, 10:25mA, 11:27.5mA, 12:30mA, 13:32.5mA, 14:35mA, 15:37.5mA
    u8 charge_constant_curr;                    //恒流充电电流: 0:5mA, 1:10mA, 2:15mA, 3:20mA, 4:25mA, 5:30mA, 6:35mA, 7:40mA, 8:45mA, 9:50mA, 10:55mA, 11:60mA, 12:65mA, 13:70mA, 14:75mA, 15:80mA, 16:90mA, 17:100mA, 18:110mA, 19:120mA, 20:130mA, 21:140mA, 22:150mA, 23:160mA, 24:170mA, 25:180mA, 26:190mA, 27:200mA, 28:210mA, 29:220mA, 30:230mA, 31:240mA, 32:250mA
    u8 charge_trickle_curr;                     //涓流充电电流: 0:5mA, 1:10mA, 2:15mA, 3:20mA, 4:25mA, 5:30mA, 6:35mA, 7:40mA
    u32 charge_full_bled                 : 3;   //充满电蓝灯亮: 不亮蓝灯: 0, 亮10秒: 1, 亮20秒: 2, 亮30秒: 3, 1分钟: 4, 2分钟: 5, 3分钟: 6, 常亮: 7
    u32 ch_full_auto_pwrdwn_en           : 1;   //充满电自动关机
    u32 chbox_en                         : 1;   //充电仓功能
    u32 ch_box_type_sel                  : 2;   //充电仓类型选择: 兼容5V短暂掉0V后维持电压: 0, 5V不掉电的充电仓: 1, 5V掉电但有维持电压: 2, 5V完全掉电无维持电压: 3
    u32 ch_inbox_sel                     : 1;   //仓允许最低维持电压: 1.1V: 0, 1.7V: 1
    u32 ch_out_auto_pwron_en             : 1;   //从充电仓拿出自动开机
    u32 ch_leakage_sel                   : 2;   //出仓或入仓VUSB漏电配置: None: 0, 漏电电流X1档: 1, 漏电电流X2档: 2, 漏电电流X3档: 3
    u32 chg_inbox_pwrdwn_en              : 1;   //入仓耳机关机
    u32 chbox_out_delay                  : 4;   //短暂掉0V的仓稳定检测时间
    char bt_name[32];                           //蓝牙名称
    u8 bt_addr[6];                              //蓝牙地址
    u32 bt_pwrkey_nsec_discover          : 3;   //长按几秒开机进配对: 不支持: 0, 2秒: 1, 3秒: 2, 4秒: 3, 5秒: 4, 6秒: 5, 7秒: 6, 8秒: 7
    u32 warning_bt_pair                  : 1;   //配对提示音
    u32 bt_2acl_en                       : 1;   //连接两部手机功能
    u32 bt_a2dp_en                       : 1;   //音乐播放功能
    u32 bt_a2dp_vol_ctrl_en              : 1;   //音乐音量同步
    u32 bt_sco_en                        : 1;   //通话功能
    u32 bt_hfp_private_en                : 1;   //私密接听功能
    u32 bt_hfp_ring_number_en            : 1;   //来电报号功能
    u32 bt_spp_en                        : 1;   //串口功能
    u32 bt_hid_en                        : 1;   //拍照功能
    u32 bt_hid_manu_en                   : 1;   //按键HID连接/断开功能
    u32 bt_hid_discon_default_en         : 1;   //HID默认不连接
    u32 func_bthid_en                    : 1;   //HID独立自拍器模式
    char bthid_name[32];                        //HID蓝牙名称
    u32 ble_en                           : 1;   //BLE控制功能
    char le_name[32];                           //BLE名称
    u32 qtest_en                         : 1;   //快速测试使能
    u8 qtest_pickup_status;                     //耳机拿起后状态: 0:不操作, 1:拿起关机, 2:拿起复位, 3:断开蓝牙连接后关机
    u32 bt_tws_en                        : 1;   //TWS功能
    u32 bt_tws_pair_bonding_en           : 1;   //TWS组队绑定功能
    u32 bt_tws_pair_mode                 : 3;   //TWS组队方式选择: 自动组队: 0, 自定义组队<调用api>: 1, 双击USER_DEF键组队: 2, 单击USER_DEF键组队: 3, 长按USER_DEF键组队: 4, 双击MODE/POWER键组队: 5
    u32 bt_tws_lr_mode                   : 4;   //TWS声道分配选择: 不分配，主副均双声道输出: 0, 自动分配，主右声道副左声道: 1, 自动分配，主左声道副右声道: 2, PWRKEY,有820K接地为左: 8, GPIOx有接地为左声道: 9, 配置选择为左声道: 10, 配置选择为右声道: 11
    u32 tws_sel_left_gpio_sel            : 5;   //TWS声道GPIOx: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 bt_tws_name_suffix_en            : 1;   //TWS蓝牙名加L或R后缀
    u32 bt_tswi_en                       : 1;   //TWS主从切换功能
    u32 bt_tswi_kpwr_en                  : 1;   //TWS按键关机支持主从切换
    u32 bt_tswi_msc_en                   : 1;   //TWS音乐模式支持主从切换
    u32 bt_tswi_sco_en                   : 1;   //TWS通话模式支持主从切换
    u32 bt_tswi_charge_rst_en            : 1;   //TWS耳机入仓充电复位
    u32 bt_outbox_voice_pwron_en         : 1;   //TWS耳机出仓播放开机音
    u8 bt_rf_pwrdec;                            //降低预置RF参数发射功率
    u32 ft_rf_param_en                   : 1;   //优先使用FT的RF参数
    u32 bt_rf_param_en                   : 1;   //自定义RF参数
    u8 rf_pa_gain;                              //GL_PA_GAIN
    u8 rf_mix_gain;                             //GL_MIX_GAIN
    u8 rf_dig_gain;                             //GL_DIG_GAIN
    u8 rf_pa_cap;                               //GL_PA_CAP
    u8 rf_mix_cap;                              //GL_MIX_CAP
    u8 rf_txdbm;                                //GL_TX_DBM
    u8 rf_udf0;                                 //GL_UDF0
    u8 rf_udf1;                                 //GL_UDF1
    u32 bt_rf_page_pwrdec                : 3;   //降低回连功率: 不降低: 0, 降低3dbm: 1, 降低6dbm: 2, 降低9dbm: 3
    u32 ble_rf_page_pwrdec               : 3;   //降低组队功率: 不降低: 0, 降低3dbm: 1, 降低6dbm: 2, 降低9dbm: 3
    u8 ble_page_rssi_thr;                       //设置组队范围RSSI(-dbm)
    u8 ble_delta_value;                         //BLE_value
    u32 bt_rf_param_sco_en               : 1;   //通话RF参数
    u8 rf_pa_gain_sco;                          //SCO_PA_GAIN
    u8 rf_mix_gain_sco;                         //SCO_MIX_GAIN
    u8 rf_dig_gain_sco;                         //SCO_DIG_GAIN
    u8 rf_pa_cap_sco;                           //SCO_PA_CAP
    u8 rf_mix_cap_sco;                          //SCO_MIX_CAP
    u8 rf_txdbm_sco;                            //SCO_TX_DBM
    u8 rf_udf0_sco;                             //SCO_UDF0
    u8 rf_udf1_sco;                             //SCO_UDF1
    u32 anc_en                           : 1;   //ANC功能
    u32 anc_mode                         : 4;   //ANC模式: TWS FF/FB Mode: 0, FF/FB Mode: 1, TWS Hybrid Mode: 8, Hybrid Mode: 9
    u32 anc_gain_en                      : 1;   //MIC Gain产测配置
    u32 bt_mmic_cfg                      : 3;   //通话主MIC配置: None: 5, MIC0: 0, MIC1: 1, MIC2: 2, MIC3: 3, MIC4: 4
    u32 bt_smic_cfg                      : 3;   //通话副MIC配置: None: 5, MIC0: 0, MIC1: 1, MIC2: 2, MIC3: 3, MIC4: 4
    u32 micr_phase_en                    : 1;   //MICR反相
    u32 bt_sco_post_gain                 : 4;   //通话后置数字增益
    u32 mic_pwr_level                    : 3;   //MIC供电IO电压选择: 1.8v: 0, 2.4v: 1, 2.5v: 2, 2.6v: 3, 2.7v: 4, 2.8v: 5, 2.9v: 6, 3.0v: 7
    u32 mic0_en                          : 1;   //MIC0
    u32 mic0_pwr_sel                     : 2;   //供电IO选择: None: 0, PF0: 1, PF1: 2, PE7: 3
    u32 mic0_bias_method                 : 1;   //MIC0偏置电路配置: 单端MIC外部电阻电容: 0, 差分MIC: 1
    u8 mic0_anl_gain;                           //MIC0模拟增益
    u8 bt_mic0_dig_gain;                        //MIC0通话数字增益
    u32 mic1_en                          : 1;   //MIC1
    u32 mic1_pwr_sel                     : 2;   //供电IO选择: None: 0, PF0: 1, PF1: 2, PE7: 3
    u32 mic1_bias_method                 : 1;   //MIC1偏置电路配置: 单端MIC外部电阻电容: 0, 差分MIC: 1
    u8 mic1_anl_gain;                           //MIC1模拟增益
    u8 bt_mic1_dig_gain;                        //MIC1通话数字增益
    u32 mic2_en                          : 1;   //MIC2
    u32 mic2_pwr_sel                     : 2;   //供电IO选择: None: 0, PF0: 1, PF1: 2, PE7: 3
    u32 mic2_bias_method                 : 1;   //MIC2偏置电路配置: 单端MIC外部电阻电容: 0, 差分MIC: 1
    u8 mic2_anl_gain;                           //MIC2模拟增益
    u8 bt_mic2_dig_gain;                        //MIC2通话数字增益
    u32 mic3_en                          : 1;   //MIC3
    u32 mic3_pwr_sel                     : 2;   //供电IO选择: None: 0, PF0: 1, PF1: 2, PE7: 3
    u32 mic3_bias_method                 : 1;   //MIC3偏置电路配置: 单端MIC外部电阻电容: 0, 差分MIC: 1
    u8 mic3_anl_gain;                           //MIC3模拟增益
    u8 bt_mic3_dig_gain;                        //MIC3通话数字增益
    u32 mic4_en                          : 1;   //MIC4
    u32 mic4_pwr_sel                     : 2;   //供电IO选择: None: 0, PF0: 1, PF1: 2, PE7: 3
    u32 mic4_bias_method                 : 1;   //MIC4偏置电路配置: 单端MIC外部电阻电容: 0, 差分MIC: 1
    u8 mic4_anl_gain;                           //MIC4模拟增益
    u8 bt_mic4_dig_gain;                        //MIC4通话数字增益
    u32 anc_alg_dgain_en                 : 1;   //ANC算法独立MIC数字增益
    u8 anc_mic0_dig_gain;                       //ANC算法MIC0数字增益
    u8 anc_mic1_dig_gain;                       //ANC算法MIC1数字增益
    u8 anc_mic2_dig_gain;                       //ANC算法MIC2数字增益
    u8 anc_mic3_dig_gain;                       //ANC算法MIC3数字增益
    u8 anc_mic4_dig_gain;                       //ANC算法MIC4数字增益
    u32 bt_aec_en                        : 1;   //AEC功能
    u8 bt_echo_level;                           //AEC回声消除级别
    u32 aec_dig_gain                     : 5;   //AEC数字增益
    u32 aec_far_offset                   : 16;  //远端补偿
    u32 bt_nr_en                         : 1;   //NR降噪功能
    u32 bt_sndp_smic_ai_en               : 1;   //声加单麦AI降噪
    u8 bt_sndp_level;                           //稳态噪声降噪量
    u8 bt_sndp_dnn_level;                       //DNN降噪量
    u32 bt_sndp_dm_ai_en                 : 1;   //声加双麦AI降噪
    u8 sndp_dm_level;                           //稳态噪声降噪量
    u8 sndp_dm_dnn_level;                       //DNN降噪量
    u8 sndp_dm_distance;                        //双麦间距
    u8 sndp_dm_degree;                          //拾音角度(单位:°)
    u8 sndp_dm_degree1;                         //抑制角度(单位:°)
    u32 bt_sndp_fbdm_en                  : 1;   //声加双麦1+1降噪
    u8 sndp_fbdm_level;                         //稳态噪声降噪量
    u8 sndp_fbdm_dnn_level;                     //DNN降噪量
    u8 sndp_fbdm_noiserms_th0;                  //混音阈值
    u32 bt_dnn_en                        : 1;   //自研单麦AI降噪
    u8 bt_dnn_level;                            //通话降噪量
    u32 bt_ffdm_en                       : 1;   //自研耳夹1+1降噪
    u8 bt_ffdm_distance;                        //耳夹1+1间距
    u32 bt_dmns_en                       : 1;   //自研双麦/长麦AI降噪
    u32 bt_dmns_distance                 : 16;  //双麦间距
    u8 bt_dmns_level;                           //通话降噪量
    u32 music_sdcard_en                  : 1;   //SDCARD播放功能
    u32 music_udisk_en                   : 1;   //UDISK播放功能
    u32 sddet_iosel                      : 6;   //SDCARD检测IO选择: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5/WKO: 14, PE0: 17, PE1: 18, PE2: 19, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26, 复用SDCLK检测: 60, 复用SDCMD检测: 61, 复用PWRKEY检测: 62, 软件检测: 63
    u32 sd1det_iosel                     : 6;   //SDCARD1检测IO选择: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5/WKO: 14, PE0: 17, PE1: 18, PE2: 19, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26, PF2: 27, PF3: 28, 复用SDCLK检测: 60, 复用SDCMD检测: 61, 复用PWRKEY检测: 62
    u32 linein_2_pwrdown_en              : 1;   //插入LINEIN关机
    u32 mode_2_aux_en                    : 1;   //模式切换进AUX模式
    u32 aux_anl_gain                     : 4;   //AUX模拟增益: -12DB: 0, -9DB: 1, -6DB: 2, -3DB: 3, 0DB: 4, +6DB: 5, +9DB: 6, +12DB: 7
    u32 aux_dig_gain                     : 6;   //AUX数字增益(0~39DB)
    u32 aux_comb_en                      : 1;   //合并通路输入音频
    u32 aux_comb_sel                     : 2;   //aux io组合选择: None: 0, PE6\PE7: 1, PA6\PA7: 2
    u32 aux_diff_en                      : 1;   //独立通路输入音频
    u32 auxl_sel                         : 3;   //AUXL通路选择: 无AUXL输入: 0, AUXL0_PE6: 1, AUXL1_PA6: 2
    u32 auxr_sel                         : 3;   //AUXR通路选择: 无AUXR输入: 0, AUXR0_PE7: 1, AUXR1_PA7: 2
    u32 linein_det_iosel                 : 6;   //LINEIN插入检测配置: None: 0, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5/WKO: 14, PE0: 17, PE1: 18, PE2: 19, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26, PF2: 27, PF3: 28, 复用SDCLK检测: 60, 复用SDCMD检测: 61, 复用PWRKEY检测: 62
    u32 user_pwrkey_en                   : 1;   //PWRKEY功能
    u32 user_adkey_en                    : 1;   //ADKEY1功能
    u32 user_adkey2_en                   : 1;   //ADKEY2功能
    u32 user_iokey_en                    : 1;   //IOKEY功能
    u32 user_adkey_mux_sdclk_en          : 1;   //复用SDCLK的ADKEY
    u32 adkey_ch                         : 4;   //ADKEY1通路选择: ADCCH_PA5: 0, ADCCH_PA6: 1, ADCCH_PA7: 2, ADCCH_PB1: 3, ADCCH_PB2: 4, ADCCH_PB3: 5, ADCCH_PB4: 6, ADCCH_PE5: 7, ADCCH_PE6: 8, ADCCH_PE7: 9, ADCCH_PF0: 10, ADCCH_PB0: 11
    u32 adkey2_ch                        : 4;   //ADKEY2通路选择: ADCCH_PA5: 0, ADCCH_PA6: 1, ADCCH_PA7: 2, ADCCH_PB1: 3, ADCCH_PB2: 4, ADCCH_PB3: 5, ADCCH_PB4: 6, ADCCH_PE5: 7, ADCCH_PE6: 8, ADCCH_PE7: 9, ADCCH_PF0: 10, ADCCH_PB0: 11
    u32 user_key_multi_press_en          : 1;   //按键多击功能
    u32 double_key_time                  : 3;   //双击响应时间选择: 200ms: 0, 300ms: 1, 400ms: 2, 500ms: 3, 600ms: 4, 700ms: 5, 800ms: 6, 900ms: 7
    u32 user_def_ks_sel                  : 4;   //USER_DEF键短按: 无: 0, 回拨: 1, SIRI: 2, PREV: 3, NEXT: 4, VOL+: 5, VOL-: 6, MODE: 7, HID拍照: 8, IOS HOME: 9, 中英文切换: 10, PLAY/PAUSE: 11, CBT_TEST: 12, 低延时: 13, 降噪控制: 14
    u32 user_def_kl_sel                  : 4;   //USER_DEF键长按: 无: 0, 回拨: 1, SIRI: 2, PREV: 3, NEXT: 4, VOL+: 5, VOL-: 6, MODE: 7, HID拍照: 8, IOS HOME: 9, 中英文切换: 10, PLAY/PAUSE: 11, CBT_TEST: 12, 低延时: 13, 降噪控制: 14
    u32 user_def_kd_sel                  : 4;   //USER_DEF键双击: 无: 0, 回拨: 1, SIRI: 2, PREV: 3, NEXT: 4, VOL+: 5, VOL-: 6, MODE: 7, HID拍照: 8, IOS HOME: 9, 中英文切换: 10, PLAY/PAUSE: 11, CBT_TEST: 12, 低延时: 13, 降噪控制: 14
    u32 user_def_kt_sel                  : 4;   //USER_DEF键三击: 无: 0, 回拨: 1, SIRI: 2, PREV: 3, NEXT: 4, VOL+: 5, VOL-: 6, MODE: 7, HID拍照: 8, IOS HOME: 9, 中英文切换: 10, PLAY/PAUSE: 11, CBT_TEST: 12, 低延时: 13, 降噪控制: 14
    u32 user_def_kfour_sel               : 4;   //USER_DEF键四击: 无: 0, 回拨: 1, SIRI: 2, PREV: 3, NEXT: 4, VOL+: 5, VOL-: 6, MODE: 7, HID拍照: 8, IOS HOME: 9, 中英文切换: 10, PLAY/PAUSE: 11, CBT_TEST: 12, 低延时: 13, 降噪控制: 14
    u32 user_def_kfive_sel               : 4;   //USER_DEF键五击: 无: 0, 回拨: 1, SIRI: 2, PREV: 3, NEXT: 4, VOL+: 5, VOL-: 6, MODE: 7, HID拍照: 8, IOS HOME: 9, 中英文切换: 10, PLAY/PAUSE: 11, CBT_TEST: 12, 低延时: 13, 降噪控制: 14
    u32 user_def_kl_tone_en              : 1;   //USER_DEF键长按按键音
    u32 user_def_kd_lang_en              : 1;   //USER_DEF键双击切语言
    u32 user_def_lr_en                   : 1;   //USER_DEF键区分左右耳
    u32 key_multi_config_en              : 1;   //支持多击的按键定制
    u8 dblkey_num0;                             //多击按键1选择: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21
    u8 dblkey_num1;                             //多击按键2选择: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21
    u8 dblkey_num2;                             //多击按键3选择: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21
    u8 dblkey_num3;                             //多击按键4选择: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21
    u8 dblkey_num4;                             //多击按键5选择: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21
    u32 pwrkey_config_en                 : 1;   //PWRKEY按键定制
    u8 pwrkey_num0;                             //PWRKEY按键1功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 pwrkey_num1;                             //PWRKEY按键2功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 pwrkey_num2;                             //PWRKEY按键3功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 pwrkey_num3;                             //PWRKEY按键4功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 pwrkey_num4;                             //PWRKEY按键5功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u32 adkey_config_en                  : 1;   //ADKEY1按键定制
    u8 adkey1_num0;                             //ADKEY1按键1功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num1;                             //ADKEY1按键2功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num2;                             //ADKEY1按键3功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num3;                             //ADKEY1按键4功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num4;                             //ADKEY1按键5功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num5;                             //ADKEY1按键6功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num6;                             //ADKEY1按键7功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num7;                             //ADKEY1按键8功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num8;                             //ADKEY1按键9功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num9;                             //ADKEY1按键10功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num10;                            //ADKEY1按键11功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 adkey1_num11;                            //ADKEY1按键12功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u32 iokey_config_en                  : 1;   //IOKEY按键定制
    u32 iokey_io0                        : 5;   //IOKEY按键1的IO: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 iokey_io1                        : 5;   //IOKEY按键2的IO: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 iokey_io2                        : 5;   //IOKEY按键3的IO: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 iokey_io3                        : 5;   //IOKEY按键4的IO: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 iokey_io4                        : 5;   //IOKEY按键5的IO: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u8 iokey_num0;                              //IOKEY按键1功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 iokey_num1;                              //IOKEY按键2功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 iokey_num2;                              //IOKEY按键3功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 iokey_num3;                              //IOKEY按键4功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 iokey_num4;                              //IOKEY按键5功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u8 iokey_midkey_num;                        //两IO中间的按键功能: None: 0, P/P: 1, PWR: 2, NEXT/VOL+: 3, PREV/VOL-: 4, NEXT/VOL-: 5, PREV/VOL+: 6, VOL+/NEXT: 7, VOL-/PREV: 8, VOL-/NEXT: 9, VOL+/PREV: 10, VOL+: 11, VOL-: 12, NEXT: 13, PREV: 14, MODE: 15, HSF: 16, MODE/PWR: 17, SIRI: 18, HOME: 19, PP/PWR/USER_DEF: 20, PP/USER_DEF: 21, VOL+/VOL-: 22, ANC: 23, REC: 24
    u32 led_disp_en                      : 1;   //系统指示灯(蓝灯)
    u32 led_pwr_en                       : 1;   //电源状态灯(红灯)
    u32 port_2led_resless_en             : 1;   //省电阻1个IO推两个灯
    u32 rled_lowbat_en                   : 1;   //电池低电闪灯
    u32 bled_io_sel                      : 5;   //蓝灯IO选择: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 rled_io_sel                      : 5;   //红灯IO选择: None: 0, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PB5: 14, PE0: 17, PE4: 21, PE5: 22, PE6: 23, PE7: 24, PF0: 25, PF1: 26
    u32 led_sta_config_en                : 1;   //自定义LED闪灯状态
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_poweron;                              //开机闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_poweroff;                             //关机闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_btinit;                               //初始化闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_reconnect;                            //回连闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_pairing;                              //配对闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_connected;                            //已连接闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_playing;                              //播放闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_ring;                                 //来电闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_calling;                              //通话闪灯控制
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_lowbat;                               //低电闪灯控制
    u32 ntc_en                           : 1;   //是否使能NTC
    u8 ntc_charge_thd_risk_high;                //充电高温保护的温度
    u8 ntc_charge_thd_normal_high;              //充电高温正常的温度
    u8 ntc_charge_thd_normal_low;               //充电低温正常的温度
    u8 ntc_charge_thd_risk_low;                 //充电低温保护的温度
    u8 ntc_discharge_thd_risk_high;             //放电高温保护的温度
    u8 ntc_discharge_thd_risk_low;              //放电低温保护的温度
    u32 ntc_0p2c_en                      : 1;   //是否使能NTC的0.2C充电模式
    u8 ntc_0p2c_enter;                          //进入0.2C充电的温度
    u8 ntc_0p2c_exit;                           //退出0.2C充电的温度
    u8 ntc_0p2c_constant_curr;                  //0.2C恒流充电电流: 0:5mA, 1:10mA, 2:15mA, 3:20mA, 4:25mA, 5:30mA, 6:35mA, 7:40mA, 8:45mA, 9:50mA, 10:55mA, 11:60mA, 12:65mA, 13:70mA, 14:85mA, 15:90mA, 16:105mA, 17:110mA, 18:125mA, 19:130mA, 20:145mA, 21:150mA, 22:165mA, 23:170mA
    u32 ntc_0p2c_charge_voltage_follow   : 1;   //0.2C充电电压跟随模式
    u8 ntc_0p2c_charge_stop_curr;               //0.2C充电截止电流: 0:2.5mA, 1:5mA, 2:7.5mA, 3:10mA, 4:15mA, 5:20mA, 6:25mA, 7:30mA
    u8 ntc_0p2c_charge_trickle_curr;            //0.2C涓流充电电流: 0:5mA, 1:10mA, 2:15mA, 3:20mA, 4:25mA, 5:30mA, 6:35mA, 7:40mA
    u8 xm_keep_start[0];                        //For Keep Area Start
    u8 osci_cap;                                //产测OSCI电容
    u8 osco_cap;                                //产测OSCO电容
    u8 xm_keep_end[0];                          //For Keep Area End
} xcfg_cb_t;

extern xcfg_cb_t xcfg_cb;
#endif
