#include "include.h"
#include "wsbc.h"

#define WSBC_BUF_SIZE           2500
#define WSBC_PKT_LEN            40
#define WSBC_MAX_FRAME_SIZE     20*16

#if WARNING_WSBC_EN

AT(.wsbc_buf.dec.cb)
static u8 wsbc_mem[WSBC_BUF_SIZE];

bool wsbc_play_init(u8 *header, u16 *sample_rate, u8 *frame_size)
{
    u8 *buf = header;
    int err;

    if(buf[0] != 0x7a) {
        return false;
    }
    *sample_rate = GET_LE16(&buf[2]);
    *frame_size = buf[1];

    void *handle = wsbc_dec_init(header, *sample_rate, 1, &err, (void *)wsbc_mem, WSBC_BUF_SIZE);

    printf("wsbc_init: %d, %x, spr=%d\n", err, handle, *sample_rate);
	if(err != 0 || handle == NULL) {
        return false;
    }

    return true;
}

//测试全部wsbc提示音
void wsbc_play_test(void)
{
//    bt_audio_bypass();
//    warning_play((u8 *)RES_BUF_EN_POWERON_SBC, RES_LEN_EN_POWERON_SBC);
//    warning_play((u8 *)RES_BUF_EN_POWEROFF_SBC, RES_LEN_EN_POWEROFF_SBC);
//    warning_play((u8 *)RES_BUF_EN_CONNECTED_SBC, RES_LEN_EN_CONNECTED_SBC);
//    warning_play((u8 *)RES_BUF_EN_DISCONNECT_SBC, RES_LEN_EN_DISCONNECT_SBC);
//    bt_audio_enable();
}
#endif
