#include "leds.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "serial.h"

void timer_init() {
    setupLeds();
    clearLeds();

    // initilises the timer with prescale for miliseconds
    TIM_TIMERCFG_Type cfg;
    cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    cfg.PrescaleValue = 1000;

    TIM_Init(LPC_TIM3, TIM_TIMER_MODE, &cfg);
    TIM_Cmd(LPC_TIM3, ENABLE);

    LPC_TIM3->MCR = 0b001;
    LPC_TIM3->MR0 = 500;

    LPC_TIM3->CCR = 0;
    LPC_TIM3->EMR = 0;
    NVIC_EnableIRQ(TIMER3_IRQn);
}

uint32_t timer_get() {
    return LPC_TIM3->TC;
}

void timer_block(uint32_t ms) {
    uint32_t init = timer_get();
    // serial_printf("[Timer]: init %dms block:%d\r\n", ms, init);
    while (timer_get() - init < ms)
        ;
}

volatile uint8_t on = 0;
void TIMER3_IRQHandler() {
    TIM_ClearIntPending(LPC_TIM3, TIM_MR0_INT);

    if (on == 0) {
        GPIO_SetValue(LEDPORT, LEDS[1]);
        on = 1;
    } else if (on == 1) {
        GPIO_ClearValue(LEDPORT, LEDS[1]);
        on = 0;
    }
}