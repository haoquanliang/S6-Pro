#ifndef _MULTI_LANG_H
#define _MULTI_LANG_H

#define RES_CONST_CAT(a, b, c)      a ## b ## _ ## c

#define RES_BUF_MP3(name)           RES_CONST_CAT(RES_BUF_, name, MP3)
#define RES_LEN_MP3(name)           RES_CONST_CAT(RES_LEN_, name, MP3)
#define RES_BUF_SBC(name)           RES_CONST_CAT(RES_BUF_, name, SBC)
#define RES_LEN_SBC(name)           RES_CONST_CAT(RES_LEN_, name, SBC)

#if WARNING_WSBC_EN
#define RES_BUF(n)              	RES_BUF_SBC(n)          //主要格式，个别可单独修改tbl
#define RES_LEN(n)              	RES_LEN_SBC(n)          //主要格式，个别可单独修改tbl
#else
#define RES_BUF(n)              	RES_BUF_MP3(n)          //主要格式，个别可单独修改tbl
#define RES_LEN(n)              	RES_LEN_MP3(n)          //主要格式，个别可单独修改tbl
#endif

enum {
    RES_IDX_LANGUAGE,
    RES_IDX_LOW_BATTERY,
    RES_IDX_POWERON,
    RES_IDX_POWEROFF,
    RES_IDX_MAX_VOL,
    RES_IDX_SDCARD_MODE,
    RES_IDX_USB_MODE,
    RES_IDX_AUX_MODE,
    RES_IDX_CLOCK_MODE,
    RES_IDX_FM_MODE,
    RES_IDX_SPK_MODE,
    RES_IDX_PC_MODE,
    RES_IDX_BT_MODE,
    RES_IDX_CAMERA_MODE,
    RES_IDX_LEFT_CH,
    RES_IDX_RIGHT_CH,
    RES_IDX_PAIRING,
    RES_IDX_WAIT4CONN,
    RES_IDX_CONNECTED,
    RES_IDX_DISCONNECT,
    RES_IDX_CAMERA_ON,
    RES_IDX_CAMERA_OFF,
    RES_IDX_CALL_HANGUP,
    RES_IDX_CALL_REJECT,
    RES_IDX_REDIALING,
    RES_IDX_NUM_0,
    RES_IDX_NUM_1,
    RES_IDX_NUM_2,
    RES_IDX_NUM_3,
    RES_IDX_NUM_4,
    RES_IDX_NUM_5,
    RES_IDX_NUM_6,
    RES_IDX_NUM_7,
    RES_IDX_NUM_8,
    RES_IDX_NUM_9,
    RES_IDX_MUSIC_MODE,
    RES_IDX_GAME_MODE,
    RES_IDX_MAX,
};

#if RES_USERBIN_EN
typedef struct {
    u32 ptr;
    u32 len;
} res_addr_t;
#define RES_ITEM_PTR(a)         ((a).ptr)
#define RES_ITEM_LEN(a)         ((a).len)
#else
typedef struct {
    u32 *ptr;
    u32 *len;
} res_addr_t;
#define RES_ITEM_PTR(a)         (*(a).ptr)
#define RES_ITEM_LEN(a)         (*(a).len)
#endif

const res_addr_t *res_get_ring_num(u8 index);

#if (LANG_SELECT == LANG_EN_ZH)
void multi_lang_init(uint lang_id);

extern const res_addr_t mul_lang_tbl[2][RES_IDX_MAX];
#define MUTIL_LANG_TBL_EN           1
#define RES_BUF_LANGUAGE        RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_LANGUAGE   ])
#define RES_LEN_LANGUAGE        RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_LANGUAGE   ])
#define RES_BUF_LOW_BATTERY     RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_LOW_BATTERY])
#define RES_LEN_LOW_BATTERY     RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_LOW_BATTERY])
#define RES_BUF_POWERON         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_POWERON    ])
#define RES_LEN_POWERON         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_POWERON    ])
#define RES_BUF_POWEROFF        RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_POWEROFF   ])
#define RES_LEN_POWEROFF        RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_POWEROFF   ])
#define RES_BUF_SDCARD_MODE     RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_SDCARD_MODE])
#define RES_LEN_SDCARD_MODE     RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_SDCARD_MODE])
#define RES_BUF_USB_MODE        RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_USB_MODE   ])
#define RES_LEN_USB_MODE        RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_USB_MODE   ])
#define RES_BUF_AUX_MODE        RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_AUX_MODE   ])
#define RES_LEN_AUX_MODE        RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_AUX_MODE   ])
#define RES_BUF_CLOCK_MODE      RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CLOCK_MODE ])
#define RES_LEN_CLOCK_MODE      RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CLOCK_MODE ])
#define RES_BUF_FM_MODE         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_FM_MODE    ])
#define RES_LEN_FM_MODE         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_FM_MODE    ])
#define RES_BUF_SPK_MODE        RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_SPK_MODE   ])
#define RES_LEN_SPK_MODE        RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_SPK_MODE   ])
#define RES_BUF_PC_MODE         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_PC_MODE    ])
#define RES_LEN_PC_MODE         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_PC_MODE    ])
#define RES_BUF_MAX_VOL         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_MAX_VOL    ])
#define RES_LEN_MAX_VOL         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_MAX_VOL    ])
#define RES_BUF_BT_MODE         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_BT_MODE    ])
#define RES_LEN_BT_MODE         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_BT_MODE    ])
#define RES_BUF_CAMERA_MODE     RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CAMERA_MODE])
#define RES_LEN_CAMERA_MODE     RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CAMERA_MODE])
#define RES_BUF_LEFT_CH         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_LEFT_CH    ])
#define RES_LEN_LEFT_CH         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_LEFT_CH    ])
#define RES_BUF_RIGHT_CH        RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_RIGHT_CH   ])
#define RES_LEN_RIGHT_CH        RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_RIGHT_CH   ])
#define RES_BUF_PAIRING         RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_PAIRING    ])
#define RES_LEN_PAIRING         RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_PAIRING    ])
#define RES_BUF_WAIT4CONN       RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_WAIT4CONN  ])
#define RES_LEN_WAIT4CONN       RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_WAIT4CONN  ])
#define RES_BUF_CONNECTED       RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CONNECTED  ])
#define RES_LEN_CONNECTED       RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CONNECTED  ])
#define RES_BUF_DISCONNECT      RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_DISCONNECT ])
#define RES_LEN_DISCONNECT      RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_DISCONNECT ])
#define RES_BUF_CAMERA_ON       RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CAMERA_ON  ])
#define RES_LEN_CAMERA_ON       RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CAMERA_ON  ])
#define RES_BUF_CAMERA_OFF      RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CAMERA_OFF ])
#define RES_LEN_CAMERA_OFF      RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CAMERA_OFF ])
#define RES_BUF_CALL_HANGUP     RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CALL_HANGUP])
#define RES_LEN_CALL_HANGUP     RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CALL_HANGUP])
#define RES_BUF_CALL_REJECT     RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CALL_REJECT])
#define RES_LEN_CALL_REJECT     RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_CALL_REJECT])
#define RES_BUF_REDIALING       RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_REDIALING  ])
#define RES_LEN_REDIALING       RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_REDIALING  ])
#define RES_BUF_NUM_0           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_0      ])
#define RES_LEN_NUM_0           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_0      ])
#define RES_BUF_NUM_1           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_1      ])
#define RES_LEN_NUM_1           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_1      ])
#define RES_BUF_NUM_2           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_2      ])
#define RES_LEN_NUM_2           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_2      ])
#define RES_BUF_NUM_3           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_3      ])
#define RES_LEN_NUM_3           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_3      ])
#define RES_BUF_NUM_4           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_4      ])
#define RES_LEN_NUM_4           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_4      ])
#define RES_BUF_NUM_5           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_5      ])
#define RES_LEN_NUM_5           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_5      ])
#define RES_BUF_NUM_6           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_6      ])
#define RES_LEN_NUM_6           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_6      ])
#define RES_BUF_NUM_7           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_7      ])
#define RES_LEN_NUM_7           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_7      ])
#define RES_BUF_NUM_8           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_8      ])
#define RES_LEN_NUM_8           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_8      ])
#define RES_BUF_NUM_9           RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_9      ])
#define RES_LEN_NUM_9           RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_NUM_9      ])
#define RES_BUF_MUSIC_MODE      RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_MUSIC_MODE ])
#define RES_LEN_MUSIC_MODE      RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_MUSIC_MODE ])
#define RES_BUF_GAME_MODE       RES_ITEM_PTR(mul_lang_tbl[sys_cb.lang_id][RES_IDX_GAME_MODE  ])
#define RES_LEN_GAME_MODE       RES_ITEM_LEN(mul_lang_tbl[sys_cb.lang_id][RES_IDX_GAME_MODE  ])

#elif (LANG_SELECT == LANG_ZH)
#define MUTIL_LANG_TBL_EN               0
#define RES_BUF_POWERON             RES_BUF(ZH_POWERON)
#define RES_LEN_POWERON             RES_LEN(ZH_POWERON)
#define RES_BUF_LOW_BATTERY         RES_BUF(ZH_LOW_BATTERY)
#define RES_LEN_LOW_BATTERY         RES_LEN(ZH_LOW_BATTERY)
#define RES_BUF_SDCARD_MODE         RES_BUF(ZH_SDCARD_MODE)
#define RES_LEN_SDCARD_MODE         RES_LEN(ZH_SDCARD_MODE)
#define RES_BUF_POWEROFF            RES_BUF(ZH_POWEROFF)
#define RES_LEN_POWEROFF            RES_LEN(ZH_POWEROFF)
#define RES_BUF_USB_MODE            RES_BUF(ZH_USB_MODE)
#define RES_LEN_USB_MODE            RES_LEN(ZH_USB_MODE)
#define RES_BUF_AUX_MODE            RES_BUF(ZH_AUX_MODE)
#define RES_LEN_AUX_MODE            RES_LEN(ZH_AUX_MODE)
#define RES_BUF_BT_MODE             RES_BUF(ZH_BT_MODE)
#define RES_LEN_BT_MODE             RES_LEN(ZH_BT_MODE)
#define RES_BUF_CLOCK_MODE          RES_BUF(ZH_CLOCK_MODE)
#define RES_LEN_CLOCK_MODE          RES_LEN(ZH_CLOCK_MODE)
#define RES_BUF_CAMERA_MODE         RES_BUF(ZH_CAMERA_MODE)
#define RES_LEN_CAMERA_MODE         RES_LEN(ZH_CAMERA_MODE)
#define RES_BUF_FM_MODE             RES_BUF(ZH_FM_MODE)
#define RES_LEN_FM_MODE             RES_LEN(ZH_FM_MODE)
#define RES_BUF_SPK_MODE            RES_BUF(ZH_SPK_MODE)
#define RES_LEN_SPK_MODE            RES_LEN(ZH_SPK_MODE)
#define RES_BUF_PC_MODE             RES_BUF(ZH_PC_MODE)
#define RES_LEN_PC_MODE             RES_LEN(ZH_PC_MODE)
#define RES_BUF_MAX_VOL             RES_BUF(ZH_MAX_VOL)
#define RES_LEN_MAX_VOL             RES_LEN(ZH_MAX_VOL)

#define RES_BUF_LEFT_CH             RES_BUF(ZH_LEFT_CH)
#define RES_LEN_LEFT_CH             RES_LEN(ZH_LEFT_CH)
#define RES_BUF_RIGHT_CH            RES_BUF(ZH_RIGHT_CH)
#define RES_LEN_RIGHT_CH            RES_LEN(ZH_RIGHT_CH)
#define RES_BUF_PAIRING             RES_BUF(ZH_PAIRING)
#define RES_LEN_PAIRING             RES_LEN(ZH_PAIRING)
#define RES_BUF_WAIT4CONN           RES_BUF(ZH_WAIT4CONN)
#define RES_LEN_WAIT4CONN           RES_LEN(ZH_WAIT4CONN)
#define RES_BUF_CONNECTED           RES_BUF(ZH_CONNECTED)
#define RES_LEN_CONNECTED           RES_LEN(ZH_CONNECTED)
#define RES_BUF_DISCONNECT          RES_BUF(ZH_DISCONNECT)
#define RES_LEN_DISCONNECT          RES_LEN(ZH_DISCONNECT)
#define RES_BUF_CAMERA_ON           RES_BUF(ZH_CAMERA_ON)
#define RES_LEN_CAMERA_ON           RES_LEN(ZH_CAMERA_ON)
#define RES_BUF_CAMERA_OFF          RES_BUF(ZH_CAMERA_OFF)
#define RES_LEN_CAMERA_OFF          RES_LEN(ZH_CAMERA_OFF)

#define RES_BUF_DISCON_HID          RES_BUF(ZH_DISCON_HID)
#define RES_LEN_DISCON_HID          RES_LEN(ZH_DISCON_HID)
#define RES_BUF_CONN_HID            RES_BUF(ZH_CONN_HID)
#define RES_LEN_CONN_HID            RES_LEN(ZH_CONN_HID)
#define RES_BUF_MUSIC_MODE          RES_BUF(ZH_MUSIC_MODE)
#define RES_LEN_MUSIC_MODE          RES_LEN(ZH_MUSIC_MODE)
//#define RES_BUF_TAKE_PHOTO          RES_BUF(ZH_TAKE_PHOTO)
//#define RES_LEN_TAKE_PHOTO          RES_LEN(ZH_TAKE_PHOTO)
#define RES_BUF_PRIV_CALL           RES_BUF(ZH_PRIV_CALL)
#define RES_LEN_PRIV_CALL           RES_LEN(ZH_PRIV_CALL)
#define RES_BUF_CALL_HANGUP         RES_BUF(ZH_CALL_HANGUP)
#define RES_LEN_CALL_HANGUP         RES_LEN(ZH_CALL_HANGUP)
#define RES_BUF_CALL_REJECT         RES_BUF(ZH_CALL_REJECT)
#define RES_LEN_CALL_REJECT         RES_LEN(ZH_CALL_REJECT)
#define RES_BUF_REDIALING           RES_BUF(ZH_REDIALING)
#define RES_LEN_REDIALING           RES_LEN(ZH_REDIALING)
#define RES_BUF_REC_START           RES_BUF(ZH_REC_START)
#define RES_LEN_REC_START           RES_LEN(ZH_REC_START)

#define RES_BUF_NUM_0               RES_BUF(ZH_NUM_0)
#define RES_LEN_NUM_0               RES_LEN(ZH_NUM_0)
#define RES_BUF_NUM_1               RES_BUF(ZH_NUM_1)
#define RES_LEN_NUM_1               RES_LEN(ZH_NUM_1)
#define RES_BUF_NUM_2               RES_BUF(ZH_NUM_2)
#define RES_LEN_NUM_2               RES_LEN(ZH_NUM_2)
#define RES_BUF_NUM_3               RES_BUF(ZH_NUM_3)
#define RES_LEN_NUM_3               RES_LEN(ZH_NUM_3)
#define RES_BUF_NUM_4               RES_BUF(ZH_NUM_4)
#define RES_LEN_NUM_4               RES_LEN(ZH_NUM_4)
#define RES_BUF_NUM_5               RES_BUF(ZH_NUM_5)
#define RES_LEN_NUM_5               RES_LEN(ZH_NUM_5)
#define RES_BUF_NUM_6               RES_BUF(ZH_NUM_6)
#define RES_LEN_NUM_6               RES_LEN(ZH_NUM_6)
#define RES_BUF_NUM_7               RES_BUF(ZH_NUM_7)
#define RES_LEN_NUM_7               RES_LEN(ZH_NUM_7)
#define RES_BUF_NUM_8               RES_BUF(ZH_NUM_8)
#define RES_LEN_NUM_8               RES_LEN(ZH_NUM_8)
#define RES_BUF_NUM_9               RES_BUF(ZH_NUM_9)
#define RES_LEN_NUM_9               RES_LEN(ZH_NUM_9)
#define RES_BUF_MUSIC_MODE          RES_BUF(ZH_MUSIC_MODE)
#define RES_LEN_MUSIC_MODE          RES_LEN(ZH_MUSIC_MODE)
#define RES_BUF_GAME_MODE           RES_BUF(ZH_GAME_MODE)
#define RES_LEN_GAME_MODE           RES_LEN(ZH_GAME_MODE)

#elif (LANG_SELECT == LANG_EN)
#define MUTIL_LANG_TBL_EN               0
#define RES_BUF_POWERON             RES_BUF(EN_POWERON)
#define RES_LEN_POWERON             RES_LEN(EN_POWERON)
#define RES_BUF_LOW_BATTERY         RES_BUF(EN_LOW_BATTERY)
#define RES_LEN_LOW_BATTERY         RES_LEN(EN_LOW_BATTERY)
#define RES_BUF_SDCARD_MODE         RES_BUF(EN_SDCARD_MODE)
#define RES_LEN_SDCARD_MODE         RES_LEN(EN_SDCARD_MODE)
#define RES_BUF_POWEROFF            RES_BUF(EN_POWEROFF)
#define RES_LEN_POWEROFF            RES_LEN(EN_POWEROFF)
#define RES_BUF_USB_MODE            RES_BUF(EN_USB_MODE)
#define RES_LEN_USB_MODE            RES_LEN(EN_USB_MODE)
#define RES_BUF_AUX_MODE            RES_BUF(EN_AUX_MODE)
#define RES_LEN_AUX_MODE            RES_LEN(EN_AUX_MODE)
#define RES_BUF_BT_MODE             RES_BUF(EN_BT_MODE)
#define RES_LEN_BT_MODE             RES_LEN(EN_BT_MODE)
#define RES_BUF_CLOCK_MODE          RES_BUF(EN_CLOCK_MODE)
#define RES_LEN_CLOCK_MODE          RES_LEN(EN_CLOCK_MODE)
#define RES_BUF_CAMERA_MODE         RES_BUF(EN_CAMERA_MODE)
#define RES_LEN_CAMERA_MODE         RES_LEN(EN_CAMERA_MODE)
#define RES_BUF_FM_MODE             RES_BUF(EN_FM_MODE)
#define RES_LEN_FM_MODE             RES_LEN(EN_FM_MODE)
#define RES_BUF_SPK_MODE            RES_BUF(EN_SPK_MODE)
#define RES_LEN_SPK_MODE            RES_LEN(EN_SPK_MODE)
#define RES_BUF_PC_MODE             RES_BUF(EN_PC_MODE)
#define RES_LEN_PC_MODE             RES_LEN(EN_PC_MODE)
#define RES_BUF_MAX_VOL             RES_BUF(EN_MAX_VOL)
#define RES_LEN_MAX_VOL             RES_LEN(EN_MAX_VOL)

#define RES_BUF_LEFT_CH             RES_BUF(EN_LEFT_CH)
#define RES_LEN_LEFT_CH             RES_LEN(EN_LEFT_CH)
#define RES_BUF_RIGHT_CH            RES_BUF(EN_RIGHT_CH)
#define RES_LEN_RIGHT_CH            RES_LEN(EN_RIGHT_CH)
#define RES_BUF_PAIRING             RES_BUF(EN_PAIRING)
#define RES_LEN_PAIRING             RES_LEN(EN_PAIRING)
#define RES_BUF_WAIT4CONN           RES_BUF(EN_WAIT4CONN)
#define RES_LEN_WAIT4CONN           RES_LEN(EN_WAIT4CONN)
#define RES_BUF_CONNECTED           RES_BUF(EN_CONNECTED)
#define RES_LEN_CONNECTED           RES_LEN(EN_CONNECTED)
#define RES_BUF_DISCONNECT          RES_BUF(EN_DISCONNECT)
#define RES_LEN_DISCONNECT          RES_LEN(EN_DISCONNECT)
#define RES_BUF_CAMERA_ON           RES_BUF(EN_CAMERA_ON)
#define RES_LEN_CAMERA_ON           RES_LEN(EN_CAMERA_ON)
#define RES_BUF_CAMERA_OFF          RES_BUF(EN_CAMERA_OFF)
#define RES_LEN_CAMERA_OFF          RES_LEN(EN_CAMERA_OFF)

#define RES_BUF_DISCON_HID          RES_BUF(EN_DISCON_HID)
#define RES_LEN_DISCON_HID          RES_LEN(EN_DISCON_HID)
#define RES_BUF_CONN_HID            RES_BUF(EN_CONN_HID)
#define RES_LEN_CONN_HID            RES_LEN(EN_CONN_HID)
#define RES_BUF_MUSIC_MODE          RES_BUF(EN_MUSIC_MODE)
#define RES_LEN_MUSIC_MODE          RES_LEN(EN_MUSIC_MODE)
//#define RES_BUF_TAKE_PHOTO          RES_BUF(EN_TAKE_PHOTO)
//#define RES_LEN_TAKE_PHOTO          RES_LEN(EN_TAKE_PHOTO)
#define RES_BUF_PRIV_CALL           RES_BUF(EN_PRIV_CALL)
#define RES_LEN_PRIV_CALL           RES_LEN(EN_PRIV_CALL)
#define RES_BUF_CALL_HANGUP         RES_BUF(EN_CALL_HANGUP)
#define RES_LEN_CALL_HANGUP         RES_LEN(EN_CALL_HANGUP)
#define RES_BUF_CALL_REJECT         RES_BUF(EN_CALL_REJECT)
#define RES_LEN_CALL_REJECT         RES_LEN(EN_CALL_REJECT)
#define RES_BUF_REDIALING           RES_BUF(EN_REDIALING)
#define RES_LEN_REDIALING           RES_LEN(EN_REDIALING)
#define RES_BUF_REC_START           RES_BUF(EN_REC_START)
#define RES_LEN_REC_START           RES_LEN(EN_REC_START)

#define RES_BUF_NUM_0               RES_BUF(EN_NUM_0)
#define RES_LEN_NUM_0               RES_LEN(EN_NUM_0)
#define RES_BUF_NUM_1               RES_BUF(EN_NUM_1)
#define RES_LEN_NUM_1               RES_LEN(EN_NUM_1)
#define RES_BUF_NUM_2               RES_BUF(EN_NUM_2)
#define RES_LEN_NUM_2               RES_LEN(EN_NUM_2)
#define RES_BUF_NUM_3               RES_BUF(EN_NUM_3)
#define RES_LEN_NUM_3               RES_LEN(EN_NUM_3)
#define RES_BUF_NUM_4               RES_BUF(EN_NUM_4)
#define RES_LEN_NUM_4               RES_LEN(EN_NUM_4)
#define RES_BUF_NUM_5               RES_BUF(EN_NUM_5)
#define RES_LEN_NUM_5               RES_LEN(EN_NUM_5)
#define RES_BUF_NUM_6               RES_BUF(EN_NUM_6)
#define RES_LEN_NUM_6               RES_LEN(EN_NUM_6)
#define RES_BUF_NUM_7               RES_BUF(EN_NUM_7)
#define RES_LEN_NUM_7               RES_LEN(EN_NUM_7)
#define RES_BUF_NUM_8               RES_BUF(EN_NUM_8)
#define RES_LEN_NUM_8               RES_LEN(EN_NUM_8)
#define RES_BUF_NUM_9               RES_BUF(EN_NUM_9)
#define RES_LEN_NUM_9               RES_LEN(EN_NUM_9)
#define RES_BUF_MUSIC_MODE          RES_BUF(EN_MUSIC_MODE)
#define RES_LEN_MUSIC_MODE          RES_LEN(EN_MUSIC_MODE)
#define RES_BUF_GAME_MODE           RES_BUF(EN_GAME_MODE)
#define RES_LEN_GAME_MODE           RES_LEN(EN_GAME_MODE)

#else
    #error "请选择提示音语言\n"
#endif

#endif
