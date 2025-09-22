#include "include.h"

#if USER_TKEY
AT(.rodata.tkey)
const tkey_ch_t tkey0 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x08,
    .dlypr = 13,
};

#if USER_TKEY_MULT
const tkey_ch_t tkey1 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x08,
    .dlypr = 13,
};

const tkey_ch_t tkey2 = {
    .cdpr = CDPR,
    .type = 1,
    .ctrim = 0,
    .itrim = 0x08,
    .dlypr = 13,
};
#endif // USER_TKEY_MULT

#if USER_TKEY_INEAR
AT(.rodata.tkey)
const tkey_ch_t tkey_inear = {
    .cdpr = EAR_CDPR,
    .type = 2,
    .ctrim = 0,
    .itrim = 0x08,
    .dlypr = 8,
};
#endif

//calibration专用
AT(.rodata.tkey)
const tkey_ch_t tkey_temp_cali_ch = {       //温度通道
    .cdpr = EAR_CDPR,
    .type = 0,
    .ctrim = 0x1f,
    .itrim = 0x08,
    .dlypr = 4,
};

AT(.rodata.tkey)
const tkey_cfg_t tkey_cfg = {
    .key[0] = &tkey0,               //PB5
    .key[1] = NULL,                 //PB0
#if USER_TKEY_INEAR
    .key[2] = &tkey_inear,          //PB1
#endif
    .key[3] = NULL,                 //PB2
    .key[4] = NULL,                 //PA7
    .key[5] = NULL,					//温度通道

    //TKTMR Register
    .fil_low = FIL_LOW,
    .fil_high = FIL_HIGH,
    .fil_except = FIL_EXCEPT,
    .fil_val = FIL_VAL,
    .to_except = TO_EXCEPT,

    //TKPTHD Register
    .tkpthd = TKPTHRESH,
    .tkrthd = TKRTHRESH,
    .pto_except = PTO_EXCEPT,

    //TKETHD Register
    .tksthd = SMALLTHD,
    .tklthd = LARGETHD,

    //TKVARI Register
    .tkvthd = 28,

    //TKVARITHD Register
    .tkarthd = TKRTHRESH-3,
    .tkaethd = SMALLTHD,
    .tkvfil  = 20,
    .tkbadd  = SMALLTHD,

    //TKCON2 Register
     .press_vari_en = 0,
     .rels_vari_en = 0,
     .press_fil_sel = 1,
     .rels_fil_sel = 1,
     .press_vari_thd = 10,
     .rels_vari_thd = 10,
     .to_bcnt_thd = 10,

    //TETMR Register
    .ear_fil_low = EAR_FIL_LOW,
    .ear_fil_high = EAR_FIL_HIGH,
    .ear_fil_except = EAR_FIL_EXCEPT,
    .ear_fil_val = EAR_FIL_VAL,
    .tkpwup = TKPWUP,

    //TEPTHD Register
    .tepthd = TEPTHRESH,
    .terthd = TERTHRESH,

    //TEETHD Register
    .testhd = TE_SMALLTHD,
    .telthd = TE_LARGETHD,
};

#endif
