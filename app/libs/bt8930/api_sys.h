#ifndef _API_SYS_H_
#define _API_SYS_H_

#define IRQ_TMR1_VECTOR                 4
#define IRQ_TMR2_VECTOR                 5
#define IRQ_IRRX_VECTOR                 6
#define IRQ_UART_VECTOR                 14
#define IRQ_TMR3_VECTOR                 16
#define IRQ_TMR4_VECTOR                 17
#define IRQ_TMR5_VECTOR                 18
#define IRQ_PORT_VECTOR                 26
#define IRQ_SPI_VECTOR                  20
#define IRQ_I2C_VECTOR                  30

#define SW_RST_DC_IN                    1
#define SW_RST_QTEST_NORMAL             2
#define SW_RST_QTEST_ONLY_PAIR          3
#define SW_RST_FLAG                     4 //不管什么情况，直接复位
#define SW_RST_DAC_SYNC                 5 //DAC sync

typedef void (*isr_t)(void);

//clk_out type
enum {
    DBG_CLK_NO,
    DBG_CLK_XOSC,
    DBG_CLK_XOSC_32K,
    DBG_CLK_OSC32K,
    DBG_CLK_PLL0DIV2,
    DBG_CLK_XOSC_X2CLK,
    DBG_CLK_PLL1DIV2,
    DBG_CLK_RC2M,
    DBG_CLK_RTC_RC2M,
    DBG_CLK_SYS_CLK,
    DBG_CLK_BT_CLK,
    DBG_CLK_PLL_TS_CLKO,
    DBG_CLK_DAC_CLKPP,
    DBG_CLK_TKA_OUT,
};

void sys_clk_output(u32 type, u32 mapping);                 //输出时钟, mapping参考CLKOMAP_xx

extern u16 tmr5ms_cnt;

int64_t __divdi3(int64_t num, int64_t den);
int mul_r(int x, int y, int frat);
int s_abs(int x);

u32 sys_get_rc2m_rtc_clk(void);

void sys_set_tmr_enable(bool tmr5ms_en, bool tmr1ms_en);
void sys_set_tmr_tick(bool tick_en);
u32 sys_get_rand_key(void);
void sys_run_loop(void);

void xosc_init(void);

bool sys_irq_init(int vector, int pr, isr_t isr);           //初始化一个中断, pr为优先级

u16 get_random(u16 num);                                    //获取[0, num-1]之间的随机数
void sys_clk_output(u32 type, u32 mapping);                 //输出时钟   type: 0->xosc26m, 1->x26m_13m, 2->12m, 3->24m

u16 convert_uni2gbk(u16 code, u32 addr, u32 len);           //输入utf16, 返回gbk编码, 找不到编码，使用'□'
u8 utf8_char_size(u8 code);                                 //返回UTF8占用几字节
u16 utf8_convert_to_unicode(u8 *in, u8 char_size);          //UTF-8转成UTF-16 LE编码

u32 sys_rst_init(bool wko10s_rst);
void sw_reset_kick(u8 source);
u8 sw_reset_source_get(void);
void set_uart0_baud(u32 baud);
void set_uart1_baud(u32 baud);
void set_uart1_baud_rc2m_clk(u32 baud);

void sys_pe4_power_gate_set(u8 on);         //PE4供电开关

bool bt_get_ft_trim_value(void *rf_param);
uint calc_crc(void *buf, uint len, uint seed);
void rtc_32k_configure(void);

void fs_lock(void);
void fs_unlock(void);

bool spiflash_read_kick(void *buf, u32 addr, uint len);
bool spiflash_read_wait(void);
void spiflash_lock(void);
void spiflash_unlock(void);

#define anc_alg_enter_critical()            fs_lock()
#define anc_alg_exit_critical()             fs_unlock()

//lv0: just sleep
//lv1: sleep with nomal xosc
//lv2: sleep with lowpwr xosc
//lv3: sleep with lowpwr buck
//lv4: sleep with lowpwr ldo
void sys_sleep_proc_lv(u8 lpclk_type, uint level);
void sys_sleep_proc_lv0(void);
u8 crc8_maxim(u8 *buf, u8 length);
extern const u8 crc8_tbl[256];

extern const uint16_t libs_version;
#define LIBS_VERSION  libs_version

//vusb 5v reset
#define vusb_reset_set_time(nsec)                  {VBRSTCON = ((nsec * 33) << 20);}                        //vusb delay n second
#define vusb_reset_kick_mode(x)                    {VBRSTCON = (VBRSTCON & ~(1<<30)) | ((x)<<30);}          //0:vusb HW kick, 1:soft kick
#define vusb_reset_soft_start_kick()               {RTCCPND |= BIT(30);}                                    // soft start kick
#define vusb_reset_clr()                           {RTCCPND = 3 << 28;}                                     //clr counter and pending, disable
#define vusb_reset_en(x)                           {RTCCON = (RTCCON & ~(1<<6)) | ((x)<<6);}                //0:reset disable, 1:reset enable

//vusb 4s reset
#define vusb4s_reset_is_recover()                   ((bool)(RTCCON12 & (3 << 30)))                          //VUSB拔出后是否恢复4S复位功能标志
#define vusb4s_reset_recover_clr()                  RTCCON10 = BIT(31)                                      //清除恢复4S复位功能标志
#define vusb4s_reset_dis()                          RTCCON12 |= 3 << 2;vusb4s_reset_recover_clr()           //关闭VUSB4S复位
#define vusb4s_reset_en()                           vusb4s_reset_recover_clr(); RTCCON12 &= ~(3 << 2); RTCCON11 |= BIT(4)   //打开VUSB4S复位和拔出恢复功能
#define vusb4s_reset_clr_cnt()                      RTCCON10 = BIT(15)                                      //清除VUSB4S计数

#endif // _API_SYS_H_

