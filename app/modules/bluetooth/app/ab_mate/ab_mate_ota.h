#ifndef __AB_MATE_OTA_H
#define __AB_MATE_OTA_H

void ab_mate_ota_init(void);
bool ab_mate_ota_is_start(void);
void ab_mate_ota_tws_data_proc(uint8_t *data_ptr, u16 size);
void ab_mate_ota_proc(u8 cmd,u8 *payload,u8 payload_len);
void ab_mate_ota_seq_err_notify(void);
void ab_mate_ota_process(void);
void ab_mate_ota_disconnect_callback(void);

#endif
