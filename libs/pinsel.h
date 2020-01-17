#pragma once

#include <lpc17xx_pinsel.h>

/**
    Pinsel a pin
*/
void pinsel_enable(int port, int pin, int func);
