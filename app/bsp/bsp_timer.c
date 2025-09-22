#include "include.h"


#if 0
///软件timer capture示例

#define TMR3_RCLK                       (1000*cfg_bt_tws_tick_freq)

//timer3 capture
AT(.com_text.tmr.isr)
void timer3_isr(void)
{
    u32 tmrcnt;

    if (TMR3CON & BIT(10)) {            //timer3 capture interrupt

        TMR3CNT  = TMR3CNT - TMR3CPT;
        tmrcnt = TMR3CPT;               //capture value
        TMR3CPND = BIT(10);
        tmrcnt /= TMR3_RCLK;            //convert to ms
    } else if (TMR3CON & BIT(9)) {       //timer3 overflow interrupt
        TMR3CPND = BIT(9);
        tmrcnt = 110;                   //110ms overflow
    } else {
        return;
    }
//    irrx_sw_isr(tmrcnt);
}

#define IR_CAPTURE_PORT()               {GPIOEDE |= BIT(4); GPIOEPU |= BIT(4); GPIOEDIR |= BIT(4);}
#define TMR3CAP_MAPPING                 TMR3MAP_PE4

AT(.text.tmr.init)
void timer3_init(void)
{
    IR_CAPTURE_PORT();
    FUNCMCON2 = TMR3CAP_MAPPING;

    sys_irq_init(IRQ_TMR3_VECTOR, 0, timer3_isr);
	TMR3CNT  = 0;
	TMR3PR   = TMR3_RCLK*110 - 1;                                               //110ms Timer overflow interrupt
	TMR3CON  = BIT(8) | BIT(7) | BIT(5) | BIT(3)| BIT(2) | BIT(1) | BIT(0);     //capture & overflow interrupt enable, falling edge, Capture Mode
}

#endif

//软件timer timer-mode示例:timer3 time-mode
//AT(.com_rodata.isr)
//const char str_t3[] = " -------------- T3 --------------\n";
//
//AT(.com_text.isr)
//void timer3_isr(void)                   //timer3中断函数
//{
//    if (TMR3CON & BIT(9)) {             //overflow Pending
//        TMR3CPND = BIT(9);
//        my_printf(str_t3);
//    }
//}
//
///*****************************************************************************
// * 功能   : 配置timer3计时模式
// * 注意   : TMRCON3 bit[6:4]为分频系数, 001:div2, 010:div4,..., 110:div64 至少配1;
//            TMRCON3 bit7:计数模式, 触发计数中断; bit8:捕捉模式，触发捕捉中断;
//            中断触发函数timer3_isr, 整个函数都要放在公共区;
// * 返回   : 无
// *****************************************************************************/
//void timer3_init(void)
//{
//    CLKGAT0 |= BIT(3);                          //timer3 clkgat 要放在最前面
//    asm("nop");asm("nop");
//    asm("nop");asm("nop");
//
//    u8 div_sel = 2;                             //对应BIT4
//    u32 tim3pr_1s = 24000000 / div_sel - 1;     //1s对应的CNT
//    TMR3CON = 0;
//    TMR3CON |= BIT(4) | BIT(2);                 //BIT2:Timer clk select xosc24mhz; BIT(4):prepare clk div2
//
//	TMR3CNT = 0;
//	TMR3PR  = tim3pr_1s / 2;                    //示例500ms
//	TMR3CON |= BIT(7);                          //Timer works in Timer Mode，Timer overflow interrupt enable
//    TMR3CON |= BIT(0);                          //Timer enable
//    sys_irq_init(IRQ_TMR3_VECTOR, 0, timer3_isr);
//}

///PWM 呼吸灯示例
//void timer5_pwm2_g2_pb4(void)
//{
//    printf("timer5_pwm2_g2_pb4\n");
//    //初始化io
//    GPIOBDIR &= ~BIT(4);
//    GPIOBDE |= BIT(4);
//    GPIOBFEN |= BIT(4);
//    //选择功能映射io
//    FUNCMCON1 = (2 << 16);
//
//    //开启时钟源
//    CLKGAT0 |= BIT(5);
//    asm("nop");asm("nop");
//    asm("nop");asm("nop");
//    TMR5CON = BIT(20);
//    TMR5CON |= (1 << 4) | BIT(2);
//
//    //填写timer的pr值；此示例选择的是24mhz时钟源，即1s内 timer_cnt会自增 23999999次；
//    //当timer_cnt自增值达到pr时，则timer_cnt会重新置1，再继续自增；
//    //当pr填 24000000 / 500 - 1，意味着timer_cnt 自增到该pr值，则恢复1，即1s内会进行500次循环 =>500hz
//    u32 tim5pr = 24000000 / 500 - 1;
//    TMR5PR = tim5pr/2;         //500hz
//    TMR5CNT = 1;
//    //开启timer和pwm2，1s内，500次去读占空比和推出占空比的高电平
//    TMR5CON |= BIT(0);
//    //初始化占空比为0，在5ms中断里，会继续修改此占空比
////    TMR5DUTY2 = 0;
//    TMR5DUTY2 = tim5pr/4;       //示例：50%占空比
//}
//
//#define DUTY_STEP  (48000 /200)              //调整呼吸灯步进，分子固定 (24000000 / 500)，增大分母，占空比则步进增大
//AT(.com_text.timer)
//void breath_led_scan(void)                   //调整呼吸灯频率，放到5ms中断就跑的更快，20ms中断就慢一些，和步进配合决定效果
//{
//    static bool dir_up = true;               //判断呼还是吸
//    static u32 duty = 0;                     //当前输出高平的带宽
//
//    if (dir_up) {                            //开始吸
//        duty += DUTY_STEP;
//        if (duty > 48000) {
//            dir_up = false;
//            duty = 48000;
//        }
//    } else {
//        if (duty > DUTY_STEP) {               //开始呼
//            duty -= DUTY_STEP;
//        } else {
//            duty = 0;
//            dir_up = true;
//        }
//    }
//    TMR5DUTY2 = duty;
//}


