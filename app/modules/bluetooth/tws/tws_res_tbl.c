#include "include.h"

#if WARNING_WSBC_EN
#define TWS_RES_TYPE        RES_TYPE_WSBC
#else
#define TWS_RES_TYPE        RES_TYPE_MP3
#endif

//可自定义修改tws_res的资源类型
AT(.rodata.tws.res.type)
static const u8 tws_res_type_tbl[TWS_RES_MAX] =
{
    [TWS_RES_NUM_0]         = TWS_RES_TYPE,
    [TWS_RES_NUM_1]         = TWS_RES_TYPE,
    [TWS_RES_NUM_2]         = TWS_RES_TYPE,
    [TWS_RES_NUM_3]         = TWS_RES_TYPE,
    [TWS_RES_NUM_4]         = TWS_RES_TYPE,
    [TWS_RES_NUM_5]         = TWS_RES_TYPE,
    [TWS_RES_NUM_6]         = TWS_RES_TYPE,
    [TWS_RES_NUM_7]         = TWS_RES_TYPE,
    [TWS_RES_NUM_8]         = TWS_RES_TYPE,
    [TWS_RES_NUM_9]         = TWS_RES_TYPE,
    [TWS_RES_RING]          = TWS_RES_TYPE,
    [TWS_RES_MAX_VOL]       = WARING_MAXVOL_TYPE,
    [TWS_RES_LOW_BATTERY]   = TWS_RES_TYPE,
    [TWS_RES_CONNECTED]     = TWS_RES_TYPE,
    [TWS_RES_DISCONNECT]    = TWS_RES_TYPE,
    [TWS_RES_PAIRING]       = TWS_RES_TYPE,
    [TWS_RES_LANGUAGE_EN]   = TWS_RES_TYPE,
    [TWS_RES_LANGUAGE_ZH]   = TWS_RES_TYPE,

    [TWS_RES_CALL_HANGUP]   = TWS_RES_TYPE,
    [TWS_RES_CALL_REJECT]   = TWS_RES_TYPE,
    [TWS_RES_REDIALING]     = TWS_RES_TYPE,
    [TWS_RES_MUSIC_MODE]    = TWS_RES_TYPE,
    [TWS_RES_GAME_MODE]     = TWS_RES_TYPE,
    [TWS_RES_NR_DISABLE]    = RES_TYPE_WAV,
    [TWS_RES_ANC]           = RES_TYPE_WAV,
    [TWS_RES_TRANSPARENCY]  = RES_TYPE_WAV,
#if SWETZ
    [TWS_RES_UGRING]        = RES_TYPE_WAV,
    [TWS_RES_EVEN]          = RES_TYPE_MP3,
    [TWS_RES_EQ_TONE]       = RES_TYPE_MP3,
#endif
#if ASR_EN
    [TWS_RES_ASR_ON]        = TWS_RES_TYPE,
    [TWS_RES_ASR_OFF]       = TWS_RES_TYPE,
#endif
};

AT(.text.tws.res.type) WEAK
u8 tws_res_get_type(u32 index)
{
    if(index >= TWS_RES_MAX) {
        printf("tws_res: invaild index = %d\n", index);
        return RES_TYPE_INVALID;
    }

    return tws_res_type_tbl[index];
}

AT(.text.tws.res.addr) WEAK
void tws_res_get_addr(u32 index, u32 *addr, u32 *len)
{
    switch(index) {
    case TWS_RES_NUM_0:
        *addr = RES_BUF_NUM_0;
        *len = RES_LEN_NUM_0;
        break;

    case TWS_RES_NUM_1:
        *addr = RES_BUF_NUM_1;
        *len = RES_LEN_NUM_1;
        break;

    case TWS_RES_NUM_2:
        *addr = RES_BUF_NUM_2;
        *len = RES_LEN_NUM_2;
        break;

    case TWS_RES_NUM_3:
        *addr = RES_BUF_NUM_3;
        *len = RES_LEN_NUM_3;
        break;

    case TWS_RES_NUM_4:
        *addr = RES_BUF_NUM_4;
        *len = RES_LEN_NUM_4;
        break;

    case TWS_RES_NUM_5:
        *addr = RES_BUF_NUM_5;
        *len = RES_LEN_NUM_5;
        break;

    case TWS_RES_NUM_6:
        *addr = RES_BUF_NUM_6;
        *len = RES_LEN_NUM_6;
        break;

    case TWS_RES_NUM_7:
        *addr = RES_BUF_NUM_7;
        *len = RES_LEN_NUM_7;
        break;

    case TWS_RES_NUM_8:
        *addr = RES_BUF_NUM_8;
        *len = RES_LEN_NUM_8;
        break;

    case TWS_RES_NUM_9:
        *addr = RES_BUF_NUM_9;
        *len = RES_LEN_NUM_9;
        break;


    case TWS_RES_RING:
        *addr = RES_BUF(RING);
        *len = RES_LEN(RING);
        break;
#if SWETZ_UGRING
        case TWS_RES_UGRING:
        *addr = RES_BUF_UG_RING_WAV;
        *len = RES_LEN_UG_RING_WAV;

        break;
#endif

#if APP_EQ_TONE
        case TWS_RES_EQ_TONE:
            *addr = RES_BUF_EQ_MP3;
            *len = RES_LEN_EQ_MP3;
        break;   
#endif



    case TWS_RES_CONNECTED:
        *addr = RES_BUF_CONNECTED;
        *len = RES_LEN_CONNECTED;
        break;

    case TWS_RES_DISCONNECT:
        *addr = RES_BUF_DISCONNECT;
        *len = RES_LEN_DISCONNECT;
        break;

    case TWS_RES_EVEN:
        *addr = RES_BUF_EVEN_TONE_MP3;
        *len  = RES_LEN_EVEN_TONE_MP3;
        break;
        
    case TWS_RES_PAIRING:
        *addr = RES_BUF_PAIRING;
        *len = RES_LEN_PAIRING;
        break;

    case TWS_RES_LOW_BATTERY:
        *addr = RES_BUF_LOW_BATTERY;
        *len = RES_LEN_LOW_BATTERY;
        break;

    case TWS_RES_MAX_VOL:
#if WARING_MAXVOL_TYPE == RES_TYPE_PIANO
//        *addr = (u32)&warning_piano_tbl[PIANO_MAX_VOL];
//        *len = 1;                       //flag
#elif WARING_MAXVOL_TYPE == RES_TYPE_TONE
#if WARNING_PIANO_EN
        *addr = (u32)&warning_tone_tbl[TONE_MAX_VOL];
        *len = 1;                       //flag
#endif
#elif WARING_MAXVOL_TYPE == RES_TYPE_WAV
        *addr = RES_BUF_MAX_VOL_WAV;
        *len = RES_LEN_MAX_VOL_WAV;
#elif WARING_MAXVOL_TYPE == RES_TYPE
        *addr = RES_BUF_MAX_VOL;
        *len = RES_LEN_MAX_VOL;
#endif
        break;

#if (LANG_SELECT == LANG_EN_ZH)
    case TWS_RES_LANGUAGE_ZH:
        *addr = RES_BUF(ZH_LANGUAGE);
        *len = RES_LEN(ZH_LANGUAGE);
        break;

    case TWS_RES_LANGUAGE_EN:
        *addr = RES_BUF(EN_LANGUAGE);
        *len = RES_LEN(EN_LANGUAGE);
        break;
#endif

    case TWS_RES_CALL_HANGUP:
        *addr = RES_BUF_CALL_HANGUP;
        *len = RES_LEN_CALL_HANGUP;
        break;

    case TWS_RES_CALL_REJECT:
        *addr = RES_BUF_CALL_REJECT;
        *len = RES_LEN_CALL_REJECT;
        break;

    case TWS_RES_REDIALING:
        *addr = RES_BUF_REDIALING;
        *len = RES_LEN_REDIALING;
        break;

    case TWS_RES_MUSIC_MODE:
        *addr = RES_BUF_MUSIC_MODE;
        *len = RES_LEN_MUSIC_MODE;
        break;

    case TWS_RES_GAME_MODE:
        *addr = RES_BUF_GAME_MODE;
        *len = RES_LEN_GAME_MODE;
        break;

    case TWS_RES_NR_DISABLE:
        *addr = RES_BUF_NR_DISABLE_WAV;
        *len = RES_LEN_NR_DISABLE_WAV;
        break;
    case TWS_RES_ANC:
        *addr = RES_BUF_ANC_ON_WAV;
        *len = RES_LEN_ANC_ON_WAV;
        break;
    case TWS_RES_TRANSPARENCY:
        *addr = RES_BUF_TRANSPARENCY_WAV;
        *len = RES_LEN_TRANSPARENCY_WAV;
        break;
#if ASR_EN
    case TWS_RES_ASR_ON:
        *addr = RES_BUF_ASR_ON;
        *len = RES_LEN_ASR_ON;
        break;

    case TWS_RES_ASR_OFF:
        *addr = RES_BUF_ASR_OFF;
        *len = RES_LEN_ASR_OFF;
        break;
#endif
    default :
        *addr = 0;
        *len = 0;
        break;
    }
}
