#include "lpc17xx_timer.h"

void timer_init() {
    // initilises the timer with prescale for miliseconds
    TIM_TIMERCFG_Type cfg;
    cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    cfg.PrescaleValue = 1000;

    TIM_Init(LPC_TIM3, TIM_TIMER_MODE, &cfg);
    TIM_Cmd(LPC_TIM3, ENABLE);
}

uint32_t timer_millis() {
    return LPC_TIM3->TC;
}
