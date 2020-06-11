#include <lpc17xx_adc.h>
#include <lpc17xx_pinsel.h>

#include "adc.h"

void (*adc_callback)(uint32_t value);

void adc_init() {
    PINSEL_CFG_Type PinCfg = {
        .OpenDrain = PINSEL_PINMODE_NORMAL,
        .Pinmode = PINSEL_PINMODE_TRISTATE,
        .Funcnum = ADC_1_FUNC,
        .Portnum = ADC_1_PORT,
        .Pinnum = ADC_1_PIN,
    };
    PINSEL_ConfigPin(&PinCfg);
    ADC_Init(LPC_ADC, 200000);
}

uint32_t adc_read_blocking() {
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);

    while (!ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_1, ADC_DATA_DONE))
        ;

    return ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_1);
}

void adc_int_config(void (*adc_int_handler)(uint32_t value)) {
    adc_callback = adc_int_handler;
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);
    ADC_ChannelCmd(LPC_ADC, 1, ENABLE);
}

void adc_start() {
    NVIC_EnableIRQ(ADC_IRQn);
    ADC_BurstCmd(LPC_ADC, ENABLE);
    ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);
}

void adc_stop() {
    NVIC_DisableIRQ(ADC_IRQn);
    ADC_PowerdownCmd(LPC_ADC, DISABLE);
    ADC_DeInit(LPC_ADC);
}

void ADC_IRQHandler() {
    if (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_1, ADC_DATA_DONE)) {
        adc_callback(ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_1));
    }
}
