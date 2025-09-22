#include "include.h"
#include "func.h"


#if FUNC_AUX_EN
AT(.text.func.aux.msg)
void func_aux_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_USER_DEF:
        case KU_PLAY_PWR_USER_DEF:
            func_aux_pause_play();
            break;

        case EVT_LINEIN_REMOVE:
            func_cb.sta = FUNC_BT;
            break;

#if ANC_EN
        case KU_ANC:
        case KD_ANC:
        case KL_ANC:
            sys_cb.anc_user_mode++;
            if (sys_cb.anc_user_mode > 2) {
                sys_cb.anc_user_mode = 0;
            }
            bsp_anc_set_mode(sys_cb.anc_user_mode);
            break;
#endif

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_AUX_EN
