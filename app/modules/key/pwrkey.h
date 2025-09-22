#ifndef _PWR_KEY_H
#define _PWR_KEY_H

void pwrkey_init(void);
void pwrkey_off(void);
uint8_t pwrkey_get_val(void);
bool pwrkey_get_status(void);

#endif // _PWR_KEY_H
