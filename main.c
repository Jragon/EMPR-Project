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

    int xsteps = 1000 - movex(-1000);
    int ysteps = 1000 - movey(-1000);

    serial_printf("home steps x: %d\r\n", xsteps);
    serial_printf("home steps y: %d\r\n", ysteps);

    movex(xsteps);
    movey(ysteps);

    return 0;
}

