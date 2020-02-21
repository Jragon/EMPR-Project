#include "leds.h"

#include "lpc17xx_gpio.h"

const int LEDS[] = {1 << 18, 1 << 20, 1 << 21, 1 << 23};

void ledNumber(int number) {
    int output = 0;

    int i;
    for (i = 0; i < 4; i++) {
        if ((1 << i) & number) {
            output = output | LEDS[i];
        }
    }

    GPIO_SetValue(LEDPORT, output);
}

void clearLeds() {
    GPIO_ClearValue(LEDPORT, LEDPINS);
}

void setLeds(uint32_t value) {
    GPIO_SetValue(LEDPORT, LEDPINS);
}

void setupLeds() {
    GPIO_SetDir(LEDPORT, LEDPINS, 1);
}