/*
 *  irrx.h
 *
 *  Created by zoro on 2021-3-12.
 */
#ifndef __API_IRRX_H
#define __API_IRRX_H

//IRRX IO
enum {
    IRRX_PA7    = 1,            //IR_G1
    IRRX_PA8,
    IRRX_PA10,
    IRRX_PB2,
    IRRX_PB6,
    IRRX_PB7,
    IRRX_PB13,
    IRRX_PE4,
    IRRX_PE10,
    IRRX_PE13,
};

typedef struct {
    uint8_t ir_port;            //IRRX IO
    uint8_t clock_sel;          //rsvd
} irrx_t;

void irrx_init(irrx_t *irrx);   //硬件IRRX初始化
void irrx_exit(void);           //硬件IRRX关闭

//使用时，需要在APP中实现两个回调函数
//void ir_rx_press_cb(uint16_t addr, uint16_t cmd) {}     //IRRX收到一个有效addr, cmd
//void ir_rx_release_cb(void) {}                          //IRRX超时或按键松开

#endif //__API_IRRX_H
