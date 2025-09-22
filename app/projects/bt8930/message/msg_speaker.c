#include "include.h"
#include "func.h"

#if FUNC_SPEAKER_EN
AT(.text.func.speaker.msg)
void func_speaker_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_USER_DEF:
        case KU_PLAY_PWR_USER_DEF:
            bsp_clr_mute_sta();
            func_speaker_pause_play();
            break;

#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_SPEAKER_EN
