#pragma once

#include <lpc_types.h>

#define DAC_PORT 0
#define DAC_FUNC 2
#define DAC_PIN 26

#define MAX_VOLTAGE 3.3

/**
    @brief initialises the DAC, on pin 18 of the ioboard

*/
void dac_init();

/**
    @brief updates the DACR output (10 bit range)

    @param val 10 bit value, no need to bitshift
*/
void dac_update_data(uint16_t val);

/**
    @brief takes a voltage and updates the dac with it

    @param val voltage between 0-3.3v
*/
void dac_update_voltage(float val);

/**
    @brief configures the dac out pin

*/
void dac_pincfg();

/**
    @brief disables the counter and dma

*/
void dac_default_cfg();
