#ifndef _USB_TABLE_H
#define _USB_TABLE_H

extern const u8 user_usb_vid_pid[4];
extern const u8 user_usb_manufacturer[16];
extern const u8 user_usb_product[28];
extern const u8 user_usb_serial_number[30];

#if USB_USER_CFG_DEV_EN
//USB Audio Feature Unit ID
#define UID_SPK                         0x02
#define UID_MIC                         0x05

extern const u8 desc_usb_device[18];
extern const u8 str_language_id[4];
extern const u8 str_manufacturer[16];
extern const u8 str_product[28];
extern const u8 str_serial_number[30];

//UAC1.0
extern const u8 desc_config_general[9];
extern const u8 desc_config_storage[23];
extern const u8 desc_config_audio[19];
extern const u8 desc_config_audio_speaker[31];
extern const u8 desc_config_audio_mic[31];
extern const u8 desc_config_audio_dmic[31];
extern const u8 desc_config_speaker_stream[9];
extern const u8 desc_config_speaker_stream_16bit[24];
extern const u8 desc_config_speaker_stream_24bit[24];
extern const u8 desc_config_speaker_stream_32bit[24];
extern const u8 desc_spk_endpoint_tbl[16];
extern const u8 desc_config_mic_stream[9];
extern const u8 desc_config_mic_stream_16bit[24];
extern const u8 desc_config_mic_stream_24bit[24];
extern const u8 desc_mic_endpoint_tbl[16];
extern const u8 desc_8k_spl_tbl[3];
extern const u8 desc_44k_spl_tbl[3];
extern const u8 desc_48k_spl_tbl[3];
extern const u8 desc_96k_spl_tbl[3];

extern const u8 desc_config_hid[9];
extern const u8 desc_hid_desc[9];
extern const u8 desc_hid_ep_in[7];
extern const u8 desc_hid_ep_out[7];

extern const u8 str_ms_os_string[18];
extern const u8 str_ms_os_feature[16];

extern const u8 desc_config_audio_balance_tbl[13];
#endif

#endif
