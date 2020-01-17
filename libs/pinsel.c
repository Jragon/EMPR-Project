#include <lpc17xx_pinsel.h>

void pinsel_enable(int port, int pin, int func) {
    PINSEL_CFG_Type pinCfg;
    pinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pinCfg.Funcnum = func;
    pinCfg.Portnum = port;
    pinCfg.Pinnum = pin;
    PINSEL_ConfigPin(&pinCfg);
}
