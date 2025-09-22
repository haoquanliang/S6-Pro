#ifndef _API_SYSCLK_H_
#define _API_SYSCLK_H_

enum {
    SYS_RC2M,
    SYS_RC24M,
    SYS_24M,
    SYS_48M,
    SYS_60M,
    SYS_80M,
    SYS_100M,
    SYS_120M,
    SYS_160M,
};

enum {
    INDEX_VOICE,
    INDEX_DECODE,
    INDEX_STACK,
    INDEX_KARAOK,
    INDEX_RES_PLAY,
    INDEX_ENCODE,
    INDEX_MUSIC,
    INDEX_ANC,
    INDEX_GFPS,
    INDEX_MAX_NB = 9,
};

uint8_t sys_clk_get_cur(void);
uint8_t sys_clk_get(void);                              //获取主线程系统时钟
bool sys_clk_set(uint8_t sys_clk);                      //主线程设置系统时钟
bool sys_clk_req(uint8_t index, uint8_t sys_clk);       //其它线程请求系统时钟，一般在库内调用，跟sys_clk_free成对使用
bool sys_clk_free(uint8_t index);                       //其它线程释放系统时钟，一般在库内调用，跟sys_clk_req成对使用
bool sys_clk_free_all(void);                            //释放所有请求的系统时钟，一般关机前调用
u32 get_sysclk_nhz(void);

void clk_xosc_dis(void);
void clk_xosc_en(void);
void clk_pll0_dis(void);
void clk_pll0_en(void);
#endif
