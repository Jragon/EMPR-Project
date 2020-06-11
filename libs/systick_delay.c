#include <lpc17xx_systick.h>

#include "systick_delay.h"

volatile uint32_t SysTick_Count = 0;
volatile uint32_t SysTick_FlagLimit = 0;
volatile uint32_t SysTick_FlagCount = 0;
volatile uint8_t SysTick_Flag = 0;

void SysTick_Handler() {
    if (SysTick_Count > 0) {
        SysTick_Count--;
    }

    if (!SysTick_Flag && SysTick_FlagLimit > 0) {
        if (SysTick_FlagCount < SysTick_FlagLimit) {
            SysTick_FlagCount++;

        } else {
            SysTick_Flag = 1;
            SysTick_FlagCount = 0;
        }
    }
}

// ms must be less than 168
void systick_enable(uint8_t ms) {
    // limit of 168
    SYSTICK_InternalInit(ms);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
}

void systick_disable() {
    SYSTICK_IntCmd(DISABLE);
    SYSTICK_Cmd(DISABLE);
}

void systick_init() {
    systick_enable(1);
}

void systick_delay_blocking(uint32_t ms) {
    SysTick_Count = ms;

    while (SysTick_Count > 0)
        ;
}

void systick_delay_flag_init(uint32_t ms) {
    systick_delay_flag_reset();
    SysTick_FlagLimit = ms;
}

void systick_delay_flag_reset() {
    SysTick_FlagCount = 0;
    SysTick_Flag = 0;
}

uint8_t systick_flag() {
    return SysTick_Flag;
}
