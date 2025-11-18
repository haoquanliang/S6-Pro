#ifndef __AB_MATE_PROFILE_H
#define __AB_MATE_PROFILE_H

#define AB_MATE_VID     2           //广播包协议版本号
#if SWETZ
#define AB_MATE_BID     0x000400    //代理商和客户ID，0表示原厂bluetrum
#else
#define AB_MATE_BID     0x000000    //代理商和客户ID，0表示原厂bluetrum
#endif


//B6632277-0642-458B-A7A0-23FB1DC92C93
//#define AB_MATE_SPP_UUID    0xB6, 0x63, 0x22, 0x77, 0x06, 0x42, 0x45, 0x8B, 0xA7, 0xA0, 0x23, 0xFB, 0x1D, 0xC9, 0x2C, 0x93


void ab_mate_update_ble_adv_bt_sta(u8 val, u8 proc);

#endif
