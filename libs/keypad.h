#pragma once

#include <lpc_types.h>

#define KEYPAD_ADDRESS 0x21
#define KEYPAD_INT_PIN 23
#define KEYPAD_INT_PORT 0
#define KEYPAD_INT_EDGE 1

/**
    @brief reads i2c keypad and maps it to a char

    @return char 0 if nothing is pressed
*/
char keypad_read();

/**
    @brief sends 0xFF to keypad pcf controller so that it can be used for interupts
    This must be run every time

*/
void keypad_set_as_inputs();
