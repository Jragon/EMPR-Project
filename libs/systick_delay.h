#pragma once

#include <lpc_types.h>

/**
    @brief SysTick IRQ handler
*/
void SysTick_Handler();

/**
    @brief blocking delay using the systick handler

    @param ms delay length in milliseconds
*/
void systick_delay_blocking(uint32_t ms);

/**
    @brief initialises the flag delay for non-blocking delays.
    systick_flag goes true once the systick counter reaches ms.
    @param ms delay before setting systick_flag true in milliseconds.
*/
void systick_delay_flag_init(uint32_t ms);

/**
    @brief resets the systick_flag and sets the interal counter to zero.
*/
void systick_delay_flag_reset();

/**
    @brief initilises the systick counter to interrupt every millisecond

*/
void systick_init();

/**
    @brief gets the systick_flag

    @return uint8_t current value of the systick_flag
*/
uint8_t systick_flag();

/**
    @brief enables systick interrupts to go at ms milliseconds

    @param ms how often to fire systick int. Must be less than 168
*/
void systick_enable(uint8_t ms);
