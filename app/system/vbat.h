#ifndef __VBAT_H
#define __VBAT_H


void vbat_init(void);
void vbat_voltage_init(void);
uint16_t vbat_get_voltage(void);
void sys_dump_vbat(void);

int is_lowpower_vbat_warning(void);

#endif // __VBAT_H
