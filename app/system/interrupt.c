#include "include.h"


AT(.com_text.timer)
void timer1_isr(void)
{
    if (TMR1CON & BIT(9)) {
        TMR1CPND = BIT(9);              //Clear Pending
        TMR1CON = 0;
#if (GUI_SELECT == GUI_LEDSEG_7P7S)
        ledseg_7p7s_clr();              //close display
#elif (GUI_SELECT == GUI_LEDSEG_6C6S)
        ledseg_6c6s_clr();
#endif
    }
}
