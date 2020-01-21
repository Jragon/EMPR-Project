#include <string.h>

#include "libs/i2c.h"
#include "libs/serial.h"
#include "libs/pinsel.h"
#include "libs/systick_delay.h"
#include "libs/motors.h"


int main() {
    serial_init();
    systick_init();
    i2c_init();
    serial_printf("hello\r\n");
    setup_switches();

    home_x();
    home_y();

    return 0;
}

