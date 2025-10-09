depend(0x01020100);

config(SUB, "系统配置", "系统的相关配置");
config(LIST, "提示音语言选择", "选择系统的语言", LANG_ID, 4, "英文", "中文", "中英文(出厂默认英文)", "中英文(出厂默认中文)", 0);
config(CHECK, "SD/U盘播放功能", "是否需要SD/UDISK播放功能", FUNC_MUSIC_EN, 0);
config(LEVEL, 0x00);
config(CHECK, "AUX输入功能", "是否需要AUX功能", FUNC_AUX_EN, 0);
config(LEVEL, 0x03);
config(CHECK, "POWKEY 10s复位系统", "是否长按POWKEY 10s复位系统，用于防止系统死机", POWKEY_10S_RESET, 1);
config(CHECK, "功放MUTE功能", "是否使能功放MUTE功能", SPK_MUTE_EN, 0);
config(LISTVAL, "功放MUTE控制IO选择", "功放MUTE控制GPIO选择", SPK_MUTE_IO_SEL, BIT, 5, 0, 23, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), ("PF2", 27), ("PF3", 28), SPK_MUTE_EN);
config(CHECK, "高MUTE", "是否为高MUTE，否则为低MUTE", HIGH_MUTE, 1, SPK_MUTE_EN);
config(BYTE, "功放MUTE延时(单位5ms)", "功放MUTE的延时控制，防止解MUTE时间不够导致声音不全。", LOUDSPEAKER_UNMUTE_DELAY, 0, 255, 6, SPK_MUTE_EN);
config(LEVEL, 0x00);
config(LIST, "功放AB/D控制模式", "功放AB/D控制模式选择", AMPABD_TYPE, 2, "独立IO电平控制", "mute脉冲控制", 0);
config(LISTVAL, "功放AB/D控制IO选择", "功放AB/D控制GPIO选择", AMPABD_IO_SEL, BIT, 5, 0, 23, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), ("PF2", 27), ("PF3", 28));
config(LISTVAL, "耳机检测IO选择", "选择耳机检测IO选择", EARPHONE_DET_IOSEL, BIT, 6, 0, 26, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), ("PF2", 27), ("PF3", 28), ("复用SDCLK检测", 60), ("复用SDCMD检测", 61), ("复用PWRKEY检测", 62));
config(LEVEL, 0x03);
config(LISTVAL, "自动休眠时间", "设置自动休眠时间", SYS_SLEEP_TIME, SHORT, 30, 21, ("不休眠", 0), ("10秒钟后", 10), ("20秒钟后", 20), ("30秒钟后", 30), ("45秒钟后", 45), ("1分钟后", 60), ("2分钟后", 120), ("3分钟后", 180), ("4分钟后", 240), ("5分钟后", 300), ("6分钟后", 360), ("7分钟后", 420), ("8分钟后", 480), ("9分钟后", 540), ("10分钟后", 600), ("15分钟后", 900), ("20分钟后", 1200), ("25分钟后", 1500), ("30分钟后", 1800), ("45分钟后", 2700), ("1小时后", 3600));
config(LISTVAL, "自动关机时间", "设置自动关机时间", SYS_OFF_TIME, 	SHORT, 300, 18, ("不关机", 0), ("30秒钟后", 30), ("1分钟后", 60),  ("2分钟后", 120), ("3分钟后", 180), ("4分钟后", 240), ("5分钟后", 300), ("6分钟后", 360), ("7分钟后", 420), ("8分钟后", 480), ("9分钟后", 540), ("10分钟后", 600), ("15分钟后", 900), ("20分钟后", 1200), ("25分钟后", 1500), ("30分钟后", 1800), ("45分钟后", 2700), ("1小时后", 3600));
config(CHECK, "低电提示", "当电压比较低的时候，会有提示音警告", LOWPOWER_WARNING_EN, 1);
config(LIST, "低电语音提示电压", "选择低电语音提示电压", LPWR_WARNING_VBAT, 10, "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 4, LOWPOWER_WARNING_EN);
config(LIST, "低电关机电压", "选择低电关机电压", LPWR_OFF_VBAT, 11, "不关机", "2.8V", "2.9V", "3.0V", "3.1V", "3.2V", "3.3V", "3.4V", "3.5V", "3.6V", "3.7V", 2, LOWPOWER_WARNING_EN);
config(BYTE, "低电语音播报周期(秒)", "设置低电语音播报周期(秒)", LPWR_WARNING_PERIOD, 1, 240, 30, LOWPOWER_WARNING_EN);
config(LIST, "OSC基础电容", "配置24M晶振OSCI与OSCO基础电容（6PF）", OSC_BOTH_CAP, 2, "0PF", "6PF", 0);
config(BYTE, "自定义OSCI电容", "配置24M晶振OSCI负载电容（0.25PF）。电容大小：n * 0.25PF + 基础电容", UOSCI_CAP, 0, 63, 45);
config(BYTE, "自定义OSCO电容", "配置24M晶振OSCO负载电容（0.25PF）。电容大小：n * 0.25PF + 基础电容", UOSCO_CAP, 0, 63, 45);
config(CHECK,  "优先使用产测电容值",  "是否优先使用产测校准的OSC电容值，没过产测或者关闭时使用<自定义OSCI电容>、<自定义OSCO电容>",  FT_OSC_CAP_EN,  1);

config(LISTVAL, "软开机长按时间选择", "长按PWRKEY多长时间后软开机", PWRON_PRESS_TIME,  BIT, 3, 3, 8, ("0.1秒", 0), ("0.5秒", 1), ("1秒", 2), ("1.5秒", 3), ("2秒", 4), ("2.5秒", 5), ("3秒", 6), ("3.5秒", 7));
config(LISTVAL, "软关机长按时间选择", "长按PWRKEY多长时间后软关机", PWROFF_PRESS_TIME, BIT, 3, 3, 8, ("1.5秒", 0), ("2秒", 1), ("2.5秒", 2), ("3秒", 3), ("3.5秒", 4), ("4秒", 5), ("4.5秒", 6), ("5秒", 7));
config(CHECK, "EQ调试（蓝牙串口）", "是否使用SPP调试EQ功能", EQ_DGB_SPP_EN, 0);
config(CHECK, "HUART调试(EQ/FCC)", "是否使用HUART调试功能", HUART_EN, 0);
config(LISTVAL, "HUART串口选择", "选择HUART的IO", HUART_SEL, BIT, 4, 0, 11, ("PA7", 0), ("PB2", 1), ("PB3", 2), ("PE7", 3), ("PF0", 4), ("PA6", 5), ("PB1", 6), ("PB4", 7), ("PE6", 8), ("PF1", 9), ("VUSB", 10), HUART_EN);
config(CHECK, "UART1调试(仓/IODM)", "是否使用UART1调试功能", UART1_EN, 0);
config(LISTVAL, "UART1串口选择", "选择UART1的IO", UART1_SEL, BIT, 2, 2, 3, ("PA7", 0), ("PF0", 1), ("VUSB", 2), UART1_EN);
config(CHECK, "UART2调试(仓/IODM)", "是否使用UART2调试功能", UART2_EN, 0);
config(LISTVAL, "UART2串口选择", "选择UART2的IO", UART2_SEL, BIT, 2, 2, 3, ("PE7", 0), ("PB2", 1), ("VUSB", 2), UART2_EN);
config(CHECK,  "是否使能蓝汛默认NTC",  "是否使能蓝汛默认NTC",  NTC_EN_BT,  0);
config(CHECK, "NTC使用内部10K上拉", "NTC是否使用内部10K上拉,使能程序中的宏USER_NTC后才有效", NTC_USER_INNER_PU,0, NTC_EN);
config(BYTE,  "NTC阈值",  "NTC阈值",  NTC_THD_VAL,  0, 255, 100,  NTC_EN);

config(SUB, "DAC配置", "DAC的相关配置");
config(LISTVAL, "DAC声道选择", "选择DAC的输出方式, 最大输出功率: 32Ω 11.3mW", DAC_SEL, BIT, 4, 4, 4,  ("VCMBUF单声道", 2), ("VCMBUF双声道", 3), ("差分单声道", 4), ("差分双声道", 5));
config(LISTVAL, "DAC输出采样率", "DAC选择输出的采样率", DAC_SPR_SEL, BIT, 2, 0, 2, ("44.1KHz", 0), ("48KHz", 1));
config(CHECK, "DAC使能24bit", "DAC使能24bit，默认使用24bit", DAC_24BITS_EN, 1);
config(CHECK, "DAC扩展模式", "提高DAC输出功率，功耗会增加", DAC_MAXOUT_EN, 0);
config(CHECK, "省VCM方案", "是否使用内部VCM，SOP16, SSOP24, SSOP28使用内部VCM", DAC_VCM_LESS_EN, 0);
config(LEVEL, 0x00);
config(CHECK, "蓝牙连接时关闭DAC", "蓝牙连接过程关闭DAC，仅对差分/VCMVUF输出有效", DAC_OFF_FOR_CONN, 0);
config(LEVEL, 0x03);

config(SUB, "音量配置", "音量的相关配置");
config(LIST, "音乐衰减增益", "配置系统最大增益基础上衰减，自动调整音量表。", DAC_MAX_GAIN, 10, "0DB", "-1DB", "-2DB", "-3DB", "-4DB", "-5DB", "-6DB", "-7DB", "-8DB", "-9DB", 3);
config(LIST, "通话衰减增益", "配置通话时最大增益基础上衰减，自动调整音量表。", BT_CALL_MAX_GAIN, 10, "0DB", "-1DB", "-2DB", "-3DB", "-4DB", "-5DB", "-6DB", "-7DB", "-8DB", "-9DB", 1);
config(LIST, "音量级数", "选择系统音量级数", VOL_MAX, 2, "16级音量", "32级音量", 1);
config(BYTE, "开机默认音量", "开机默认音量级数", SYS_INIT_VOL, 0, 50, 19);
config(BYTE, "恢复默认音量阈值", "小于多少级音量后恢复成系统音量阈值", SYS_RECOVER_INITVOL, 0, 50, 5);
config(BYTE, "提示音播放音量", "提示音播放音量级数", WARNING_VOLUME, 0, 50, 21);

config(SUB, "电源配置", "电源的相关配置");
config(CHECK, "BUCK MODE", "是否设置为BUCK MODE", BUCK_MODE_EN, 0);
config(CHECK, "双BUCK模式", "是否打开VDDCORE BUCK", VDDCORE_BUCK_EN, 1, BUCK_MODE_EN);
config(CHECK, "VDDBT省电容", "是否VDDBT省电容，只能LDO模式才能省电容", VDDBT_CAPLESS_EN, 0, BUCK_MODE_EN);
config(LISTVAL, "VDDIO电压", "选择VDDIO电压, VDDAUD与VDDIO短接可能需要调VDDIO电压。", VDDIO_SEL, BIT, 4, 7, 9, ("None", 0), ("2.9V", 5), ("3.0V", 6), ("3.1V", 7), ("3.2V", 8), ("3.3V", 9), ("3.4V", 10), ("3.5V", 11), ("3.6V", 12));

config(SUB, "充电配置", "充电功能的相关配置");
config(CHECK, "充电使能", "是否打开充电功能", CHARGE_EN, 1);
config(CHECK, "涓流充电使能", "是否使能涓流充电", CHARGE_TRICK_EN, 1, CHARGE_EN);
config(CHECK, "插入DC禁止软开机", "PWRKEY软开机，DC Online时，禁止软开机", CHARGE_DC_NOT_PWRON, 1, CHARGE_EN);
config(CHECK, "充电电压跟随模式", "是否打开充电电压跟随模式，需支持快充的仓", CHARGE_VOLTAGE_FOLLOW, 0, CHARGE_EN);
config(LIST, "充电截止电流", "选择充电截止电流", CHARGE_STOP_CURR, 16, "0mA", "2.5mA", "5mA", "7.5mA", "10mA", "12.5mA", "15mA", "17.5mA", "20mA", "22.5mA", "25mA", "27.5mA", "30mA", "32.5mA", "35mA", "37.5mA", 4, CHARGE_EN);
config(LIST, "恒流充电电流", "恒流充电（电池电压大于2.9v）电流", CHARGE_CONSTANT_CURR, 33, "5mA", "10mA", "15mA", "20mA", "25mA", "30mA", "35mA", "40mA", "45mA", "50mA", "55mA", "60mA", "65mA", "70mA", "75mA", "80mA", "90mA", "100mA", "110mA", "120mA", "130mA", "140mA", "150mA", "160mA", "170mA", "180mA", "190mA", "200mA",, "210mA", "220mA", "230mA", "240mA", "250mA" 7, CHARGE_EN);
config(LIST, "涓流充电电流", "涓流充电（电池电压小于2.9v）电流", CHARGE_TRICKLE_CURR, 8, "5mA", "10mA", "15mA", "20mA", "25mA", "30mA", "35mA", "40mA", 3, CHARGE_TRICK_EN);
config(LISTVAL, "充满电蓝灯亮", "设置充满电蓝灯亮时间", CHARGE_FULL_BLED, BIT, 3, 3, 8,  ("不亮蓝灯", 0), ("亮10秒", 1), ("亮20秒", 2), ("亮30秒", 3), ("1分钟", 4), ("2分钟", 5), ("3分钟", 6), ("常亮", 7), CHARGE_EN);
config(CHECK, "充满电自动关机", "电池充满自动关机功能，主要用于放入电池仓充电的选项", CH_FULL_AUTO_PWRDWN_EN, 1, CHARGE_EN);
config(CHECK, "充电仓功能", "是否使能充电仓功能", CHBOX_EN, 1, CHARGE_EN);
config(LISTVAL, "充电仓类型选择", "选择正确类型充电仓，确保充满关机和拿起开机功能正常", CH_BOX_TYPE_SEL, BIT, 2, 0, 4, ("兼容5V短暂掉0V后维持电压", 0), ("5V不掉电的充电仓", 1), ("5V掉电但有维持电压", 2), ("5V完全掉电无维持电压", 3), CHBOX_EN);
config(LISTVAL, "仓允许最低维持电压", "inbox信号电压，即充满维持电压的充电仓允许的最低维持电压", CH_INBOX_SEL, BIT, 1, 0, 2, ("1.1V", 0), ("1.7V", 1), CHBOX_EN);
config(CHECK, "从充电仓拿出自动开机", "耳机从充电仓拿出自动开机，注意充满后电池仓自动断电的不能开此选项", CH_OUT_AUTO_PWRON_EN, 1 CHBOX_EN);
config(LISTVAL, "出仓或入仓VUSB漏电配置", "加速耳机出仓时自动开机或入仓时唤醒充电仓", CH_LEAKAGE_SEL, BIT, 2, 2, 4, ("None", 0), ("漏电电流X1档", 1), ("漏电电流X2档", 2), ("漏电电流X3档", 3), CHARGE_EN);
config(CHECK, "入仓耳机关机", "有些维持电压的电池仓，耳机入仓后电池仓的5V不会自动起来进入充电, 这时耳机是否进入关机。", CHG_INBOX_PWRDWN_EN, 1, CHBOX_EN);
config(BIT, "短暂掉0V的仓稳定检测时间", "短暂掉0V的仓稳定检测时间: 500ms + n*100ms", CHBOX_OUT_DELAY,  4, 0, 15, 0, CHBOX_EN);

config(SUB, "蓝牙配置", "蓝牙的相关配置");
config(TEXT, "蓝牙名称", "手机上可以看到的蓝牙名称", BT_NAME, 32, "Bluetrum-TWS");
config(MAC, "蓝牙地址", "蓝牙的MAC地址", BT_ADDR, 6, 41:42:00:00:00:00, 41:42:FF:FF:FF:FF, 41:42:00:00:00:01);
config(LISTVAL, "长按几秒开机进配对", "是否支持长按N秒开机直接进入配对状态", BT_PWRKEY_NSEC_DISCOVER, BIT, 3, 0, 8, ("不支持", 0), ("2秒", 1), ("3秒", 2), ("4秒", 3), ("5秒", 4), ("6秒", 5), ("7秒", 6), ("8秒", 7));
config(CHECK, "配对提示音", "是否播放配对提示音", WARNING_BT_PAIR, 1);
config(CHECK, "连接两部手机功能", "是否支持连接两部手机功能（打开<TWS功能>后无效）", BT_2ACL_EN, 0);
config(CHECK, "音乐播放功能", "是否支持蓝牙音乐播放功能", BT_A2DP_EN, 1);
config(CHECK, "音乐音量同步", "是否支持A2DP音量与手机同步", BT_A2DP_VOL_CTRL_EN, 0, BT_A2DP_EN);
config(CHECK, "通话功能", "是否支持蓝牙通话的功能", BT_SCO_EN, 1);
config(CHECK, "私密接听功能", "是否使用手动私密接听（切换到手机端接听）", BT_HFP_PRIVATE_EN, 1, BT_SCO_EN);
config(CHECK, "来电报号功能", "是否支持来电报号功能", BT_HFP_RING_NUMBER_EN, 1, BT_SCO_EN);
config(CHECK, "串口功能", "是否支持蓝牙串口的功能", BT_SPP_EN, 0);
config(CHECK, "拍照功能", "是否支持蓝牙HID拍照的功能", BT_HID_EN, 0);
config(CHECK, "按键HID连接/断开功能", "是否支持蓝牙HID服务，按键手动连接/断开", BT_HID_MANU_EN, 0, BT_HID_EN);
config(CHECK, "HID默认不连接", "HID服务默认不连接，需要按键手动连接", BT_HID_DISCON_DEFAULT_EN, 0, BT_HID_EN);
config(LEVEL, 0x00);
config(CHECK, "HID独立自拍器模式", "是否支持独立自拍器模式功能", FUNC_BTHID_EN, 0);
config(TEXT, "HID蓝牙名称", "手机上可以看到的独立自拍器模式蓝牙名称", BTHID_NAME, 32, "BT-Photo", FUNC_BTHID_EN);
config(LEVEL, 0x03);
config(CHECK, "BLE控制功能", "是否支持BLE音乐控制的功能", BLE_EN, 0);
config(TEXT, "BLE名称", "手机上可以看到的BLE蓝牙名称", LE_NAME, 32, "LE-Remoter", BLE_EN);
config(CHECK, "快速测试使能", "快速测试使能", QTEST_EN, 0);
config(LIST, "耳机拿起后状态", "设置耳机拿起后的状态,以测试盒设置为准",  QTEST_PICKUP_STATUS, 4, "不操作", "拿起关机", "拿起复位", "断开蓝牙连接后关机" 0,QTEST_EN);

#config(SUB, "涂鸦配置", "涂鸦的相关配置");
#config(CHECK, "涂鸦授权码更新", "是否使用配置文件的授权码更新当前样机已有的授权码", TUYA_AUTH_UPDATE, 0);
#config(TEXT, "BLE地址", "手机上可以看到的BLE蓝牙地址", LE_MAC, 12, "000000");
#config(TEXT, "涂鸦产品ID", "涂鸦的产品PID", TUYA_PID, 8, "0000");
#config(TEXT, "涂鸦鉴权秘钥", "涂鸦APP鉴权密钥Key", TUYA_AUTH_KEY, 32, "000000");
#config(TEXT, "涂鸦设备ID", "涂鸦设备的UUID", TUYA_DEVICE_UUID, 16, "000000");

config(SUB, "TWS配置", "TWS的相关配置");
config(CHECK, "TWS功能", "是否支持TWS功能，打开后<连接两部手机功能>无效", BT_TWS_EN, 1);
config(CHECK, "TWS组队绑定功能", "打开TWS组队绑定后，需要手动删除组队信息才能重新组队", BT_TWS_PAIR_BONDING_EN, 0, BT_TWS_EN);
config(LISTVAL, "TWS组队方式选择", "选择TWS组队操作方式", BT_TWS_PAIR_MODE, BIT, 3, 0, 6, ("自动组队", 0), ("自定义组队<调用api>", 1), ("双击USER_DEF键组队", 2), ("单击USER_DEF键组队", 3), ("长按USER_DEF键组队", 4), ("双击MODE/POWER键组队", 5) BT_TWS_EN);
config(LISTVAL, "TWS声道分配选择", "选择TWS声道分配方式", BT_TWS_LR_MODE, BIT, 4, 1, 7, ("不分配，主副均双声道输出", 0), ("自动分配，主右声道副左声道", 1), ("自动分配，主左声道副右声道", 2), ("PWRKEY,有820K接地为左", 8), ("GPIOx有接地为左声道", 9), ("配置选择为左声道", 10), ("配置选择为右声道", 11), BT_TWS_EN);
config(LISTVAL, "TWS声道GPIOx", "GPIOx有接地为左声道", TWS_SEL_LEFT_GPIO_SEL, BIT, 5, 0, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), BT_TWS_EN);
config(CHECK, "TWS蓝牙名加L或R后缀", "是否使能TWS蓝牙名称加L或R后缀, 需要GPIOx固定左右耳", BT_TWS_NAME_SUFFIX_EN, 0, BT_TWS_EN);
config(CHECK, "TWS主从切换功能", "是否支持TWS主从自动切换功能", BT_TSWI_EN, 1, BT_TWS_EN);
config(CHECK, "TWS按键关机支持主从切换", "TWS按键手动关机是否自动切换主从, 否则同步关机", BT_TSWI_KPWR_EN, 1, BT_TSWI_EN);
config(CHECK, "TWS音乐模式支持主从切换", "TWS音乐模式,根据主副耳信号质量自动主从切换", BT_TSWI_MSC_EN, 0, BT_TSWI_EN);
config(CHECK, "TWS通话模式支持主从切换", "TWS通话模式,根据主副耳信号质量自动主从切换", BT_TSWI_SCO_EN, 0, BT_TSWI_EN);
config(CHECK, "TWS耳机入仓充电复位", "TWS耳机入仓主从切换后是否复位", BT_TSWI_CHARGE_RST_EN, 0, BT_TSWI_EN);
config(CHECK, "TWS耳机出仓播放开机音", "TWS耳机出仓是否播放开机音", BT_OUTBOX_VOICE_PWRON_EN, 0, BT_TWS_EN);

config(SUB, "RF参数", "蓝牙RF参数");
config(S8, "降低预置RF参数发射功率", "微调<预置RF参数>或<FT的RF参数>的发射功率，每级可降低0.5dbm发射功率（负数则增加功率）", BT_RF_PWRDEC, -6, 32, 0);
config(CHECK,  "优先使用FT的RF参数",  "是否优先使用FT校准的RF参数，没过FT或关闭时使用<预置RF参数>或<自定义RF参数>",  FT_RF_PARAM_EN,  1);
config(CHECK, "自定义RF参数", "打开后，有FT的芯片优先使用<FT的RF参数>，否则使用<自定义RF参数>", BT_RF_PARAM_EN, 0);
config(LEVEL, 0x03);
config(BYTE, "GL_PA_GAIN",    "PA_GAIN, 参考值3",         RF_PA_GAIN,    0,  7,  7, BT_RF_PARAM_EN);
config(LEVEL, 0x03);
config(BYTE, "GL_MIX_GAIN",   "MIX_GAIN, 参考值3~5",      RF_MIX_GAIN,   0,  7,  6, BT_RF_PARAM_EN);
config(BYTE, "GL_DIG_GAIN",   "DIG_GAIN, 参考值30~59",    RF_DIG_GAIN,   24, 59, 55, BT_RF_PARAM_EN);
config(LEVEL, 0x00);
config(BYTE, "GL_PA_CAP",     "PA_CAP, 参考值12~14",      RF_PA_CAP,     0, 15, 4, BT_RF_PARAM_EN);
config(BYTE, "GL_MIX_CAP",    "MIX_CAP, 参考值7~9",       RF_MIX_CAP,    0, 15,  8, BT_RF_PARAM_EN);
config(BYTE, "GL_TX_DBM",     "Cable实测dbm值",           RF_TXDBM,      0, 12,  8, BT_RF_PARAM_EN);
config(BYTE, "GL_UDF0",       "UDF(保留使用)",            RF_UDF0,       0,  0,  0, BT_RF_PARAM_EN);
config(BYTE, "GL_UDF1",       "UDF(保留使用)",            RF_UDF1,       0,  0,  0, BT_RF_PARAM_EN);
config(LEVEL, 0x03);
config(LISTVAL, "降低回连功率", "是否降低回连手机或TWS的功率", BT_RF_PAGE_PWRDEC, BIT, 3, 1, 4, ("不降低", 0), ("降低3dbm", 1), ("降低6dbm", 2), ("降低9dbm", 3));
config(LISTVAL, "降低组队功率", "是否降低TWS搜索组队的功率", BLE_RF_PAGE_PWRDEC, BIT, 3, 0, 4, ("不降低", 0), ("降低3dbm", 1), ("降低6dbm", 2), ("降低9dbm", 3));
config(BYTE, "设置组队范围RSSI(-dbm)", "该值越小(-90dbm最小),组队范围约大", BLE_PAGE_RSSI_THR, 20, 90, 90);
config(BYTE, "BLE_value",     "BLE与BT功率差值，BLE功率相对于BT每提高1档降低0.5db",   BLE_DELTA_VALUE,   0, 12,  0);
config(CHECK, "通话RF参数", "是否单独配置通话RF参数", BT_RF_PARAM_SCO_EN, 0, BT_RF_PARAM_EN);
config(BYTE, "SCO_PA_GAIN",   "通话PA_GAIN, 参考值7",     RF_PA_GAIN_SCO,    0,  5,  4, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_MIX_GAIN",  "通话MIX_GAIN, 参考值3~5",  RF_MIX_GAIN_SCO,   0,  7,  6, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_DIG_GAIN",  "通话DIG_GAIN, 参考值30~59",RF_DIG_GAIN_SCO,   24,61, 55, BT_RF_PARAM_SCO_EN);
config(LEVEL, 0x00);
config(BYTE, "SCO_PA_CAP",    "通话PA_CAP, 参考值7~9",    RF_PA_CAP_SCO,     0, 15,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_MIX_CAP",   "通话MIX_CAP, 参考值7~9",   RF_MIX_CAP_SCO,    0, 15,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_TX_DBM",    "通话Cable实测dbm值",       RF_TXDBM_SCO,      0, 12,  8, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_UDF0",      "通话UDF(保留使用)",        RF_UDF0_SCO,       0,  0,  0, BT_RF_PARAM_SCO_EN);
config(BYTE, "SCO_UDF1",      "通话UDF(保留使用)",        RF_UDF1_SCO,       0,  0,  0, BT_RF_PARAM_SCO_EN);
config(LEVEL, 0x03);


config(SUB, "MIC参数", "蓝牙通话和ANC的MIC参数");

config(CHECK, "ANC功能", "是否打开ANC功能", ANC_EN, 0);
config(LISTVAL, "ANC模式", "选择ANC的模式，请配置对应MIC通路参数", ANC_MODE, BIT, 4, 0, 4, ("TWS FF/FB Mode", 0), ("FF/FB Mode", 1), ("TWS Hybrid Mode", 8), ("Hybrid Mode", 9), ANC_EN);
config(CHECK, "MIC Gain产测配置", "MIC Gain是否取产测值，否则取资源文件值", ANC_GAIN_EN, 0, ANC_EN);

config(LISTVAL, "通话主MIC配置", "选择通话主MIC，注意配置对应MIC通路参数", BT_MMIC_CFG, BIT, 3, 4, 6,  ("None", 5), ("MIC0", 0), ("MIC1", 1), ("MIC2", 2), ("MIC3", 3), ("MIC4", 4), BT_SCO_EN);
config(LISTVAL, "通话副MIC配置", "选择通话副MIC，注意配置对应MIC通路参数", BT_SMIC_CFG, BIT, 3, 4, 6,  ("None", 5), ("MIC0", 0), ("MIC1", 1), ("MIC2", 2), ("MIC3", 3), ("MIC4", 4), BT_SCO_EN);
config(CHECK, "MICR反相", "双麦降噪两麦相位相反需要使能", MICR_PHASE_EN, 0);
config(BIT, "通话后置数字增益", "调节算法后置数字增益", BT_SCO_POST_GAIN, 4, 0, 15, 2, BT_SCO_EN);
config(LISTVAL, "MIC供电IO电压选择", "MIC供电IO电压选择", MIC_PWR_LEVEL, BIT, 3, 6, 8, ("1.8v", 0), ("2.4v", 1), ("2.5v", 2), ("2.6v", 3), ("2.7v", 4), ("2.8v", 5), ("2.9v", 6), ("3.0v", 7));

config(CHECK, "MIC0", "是否打开MIC0", MIC0_EN, 0);
config(LISTVAL, "供电IO选择", "选择对应IO给MIC0供电", MIC0_PWR_SEL, BIT, 2, 0, 4, ("None", 0), ("PF0", 1), ("PF1", 2), ("PE7", 3), MIC0_EN);
config(LISTVAL, "MIC0偏置电路配置", "MIC0电路BIAS配置，省电容，省电阻配置", MIC0_BIAS_METHOD, BIT, 1, 1, 2, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), MIC0_EN);
config(BYTE, "MIC0模拟增益", "MIC0模拟增益配置(3~42DB), Step 3DB", MIC0_ANL_GAIN, 0, 13, 8, MIC0_EN);
config(BYTE, "MIC0通话数字增益", "通话MIC0数字增益配置(0~39DB), Step 1DB", BT_MIC0_DIG_GAIN, 0, 39, 20, MIC0_EN);

config(CHECK, "MIC1", "是否打开MIC1", MIC1_EN, 0);
config(LISTVAL, "供电IO选择", "选择对应IO给MIC1供电", MIC1_PWR_SEL, BIT, 2, 0, 4, ("None", 0), ("PF0", 1), ("PF1", 2),  ("PE7", 3),MIC1_EN);
config(LISTVAL, "MIC1偏置电路配置", "MIC1电路BIAS配置，省电容，省电阻配置", MIC1_BIAS_METHOD, BIT, 1, 1, 2, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), MIC1_EN);
config(BYTE, "MIC1模拟增益", "MIC1模拟增益配置(3~42DB), Step 3DB", MIC1_ANL_GAIN, 0, 13, 8, MIC1_EN);
config(BYTE, "MIC1通话数字增益", "通话MIC1数字增益配置(0~39DB), Step 1DB", BT_MIC1_DIG_GAIN, 0, 39, 20, MIC1_EN);

config(CHECK, "MIC2", "是否打开MIC2", MIC2_EN, 0);
config(LISTVAL, "供电IO选择", "选择对应IO给MIC2供电", MIC2_PWR_SEL, BIT, 2, 0, 4, ("None", 0), ("PF0", 1),), ("PF1", 2), ("PE7", 3), MIC2_EN);
config(LISTVAL, "MIC2偏置电路配置", "MIC2电路BIAS配置，省电容，省电阻配置", MIC2_BIAS_METHOD, BIT, 1, 1, 2, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), MIC2_EN);
config(BYTE, "MIC2模拟增益", "MIC2模拟增益配置(3~42DB), Step 3DB", MIC2_ANL_GAIN, 0, 13, 8, MIC2_EN);
config(BYTE, "MIC2通话数字增益", "通话MIC2数字增益配置(0~39DB), Step 1DB", BT_MIC2_DIG_GAIN, 0, 39, 20, MIC2_EN);

config(CHECK, "MIC3", "是否打开MIC3", MIC3_EN, 0);
config(LISTVAL, "供电IO选择", "选择对应IO给MIC3供电", MIC3_PWR_SEL, BIT, 2, 0, 4, ("None", 0), ("PF0", 1), ("PF1", 2), ("PE7", 3), MIC3_EN);
config(LISTVAL, "MIC3偏置电路配置", "MIC3电路BIAS配置，省电容，省电阻配置", MIC3_BIAS_METHOD, BIT, 1, 1, 2, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), MIC3_EN);
config(BYTE, "MIC3模拟增益", "MIC3模拟增益配置(3~42DB), Step 3DB", MIC3_ANL_GAIN, 0, 13, 8, MIC3_EN);
config(BYTE, "MIC3通话数字增益", "通话MIC3数字增益配置(0~39DB), Step 1DB", BT_MIC3_DIG_GAIN, 0, 39, 20, MIC3_EN);

config(CHECK, "MIC4", "是否打开MIC4", MIC4_EN, 1);
config(LISTVAL, "供电IO选择", "选择对应IO给MIC4供电", MIC4_PWR_SEL, BIT, 2, 0, 4, ("None", 0), ("PF0", 1), ("PF1", 2), ("PE7", 3), MIC4_EN);
config(LISTVAL, "MIC4偏置电路配置", "MIC4电路BIAS配置，省电容，省电阻配置", MIC4_BIAS_METHOD, BIT, 1, 0, 2, ("单端MIC外部电阻电容", 0), ("差分MIC", 1), MIC4_EN);
config(BYTE, "MIC4模拟增益", "MIC4模拟增益配置(3~42DB), Step 3DB", MIC4_ANL_GAIN, 0, 13, 8, MIC4_EN);
config(BYTE, "MIC4通话数字增益", "通话MIC4数字增益配置(0~39DB), Step 1DB", BT_MIC4_DIG_GAIN, 0, 39, 20, MIC4_EN);

config(CHECK, "ANC算法独立MIC数字增益", "是否使能ANC算法使用单独的MIC数字增益，否则使用通话数字增益", ANC_ALG_DGAIN_EN, 0, ANC_EN);
config(BYTE, "ANC算法MIC0数字增益", "ANC算法MIC0数字增益配置(0~39DB), Step 1DB", ANC_MIC0_DIG_GAIN, 0, 39, 20, ANC_ALG_DGAIN_EN);
config(BYTE, "ANC算法MIC1数字增益", "ANC算法MIC1数字增益配置(0~39DB), Step 1DB", ANC_MIC1_DIG_GAIN, 0, 39, 20, ANC_ALG_DGAIN_EN);
config(BYTE, "ANC算法MIC2数字增益", "ANC算法MIC2数字增益配置(0~39DB), Step 1DB", ANC_MIC2_DIG_GAIN, 0, 39, 20, ANC_ALG_DGAIN_EN);
config(BYTE, "ANC算法MIC3数字增益", "ANC算法MIC3数字增益配置(0~39DB), Step 1DB", ANC_MIC3_DIG_GAIN, 0, 39, 20, ANC_ALG_DGAIN_EN);
config(BYTE, "ANC算法MIC4数字增益", "ANC算法MIC4数字增益配置(0~39DB), Step 1DB", ANC_MIC4_DIG_GAIN, 0, 39, 20, ANC_ALG_DGAIN_EN);

config(SUB, "通话参数", "蓝牙的通话参数", BT_SCO_EN);
config(CHECK, "AEC功能", "是否支持AEC功能", BT_AEC_EN, 1, BT_SCO_EN);
config(BYTE, "AEC回声消除级别", "回声消除级别(级别越高，回声衰减越明显，但通话效果越差)", BT_ECHO_LEVEL, 0, 15, 0, BT_AEC_EN);
config(BIT, "AEC数字增益", "AEC频域数字增益(0~31DB), Step 1DB", AEC_DIG_GAIN, 5, 0, 31, 0, BT_AEC_EN);
config(BIT, "远端补偿", "远端信号的偏移值", AEC_FAR_OFFSET,  16, 0, 600, 402);

config(CHECK, "NR降噪功能", "是否支持ENC功能", BT_NR_EN, 1, BT_SCO_EN);
config(CHECK, "声加单麦AI降噪", "config.h打开BT_SNDP_SMIC_EN", BT_SNDP_SMIC_AI_EN, 0, BT_NR_EN);
config(BYTE, "稳态噪声降噪量", "单位：dB", BT_SNDP_LEVEL, 0, 29, 10, BT_SNDP_SMIC_AI_EN);
config(BYTE, "DNN降噪量", "单位：dB", BT_SNDP_DNN_LEVEL, 0, 29, 26, BT_SNDP_SMIC_AI_EN);

config(CHECK, "声加双麦AI降噪", "config.h打开BT_SNDP_DM_AI_EN", BT_SNDP_DM_AI_EN, 0, BT_NR_EN);
config(BYTE, "稳态噪声降噪量", "单位：dB", SNDP_DM_LEVEL, 0, 29, 10, BT_SNDP_DM_AI_EN);
config(BYTE, "DNN降噪量", "单位：dB", SNDP_DM_DNN_LEVEL, 0, 29, 26, BT_SNDP_DM_AI_EN);
config(BYTE, "双麦间距", "出音孔之间的直线距离（15~30mm）", SNDP_DM_DISTANCE, 15, 30, 23, BT_SNDP_DM_AI_EN);
config(BYTE, "拾音角度(单位:°)", "语音保护角，范围0~90度", SNDP_DM_DEGREE, 0, 90, 60, BT_SNDP_DM_AI_EN);
config(BYTE, "抑制角度(单位:°)", "小于保护角的语音被保留，大于抑制角语音被抑制，中间平滑过渡", SNDP_DM_DEGREE1, 0, 90, 75, BT_SNDP_DM_AI_EN);

config(CHECK, "声加双麦1+1降噪", "config.h打开BT_SNDP_FBDM_EN", BT_SNDP_FBDM_EN, 0, BT_NR_EN);
config(BYTE, "稳态噪声降噪量", "单位：dB", SNDP_FBDM_LEVEL, 0, 29, 10, BT_SNDP_FBDM_EN);
config(BYTE, "DNN降噪量", "单位：dB", SNDP_FBDM_DNN_LEVEL, 0, 29, 26, BT_SNDP_FBDM_EN);
config(BYTE, "混音阈值", "混音阈值", SNDP_FBDM_NOISERMS_TH0, 0, 100, 61, BT_SNDP_FBDM_EN);

config(CHECK, "自研单麦AI降噪", "config.h打开BT_DNN_EN", BT_DNN_EN, 0, BT_NR_EN);
config(BYTE, "通话降噪量", "参考值为0，值越大平稳噪声越少", BT_DNN_LEVEL, 0, 40, 0, BT_DNN_EN);

config(CHECK, "自研耳夹1+1降噪", "config.h打开BT_SCO_FFDM_EN", BT_FFDM_EN, 0, BT_NR_EN);
config(BYTE, "耳夹1+1间距", "出音孔之间的直线距离（20~40mm）", BT_FFDM_DISTANCE, 20, 40, 23, BT_FFDM_EN);

config(CHECK, "自研双麦/长麦AI降噪", "config.h打开BT_SCO_DMNS_EN或者BT_SCO_DMDNN_EN", BT_DMNS_EN, 0, BT_NR_EN);
config(BIT, "双麦间距", "出音孔之间的直线距离（20~200mm）", BT_DMNS_DISTANCE, 16, 20, 200, 23, BT_DMNS_EN);
config(BYTE, "通话降噪量", "参考值为0，值越大平稳噪声越少", BT_DMNS_LEVEL, 0, 20, 0, BT_DMNS_EN);

config(LEVEL, 0x00);
config(SUB, "音乐配置", "音乐播放模式的相关配置", FUNC_MUSIC_EN);
config(CHECK, "SDCARD播放功能", "是否支持SD/MMC卡播放功能", MUSIC_SDCARD_EN, 0, FUNC_MUSIC_EN);
config(CHECK, "UDISK播放功能", "是否支持UDISK播放功能", MUSIC_UDISK_EN, 0, FUNC_MUSIC_EN);
config(LISTVAL, "SDCARD检测IO选择", "选择SDCARD检测GPIO", SDDET_IOSEL, BIT, 6, 21, 24, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), ("复用SDCLK检测", 60), ("复用SDCMD检测", 61), ("复用PWRKEY检测", 62),("软件检测", 63), MUSIC_SDCARD_EN);
config(LISTVAL, "SDCARD1检测IO选择", "选择SDCARD1检测GPIO(双SD卡, 默认宏关闭的)", SD1DET_IOSEL, BIT, 6, 0, 26, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), ("PF2", 27), ("PF3", 28), ("复用SDCLK检测", 60), ("复用SDCMD检测", 61), ("复用PWRKEY检测", 62), MUSIC_SDCARD_EN);

config(SUB, "AUX配置", "AUX模式的相关配置", FUNC_AUX_EN);
config(CHECK, "插入LINEIN关机", "是否使能插入LINEIN, 直接关机功能，大耳包功能", LINEIN_2_PWRDOWN_EN, 0, FUNC_AUX_EN);
config(CHECK, "模式切换进AUX模式", "有LINEIN检测时，LINEIN线没有插入，是否能通过模式转换进入AUX模式。", MODE_2_AUX_EN, 0, FUNC_AUX_EN);
config(LISTVAL, "AUX模拟增益", "AUX模拟增益选择", AUX_ANL_GAIN, BIT, 4, 3, 8, ("-12DB", 0), ("-9DB", 1), ("-6DB", 2), ("-3DB", 3), ("0DB", 4), ("+6DB", 5), ("+9DB", 6), ("+12DB", 7), FUNC_AUX_EN);
config(BIT, "AUX数字增益(0~39DB)", "SDADC数字增益, Step 1DB", AUX_DIG_GAIN, 6, 0, 39, 0, FUNC_AUX_EN);
config(CHECK, "合并通路输入音频", "hybird mode 会使用4mic, 左右声道合并输入一个adc", AUX_COMB_EN, 0);
config(LISTVAL, "aux io组合选择", "选择对应aux 输入io", AUX_COMB_SEL, BIT, 2, 0, 3, ("None", 0), ("PE6\PE7", 1), ("PA6\PA7", 2), AUX_COMB_EN);
config(CHECK, "独立通路输入音频", "aux io单独输入音频，区分左右声道", AUX_DIFF_EN, 0);
config(LISTVAL, "AUXL通路选择", "AUX左声道通路选择", AUXL_SEL, BIT, 3, 0, 3, ("无AUXL输入", 0), ("AUXL0_PE6", 1), ("AUXL1_PA6", 2), AUX_DIFF_EN);
config(LISTVAL, "AUXR通路选择", "AUX右声道通路选择", AUXR_SEL, BIT, 3, 0, 3, ("无AUXR输入", 0), ("AUXR0_PE7", 1), ("AUXR1_PA7", 2), AUX_DIFF_EN);
config(LISTVAL, "LINEIN插入检测配置", "选择LINEIN检测GPIO，或Disable LINEIN检测", LINEIN_DET_IOSEL, BIT, 6, 0, 26, ("None", 0), ("PA3", 4), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5/WKO", 14), ("PE0", 17), ("PE1", 18), ("PE2", 19), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), ("PF2", 27), ("PF3", 28), ("复用SDCLK检测", 60), ("复用SDCMD检测", 61), ("复用PWRKEY检测", 62), FUNC_AUX_EN);
config(LEVEL, 0x03);

config(SUB, "按键配置", "选择按键及按键功能配置");
config(CHECK, "PWRKEY功能", "是否使用PWRKEY，使用软开关机时必须使能", USER_PWRKEY_EN, 1);
config(CHECK, "ADKEY1功能", "是否使用第一组ADKEY", USER_ADKEY_EN, 0);
config(CHECK, "ADKEY2功能", "是否使用第二组ADKEY", USER_ADKEY2_EN, 0);
config(CHECK, "IOKEY功能", "是否使用IOKEY", USER_IOKEY_EN, 0);
config(LEVEL, 0x00);
config(CHECK, "复用SDCLK的ADKEY", "是否使用复用SDCLK的ADKEY按键", USER_ADKEY_MUX_SDCLK_EN, 0);
config(LEVEL, 0x03);
config(LISTVAL, "ADKEY1通路选择", "第一组ADKEY io mapping select", ADKEY_CH,   BIT, 4, 9, 12, ("ADCCH_PA5", 0), ("ADCCH_PA6", 1), ("ADCCH_PA7", 2), ("ADCCH_PB1", 3), ("ADCCH_PB2", 4), ("ADCCH_PB3", 5), ("ADCCH_PB4", 6), ("ADCCH_PE5", 7), ("ADCCH_PE6", 8), ("ADCCH_PE7", 9), ("ADCCH_PF0", 10), ("ADCCH_PB0", 11), USER_ADKEY_EN);
config(LEVEL, 0x00);
config(LISTVAL, "ADKEY2通路选择", "第二组ADKEY io mapping select", ADKEY2_CH,  BIT, 4, 9, 12, ("ADCCH_PA5", 0), ("ADCCH_PA6", 1), ("ADCCH_PA7", 2), ("ADCCH_PB1", 3), ("ADCCH_PB2", 4), ("ADCCH_PB3", 5), ("ADCCH_PB4", 6), ("ADCCH_PE5", 7), ("ADCCH_PE6", 8), ("ADCCH_PE7", 9), ("ADCCH_PF0", 10), ("ADCCH_PB0", 11), USER_ADKEY2_EN);
config(LEVEL, 0x03);
config(CHECK, "按键多击功能", "是否使能按键多击（2/3/4/5击）功能", USER_KEY_MULTI_PRESS_EN, 1);
config(LISTVAL, "双击响应时间选择", "选择双击按键响应间隔时间", DOUBLE_KEY_TIME, BIT, 3, 1, 8, ("200ms", 0), ("300ms", 1), ("400ms", 2), ("500ms", 3), ("600ms", 4), ("700ms", 5), ("800ms", 6), ("900ms", 7), USER_KEY_MULTI_PRESS_EN);
config(LISTVAL, "USER_DEF键短按", "选择USER_DEF键短按的功能", USER_DEF_KS_SEL,    BIT, 4, 11, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键长按", "选择USER_DEF键长按的功能", USER_DEF_KL_SEL,    BIT, 4, 2, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键双击", "选择USER_DEF键双击的功能", USER_DEF_KD_SEL,    BIT, 4, 1, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键三击", "选择USER_DEF键三击的功能", USER_DEF_KT_SEL, 	  BIT, 4, 0, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键四击", "选择USER_DEF键四击的功能", USER_DEF_KFOUR_SEL, BIT, 4, 0, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(LISTVAL, "USER_DEF键五击", "选择USER_DEF键五击的功能", USER_DEF_KFIVE_SEL, BIT, 4, 0, 15, ("无", 0), ("回拨", 1), ("SIRI", 2), ("PREV", 3), ("NEXT", 4), ("VOL+", 5), ("VOL-", 6), ("MODE", 7), ("HID拍照", 8), ("IOS HOME", 9), ("中英文切换", 10), ("PLAY/PAUSE", 11), ("CBT_TEST", 12), ("低延时", 13), ("降噪控制", 14));
config(CHECK, "USER_DEF键长按按键音", "选择USER_DEF键长按功能时，是否滴一下提示", USER_DEF_KL_TONE_EN, 1);
config(CHECK, "USER_DEF键双击切语言", "蓝牙未连接手机时，双击切换中英文语音", USER_DEF_KD_LANG_EN, 1);
config(CHECK, "USER_DEF键区分左右耳", "USER_DEF键区分左右耳的功能，配置中为左耳功能（例如左耳上一曲，则右耳为下一曲，音量加减同理）", USER_DEF_LR_EN, 0);

config(CHECK, "支持多击的按键定制", "是否配置支持多击的按键，不勾选使用SDK默认的多击按键配置列表", KEY_MULTI_CONFIG_EN, 0, USER_KEY_MULTI_PRESS_EN);
config(LISTVAL, "多击按键1选择", "支持多击第1个按键", DBLKEY_NUM0, BYTE, 20, 22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键2选择", "支持多击第2个按键", DBLKEY_NUM1, BYTE, 21, 22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键3选择", "支持多击第3个按键", DBLKEY_NUM2, BYTE, 0,  22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键4选择", "支持多击第4个按键", DBLKEY_NUM3, BYTE, 0,  22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);
config(LISTVAL, "多击按键5选择", "支持多击第5个按键", DBLKEY_NUM4, BYTE 0,  22, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), KEY_MULTI_CONFIG_EN);

config(CHECK, "PWRKEY按键定制", "是否配置PWRKEY按键功能，不勾选使用SDK默认按键配置", PWRKEY_CONFIG_EN, 1, USER_PWRKEY_EN);
config(LISTVAL, "PWRKEY按键1功能", "PWRKEY第1个按键功能定制", PWRKEY_NUM0, BYTE, 20, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键2功能", "PWRKEY第2个按键功能定制", PWRKEY_NUM1, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24),  PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键3功能", "PWRKEY第3个按键功能定制", PWRKEY_NUM2, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24),  PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键4功能", "PWRKEY第4个按键功能定制", PWRKEY_NUM3, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24),  PWRKEY_CONFIG_EN);
config(LISTVAL, "PWRKEY按键5功能", "PWRKEY第5个按键功能定制", PWRKEY_NUM4, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24),  PWRKEY_CONFIG_EN);

config(CHECK, "ADKEY1按键定制", "是否配置第一组ADKEY按键功能，不勾选使用SDK默认按键配置", ADKEY_CONFIG_EN, 0, USER_ADKEY_EN);
config(LISTVAL, "ADKEY1按键1功能", "ADKEY1第1个按键功能定制", ADKEY1_NUM0, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键2功能", "ADKEY1第2个按键功能定制", ADKEY1_NUM1, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键3功能", "ADKEY1第3个按键功能定制", ADKEY1_NUM2, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键4功能", "ADKEY1第4个按键功能定制", ADKEY1_NUM3, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键5功能", "ADKEY1第5个按键功能定制", ADKEY1_NUM4, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键6功能", "ADKEY1第6个按键功能定制", ADKEY1_NUM5, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键7功能", "ADKEY1第7个按键功能定制", ADKEY1_NUM6, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键8功能", "ADKEY1第8个按键功能定制", ADKEY1_NUM7, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键9功能", "ADKEY1第9个按键功能定制", ADKEY1_NUM8, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键10功能", "ADKEY1第10个按键功能定制", ADKEY1_NUM9, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键11功能", "ADKEY1第11个按键功能定制", ADKEY1_NUM10, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);
config(LISTVAL, "ADKEY1按键12功能", "ADKEY1第12个按键功能定制", ADKEY1_NUM11, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), ADKEY_CONFIG_EN);

config(CHECK, "IOKEY按键定制", "是否配置IOKEY，不勾选使用SDK默认按键配置", IOKEY_CONFIG_EN, 0, USER_IOKEY_EN);
config(LISTVAL, "IOKEY按键1的IO", "IOKEY第1个按键的GPIO口选择", IOKEY_IO0, BIT, 5, 0, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键2的IO", "IOKEY第2个按键的GPIO口选择", IOKEY_IO1, BIT, 5, 0, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键3的IO", "IOKEY第3个按键的GPIO口选择", IOKEY_IO2, BIT, 5, 0, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键4的IO", "IOKEY第4个按键的GPIO口选择", IOKEY_IO3, BIT, 5, 0, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键5的IO", "IOKEY第5个按键的GPIO口选择", IOKEY_IO4, BIT, 5, 0, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), IOKEY_CONFIG_EN);

config(LISTVAL, "IOKEY按键1功能", "IOKEY第1个按键功能定制", IOKEY_NUM0, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键2功能", "IOKEY第2个按键功能定制", IOKEY_NUM1, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键3功能", "IOKEY第3个按键功能定制", IOKEY_NUM2, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键4功能", "IOKEY第4个按键功能定制", IOKEY_NUM3, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), IOKEY_CONFIG_EN);
config(LISTVAL, "IOKEY按键5功能", "IOKEY第5个按键功能定制", IOKEY_NUM4, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), IOKEY_CONFIG_EN);
config(LISTVAL, "两IO中间的按键功能", "IOKEY1与IOKEY2两IO中间的按键", IOKEY_MIDKEY_NUM, BYTE, 0, 25, ("None", 0), ("P/P", 1), ("PWR", 2), ("NEXT/VOL+", 3), ("PREV/VOL-", 4), ("NEXT/VOL-", 5), ("PREV/VOL+", 6), ("VOL+/NEXT", 7), ("VOL-/PREV", 8), ("VOL-/NEXT", 9), ("VOL+/PREV", 10), ("VOL+", 11), ("VOL-", 12), ("NEXT", 13), ("PREV", 14), ("MODE", 15), ("HSF", 16), ("MODE/PWR", 17), ("SIRI", 18), ("HOME", 19), ("PP/PWR/USER_DEF", 20), ("PP/USER_DEF", 21), ("VOL+/VOL-", 22), ("ANC", 23), ("REC", 24), IOKEY_CONFIG_EN);

config(SUB, "LED灯配置", "选择及配置LED状态指示灯");
config(CHECK, "系统指示灯(蓝灯)", "是否使用系统状态指示灯", LED_DISP_EN, 1);
config(CHECK, "电源状态灯(红灯)", "是否使用充电/电源指示灯", LED_PWR_EN, 1);
config(CHECK, "省电阻1个IO推两个灯", "省电阻1个IO推两个灯，不能兼容升级功能", PORT_2LED_RESLESS_EN, 1, LED_DISP_EN);
config(CHECK, "电池低电闪灯", "电池低电时，红灯或蓝灯是否闪烁？", RLED_LOWBAT_EN, 0, LED_DISP_EN);
config(LISTVAL, "蓝灯IO选择", "蓝灯的GPIO口选择", BLED_IO_SEL, BIT, 5, 25, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), LED_DISP_EN);
config(LISTVAL, "红灯IO选择", "红灯的GPIO口选择", RLED_IO_SEL, BIT, 5, 25, 18, ("None", 0), ("PA4", 5), ("PA5", 6), ("PA6", 7), ("PA7", 8), ("PB0", 9), ("PB1", 10), ("PB2", 11), ("PB3", 12), ("PB4", 13), ("PB5", 14), ("PE0", 17), ("PE4", 21), ("PE5", 22), ("PE6", 23), ("PE7", 24), ("PF0", 25), ("PF1", 26), LED_PWR_EN);

config(CHECK, "自定义LED闪灯状态", "是否需要自定义LED闪灯状态", LED_STA_CONFIG_EN, 0);
config(LED, "开机", "配置开机状态下的闪灯状态", LED_POWERON, 0x02, 0x01, 10, 255, 		LED_STA_CONFIG_EN);
config(LED, "关机", "配置关机状态下的闪灯状态", LED_POWEROFF, 0x40, 0x15, 4, 255, 		LED_STA_CONFIG_EN);
config(LED, "初始化", "配置蓝牙初始化状态下的闪灯状态", LED_BTINIT, 0x00, 0xaa, 10, 0, 	LED_STA_CONFIG_EN);
config(LED, "回连", "配置回连状态下的闪灯状态", LED_RECONNECT, 0x00, 0xaa, 6, 0, 		LED_STA_CONFIG_EN);
config(LED, "配对", "配置配对状态下的闪灯状态", LED_PAIRING, 0x60, 0x03, 3, 6, 			LED_STA_CONFIG_EN);
config(LED, "已连接", "配置已连接状态下的闪灯状态", LED_CONNECTED, 0x00, 0x02, 2, 86, 	LED_STA_CONFIG_EN);
config(LED, "播放", "配置播放状态下的闪灯状态", LED_PLAYING, 0x00, 0x02, 2, 86, 		LED_STA_CONFIG_EN);
config(LED, "来电", "配置来电状态下的闪灯状态", LED_RING, 0x00, 0x02, 2, 86, 			LED_STA_CONFIG_EN);
config(LED, "通话", "配置通话状态下的闪灯状态", LED_CALLING, 0x00, 0x02, 2, 86, 		LED_STA_CONFIG_EN);
config(LED, "低电", "配置低电状态下的闪灯状态", LED_LOWBAT, 0xaa, 0x00, 6, 0, 			LED_STA_CONFIG_EN);


config(CHECK,  "是否使能NTC",  "是否使能NTC",  NTC_EN,  0);
config(BYTE,  "充电高温保护的温度",  "充电高温保护的温度",  NTC_CHARGE_THD_RISK_HIGH,  0, 255, 200,  NTC_EN);
config(BYTE,  "充电高温正常的温度",  "充电高温正常的温度，比充电高温保护温度低5度",  NTC_CHARGE_THD_NORMAL_HIGH,  0, 255, 190,  NTC_EN);
config(BYTE,  "充电低温正常的温度",  "充电低温正常的温度，比充电低温保护温度高5度",  NTC_CHARGE_THD_NORMAL_LOW,  0, 255, 20,  NTC_EN);
config(BYTE,  "充电低温保护的温度",  "充电低温保护的温度",  NTC_CHARGE_THD_RISK_LOW,  0, 255, 10,  NTC_EN);
config(BYTE,  "放电高温保护的温度",  "放电高温保护的温度",  NTC_DISCHARGE_THD_RISK_HIGH,  0, 255, 200,  NTC_EN);
config(BYTE,  "放电低温保护的温度",  "放电低温保护的温度",  NTC_DISCHARGE_THD_RISK_LOW,  0, 255, 10,  NTC_EN);

config(LEVEL, 0x0E);
config(SUB, "产测参数", "产测校准的一些参数，不要改动！");
config(KEEP);
config(BYTE, "产测OSCI电容", "产测校准的24M晶振OSCI负载电容，单位0.25PF。不要改动默认值0。", OSCI_CAP, 0, 63, 0);
config(BYTE, "产测OSCO电容", "产测校准的24M晶振OSCO负载电容，单位0.25PF。不要改动默认值0。", OSCO_CAP, 0, 63, 0);
#config(BUF, "授权密钥", "程序运行需要判断的合法授权密钥", SOFT_KEY1, 20);
#config(BUF, "授权密钥", "程序运行需要判断的合法授权密钥", SOFT_KEY2, 20);
config(KEEPEND)

config(LEVEL, 0x100);
makecfgfile(xcfg.bin);
makecfgdef(xcfg.h);
xcopy(xcfg.h, ../../xcfg.h);
