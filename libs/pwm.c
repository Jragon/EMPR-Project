#include <lpc17xx_pinsel.h>
#include <lpc17xx_pwm.h>

#include "pwm.h"

void pwm_init(uint32_t match0) {
    PWM_TIMERCFG_Type cfg;
    cfg.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
    cfg.PrescaleValue = 1;
    PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &cfg);
    PWM_MatchUpdate(LPC_PWM1, 0, match0, PWM_MATCH_UPDATE_NOW);
    PWM_MATCHCFG_Type match_cfg;
    match_cfg.MatchChannel = 0;
    match_cfg.IntOnMatch = DISABLE;
    match_cfg.ResetOnMatch = ENABLE;
    match_cfg.StopOnMatch = DISABLE;
    PWM_ConfigMatch(LPC_PWM1, &match_cfg);
}

void pwm_config_pin(pwm_pin_t pin) {
    pwm_pincfg(pin);
    PWM_ChannelConfig(LPC_PWM1, (uint8_t)pin, PWM_CHANNEL_SINGLE_EDGE);
    PWM_MATCHCFG_Type match_cfg;
    match_cfg.MatchChannel = PWM_PIN_CHANNEL(pin);
    match_cfg.IntOnMatch = DISABLE;
    match_cfg.ResetOnMatch = DISABLE;
    match_cfg.StopOnMatch = DISABLE;
    PWM_ConfigMatch(LPC_PWM1, &match_cfg);
}

void pwm_config_pin_match(pwm_pin_t pin, uint32_t value) {
    PWM_MatchUpdate(LPC_PWM1, PWM_PIN_CHANNEL(pin), value, PWM_MATCH_UPDATE_NOW);
}

void pwm_enable_pin(pwm_pin_t pin) {
    PWM_ChannelCmd(LPC_PWM1, PWM_PIN_CHANNEL(pin), ENABLE);
}

void pwm_start() {
    PWM_ResetCounter(LPC_PWM1);
    PWM_CounterCmd(LPC_PWM1, ENABLE);
    PWM_Cmd(LPC_PWM1, ENABLE);
}

void pwm_stop() {
    PWM_CounterCmd(LPC_PWM1, DISABLE);
    PWM_Cmd(LPC_PWM1, DISABLE);
}

void pwm_pincfg(pwm_pin_t pin) {
    PINSEL_CFG_Type PinCfg = {
        .OpenDrain = PINSEL_PINMODE_NORMAL,
        .Pinmode = PINSEL_PINMODE_NORMAL,
        .Funcnum = PWM_FUNC,
        .Portnum = PWM_PORT,
        .Pinnum = (uint8_t)pin,
    };
    PINSEL_ConfigPin(&PinCfg);
}
