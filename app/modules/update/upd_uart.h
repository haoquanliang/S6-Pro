#ifndef __UPD_UART_H
#define __UPD_UART_H

void upd_uart_process(void);
u8 upd_uart_get_step(void);
void upd_uart_set_step(u8 step);
void upd_uart_check_update(void);
void upd_uart_init(void);
void upd_uart_check_process(u8 ch);
#endif
