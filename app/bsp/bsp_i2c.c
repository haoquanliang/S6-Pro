#include "include.h"

#if I2C_SW_EN
//AT(.text.bsp.i2c)
//static void bsp_i2c_delay(void)
//{
//    u8 delay = 60;
//    while (delay--) {
//        asm("nop");
//    }
//}
#define bsp_i2c_delay() delay_us(5)

//ACK: The transmitter releases the SDA line (HIGH->LOW) during the acknowledge clock pulse
AT(.text.bsp.i2c)
void bsp_i2c_tx_ack(void)
{
    I2C_SDA_OUT();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SCL_L();
}

AT(.text.bsp.i2c)
bool bsp_i2c_rx_ack(void)
{
    bool ret = false;
    I2C_SDA_IN();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    if (!I2C_SDA_IS_H()) {
        ret = true;
    }
    I2C_SCL_L();
    return ret;
}

//NACK: The transmitter holds the SDA line (keep HIGH) during the acknowledge clock pulse
AT(.text.bsp.i2c)
void bsp_i2c_tx_nack(void)
{
    I2C_SDA_OUT();
    I2C_SDA_H();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SCL_L();
}

//START: A HIGH to LOW transition on the SDA line while SCL is HIGH is one such unique case.
AT(.text.bsp.i2c)
void bsp_i2c_start(void)
{
#if I2C_MUX_SD_EN
    if (sdcard_detect_is_busy()) {
        return;
    }
    if (FUNCMCON0 & 0x0f) {
        FUNCMCON0 = 0x0f;
        delay_us(5);
    }
#endif // I2C_MUX_SD_EN

    I2C_SDA_SCL_OUT();
    I2C_SDA_SCL_H();
    bsp_i2c_delay();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_L();
}

//STOP: A LOW to HIGH transition on the SDA line while SCL is HIGH
AT(.text.bsp.i2c)
void bsp_i2c_stop(void)
{
    I2C_SDA_OUT();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SDA_H();
}

//tx 1byte
AT(.text.bsp.i2c)
void bsp_i2c_tx_byte(uint8_t dat)
{
    u8 i;
    I2C_SDA_OUT();
    for (i=0; i<8; i++) {
        if (dat & BIT(7)) {
            I2C_SDA_H();
        } else {
            I2C_SDA_L();
        }
        bsp_i2c_delay();
        I2C_SCL_H();
        bsp_i2c_delay();
        I2C_SCL_L();
        dat <<= 1;
    }
}

//rx 1byte
AT(.text.bsp.i2c)
uint8_t bsp_i2c_rx_byte(void)
{
    u8 i, dat = 0;
    I2C_SDA_IN();
    for (i=0; i<8; i++) {
        bsp_i2c_delay();
        I2C_SCL_H();
        bsp_i2c_delay();
        dat <<= 1;
        if (I2C_SDA_IS_H()) {
            dat |= BIT(0);
        }
        I2C_SCL_L();
    }
    return dat;
}

AT(.text.bsp.i2c)
void bsp_i2c_init(void)
{
    I2C_SDA_SCL_OUT();
    I2C_SDA_H();
    delay_5ms(2);
}
#endif

#if I2C_HW_EN

//AT(.com_rodata.i2c)
//const char str[] = "i2c read data:%x\n";
//AT(.com_rodata.i2c)
//const char str1[] = "i2c send\n";

volatile uint8_t i2c_tx_done_flag;
volatile uint8_t i2c_rx_done_flag;

AT(.com_text.i2c) WEAK
void bsp_i2c_isr(void)
{
    if (IICCON0 & BIT(31)) {
        IICCON0 |= BIT(29);
        if (IICCON1 & RDATA) {     //read data done
//        printf(str, IICDATA);
            i2c_rx_done_flag = 1;
        }
        if (IICCON1 & WDATA) {     //send data done
//            printf(str1);
        }
        i2c_tx_done_flag = 1;
    }
}

static u32 bsp_i2c_config(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u32 dat)
{
    u16 timeout = 1000;
    while (!i2c_tx_done_flag && timeout--) {
        delay_us(1);
    }
    i2c_tx_done_flag = 0;

    IICCMDA = (u8)dev_addr | ((u32)(dev_addr >> 8)) << 24 |
              (u32)((u8)reg_addr) << 8 | (u32)((u8)(reg_addr >> 8)) << 16;

    IICCON1 = DATA_CNT_1B | i2c_cfg;
    IICDATA = dat;

    IICCON0 |= BIT(28);                     // kick
    if (i2c_cfg & RDATA) {
        timeout = 1000;
        while (!i2c_rx_done_flag && timeout--) {
            delay_us(1);
        }
        i2c_rx_done_flag = 0;
        return IICDATA;
    } else {
        return 0;
    }
}

void bsp_i2c_tx_byte(u16 dev_addr, u16 reg_addr, u32 data)
{
    bsp_i2c_config(START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA | STOP_FLAG, dev_addr, reg_addr, data);
}

void bsp_i2c_rx_buf(u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)
{
    int i;
    u32 cfg;
    if (buf == NULL || len == 0) {
        return;
    }

    for (i = 0; i < len; i++) {
        cfg = RDATA;
        if (i == 0) {               //收第1byte
            cfg |= START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
        }
        if (i == (len - 1)) {       //收最后1byte
            cfg |= STOP_FLAG | NACK;
        }
        buf[i] = bsp_i2c_config(cfg, dev_addr, reg_addr, 0);
    }
}

void bsp_i2c_init(void)
{
    CLKCON1 = (CLKCON1 & ~(1 << 27)) | (0 << 27);     //I2C clk select, 0:rc2m_clk, 1:x26m_clkdiv8
    CLKGAT0 |= BIT(11);                             //I2C clk enable
    RSTCON0 |= BIT(3);                              //I2C release reset, enable model function
    RTCCON3 &= ~BIT(6);
    FUNCMCON2 = 0xf << 24;


#if (I2C_MAPPING == I2CMAP_PA7PA6)
    GPIOADE |= BIT(7) | BIT(6);
    GPIOADIR |= BIT(7) | BIT(6);
    GPIOAPU |= BIT(7) | BIT(6);
    GPIOAFEN |= BIT(7) | BIT(6);
    FUNCMCON2 = I2CMAP_PA7PA6;
#elif (I2C_MAPPING == I2CMAP_PA5PA6)
    GPIOADE |= BIT(5) | BIT(6);
    GPIOADIR |= BIT(5) | BIT(6);
    GPIOAPU |= BIT(5) | BIT(6);
    GPIOAFEN |= BIT(5) | BIT(6);
    FUNCMCON2 = I2CMAP_PA5PA6;
#elif (I2C_MAPPING == I2CMAP_PB2PB1)
    GPIOBDE |= BIT(2) | BIT(1);
    GPIOBDIR |= BIT(2) | BIT(1);
    GPIOBPU |= BIT(2) | BIT(1);
    GPIOBFEN |= BIT(2) | BIT(1);
    FUNCMCON2 = I2CMAP_PB2PB1;
#elif (I2C_MAPPING == I2CMAP_PB0PB1)
    GPIOBDE |= BIT(0) | BIT(1);
    GPIOBDIR |= BIT(0) | BIT(1);
    GPIOBPU |= BIT(0) | BIT(1);
    GPIOBFEN |= BIT(0) | BIT(1);
    FUNCMCON2 = I2CMAP_PB0PB1;
#elif (I2C_MAPPING == I2CMAP_PE7PE6)
    GPIOEDE |= BIT(7) | BIT(6);
    GPIOEDIR |= BIT(7) | BIT(6);
    GPIOEPU |= BIT(7) | BIT(6);
    GPIOEFEN |= BIT(7) | BIT(6);
    FUNCMCON2 = I2CMAP_PE7PE6;
#elif (I2C_MAPPING == I2CMAP_PE5PE6)
    GPIOEDE |= BIT(5) | BIT(6);
    GPIOEDIR |= BIT(5) | BIT(6);
    GPIOEPU |= BIT(5) | BIT(6);
    GPIOEFEN |= BIT(5) | BIT(6);
    FUNCMCON2 = I2CMAP_PE5PE6;
#elif (I2C_MAPPING == I2CMAP_PB4PB3)
    GPIOBDE |= BIT(4) | BIT(3);
    GPIOBDIR |= BIT(4) | BIT(3);
    GPIOBPU |= BIT(4) | BIT(3);
    GPIOBFEN |= BIT(4) | BIT(3);
    FUNCMCON2 = I2CMAP_PB4PB3;
#elif (I2C_MAPPING == I2CMAP_PF1PF0)
    GPIOFDE |= BIT(0) | BIT(1);
    GPIOFDIR |= BIT(0) | BIT(1);
    GPIOFPU |= BIT(0) | BIT(1);
    GPIOFFEN |= BIT(0) | BIT(1);
    FUNCMCON2 = I2CMAP_PF1PF0;
#endif

    i2c_tx_done_flag = 1;
    i2c_rx_done_flag = 0;

//    IICCON1 =   1 << 0 | // data cnt
//                1 << 3 | // start0 en
//                1 << 4 | // ctl0 en
//                1 << 5 | // adr0 en
//                0 << 6 | // adr1 en
//                0 << 7 | // start1 en
//                0 << 8 | // ctl1 en
//                0 << 9 | // rdat en
//                1 << 10| // wdat en
//                1 << 11| // stop en
//                0 << 12; // txnaken

    IICCON0 =   1 << 0 | // iicen
                1 << 1 | // iic int
                1 << 2 | // hold cnt[3:2]
                9 << 4 ; // pos div [9:4]
}
#else
AT(.com_text.i2c) WEAK
void bsp_i2c_isr(void)
{
}
#endif
