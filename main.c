#include <string.h>

#include "libs/i2c.h"
#include "libs/serial.h"
#include "libs/pinsel.h"
#include "libs/systick_delay.h"
#include "libs/motors.h"
#include "libs/grid.h"

int main() {
    serial_init();
    systick_init();
    i2c_init();
    setup_switches();

    serial_printf("hello\r\n");

    Grid_t grid = {
        950, 950, 10, 260, 0, 0
    };

    grid_home(&grid);
    grid_move_to_point(&grid, 950, 950);
    while(1) {
        grid_move_to_point(&grid, 100, 100);
        systick_delay_blocking(100);
        grid_move_to_point(&grid, 300, 0);
        systick_delay_blocking(100);
        grid_move_to_point(&grid, 0, 300);
        systick_delay_blocking(100);
    }

    return 0;
}

