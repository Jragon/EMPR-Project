#pragma once

#include <lpc_types.h>

#define PWM_PORT 2
#define PWM_FUNC 1

#define PWM_PIN_CHANNEL(pin) ((uint8_t)pin + 1)

/**
    @brief IO board pin to lpc pin map. Channel = pin + 1

*/
typedef enum pwm_pin {
    PWM_PIN_21 = 5,
    PWM_PIN_22 = 4,
    PWM_PIN_23 = 3,
    PWM_PIN_24 = 2,
    PWM_PIN_25 = 1,
    PWM_PIN_26 = 0
} pwm_pin_t;

/**
    @brief inits the pwm in single edge with match0 value
    (no prescaling)

    @param match0 channel 0 match value
*/
void pwm_init(uint32_t match0);

/**
    @brief configs the PWM output pin

    @param pin
*/
void pwm_config_pin(pwm_pin_t pin);

/**
    @brief configs the match value for pin channel
    Mark-space is value / match0

    @param pin
    @param value
*/
void pwm_config_pin_match(pwm_pin_t pin, uint32_t value);

/**
    @brief enables the pwm output on a pin

    @param pin
*/
void pwm_enable_pin(pwm_pin_t pin);

/**
    @brief starts pwm counter

*/
void pwm_start();

/**
    @brief stops the pwm counter

*/
void pwm_stop();

/**
    @brief pincfg for pwm

    @param pin
*/
void pwm_pincfg(pwm_pin_t pin);
