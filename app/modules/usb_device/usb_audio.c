#include "include.h"
#include "usb_audio.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if FUNC_USBDEV_EN

//u8 ep3_isoc_out[576 + 4] AT(.usb_buf.isoc);     //usb audio speaker rx buf
//u8 ep3_isoc_in[288] AT(.usb_buf.isoc);           //usb audio mic tx buf
usb_effect_t usb_effect AT(.udev_buf.efc);

AT(.text.usb.audio)
void usb_mic_rm_all(void)
{

}

AT(.text.usb.audio)
void usb_mic_mode_set(u8 mode)
{
    if((mode < 0) || (mode > MIC_MODE_NUM)){
        return;
    }
    usb_mic_rm_all();
    switch(mode){
        case MIC_NORMAL:
            TRACE("MIC_NORMAL\n");
            break;
        case MIC_KTV:
            TRACE("MIC_KTV\n");
            break;
        case MIC_ELEC:
            TRACE("MIC_ELEC\n");
            break;
        case MIC_PITCH:
            TRACE("MIC_PITCH\n");
            break;
        default:
            break;
    }
}

AT(.text.usb.audio)
void usb_mic_mode_switch(u8 direction)
{
    if(direction){
        usb_effect.mic_mode++;
        if(usb_effect.mic_mode >= MIC_MODE_NUM){
            usb_effect.mic_mode = 0;
        }
    }else{
        usb_effect.mic_mode--;
        if((usb_effect.mic_mode < 0) || (usb_effect.mic_mode >= MIC_MODE_NUM)){
            usb_effect.mic_mode = (MIC_MODE_NUM - 1);
        }
    }
    usb_mic_mode_set(usb_effect.mic_mode);
}

AT(.text.usb.audio)
void usb_dac_rm_all(void)
{

}

AT(.text.usb.audio)
void usb_dac_mode_set(u8 mode)
{
    if((mode < 0) || (mode >= DAC_MODE_NUM)){
        return;
    }
    usb_dac_rm_all();
    switch(mode){
        case DAC_NORMAL:
            TRACE("DAC_NORMAL\n");
            break;

        case DAC_3D:
            TRACE("DAC_3D\n");
            break;

        case DAC_BASS:
            TRACE("DAC_BASS\n");
            break;

        case DAC_VBASS:
            TRACE("DAC_VBASS\n");
            break;

        case DAC_CHOURS:
            TRACE("DAC_CHOURS\n");
            break;

        case DAC_PINGPONG:
            TRACE("DAC_PINGPONG\n");
            break;

        case DAC_AUTOWAH:
            TRACE("DAC_AUTOWAH\n");
            break;

        case DAC_VOCAL_REMOVER:
            TRACE("DAC_VOCAL_REMOVER\n");
            break;

    }
}

AT(.text.usb.audio)
void usb_dac_mode_switch(u8 direction)
{
    if(usb_effect.dac_mode < 2){
        usb_effect.dac_mode = 2;
    }
    if(direction){
        usb_effect.dac_mode++;
        if(usb_effect.dac_mode >= DAC_MODE_NUM * 2){
            usb_effect.dac_mode = 2;
        }
    }else{
        usb_effect.dac_mode--;
        if(usb_effect.dac_mode < 2){
            usb_effect.dac_mode = (DAC_MODE_NUM * 2 - 1);
        }
    }
    TRACE("usb_effect.dac_mode: %d\n", usb_effect.dac_mode);
    usb_dac_mode_set(usb_effect.dac_mode % 2 == 0 ? 0 : usb_effect.dac_mode / 2);
}

//USB下行数据接口，data数据传入，len返回底层长度(用于推DAC)
//host out 每1ms样点->48k:192 byte，44.1k:176 byte，22.05k:88 byte，11.025k:44 byte，8k:32 byte
AT(.usbdev.com)
void ude_rx_data(u8 *data, u32 *len)
{
#if 0
    u32 i = 0;
    u32 temp_len = *len;
    for (i = 0; i < temp_len; i++) {
       my_spi_putc(data[i]);
    }
#endif
#if 0
    u32 temp_len = *len;
    dump_putbuf(data[i], temp_len, 0);
#endif
}

//USB上行数据接口，data数据传入，len返回底层长度(用于上行主机)
//host in 每1ms样点->96K:192byte, 48k:96 bytes
AT(.usbdev.com)
void ude_tx_data(u8 *data, int *len)
{
#if 0
    u32 i = 0;
    int temp_len = *len;
    for (i = 0; i < temp_len; i++) {
       my_spi_putc(data[i]);
    }
#endif
#if 0
    int temp_len = *len;
    dump_putbuf(data[i], temp_len, 0);
#endif
}

#if USB_USER_HID_OUT_EN
static u8 hid_data[64] AT(.usb_buf.hid);
//接收PC下行HID数据
AT(.usbdev.com)
void deal_hid_get_data(u8 *ptr, u8 len)
{
    memcpy(&hid_data, ptr, len);
	my_print_r(hid_data, len);
}
#endif

AT(.text.usb.audio)
void usb_effect_init(void)
{
    usb_effect.mic_mode = MIC_NORMAL;
    usb_effect.dac_mode = DAC_NORMAL;
    usb_mic_mode_set(usb_effect.mic_mode);
    usb_dac_mode_set(2 * usb_effect.dac_mode);
}

#endif
