#include "include.h"
#include "usb_table.h"
#include "usb_enum.h"

//Vendor ID/Product ID
const u8 user_usb_vid_pid[4] = {
    0x87,               // Vendor ID LSB
    0x80,               // Vendor ID MSB
    0x24,               // Product ID LSB
    0x10,               // Product ID MSB
};

//Manufacturer String
const u8 user_usb_manufacturer[16] = {
    16,                 // Num bytes of this descriptor
    3,                  // String descriptor
    'G',    0,
    'e',    0,
    'n',    0,
    'e',    0,
    'r',    0,
    'i',    0,
    'c',    0
};

//Product String
const u8 user_usb_product[28] = {
    28,                 // Num bytes of this descriptor
    3,                  // String descriptor
    'U',    0,
    'S',    0,
    'B',    0,
    ' ',    0,
    'H',    0,
    'e',    0,
    'a',    0,
    'd',    0,
    'P',    0,
    'h',    0,
    'o',    0,
    'n',    0,
    'e',    0
};

//Serial Number
const u8 user_usb_serial_number[30] = {
    30,                 // Num bytes of this descriptor
    3,                  // String descriptor
    '2',    0,
    '0',    0,
    '2',    0,
    '4',    0,
    '0',    0,
    '8',    0,
    '0',    0,
    '8',    0,
    '0',    0,
    '9',    0,
    '3',    0,
    '6',    0,
    '2',    0,
    '6',    0
};

#if !USB_USER_CFG_DEV_EN
const u8 desc_spk_endpoint_tbl[16] = {
    //EndpointDescriptor:
    9,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_ISOOUT_CFG_NUM,      // Endpoint number
    0x09,               // ISO endpoint, ADA
    (u8)USB_ISOOUT_SIZE,// Maximum packet size LSB      1ms sample point bytes(48 * 4)
    (u8)(USB_ISOOUT_SIZE >> 8), // Maximum packet size MSB
    1,                  // ISO 1ms
    0,                  // Refresh
    0,                  // SynchAddress

    7,                  // Num bytes of this descriptor
    0x25,               // CS_ENDPOINT descriptor
    0x01,               // EP_GENENAL descriptor
    0x01,               // ATTR
    0x01,               // LockDelayUnits
    0x01,               // LockDelay LSB
    0x00,               // LockDelay MSB
};

const u8 desc_mic_endpoint_tbl[16] = {
    //EndpointDescriptor:
    9,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_ISOIN_CFG_NUM | 0x80,// Endpoint number
    0x05,               // ISO endpoint, ASY
    (u8)USB_ISOIN_SIZE, // Maximum packet size?sample point bytes by 1ms
    (u8)(USB_ISOIN_SIZE >> 8), // Maximum packet size MSB
    1,                  // ISO 1ms
    0,                  // Refresh
    0,                  // SynchAddress

    7,                  // Num bytes of this descriptor
    0x25,               // CS_ENDPOINT descriptor
    0x01,               // EP_GENENAL descriptor
    0x01,               // ATTR
    0x00,               // LockDelayUnits
    0x00,               // LockDelay LSB
    0x00,               // LockDelay MSB
};
#endif

#if USB_USER_CFG_DEV_EN || USB_USER_CFG_BCD_EN
//usb device descriptor
const u8 desc_usb_device[18] = {
    18,                 // Num bytes of the descriptor
    1,                  // Device Descriptor type
    0x00,               // Revision of USB Spec. LSB (in BCD)
    0x02,               // Revision of USB Spec. MSB (in BCD)
    0,                  // Mass Storage Class
    0,                  // General Mass Storage Sub Class
    0,                  // Do not use Class specific protocol
    USB_CTRL_SIZE,      // Max packet size of Endpoint 0

    //jl:e3b4
    0x85,               // Vendor ID LSB
    0x80,               // Vendor ID MSB
    0x22,               // Product ID LSB
    0x10,               // Product ID MSB
    (u8)(USB_BCD_DEVICE_NUM),       // Device Revision LSB (in BCD)
    (u8)(USB_BCD_DEVICE_NUM>>8),    // Device Revision MSB (in BCD)

    STR_MANUFACTURER,   // Index of Manufacture string descriptor
    STR_PRODUCT,        // Index of Product string descriptor
    STR_SERIAL_NUM,     // Index of Serial No. string descriptor
    1                   // Num Configurations, Must be 1
};
#endif

#if USB_USER_CFG_DEV_EN
//Language ID
const u8 str_language_id[4] = {
    4,                  // Num bytes of this descriptor
    3,                  // String descriptor
    0x09,               // Language ID LSB
    0x04                // Language ID
};

//Manufacturer String
const u8 str_manufacturer[16] = {
    16,                 // Num bytes of this descriptor
    3,                  // String descriptor
    'G',    0,
    'e',    0,
    'n',    0,
    'e',    0,
    'r',    0,
    'i',    0,
    'c',    0
};

//Product String
const u8 str_product[28] = {
    28,                 // Num bytes of this descriptor
    3,                  // String descriptor
    'U',    0,
    'S',    0,
    'B',    0,
    '2',    0,
    '.',    0,
    '0',    0,
    ' ',    0,
    'D',    0,
    'e',    0,
    'v',    0,
    'i',    0,
    'c',    0,
    'e',    0
};

//Serial Number
const u8 str_serial_number[30] = {
    30,                 // Num bytes of this descriptor
    3,                  // String descriptor
    '2',    0,
    '0',    0,
    '2',    0,
    '2',    0,
    '0',    0,
    '7',    0,
    '0',    0,
    '1',    0,
    '0',    0,
    '9',    0,
    '3',    0,
    '6',    0,
    '2',    0,
    '6',    0
};

const u8 str_ms_os_string[18] = {
    0x12,               // bLength
    0x03,               // bDescriptorType
    0x4D, 0x00,         // -
    0x53, 0x00,         // -
    0x46, 0x00,         // -
    0x54, 0x00,         // -
    0x31, 0x00,         // -
    0x30, 0x00,         // -
    0x30, 0x00,         // qwSignature,Signature field "MSFT100"
    0x01,               // bMS_VendorCode
    0x00,               // bPad
};

const u8 str_ms_os_feature[16] = {
    0x10, 0x00,         // -
    0x00, 0x00,         // dwLength
    0x00, 0x01,         // bcdVersion
    0x04, 0x00,         // windex
    0x00,               // bCount
    0x00, 0x00,         // -
    0x00, 0x00,         // -
    0x00, 0x00,         // -
    0x00,               // Reserved
};

//Config descriptor: general
const u8 desc_config_general[9] = {
    9,                  // Num bytes of this descriptor
    2,                  // Configuration descriptor type
    158,                // Total size of configuration LSB
    0,                  // Total size of configuration MSB
    5,                  // Total Interface number
    1,                  // Configuration number
    0,                  // Index of Configuration string descriptor
    0x80,               // Configuration characteristics: BusPowerd
    0x32,               // Max current, unit is 2mA
};

//Config descriptor: mass storage interface 9 + 7 + 7= 23
const u8 desc_config_storage[23] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    0,                  // Interface Number
    0,                  // Alternate interface number
    2,                  // Num endpoints of this interface
    0x08,               // Interface Class: Mass Storage
    0x06,               // Interface Sub Class: RCB
    0x50,               // Class specific protocol:
    0,                  // Index of Interface string descriptor

    7,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_BULK_CFG_NUM,    // Endpoint number, bit7=1 shows OUT
    2,                  // Bulk endpoint
    USB_BULK_SIZE,      // Maximum packet size LSB
    0x00,               // Maximum packet size MSB
    0,                  // no use for bulk endpoint

    7,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_BULK_CFG_NUM | 0x80, // Endpoint number, bit7=0 shows IN
    2,                  // Bulk endpoint
    0x40,               // Maximum packet size LSB
    0x00,               // Maximum packet size MSB
    0                   // No use for bulk endpoint
};

//Config descriptor: audio head (9 + 10 = 19)
const u8 desc_config_audio[19] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    1,                  // Interface Number
    0,                  // Alternate interface number
    0,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x01,               // Interface Sub Class: Audio Control
    0,                  // Class specific protocol
    0,                  // Index of Interface string descriptor

    10,                 // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x01,               // HEADER descriptor
    0x00,               // Audio 1.0 LSB
    0x01,               // Audio 1.0 MSB
    0x28,               // Total Size of Audio Control LSB
    0x00,               // Total Size of Audio Control MSB
    2,                  // Num Audio OUT Streaming Interface
    2,                  // Number of Audio OUT Streaming Interface(speaker streaming interface number)
    3,                  // Number of Audio IN Streaming Interface(mic streaming interface number)
};

const u8 desc_config_audio_balance_tbl[13] = {
    0x0D,               // bLength
    0x24,               // bDescriptorType (See Next Line)
    0x04,               // bDescriptorSubtype (CS_INTERFACE -> MIXER_UNIT)
    0x06,               // bUnitID
    0x02,               // bNrInPins 2
    0x05,               // baSourceID[0] = 0x05
    0x05,               // baSourceID[1] = 0x05
    0x02,               // bNrChannels 2
    0x03, 0x00,         // wChannelConfig (Left and Right Front)
    0x00,               // iChannelNames
    0x00,               // bmControls
    0x00,               // iMixer
};

//Config descriptor: audio speaker control (12 + 10 + 9 = 31)
const u8 desc_config_audio_speaker[31] = {
    12,                 // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // INPUT_TERMINAL descriptor
    0x01,               // Terminal ID, The same of speaker stream TID
    0x01,               // Terminal Type: 0x0101 - USB Streaming LSB
    0x01,               // Terminal Type: 0x0101 - USB Streaming MSB
    0x00,               // Associate Terminal
    0x02,               // NrChannel, Stereo
    0x03,               // Channel Config LSB  (D0: Left Front, D1:Right Front)
    0x00,               // Channel Config MSB
    0,                  // Channel Name
    0,                  // Index of Channel String

    10,                 // Num bytes of this descriptor
    0x24,               // bDescriptorType, Audio Control Interface descriptor type
    0x06,               // bDescriptorSubtype, FEATURE_UNIT descriptor subtype
    UID_SPK,            // bUnitID, Feature Unit ID
    0x01,               // bSourceID
    0x01,               // Control Size
    0x03,               // bmaControls(0): D0(Mute), D1(Volume),  mute and volume control supported for master channel
    0x00,               // bmaControls(1)
    0x00,               // bmaControls(2)
    0,                  // Index of Feature String

    9,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x03,               // OUTPUT_TERMINAL descriptor
    0x03,               // Terminal ID
    0x02,               // Terminal Type: 0x0302 = Headphones LSB
    0x03,               // Terminal Type: 0x0302 = Headphones MSB
    0x00,               // Associate Terminal
    0x02,               // bSourceID
    0,                  // Index of Channel String
};

const u8 desc_config_audio_mic[31] = {
    12,                 // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // INPUT_TERMINAL descriptor
    0x04,               // Terminal ID
    0x01,               // Terminal Type: 0x0201 = Microphone  LSB
    0x02,               // Terminal Type: 0x0201 = Microphone  MSB
    0x00,               // Associate Terminal
    0x01,               // NrChannel, Single
    0x00,               // Channel Config LSB
    0x00,               // Channel Config MSB
    0,                  // Channel Name
    0,                  // Index of Channel String

    10,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x06,               // FEATURE_UNIT descriptor
    UID_MIC,            // bUnitID, Feature Unit ID
    0x04,               // bSourceID
    0x01,               // Control Size
    0x03,               // bmaControls(0): D0(Mute) D1(Volume),  mute and volume control supported for master channel
    0x00,               // bmaControls(1)
    0x00,               // bmaControls(2)
    0,                  // Index of Feature String

    9,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x03,               // OUTPUT_TERMINAL descriptor
    0x06,               // Terminal ID, The same of mic stream TID
    0x01,               // Terminal Type: 0x0101 - USB Streaming LSB
    0x01,               // Terminal Type: 0x0101 - USB Streaming MSB
    0x00,               // Associate Terminal
    0x05,               // bSourceID
    0,                  // Index of Channel String
};

//Config descriptor: audio mic stereo (12 + 10 + 9 = 31)
const u8 desc_config_audio_dmic[31] = {
    12,                 // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // INPUT_TERMINAL descriptor
    0x04,               // Terminal ID
    0x01,               // Terminal Type: 0x0201 = Microphone  LSB
    0x02,               // Terminal Type: 0x0201 = Microphone  MSB
    0x00,               // Associate Terminal
    0x02,               // NrChannel, Single
    0x03,               // Channel Config LSB  (D0: Left Front, D1:Right Front)
    0x00,               // Channel Config MSB
    0,                  // Channel Name
    0,                  // Index of Channel String

    10,                 // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x06,               // FEATURE_UNIT descriptor
    UID_MIC,            // bUnitID, Feature Unit ID
    0x04,               // bSourceID
    0x01,               // Control Size
    0x03,               // bmaControls(0): D0(Mute) D1(Volume),  mute and volume control supported for master channel
    0x00,               // bmaControls(1)
    0x00,               // bmaControls(2)
    0,                  // Index of Feature String

    9,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x03,               // OUTPUT_TERMINAL descriptor
    0x06,               // Terminal ID, The same of mic stream TID
    0x01,               // Terminal Type: 0x0101 - USB Streaming LSB
    0x01,               // Terminal Type: 0x0101 - USB Streaming MSB
    0x00,               // Associate Terminal
    0x05,               // bSourceID
    0,                  // Index of Channel String
};

const u8 desc_config_speaker_stream[9] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    2,                  // Interface Number
    0,                  // Alternate interface number
    0,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor
};

const u8 desc_config_speaker_stream_16bit[24] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    2,                  // Interface Number
    1,                  // Alternate interface number
    1,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor

    7,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x01,               // AS_GENERAL descriptor
    0x01,               // Terminal ID of Input Terminal, Audio USB Stream ID
    0x01,               // Interface Delay
    0x01,               // PCM Format 0x0001 = 16bit LSB
    0x00,               // PCM Format MSB

    8,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // FORMAT_TYPE descriptor
    0x01,               // FORMAT_TYPE
    0x02,               // NrChannel = 2
    0x02,               // frame = 2byte
    0x10,               // 16bit per sample
    0x00,               // Frequency supported Num
};

const u8 desc_config_speaker_stream_24bit[24] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    2,                  // Interface Number
    2,                  // Alternate interface number
    1,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor

    7,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x01,               // AS_GENERAL descriptor
    0x01,               // Terminal ID of Input Terminal, Audio USB Stream ID
    0x01,               // Interface Delay
    0x01,               // PCM Format 0x0001 = 16bit LSB
    0x00,               // PCM Format MSB

    8,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // FORMAT_TYPE descriptor
    0x01,               // FORMAT_TYPE
    0x02,               // NrChannel = 2
    0x03,               // frame = 3byte
    0x18,               // 24bit per sample
    0x00,               // Frequency supported Num
};

const u8 desc_config_speaker_stream_32bit[24] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    2,                  // Interface Number
    3,                  // Alternate interface number
    1,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor

    7,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x01,               // AS_GENERAL descriptor
    0x01,               // Terminal ID of Input Terminal, Audio USB Stream ID
    0x01,               // Interface Delay
    0x01,               // PCM Format 0x0001 = 16bit LSB
    0x00,               // PCM Format MSB

    8,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // FORMAT_TYPE descriptor
    0x01,               // FORMAT_TYPE
    0x02,               // NrChannel = 2
    0x04,               // frame = 4byte
    0x20,               // 32bit per sample
    0x00,               // Frequency supported Num
};

const u8 desc_spk_endpoint_tbl[16] = {
    //EndpointDescriptor:
    9,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_ISOOUT_CFG_NUM,      // Endpoint number
    0x09,               // ISO endpoint, ADA
    (u8)USB_ISOOUT_SIZE,// Maximum packet size LSB      1ms sample point bytes(48 * 4)
    (u8)(USB_ISOOUT_SIZE >> 8), // Maximum packet size MSB
    1,                  // ISO 1ms
    0,                  // Refresh
    0,                  // SynchAddress

    7,                  // Num bytes of this descriptor
    0x25,               // CS_ENDPOINT descriptor
    0x01,               // EP_GENENAL descriptor
    0x01,               // ATTR
    0x01,               // LockDelayUnits
    0x01,               // LockDelay LSB
    0x00,               // LockDelay MSB
};

const u8 desc_config_mic_stream[9] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    3,                  // Interface Number
    0,                  // Alternate interface number
    0,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor
};
const u8 desc_config_mic_stream_16bit[24] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    3,                  // Interface Number
    1,                  // Alternate interface number
    1,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor

    7,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x01,               // AS_GENERAL descriptor
    0x06,               // Terminal ID of Input Terminal£¬MIC USB Stream ID is 6
    0x01,               // Interface Delay
    0x01,               // PCM Format 0x0001 = 16bit LSB
    0x00,               // PCM Format MSB

    8,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // FORMAT_TYPE descriptor
    0x01,               // FORMAT_TYPE
    0x01,               // NrChannel = 1£¬MIC used single channel
    0x02,               // frame = 2byte
    0x10,               // 16bit per sample
    0x00,               // Frequency supported Num
};

const u8 desc_config_mic_stream_24bit[24] = {
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    3,                  // Interface Number
    2,                  // Alternate interface number
    1,                  // Num endpoints of this interface
    0x01,               // Interface Class: Audio
    0x02,               // Interface Sub Class: Audio Streaming
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor

    7,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x01,               // AS_GENERAL descriptor
    0x06,               // Terminal ID of Input Terminal£¬MIC USB Stream ID is 6
    0x01,               // Interface Delay
    0x01,               // PCM Format 0x0001 = 16bit LSB
    0x00,               // PCM Format MSB

    8,                  // Num bytes of this descriptor
    0x24,               // Audio Control Interface descriptor type
    0x02,               // FORMAT_TYPE descriptor
    0x01,               // FORMAT_TYPE
    0x01,               // NrChannel = 1£¬MIC used single channel
    0x03,               // frame = 3byte
    0x18,               // 24bit per sample
    0x00,               // Frequency supported Num
};

const u8 desc_mic_endpoint_tbl[16] = {
    //EndpointDescriptor:
    9,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_ISOIN_CFG_NUM | 0x80,// Endpoint number
    0x05,               // ISO endpoint, ASY
    (u8)USB_ISOIN_SIZE, // Maximum packet size£¬sample point bytes by 1ms
    (u8)(USB_ISOIN_SIZE >> 8), // Maximum packet size MSB
    1,                  // ISO 1ms
    0,                  // Refresh
    0,                  // SynchAddress

    7,                  // Num bytes of this descriptor
    0x25,               // CS_ENDPOINT descriptor
    0x01,               // EP_GENENAL descriptor
    0x01,               // ATTR
    0x00,               // LockDelayUnits
    0x00,               // LockDelay LSB
    0x00,               // LockDelay MSB
};

const u8 desc_8k_spl_tbl[3] = {
    0x40,               // Sample Rate = 0x001f40 = 8000 Hz
    0x1F,               // Sample Rate
    0x00,               // Sample Rate
};

const u8 desc_44k_spl_tbl[3] = {
    0x44,               // Sample Rate = 0x00ac44 = 44100 Hz
    0xac,               // Sample Rate
    0x00,               // Sample Rate
};

const u8 desc_48k_spl_tbl[3] = {
    0x80,               // Sample Rate = 0x00BB80 = 48000 Hz
    0xBB,               // Sample Rate
    0x00,               // Sample Rate
};

const u8 desc_96k_spl_tbl[3] = {
    0x00,               // Sample Rate = 0x017700 = 96000 Hz
    0x77,               // Sample Rate
    0x01,               // Sample Rate
};

//Config descriptor: HID (9 + 9 + 7 = 25)
AT(.rodata.usb.table)
const u8 desc_config_hid[9] = {
    //Interface Descriptor:
    9,                  // Num bytes of this descriptor
    4,                  // Interface descriptor type
    4,                  // Interface Number
    0,                  // Alternate interface number
    1,                  // Num endpoints of this interface
    0x03,               // Interface Sub Class: Human Interface Device
    0,                  // Interface Sub Class:
    0,                  // Class specific protocol:
    0,                  // Index of Interface string descriptor
};

#if !USB_USER_CFG_HID_EN
//HID report, consumer control
const u8 desc_hid_report_typec[47] = {
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x09, 0xCF,        //   Usage (0xCF)
    0x09, 0x40,        //   Usage (HOME)
    0x09, 0xb5,        //   Usage (Scan Next Track)
    0x09, 0xb6,        //   Usage (Scan Previous Track)
    // 0x09, 0xb7,        //   Usage (Stop)
    0x09, 0xe2,         // USAGE (Mute)
    0x95, 0x08,        //   Report Count (8)
    0x75, 0x01,        //   Report Size (1)
	0x81, 0x02,        //   INPUT (Data)
    0x05, 0x0B,        //   USAGE_PAGE (Telephony)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x09, 0x21,        //   Usage (Flash)
    0x81, 0x02,        //   Input (Data)
    0x95, 0x07,        //   Report Count (7)
    0x81, 0x05,        //   Input (Constant)
    0xC0,              //   End Collection
};

AT(.rodata.usb.table)
const u8 desc_hid_desc[9] = {
    //HID Descriptor:
    9,                  // Num bytes of this descriptor
    0x21,               // HID Type
    0x01, 0x02,         // HID Class Specification compliance
    0x00,               // Country Code: none
    0x01,               // Number of descriptors to follow
    0x22,               // Report descriptor type
    (u8)sizeof(desc_hid_report_typec),
    (u8)sizeof(desc_hid_report_typec) >> 8,  // Len of Report descriptor
};
#endif
AT(.rodata.usb.table)
const u8 desc_hid_ep_in[7] = {
    //EndpointDescriptor:
    7,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_HID_CFG_NUM | 0x80,  // Endpoint number: HID IN
    0x03,               // Interrupt Endpoint
    USB_HID_IN_SIZE, 0x00, // Maximum packet size
    0x01,               // Poll every 10msec seconds
};

AT(.rodata.usb.table)
const u8 desc_hid_ep_out[7] = {
    //EndpointDescriptor:
    7,                  // Num bytes of this descriptor
    5,                  // Endpoint descriptor type
    EP_HID_CFG_NUM,     // Endpoint number: HID OUT
    0x03,               // Interrupt Endpoint
    USB_HID_OUT_SIZE, 0x00, // Maximum packet size
    0x01,               // Poll every 10msec seconds
};

#endif

#if USB_USER_CFG_HID_EN
const u8 desc_hid_report_typec[47] = {
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   Report ID (1)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x09, 0xCF,        //   Usage (0xCF)
    0x09, 0x40,        //   Usage (HOME)
    0x09, 0xb5,        //   Usage (Scan Next Track)
    0x09, 0xb6,        //   Usage (Scan Previous Track)
    // 0x09, 0xb7,     //   Usage (Stop)
    0x09, 0xe2,        //   Usage (Mute)
    0x95, 0x08,        //   Report Count (8)
    0x75, 0x01,        //   Report Size (1)
	0x81, 0x02,        //   INPUT (Data)
    0x05, 0x0B,        //   USAGE_PAGE (Telephony)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x09, 0x21,        //   Usage (Flash)
    0x81, 0x02,        //   Input (Data)
    0x95, 0x07,        //   Report Count (7)
    0x81, 0x05,        //   Input (Constant)
    0xC0,              //   End Collection
};

AT(.rodata.usb.table)
const u8 desc_hid_desc[9] = {
    //HID Descriptor:
    9,                  // Num bytes of this descriptor
    0x21,               // HID Type
    0x01, 0x02,         // HID Class Specification compliance
    0x00,               // Country Code: none
    0x01,               // Number of descriptors to follow
    0x22,               // Report descriptor type
    (u8)(sizeof(desc_hid_report_typec)),
    (u8)(sizeof(desc_hid_report_typec)>>8),  // Len of Report descriptor
};
#endif