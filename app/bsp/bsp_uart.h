#ifndef _BSP_UART_H
#define _BSP_UART_H

#define VH_DATA_LEN                     40

typedef struct {
    u16 header;
    u8  distinguish;                    //����ʶ����
    u8  cmd;
    u8  length;
    u8  buf[VH_DATA_LEN];
    u8  checksum;
    u8  cnt;
    u8  crc;
} vh_packet_t;

enum {
    UART1_TR_PA7    = 0,
    UART1_TR_PF0,
    UART1_TR_VUSB,
};

enum {
    UART2_TR_PE7    = 0,
    UART2_TR_PB2,
    UART2_TR_VUSB,
};


extern const u8 vusb_crc8_tbl[256];
void bsp_uart1_init(u32 baudrate);
void bsp_uart2_init(u32 baudrate);
void bsp_vusb_uart_dis(void);
u8 bsp_uart1_get(u8 *ch);
u8 bsp_uart2_get(u8 *ch);
u8 bsp_uart_packet_parse(vh_packet_t *p, u8 data);
void bt_get_local_bd_addr(u8 *addr);
void set_vusb_uart_flag(u8 flag);
extern vh_packet_t vh_packet;

#endif // _BSP_UART_H
