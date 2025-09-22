#include "include.h"

/***************************************************
WAKEUP SOURCE   0   PA15
WAKEUP SOURCE   1   PB1
WAKEUP SOURCE   2   PB2
WAKEUP SOURCE   3   PE0
WAKEUP SOURCE   4   PE7
WAKEUP SOURCE   5   PB5

WAKEUP SOURCE   6   PORT INT SOURCE FALL
WAKEUP SOURCE   7   PORT INT SOURCE RISE
---> PORT INT SOURCE FALL/RISE = {
//src0
PB[15:0], PA[15:0],
//src1
PE[13:0],
};
***************************************************/

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if 0

//GPIO中断示例

AT(.com_text.port)
void port_isr(void)
{
//    if ((io_num == IO_PA15) || (io_num == IO_PB1) || (io_num == IO_PB2) || (io_num == IO_PE0) || (io_num == IO_PE7)) {
//        if (io_num == IO_PA15) {
//            if (WKUPEDG & (BIT(0) << 16)) {
//                WKUPCPND = (BIT(0) << 16);  //CLEAR PENDING
//                port_int34_callback(io_num);
//            }
//        } else if (io_num == IO_PB1) {
//            if (WKUPEDG & (BIT(1) << 16)) {
//                WKUPCPND = (BIT(1) << 16);  //CLEAR PENDING
//                port_int34_callback(io_num);
//            }
//        } else if (io_num == IO_PB2) {
//            if (WKUPEDG & (BIT(2) << 16)) {
//                WKUPCPND = (BIT(2) << 16);  //CLEAR PENDING
//                port_int34_callback(io_num);
//            }
//        } else if (io_num == IO_PE0) {
//            if (WKUPEDG & (BIT(3) << 16)) {
//                WKUPCPND = (BIT(3) << 16);  //CLEAR PENDING
//                port_int34_callback(io_num);
//            }
//        } else if (io_num == IO_PE7) {
//            if (WKUPEDG & (BIT(4) << 16)) {
//                WKUPCPND = (BIT(4) << 16);  //CLEAR PENDING
//                port_int34_callback(io_num);
//            }
//        }
//    }else{
//        if (WKUPEDG & (BIT(6) << 16)) { //PORT INT
//            WKUPCPND = (BIT(6) << 16);  //CLEAR PENDING
//            port_int_fall_callback();
//        }
//        if (WKUPEDG & (BIT(7) << 16)) { //PORT INT
//            WKUPCPND = (BIT(7) << 16);  //CLEAR PENDING
//            port_int_rise_callback();
//        }
//    }
}

void bsp_port_int_io_init(gpio_t *g, u8 io_num, u8 edg)
{
    bsp_gpio_cfg_init(g, io_num);
    if (g->sfr == NULL) {
        return;
    }
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxDIR] |= BIT(g->num);
    g->sfr[GPIOxFEN] &= ~BIT(g->num);
    g->sfr[GPIOxPD - edg] |= BIT(g->num);
}

#define PINT0_EN        BIT(0)      //bit[31:16]=PB[15:0], bit[15:0]=PA[15:0]
#define PINT0_EDG       BIT(0)
#define PINT1_EN        BIT(0)      //bit[31:16]=PE[15:0], bit[15:0]=PF[15:0]S
#define PINT1_EDG       BIT(0)

//测试OK
void port_int_init(void)
{
    sys_irq_init(IRQ_PORT_VECTOR, 0, port_isr);

    PORTINTEN = PINT0_EN;
    PORTINTEDG = PINT0_EDG;
    PORTINTEN1 = PINT1_EN;
    PORTINTEDG1 = PINT1_EDG;
//    WKUPEDG |= port_int_cb.wkupedg;
//    WKUPCON = port_int_cb.wkupcon | BIT(16);
}

#endif
