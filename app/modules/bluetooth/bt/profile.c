#include "include.h"
#include "api.h"

extern const uint8_t   sdp_spp_service_record[];
extern const uint8_t   sdp_hfp_service_record[];
extern const uint8_t   sdp_hsp_service_record[];
extern const uint8_t   sdp_avdtp_service_record[];      //a2dp1.3
extern const uint8_t   sdp_avctp_controller_service_record[];
extern const uint8_t   sdp_avctp_target_category1_service_record[];
extern const uint8_t   sdp_avctp_target_category2_service_record[];
extern const uint8_t   sdp_hid_keyboard_service_record[];
extern const uint8_t   sdp_hid_consumer_service_record[];
extern const uint8_t   sdp_hid_tscreen_service_record[];
extern const uint8_t   sdp_hid_tscreen_ext_service_record[];
extern const uint8_t   sdp_devic_id_service_record[];
extern const uint8_t   sdp_gfps_spp_service_record[];
extern const uint8_t   sdp_gatt_service_record[];
extern const uint8_t   sdp_hid_ios_service_record[];
extern const uint8_t   sdp_pnp_ios_service_record[];
extern const uint8_t   sdp_spp_dueros_dma_service_record[];

typedef struct {
    void    *item;
    uint32_t service_record_handle;
    uint8_t *service_record;
} service_record_item_t;

#if BT_SPP_EN
service_record_item_t spp_sdp_record_item = {
    .service_record_handle  = 0x00010001,
    .service_record         = (uint8_t *)sdp_spp_service_record,
};
#endif

#if GFPS_EN
service_record_item_t spp_gfps_sdp_record_item = {
    .service_record_handle  = 0x50010001,
    .service_record         = (uint8_t *)sdp_gfps_spp_service_record,
};
#endif

#if LE_DUEROS_DMA_EN
service_record_item_t spp_dueros_dma_sdp_record_item = {
    .service_record_handle  = 0x50010003,
    .service_record         = (uint8_t *)sdp_spp_dueros_dma_service_record,
};
#endif

#ifdef AB_MATE_SPP_UUID
extern const uint8_t  sdp_ab_mate_spp_service_record[];
service_record_item_t spp_ab_mate_sdp_record_item = {
    .service_record_handle  = 0x50010003,
    .service_record         = (uint8_t *)sdp_ab_mate_spp_service_record,
};
#endif


#if BT_HFP_EN
service_record_item_t hfp_sdp_record_item = {
    .service_record_handle  = 0x00010002,
    .service_record         = (uint8_t *)sdp_hfp_service_record,
};
#endif

#if BT_HSP_EN
service_record_item_t hsp_sdp_record_item = {
    .service_record_handle  = 0x00010008,
    .service_record         = (uint8_t *)sdp_hsp_service_record,
};
#endif

#if BT_A2DP_EN
service_record_item_t avdtp_sdp_record_item = {
    .service_record_handle  = 0x00010003,
    .service_record         = (uint8_t *)sdp_avdtp_service_record,
};

service_record_item_t avctp_ct_sdp_record_item = {
    .service_record_handle  = 0x00010004,
    .service_record         = (uint8_t *)sdp_avctp_controller_service_record,
};

service_record_item_t avctp_tg_sdp_record_item = {
    .service_record_handle  = 0x00010005,
    .service_record         = (uint8_t *)sdp_avctp_target_category2_service_record,
};
#endif // BT_A2DP_EN


#if BT_HID_EN
service_record_item_t hid_sdp_record_item = {
    .service_record_handle  = 0x00010006,
#if (BT_HID_TYPE == 0)
    .service_record         = (uint8_t *)sdp_hid_consumer_service_record,
#elif (BT_HID_TYPE == 1)
    .service_record         = (uint8_t *)sdp_hid_keyboard_service_record,
#elif (BT_HID_TYPE == 2)
    .service_record         = (uint8_t *)sdp_hid_tscreen_service_record,
#elif (BT_HID_TYPE == 3)
     .service_record         = (uint8_t *)sdp_hid_ios_service_record,
#else
    .service_record         = ...
#endif
};

service_record_item_t device_id_sdp_record_item = {
    .service_record_handle  = 0x00010007,
#if (BT_HID_TYPE == 3)
    .service_record         = (uint8_t *)sdp_pnp_ios_service_record,
#else
    .service_record         = (uint8_t *)sdp_devic_id_service_record,
#endif
};
#endif // BT_HID_EN

#if BT_ATT_EN
service_record_item_t gatt_service_record_item = {
    .service_record_handle  = 0x00010008,
    .service_record         = (uint8_t *)sdp_gatt_service_record,
};
#endif

void bt_init_lib(void)
{
    uint32_t profile = cfg_bt_support_profile;
#if BT_SPP_EN
    if (profile & PROF_SPP) {
#if GFPS_EN || LE_DUEROS_DMA_EN
		spp_support_mul_server(1);
#endif
#ifdef AB_MATE_SPP_UUID
        spp_support_mul_server(1);
#endif
        spp_init();
#if GFPS_EN
        if (gfps_is_enable()) {
            sdp_add_service(&spp_gfps_sdp_record_item);
            gfps_bt_init();
        }
#endif

#if LE_DUEROS_DMA_EN
        sdp_add_service(&spp_dueros_dma_sdp_record_item);
#endif

#ifdef AB_MATE_SPP_UUID
        sdp_add_service(&spp_ab_mate_sdp_record_item);
#endif
        sdp_add_service(&spp_sdp_record_item);
    }
#endif

#if BT_HFP_EN
    if (profile & PROF_HFP) {
        hfp_hf_init();
        sdp_add_service(&hfp_sdp_record_item);
    }
#endif

    hsp_hs_init_var();
#if BT_HSP_EN
    if (profile & PROF_HSP) {
        hsp_hs_init();
        sdp_add_service(&hsp_sdp_record_item);
    }
#endif

#if BT_A2DP_EN
    if (profile & PROF_A2DP) {
        a2dp_init();
        avctp_tg_sdp_record_item.service_record         = (uint8_t *)sdp_avctp_target_category2_service_record;
        sdp_add_service(&avdtp_sdp_record_item);
        sdp_add_service(&avctp_ct_sdp_record_item);
        sdp_add_service(&avctp_tg_sdp_record_item);
    }
#endif

#if BT_HID_EN
    if (profile & PROF_HID) {
        hid_device_init();
        sdp_add_service(&hid_sdp_record_item);
        sdp_add_service(&device_id_sdp_record_item);
    }
#endif

#if BT_PBAP_EN
    // init PBAP
    if (profile & PROF_PBAP) {
        // init PBAP Client
        goep_client_init();
        pbap_client_init();
    }
#endif

#if BT_MAP_EN
    if (profile & PROF_MAP) {
        // init MAP Client
        goep_client_init();
        map_client_init();
    }
#endif

#if BT_ATT_EN
    if(profile & PROF_GATT) {
        sdp_add_service(&gatt_service_record_item);
    }
#endif
}
