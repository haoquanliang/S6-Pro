#ifndef _USB_ENUM_H
#define _USB_ENUM_H

//Device Descriptor
#define DEVICE_DESCRIPTOR                   1
#define CONFIGURATION_DESCRIPTOR            2
#define STRING_DESCRIPTOR                   3
#define INTERFACE_DESCRIPTOR                4
#define ENDPOINT_DESCRIPTOR                 5
#define DEVICE_QUALIFIER_DESCRIPTOR         6
#define OTHER_SPEED_CONFIG_DESCRIPTOR       7
#define HID_DESCRIPTOR                      0x21    //获取HID描述符
#define HID_REPORT                          0x22    //获取HID报表

//Descriptor String Index
#define STR_LANGUAGE_ID                 0
#define STR_MANUFACTURER                1
#define STR_PRODUCT                     2
#define STR_SERIAL_NUM                  3
#define STR_MICROSOFT_OS                0xEE        //Microsoft OS Descriptors

#define UDE_HID_OUT_EN          1           //HID OUT数据接收
#define MASK_STORAGE            BIT(0)
#define MASK_SPEAKER            BIT(1)
#define MASK_HID                BIT(2)
#define MASK_MIC                BIT(3)
#define MASK_ISO_AUDIO          (MASK_SPEAKER | MASK_MIC)
#define MASK_BULK               MASK_STORAGE

//USB采样率选择
#define SPL_48000_EN            BIT(0)
#define SPL_44100_EN            BIT(1)
#define SPL_8000_EN             BIT(2)
#define SPL_96000_EN            BIT(3)
#define SPK_SPL_SEL             (SPL_48000_EN | SPL_44100_EN | SPL_96000_EN)
#if (USB_MIC_NR_EN || USB_AEC_EN)
#define MIC_SPL_SEL             (SPL_48000_EN)
#else
#define MIC_SPL_SEL             (SPL_48000_EN | SPL_44100_EN)
#endif
//USB采样位宽选择
#define USB_16BITS_EN           BIT(0)
#define USB_24BITS_EN           BIT(1)
#define USB_32BITS_EN           BIT(2)
//#define SPK_BITS_SEL            (USB_16BITS_EN | USB_24BITS_EN)                 //支持配置16bit/24bit/32bit
//#define MIC_BITS_SEL            (USB_16BITS_EN | USB_24BITS_EN)                 //支持配置16bit/24bit,不支持32bit

#define EP_BULK_CFG_NUM         1
#define EP_ISOOUT_CFG_NUM       3
#define EP_ISOIN_CFG_NUM        3
#define EP_HID_CFG_NUM          2

#define BULKTX_EPADDR           USBEP1TXADR
#define BULKRX_EPADDR           USBEP1RXADR
#define ISOCTX_EPADDR           USBEP2TXADR
#define ISOCRX_EPADDR           USBEP2RXADR
#define HIDTX_EPADDR            USBEP3TXADR

#define USB_CTRL_SIZE           64
#define USB_BULK_SIZE           64
#define USB_HID_IN_SIZE         64
#define USB_HID_OUT_SIZE        64
//#if ((SPK_BITS_SEL & USB_32BITS_EN) == USB_32BITS_EN)
//#define USB_ISOOUT_SIZE         768     //Audio speaker  samplerate * channel * pcmbits = 96 * 2 * 4,最大96k双声道32bit
//#else
//#define USB_ISOOUT_SIZE         576     //speaker 96*2*3
//#endif
//#define USB_ISOIN_SIZE          288     //MIC 48*2*3, 最大48K双声道24BIT

#define USB_ISOOUT_SIZE         48*4     //speaker 96*2*3
#define USB_ISOIN_SIZE          48*4     //MIC 48*2*3, 最大48K双声道24BIT
#define SPK_BITS_SEL            (USB_16BITS_EN)                 //支持配置16bit/24bit/32bit
#define MIC_BITS_SEL            (USB_16BITS_EN)                 //支持配置16bit/24bit,不支持32bit
void usb_cfg_init(void);

void ude_hid_setvalid(void);
u8 *get_desc_ptr(void);
u8 get_type(void);
u8 set_isoin_intf(u8 isoin_intf);                       //设置mic Interface Number
void set_cfg_desc_buf_val(u16 set_len, u8 set_inft);    //设置配置描述符总长度
void set_usb_send_cmd_len(u16 set_len);                 //设置端点0发送数据长度
const u8 *get_ms_os_string_descriptor(void);
#endif
