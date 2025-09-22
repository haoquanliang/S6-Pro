#ifndef __BT_CALL
#define __BT_CALL

extern const int mic_gain_tbl[32];
void bt_sco_aec_init(u8 *sysclk, aec_cfg_t *aec, alc_cb_t *alc);
void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_exit(void);
void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr);

void bt_sco_sndp_dm_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dmns_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_ldmdnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_sndp_sm_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_near_nr_dft_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dnn_plus_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dnn_pro_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_aiaec_dnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_aiaec_dmdnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_aiaec_dnn_exit(void);
void bt_sco_ffdm_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_ffdm_pro_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_user_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_sndp_fbdm_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_sndp_dm_init(u8 *sysclk, nr_cb_t *nr);


#if BT_SNDP_SMIC_AI_EN
    #define bt_sco_near_nr_init(a, b)    	bt_sco_sndp_sm_init(a, b)
    #define bt_sco_near_nr_exit()           bt_sndp_sm_exit()
#elif BT_SNDP_DMIC_EN
    #define bt_sco_near_nr_init(a, b)   	bt_sco_sndp_dm_init(a, b)
    #define bt_sco_near_nr_exit()
#elif BT_SCO_DMIC_AI_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dmns_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dmns_exit()
#elif BT_SCO_LDMIC_AI_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_ldmdnn_init(a, b)
    #define bt_sco_near_nr_exit()           bt_ldmdnn_exit()
#elif BT_SCO_SMIC_AI_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dnn_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dnn_exit()
#elif BT_SCO_SMIC_AI_PLUS_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dnn_plus_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dnn_plus_exit()
#elif BT_SCO_SMIC_AI_PRO_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_dnn_pro_init(a, b)
    #define bt_sco_near_nr_exit()           bt_dnn_pro_exit()
#elif BT_SCO_AIAEC_DNN_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_aiaec_dnn_init(a, b)
    #define bt_sco_near_nr_exit()           bt_aiaec_dnn_exit()
#elif BT_SCO_DMIC_AIAEC_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_aiaec_dmdnn_init(a, b)
    #define bt_sco_near_nr_exit()           bt_aiaec_dmdnn_exit()
#elif BT_SCO_FFDM_AI_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_ffdm_init(a, b)
    #define bt_sco_near_nr_exit()           bt_ffdm_exit()
#elif BT_SCO_FFDM_AI_PRO_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_ffdm_pro_init(a, b)
    #define bt_sco_near_nr_exit()           bt_ffdm_pro_exit()
#elif BT_SCO_NR_USER_SMIC_EN || BT_SCO_NR_USER_DMIC_EN
    #define bt_sco_near_nr_init(a, b)       bt_sco_nr_user_init(a, b)
    #define bt_sco_near_nr_exit()
#elif BT_SNDP_FBDM_EN
    #define bt_sco_near_nr_init(a, b)    	bt_sco_sndp_fbdm_init(a, b)
    #define bt_sco_near_nr_exit()           bt_sndp_fbdm_exit()
#elif BT_SNDP_DM_AI_EN
    #define bt_sco_near_nr_init(a, b)    	bt_sco_sndp_dm_init(a, b)
    #define bt_sco_near_nr_exit()           bt_sndp_dm_exit()
#else
    #define bt_sco_near_nr_init(a, b)       bt_sco_near_nr_dft_init(a, b)
    #define bt_sco_near_nr_exit()
#endif

#endif
