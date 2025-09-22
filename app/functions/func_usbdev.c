#include "include.h"
#include "func.h"
#include "func_usbdev.h"

#if FUNC_USBDEV_EN

f_ude_t f_ude;

uint8_t usb_det_ver_sel     = USB_DET_VER_SEL;
uint8_t usb_user_cfg_dev_en = USB_USER_CFG_DEV_EN;

uint8_t uac_ver_sel         = UAC_VER_SEL;
uint8_t cfg_desc_cfg_type   = CFG_DESC_CFG_TYPE;
uint8_t cfg_usb_maxpower    = CFG_USB_MAXPOWER;
uint8_t cfg_get_conf_len    = CFG_GET_CONF_LEN;
uint8_t usb_driver_max_en   = USB_DRIVER_MAX_EN;
uint8_t udm_vol_default_sel = UDM_VOL_DEFAULT_SEL;
uint8_t uda_balance_vol_en  = UDA_BALANCE_VOL_EN;
uint8_t uda_balance_id_sel  = UDA_BALANCE_ID_SEL;

uint8_t usb_spk_spl_en      = USB_SPK_SPL_EN;
uint8_t usb_spk_bits_en     = USB_SPK_BITS_EN;
uint8_t usb_spk_sync_mode_en= USB_SPK_SYNC_MODE_EN;

#if UDE_MIC_EN
uint8_t usb_mic_dch_en      = USB_MIC_DCH_EN;
uint8_t usb_mic_spl_en      = USB_MIC_SPL_EN;
uint8_t usb_mic_bits_en     = USB_MIC_BITS_EN;
#endif

void ude_mic_start(u8 spr);
void ude_mic_stop(void);

AT(.usbdev.com)
bool uda_is_bypass(void)
{
    return f_ude.uda_bypass;
}

AT(.text.func.usbdev)
void func_usbdev_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }
    dac_fade_out();
    dac_fade_wait();
    f_ude.uda_bypass = true;
#if UDE_MIC_EN
    u8 mic_start = sys_cb.ude_mic_start_en;
    if (mic_start) {
        ude_mic_stop();
    }
#endif  //UDE_MIC_EN
    delay_5ms(2);

    mp3_res_play(addr, len);

    dac_spr_set(uda_get_spk_spr());
    f_ude.uda_bypass = false;
#if UDE_MIC_EN
    if (mic_start) {
        ude_mic_start(uda_get_mic_spr());
    }
#endif  //UDE_MIC_EN
    dac_fade_in();
}

AT(.text.func.usbdev)
void ude_mic_start(u8 spr)
{
#if USB_MIC_NR_EN
    uda_mic_alg_init();
#endif
    set_mic_disframe(0);
    sys_cb.ude_mic_start_en = true;
    sys_cb.ude_mic_spr = spr;
    audio_path_init(AUDIO_PATH_USBMIC);
    audio_path_start(AUDIO_PATH_USBMIC);
}

AT(.text.func.usbdev)
void ude_mic_stop(void)
{
    sys_cb.ude_mic_start_en = false;
    audio_path_exit(AUDIO_PATH_USBMIC);
#if USB_MIC_NR_EN
    uda_mic_alg_exit();
#endif

}

u8 ude_get_sys_volume(void)
{
    return sys_cb.db_level;
}

u8 ude_get_sys_vol_max(void)
{
    return VOL_MAX;
}

AT(.usbdev.com)
void ude_set_sys_volume(u8 vol)
{
    if (sys_cb.db_level != vol) {
        sys_cb.db_level = vol;
        msg_enqueue(EVT_UDE_SET_VOL);
    }
}

//usb左右声道均衡
void uda_set_balance_vol(u8 vol_l, u8 vol_r)
{
#if UDA_BALANCE_VOL_EN
    u16 dvol_l = dac_dvol_tbl_db[60 - vol_l];
    u16 dvol_r = dac_dvol_tbl_db[60 - vol_r];
    dac_balance_set(dvol_l, dvol_r);
#endif
}

#if UDE_STORAGE_EN
AT(.text.func.usbdev)
void ude_sdcard_switch(u8 dev)
{
//    printf("%s, %d, %d\n", __func__, f_ude.cur_dev, dev);
    if (f_ude.cur_dev != dev) {
        ude_sd_remove();
        f_ude.cur_dev = dev;
        fsdisk_callback_init(f_ude.cur_dev);
        f_ude.dev_change = 1;
    } else {
        sd0_init();
    }
}

AT(.text.func.usbdev)
void ude_sdcard_change_process(void)
{
    static u8 cnt_s = 0;
    if (f_ude.dev_change) {
        cnt_s++;
        if (cnt_s >= 5) {
            f_ude.dev_change = 0;
            sd0_init();
        }
    } else {
        cnt_s = 0;
    }
}
#endif // UDE_STORAGE_EN

AT(.text.func.usbdev)
void func_usbdev_process(void)
{
    func_process();
    usb_device_process();
    f_ude.rw_sta = sd0_get_rw_sta();
}

static void func_usbdev_enter(void)
{
    usb_cfg_init();
    sys_cb.db_level = bsp_dac_get_gain_level(sys_cb.vol);
    f_ude.vol = sys_cb.vol;         //restore system volume
    if (!dev_is_online(DEV_USBPC)) {
        func_cb.sta = FUNC_NULL;
        return;
    }
    func_cb.mp3_res_play = func_usbdev_mp3_res_play;
    f_ude.cur_dev = DEV_SDCARD;
    f_ude.dev_change = 0;
    func_usbdev_enter_display();
    led_idle();

    cfg_get_conf_len = CFG_GET_CONF_LEN;
    udm_vol_default_sel = UDM_VOL_DEFAULT_SEL & 0x3;

#if WARNING_FUNC_USBDEV
    mp3_res_play(RES_BUF_PC_MODE_MP3, RES_LEN_PC_MODE_MP3);
#endif // WARNING_FUNC_USBDEV

#if UDE_SPEAKER_EN
    cfg_dac_out_spr = DAC_OUT_96K;
    adpll_spr_set(DAC_OUT_96K);
    dac_spr_set(DAC_OUT_96K);     	//samplerate 96K
    DACDIGCON0 |= BIT(6);           //Src0 Sample Rate Synchronization Enable
    dac_fade_in();
    aubuf0_dma_init();
    CLKGAT2 |= BIT(15);
    void uda_far_fill_init(void);
    uda_far_fill_init();
#endif // UDE_SPEAKER_EN

#if UDE_MIC_EN
#if USB_MIC_NR_EN
    src_var_init();
#endif
#endif

#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        if (dev_is_online(DEV_SDCARD)) {
            f_ude.cur_dev = DEV_SDCARD;
        } else if (dev_is_online(DEV_SDCARD1)) {
            f_ude.cur_dev = DEV_SDCARD1;
        }
        fsdisk_callback_init(f_ude.cur_dev);
        fs_mount();
    }
#endif // UDE_STORAGE_EN

#if !USB_BC_EN
    usb_bc_init(0);
#endif

    if(!UDA_BALANCE_ID_SEL && (0 == UDE_MIC_EN)){
        uda_balance_vol_en = 0;//关闭3节耳机左右声道均衡功能
    }
    usb_device_enter(UDE_ENUM_TYPE);
}

static void func_usbdev_exit(void)
{
    usb_device_exit();
    func_usbdev_exit_display();
#if UDE_MIC_EN
    ude_mic_stop();
#endif
#if UDE_SPEAKER_EN
    cfg_dac_out_spr = DAC_OUT_SPR;
    DACDIGCON0 &= ~BIT(6);
    adpll_spr_set(DAC_OUT_SPR);
    sys_cb.vol = f_ude.vol;         //recover system volume
    bsp_change_volume(sys_cb.vol);
    dac_fade_out();
#endif // UDE_SPEAKER_EN
#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        sd0_stop(1);
    #if I2C_MUX_SD_EN
        sd0_go_idle_state();
    #endif
    }
#endif // UDE_STORAGE_EN
    func_cb.last = FUNC_USBDEV;
}

AT(.text.func.usbdev)
void func_usbdev(void)
{
    printf("%s\n", __func__);

    func_usbdev_enter();

    while (func_cb.sta == FUNC_USBDEV) {
        func_usbdev_process();
        func_usbdev_message(msg_dequeue());
        func_usbdev_display();
    }

    func_usbdev_exit();
}
#else
AT(.text.func.usbdev)
void ude_mic_start(u8 spr){}
AT(.text.func.usbdev)
void ude_mic_stop(void){}
AT(.usbdev.com)
u8 ude_get_sys_volume(void){return 0;}
AT(.usbdev.com)
u8 ude_get_sys_vol_level(void){return 0;}
AT(.usbdev.com)
void ude_set_sys_volume(u8 vol){}
#endif // FUNC_USBDEV_EN
