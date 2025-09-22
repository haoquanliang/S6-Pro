#include "include.h"
#include "usb_enum.h"
#include "usb_table.h"

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#if FUNC_USBDEV_EN

extern uint8_t uda_balance_vol_en;
extern uint8_t uda_balance_id_sel;
extern uint8_t usb_mic_dch_en;
// extern uint8_t usb_remote_wakeup_en;
extern uint8_t usb_spk_sync_mode_en;
extern uint8_t usb_spk_spl_en;
extern uint8_t usb_mic_spl_en;
extern uint8_t usb_spk_bits_en;
extern uint8_t usb_mic_bits_en;
extern const u8 desc_hid_report_typec[];

#if USB_USER_CFG_DEV_EN
u8 send_unstandard_command[512] AT(.udev_buf.buf);//EP0发送数据buf,host in, set_usb_send_cmd_len()对应此数组的长度
#endif

AT(.text.usb_cfg)
void usb_cfg_init(void)
{

}

AT(.usbdev.com)
const u8 *get_usb_dev_vid_pid(void)
{
    return (const u8 *)user_usb_vid_pid;
}

AT(.usbdev.com)
const u8 *get_usb_dev_supplier(void)
{
    return (const u8 *)user_usb_manufacturer;
}

AT(.usbdev.com)
const u8 *get_usb_dev_product_name(void)
{
    return (const u8 *)user_usb_product;
}

AT(.usbdev.com)
const u8 *get_usb_dev_serial_number(void)
{
    return (const u8 *)user_usb_serial_number;
}

#if USB_USER_CFG_DEV_EN

//ude_standard_command max len is 8byte
AT(.usbdev.com)
WEAK u8 *ude_standard_command(void)
{
    return 0;
}

//WEAK void ude_ep0_receiving_ex(u8 reqtype, u8 req, u16 val, u16 index,u8 *data, u8 len)
//{
//
//}

//EP0接收接口,host out, max data lenght is 64
void ude_ep0_receiving(u8 *data, u8 len)
{
    TRACE_R(data,len);
}

//EP0发送接口,host in, set_usb_send_cmd_len()对应此数组的长度
AT(.usbdev.com)
u8 *send_standard_command(void)
{
    return (u8 *)send_unstandard_command;
}

//处理USB EP0的命令
//return true user-defined, return false use the default
AT(.usbdev.com)
bool ude_deal_command(void)
{
#if 0
    //example:
    u8 *str = ude_standard_command();
    if(str[0]== 0 && str[1]== 1 && str[2]== 1 && str[3]== 0 && str[4]== 0 && str[5]== 0 && str[6]== 0 && str[7]== 0){  //wakeup
        func_usbdev_enter();
    }
#endif
#if 0
    //example:
    if(str[0] == 0x91){
        u8 date[64] = {0x02,0x02,0x03};
        memcpy(send_unstandard_command,date,3);
        set_usb_send_cmd_len(3);
        return true;                    //true发送数据
    }
#endif
    return false;
}

//获取配置描述符，并自动裁剪
void user_got_config_descriptor(void)
{
    u16 cfg_desc_len;
    u8 intf_num = 0;                                //usb device interface number.
    u8 *desc_ptr_start = get_desc_ptr();
    u8 *desc_ptr = get_desc_ptr();
    u8 type = get_type();
    u8 *save_ptr;

    //copy general config descriptor
    memcpy(desc_ptr, desc_config_general, sizeof(desc_config_general));
    // if(usb_remote_wakeup_en){
    //     desc_ptr[7] = 0xA0;
    // }
    desc_ptr += sizeof(desc_config_general);

#if UDE_STORAGE_EN
    if ((type & MASK_BULK) == MASK_STORAGE) {
        //copy mass storage config descriptor
        memcpy(desc_ptr, desc_config_storage, sizeof(desc_config_storage));
        desc_ptr[2] = intf_num++;                   //update Interface Number
        desc_ptr += sizeof(desc_config_storage);
    }
#endif // UDE_STORAGE_EN

#if (UDE_SPEAKER_EN || UDE_MIC_EN)
    if (type & MASK_ISO_AUDIO) {
        u16 audecs_headsize, audecs_size;

        //copy audio head interface config descriptor
        audecs_headsize = sizeof(desc_config_audio);
        memcpy(desc_ptr, desc_config_audio, audecs_headsize);
        desc_ptr[2] = intf_num++;
        if ((type & MASK_ISO_AUDIO) == MASK_ISO_AUDIO) {
            desc_ptr[9]   = 10;                        //10byte
            desc_ptr[9+7] = 2;                         //total 2 interface
            desc_ptr[9+8] = intf_num;                  //speaker streaming interface number
            desc_ptr[9+9] = intf_num + 1;              //mic streaming interface number
            audecs_size = sizeof(desc_config_audio_speaker) + sizeof(desc_config_audio_mic) + uda_balance_vol_en * sizeof(desc_config_audio_balance_tbl);

        } else {
            desc_ptr[9]   = 9;                         //9byte
            desc_ptr[9+7] = 1;                         //total 1 interface
            desc_ptr[9+8] = intf_num;                  //streaming interface number
            audecs_headsize -= 1;
            if (type & MASK_SPEAKER) {
                audecs_size = sizeof(desc_config_audio_speaker) + uda_balance_vol_en * sizeof(desc_config_audio_balance_tbl);
            } else {
                audecs_size = sizeof(desc_config_audio_mic);
            }
        }
        *(u16 *)&desc_ptr[9+5] = audecs_size + audecs_headsize - 9;
        desc_ptr += audecs_headsize;

    #if UDE_SPEAKER_EN
        if (type & MASK_SPEAKER) {
            //copy audio speaker descriptor
            if (uda_balance_vol_en) {
                u32 aci_desc_len = desc_config_audio_speaker[0];
                memcpy(desc_ptr, desc_config_audio_speaker, aci_desc_len);
                desc_ptr += aci_desc_len;
                memcpy(desc_ptr, desc_config_audio_balance_tbl, sizeof(desc_config_audio_balance_tbl));
                if(uda_balance_id_sel){
                    desc_ptr[3] = 0x05;//bUnitID
                }else{
                    desc_ptr[3] = 0x06;//bUnitID
                }
                desc_ptr += sizeof(desc_config_audio_balance_tbl);
                memcpy(desc_ptr, &desc_config_audio_speaker[aci_desc_len], sizeof(desc_config_audio_speaker) - aci_desc_len);
                desc_ptr[6] = 0x1;
                desc_ptr[7] = 0x2;
                desc_ptr[8] = 0x2;
                desc_ptr += sizeof(desc_config_audio_speaker) - aci_desc_len;
            } else {
                memcpy(desc_ptr, desc_config_audio_speaker, sizeof(desc_config_audio_speaker));
                desc_ptr += sizeof(desc_config_audio_speaker);
            }
        }
    #endif // UDE_SPEAKER_EN

    #if UDE_MIC_EN
        if (type & MASK_MIC) {
            //copy audio mic descriptor
            memcpy(desc_ptr, desc_config_audio_mic, sizeof(desc_config_audio_mic));
            if(usb_mic_dch_en){
                desc_ptr[7] = 0x02;//2 channel
                desc_ptr[8] = 0x03;//Left and Right Front
                desc_ptr += sizeof(desc_config_audio_mic);
            }else{
                desc_ptr[12] = 9;
                memmove(desc_ptr+20, desc_ptr+21, 10);
                desc_ptr += sizeof(desc_config_audio_mic) - 1;
            }
        }
    #endif // UDE_MIC_EN

    #if UDE_SPEAKER_EN
        if (type & MASK_SPEAKER) {
            //copy audio speaker interface streaming descriptor
            save_ptr = desc_ptr;
            memcpy(desc_ptr, desc_config_speaker_stream, sizeof(desc_config_speaker_stream));
            save_ptr[2] = intf_num;
            desc_ptr += sizeof(desc_config_speaker_stream);

            if(usb_spk_bits_en & USB_16BITS_EN){
                save_ptr = desc_ptr;
                memcpy(desc_ptr, desc_config_speaker_stream_16bit, sizeof(desc_config_speaker_stream_16bit));
                save_ptr[2] = intf_num;
                desc_ptr += sizeof(desc_config_speaker_stream_16bit);
                if(usb_spk_spl_en & SPL_8000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_8k_spl_tbl, sizeof(desc_8k_spl_tbl));
                    desc_ptr += sizeof(desc_8k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_44100_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_44k_spl_tbl, sizeof(desc_44k_spl_tbl));
                    desc_ptr += sizeof(desc_44k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_48000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_48k_spl_tbl, sizeof(desc_48k_spl_tbl));
                    desc_ptr += sizeof(desc_48k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_96000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_96k_spl_tbl, sizeof(desc_96k_spl_tbl));
                    desc_ptr += sizeof(desc_96k_spl_tbl);
                }
                memcpy(desc_ptr, desc_spk_endpoint_tbl, sizeof(desc_spk_endpoint_tbl));
                if(usb_spk_sync_mode_en){
                    desc_ptr[3] = 0x0D;//ISO, Sync, Data EP
                }
                desc_ptr += sizeof(desc_spk_endpoint_tbl);
            }

            if(usb_spk_bits_en & USB_24BITS_EN){
                save_ptr = desc_ptr;
                memcpy(desc_ptr, desc_config_speaker_stream_24bit, sizeof(desc_config_speaker_stream_24bit));
                save_ptr[2] = intf_num;
                desc_ptr += sizeof(desc_config_speaker_stream_24bit);
                if(usb_spk_spl_en & SPL_8000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_8k_spl_tbl, sizeof(desc_8k_spl_tbl));
                    desc_ptr += sizeof(desc_8k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_44100_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_44k_spl_tbl, sizeof(desc_44k_spl_tbl));
                    desc_ptr += sizeof(desc_44k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_48000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_48k_spl_tbl, sizeof(desc_48k_spl_tbl));
                    desc_ptr += sizeof(desc_48k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_96000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_96k_spl_tbl, sizeof(desc_96k_spl_tbl));
                    desc_ptr += sizeof(desc_96k_spl_tbl);
                }
                memcpy(desc_ptr, desc_spk_endpoint_tbl, sizeof(desc_spk_endpoint_tbl));
                if(usb_spk_sync_mode_en){
                    desc_ptr[3] = 0x0D;//ISO, Sync, Data EP
                }
                desc_ptr += sizeof(desc_spk_endpoint_tbl);
            }

            if(usb_spk_bits_en & USB_32BITS_EN){
                save_ptr = desc_ptr;
                memcpy(desc_ptr, desc_config_speaker_stream_32bit, sizeof(desc_config_speaker_stream_32bit));
                save_ptr[2] = intf_num;
                desc_ptr += sizeof(desc_config_speaker_stream_32bit);
                if(usb_spk_spl_en & SPL_8000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_8k_spl_tbl, sizeof(desc_8k_spl_tbl));
                    desc_ptr += sizeof(desc_8k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_44100_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_44k_spl_tbl, sizeof(desc_44k_spl_tbl));
                    desc_ptr += sizeof(desc_44k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_48000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_48k_spl_tbl, sizeof(desc_48k_spl_tbl));
                    desc_ptr += sizeof(desc_48k_spl_tbl);
                }
                if(usb_spk_spl_en & SPL_96000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_96k_spl_tbl, sizeof(desc_96k_spl_tbl));
                    desc_ptr += sizeof(desc_96k_spl_tbl);
                }
                memcpy(desc_ptr, desc_spk_endpoint_tbl, sizeof(desc_spk_endpoint_tbl));
                if(usb_spk_sync_mode_en){
                    desc_ptr[3] = 0x0D;//ISO, Sync, Data EP
                }
                desc_ptr += sizeof(desc_spk_endpoint_tbl);
            }
            intf_num++;
        }
    #endif // UDE_SPEAKER_EN

    #if UDE_MIC_EN
        if (type & MASK_MIC) {
            //copy audio mic interface streaming descriptor
            save_ptr = desc_ptr;
            memcpy(desc_ptr, desc_config_mic_stream, sizeof(desc_config_mic_stream));
            save_ptr[2] = intf_num;
            desc_ptr += sizeof(desc_config_mic_stream);

            if(usb_mic_bits_en & USB_16BITS_EN){
                save_ptr = desc_ptr;
                memcpy(desc_ptr, desc_config_mic_stream_16bit, sizeof(desc_config_mic_stream_16bit));
                save_ptr[2] = intf_num;
                if(usb_mic_dch_en){
                    save_ptr[20] = 0x02;//MIC 2 Channel
                }
                desc_ptr += sizeof(desc_config_mic_stream_16bit);
                if(usb_mic_spl_en & SPL_48000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_48k_spl_tbl, sizeof(desc_48k_spl_tbl));
                    desc_ptr += sizeof(desc_48k_spl_tbl);
                }
                if(usb_mic_spl_en & SPL_96000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_96k_spl_tbl, sizeof(desc_96k_spl_tbl));
                    desc_ptr += sizeof(desc_96k_spl_tbl);
                }
                if(usb_mic_spl_en & SPL_44100_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_44k_spl_tbl, sizeof(desc_44k_spl_tbl));
                    desc_ptr += sizeof(desc_44k_spl_tbl);
                }
                if(usb_mic_spl_en & SPL_8000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_8k_spl_tbl, sizeof(desc_8k_spl_tbl));
                    desc_ptr += sizeof(desc_8k_spl_tbl);
                }
                memcpy(desc_ptr, desc_mic_endpoint_tbl, sizeof(desc_mic_endpoint_tbl));
                if(usb_spk_sync_mode_en){
                    desc_ptr[3] = 0x0D;//ISO, Sync, Data EP
                }
                desc_ptr += sizeof(desc_mic_endpoint_tbl);
            }

            if(usb_mic_bits_en & USB_24BITS_EN){
                save_ptr = desc_ptr;
                memcpy(desc_ptr, desc_config_mic_stream_24bit, sizeof(desc_config_mic_stream_24bit));
                save_ptr[2] = intf_num;
                if(usb_mic_dch_en){
                    save_ptr[20] = 0x02;//MIC 2 Channel
                }
                desc_ptr += sizeof(desc_config_mic_stream_24bit);
                if(usb_mic_spl_en & SPL_48000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_48k_spl_tbl, sizeof(desc_48k_spl_tbl));
                    desc_ptr += sizeof(desc_48k_spl_tbl);
                }
                if(usb_mic_spl_en & SPL_96000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_96k_spl_tbl, sizeof(desc_96k_spl_tbl));
                    desc_ptr += sizeof(desc_96k_spl_tbl);
                }
                if(usb_mic_spl_en & SPL_44100_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_44k_spl_tbl, sizeof(desc_44k_spl_tbl));
                    desc_ptr += sizeof(desc_44k_spl_tbl);
                }
                if(usb_mic_spl_en & SPL_8000_EN){
                    save_ptr[16] += 3;
                    save_ptr[23] += 1;
                    memcpy(desc_ptr, desc_8k_spl_tbl, sizeof(desc_8k_spl_tbl));
                    desc_ptr += sizeof(desc_8k_spl_tbl);
                }
                memcpy(desc_ptr, desc_mic_endpoint_tbl, sizeof(desc_mic_endpoint_tbl));
                if(usb_spk_sync_mode_en){
                    desc_ptr[3] = 0x0D;//ISO, Sync, Data EP
                }
                desc_ptr += sizeof(desc_mic_endpoint_tbl);
            }

            set_isoin_intf(intf_num);
            intf_num++;
        }
    #endif // UDE_MIC_EN
    }
#endif // (UDE_SPEAKER_EN || UDE_MIC_EN)

#if UDE_HID_EN
    if (type & MASK_HID) {
        memcpy(desc_ptr, desc_config_hid, sizeof(desc_config_hid));
        desc_ptr[2] = intf_num++;
#if UDE_HID_OUT_EN
        desc_ptr[4] = 2;    //2 endpoints
#endif
        desc_ptr += sizeof(desc_config_hid);
        memcpy(desc_ptr, desc_hid_desc, sizeof(desc_hid_desc));
        desc_ptr += sizeof(desc_hid_desc);
        memcpy(desc_ptr, desc_hid_ep_in, sizeof(desc_hid_ep_in));
        desc_ptr += sizeof(desc_hid_ep_in);
#if UDE_HID_OUT_EN
        memcpy(desc_ptr, desc_hid_ep_out, sizeof(desc_hid_ep_out));
        desc_ptr += sizeof(desc_hid_ep_out);
#endif
    }
#endif // UDE_HID_EN

    cfg_desc_len = (u32)desc_ptr - (u32)desc_ptr_start;
    set_cfg_desc_buf_val(cfg_desc_len, intf_num);
//    printf("desc: %d, %d\n", cfg_desc_len, intf_num);
}

//USB描述符处理接口
AT(.usbdev.com)
bool user_do_get_descriptor(u8 desc_type)
{
    u8 *desc_ptr = get_desc_ptr();
    u16 desc_len;
    u8 usb_device_qualifier[10];
    switch (desc_type) {
    case DEVICE_DESCRIPTOR:
        //1: 设备描述符
        desc_len = sizeof(desc_usb_device);
        memcpy(send_unstandard_command, desc_usb_device, desc_len);
        memcpy(&send_unstandard_command[8], (u8 *)get_usb_dev_vid_pid(), 4);
        set_usb_send_cmd_len(desc_len);
        return true;

    case CONFIGURATION_DESCRIPTOR:
        //2: 配置描述符
        user_got_config_descriptor();
        desc_len = ((u16)desc_ptr[3] << 8) | desc_ptr[2];
        memcpy(send_unstandard_command, desc_ptr, desc_len);
        set_usb_send_cmd_len(desc_len);
        return true;

//    case STRING_DESCRIPTOR:
//        //3: 字符串描述符
//        return do_get_string_descriptor(ude); 内部调用

    case INTERFACE_DESCRIPTOR:
        //4: 接口描述符
        break;

    case ENDPOINT_DESCRIPTOR:
        //5: 端点描述符
        break;

    case DEVICE_QUALIFIER_DESCRIPTOR:
        //6: 限定描述符
        memcpy(usb_device_qualifier, desc_usb_device, 8);
        usb_device_qualifier[0] = 10;
        usb_device_qualifier[1] = DEVICE_QUALIFIER_DESCRIPTOR;
        usb_device_qualifier[8] = desc_usb_device[17];
        usb_device_qualifier[9] = 0;
        desc_len = sizeof(usb_device_qualifier);
        memcpy(send_unstandard_command, usb_device_qualifier, desc_len);
        set_usb_send_cmd_len(desc_len);
        return true;

    case OTHER_SPEED_CONFIG_DESCRIPTOR:
        //7: 另外一组配置描述符
        break;

#if UDE_HID_EN
    case HID_DESCRIPTOR:
        desc_len = sizeof(desc_config_hid);
        memcpy(send_unstandard_command, desc_config_hid, desc_len);
        set_usb_send_cmd_len(desc_len);
        return true;

    case HID_REPORT:
        ude_hid_setvalid();
        desc_len = sizeof(desc_hid_report_typec);
        memcpy(send_unstandard_command, desc_hid_report_typec, ((desc_hid_desc[8] << 8) | desc_hid_desc[7]));
        set_usb_send_cmd_len(desc_len);
        return true;
#endif // UDE_HID_EN

    default:
        break;
    }
    return false;
}

//USB字符串处理接口
AT(.usbdev.com)
bool user_do_get_string_descriptor(uint data)
{
    const u8 *str;
    switch (data) {
    case STR_LANGUAGE_ID:
        memcpy(send_unstandard_command,str_language_id,4);
        set_usb_send_cmd_len(4);
        return true;

    case STR_MANUFACTURER:                      //setting手动修改usb供应商
        str = get_usb_dev_supplier();
        memcpy(send_unstandard_command,str,str[0]);
        set_usb_send_cmd_len(str[0]);
        return true;

    case STR_PRODUCT:                           //settings手动修改usb名称
        str = get_usb_dev_product_name();
        memcpy(send_unstandard_command,str,str[0]);
        set_usb_send_cmd_len(str[0]);
        return true;

    case STR_SERIAL_NUM:                        //settings手动修改usb序列号
        str = get_usb_dev_serial_number();
        memcpy(send_unstandard_command,str,str[0]);
        set_usb_send_cmd_len(str[0]);
        return true;

    case STR_MICROSOFT_OS:                      //固定使用,不需要修改
        str = get_ms_os_string_descriptor();
        set_usb_send_cmd_len(18);
        return true;

    default:
        break;
    }
    return false;
}
#endif
#endif
