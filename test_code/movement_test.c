#include <string.h>

#include "libs/i2c.h"
#include "libs/serial.h"
#include "libs/pinsel.h"
#include "libs/systick_delay.h"
#include "libs/motors.h"

Motor_t motor_z = {
    MOTOR_ZPEN_LATCH_ADDRESS,
    MOTOR_STEPX,
    SWITCH_Z_MASK,
    0
};

int main() {
    serial_init();
    systick_init();
    i2c_init();
    serial_printf("hello\r\n");
    setup_switches();

    int xsteps = 1000 - movex(-1000);
    int ysteps = 1000 - movey(-1000);
    int zsteps = 1000 - move(&motor_z, -1000);

    serial_printf("home steps x: %d\r\n", xsteps);
    serial_printf("home steps y: %d\r\n", ysteps);

    movex(xsteps);
    movey(ysteps);
    move(&motor_z, zsteps);

    return 0;
}

