#ifndef _IR_KEY_H
#define _IR_KEY_H

///红外遥控器地址码
#define IR_ADDR_FF00      0xFF00
#define IR_ADDR_BF00      0xBF00
#define IR_ADDR_FD02      0xFD02
#define IR_ADDR_FE01      0xFE01
#define IR_ADDR_7F80      0x7F80

extern const u8 ir_tbl_FF00[96];
extern const u8 ir_tbl_BF00[32];
extern const u8 ir_tbl_FD02[32];
extern const u8 ir_tbl_FE01[32];
extern const u8 ir_tbl_7F80[32];

void irkey_init(void);
void irkey_clr_state(void);
uint8_t irkey_get_val(void);

#endif // _IR_KEY_H
