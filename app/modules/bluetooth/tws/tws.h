#ifndef __TWS_H_

#include "tws_sync.h"


enum {
    NO_DISTRIBUTION,
    LEFT_CHANNEL,
    RIGHT_CHANNEL,
};

enum {
    TWS_SYNC_TAG_GFP = 1,
    TWS_SYNC_TAG_APP,
};


void tws_lr_xcfg_sel(void);
void tws_get_lr_channel(void);
u8 bt_tws_get_tws_channel(void);
void bt_tws_channel_read(void);
u8 bt_tws_get_tws_role(void);
bool bt_tws_channel_check(u8 channel);
void tws_user_key_process(uint32_t *opcode);
void tws_warning_process(uint32_t *opcode);

#endif // __TWS_H_
