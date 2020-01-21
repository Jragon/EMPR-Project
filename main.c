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

    // int xsteps = home_x();
    // int ysteps = home_y();

    // serial_printf("home steps x: %d\r\n", xsteps);
    // serial_printf("home steps y: %d\r\n", ysteps);

    movex(100);
    movex(-100);
    movey(100);
    movey(-100);

    serial_printf("moved back\r\n");

    return 0;
}

