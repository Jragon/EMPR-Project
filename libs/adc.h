#pragma once

#include <lpc17xx_adc.h>
#include <lpc_types.h>

#define ADC_0_PIN 23
#define ADC_0_PORT 0
#define ADC_0_FUNC 1

#define ADC_1_PIN 24
#define ADC_1_PORT 0
#define ADC_1_FUNC 1

#define ADC_INT_ENABLE NVIC_EnableIRQ(ADC_IRQn)
#define ADC_INT_DISABLE NVIC_DisableIRQ(ADC_IRQn)

/**
    @brief initialises ADC 1 for adc channel 1. (Pin 16 on ioboard)

*/
void adc_init();

/**
    @brief waits for adc to convert and the returns the value
    >> 12 instead of div by 4096 when ranging

    @return uint32_t adc value (12 bit range from 0-3.3v)
*/
uint32_t adc_read_blocking();

/**
    @brief configures the adc int to fire after each conversion

    @param adc_int_handler adc int callback
*/
void adc_int_config(void (*adc_int_handler)(uint32_t value));

/**
    @brief starts a free running conversion (for int mode)

*/
void adc_start();

/**
    @brief stops the free running conversion

*/
void adc_stop();

void ADC_IRQHandler();
