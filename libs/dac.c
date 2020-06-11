#include <lpc17xx_dac.h>
#include <lpc17xx_pinsel.h>

#include "dac.h"

void dac_init() {
    dac_pincfg();
    DAC_Init(LPC_DAC);
}

void dac_default_cfg() {
    DAC_CONVERTER_CFG_Type dac_cfg;
    dac_cfg.CNT_ENA = RESET;
    dac_cfg.DMA_ENA = RESET;
    DAC_ConfigDAConverterControl(LPC_DAC, &dac_cfg);
}

void dac_pincfg() {
    PINSEL_CFG_Type PinCfg = {
        .OpenDrain = PINSEL_PINMODE_NORMAL,
        .Pinmode = PINSEL_PINMODE_NORMAL,
        .Funcnum = DAC_FUNC,
        .Portnum = DAC_PORT,
        .Pinnum = DAC_PIN,
    };
    PINSEL_ConfigPin(&PinCfg);
}

void dac_update_data(uint16_t val) {
    DAC_UpdateValue(LPC_DAC, val);
}

void dac_update_voltage(float voltage) {
    DAC_UpdateValue(LPC_DAC, (1023 / MAX_VOLTAGE) * voltage);
}
