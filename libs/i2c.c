#include <lpc17xx_i2c.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include "i2c.h"

void i2c1_init() {
    // only initilize once
    static Bool initilized = FALSE;

    if (initilized == TRUE) {
        return;
    }

    PINSEL_CFG_Type PinCfg;
    // initial i2c pins, on P0.0/P0.1
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Funcnum = 3;
    PinCfg.Portnum = 0;
    PinCfg.Pinnum = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 1;
    PINSEL_ConfigPin(&PinCfg);
    I2C_Init(I2C1DEV, 100000);
    I2C_Cmd(I2C1DEV, ENABLE);
    initilized = TRUE;
}
