#ifndef __APP_ROLE_SWITCH_H_
#define __APP_ROLE_SWITCH_H_

void app_role_switch_by_bat(void);
void app_role_switch_by_inbox(void);
void app_role_switch_by_outbox(void);
uint16_t app_role_switch_get_user_data(uint8_t *data_ptr);
uint16_t app_role_switch_set_user_data(uint8_t *data_ptr, uint16_t len);

#endif
