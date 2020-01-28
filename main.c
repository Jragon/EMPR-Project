#include <math.h>
#include <string.h>

#include "libs/i2c.h"
#include "libs/serial.h"
#include "libs/pinsel.h"
#include "libs/systick_delay.h"
#include "libs/motors.h"
#include "libs/grid.h"
#include "libs/sensor.h"
#include "libs/sensor_commands.h"

int main() {
    serial_init();
    systick_init();
    i2c_init();

    serial_printf("hello\r\n");

    Grid_t grid = {
        700, 700, 10, 260, 0, 0
    };

    
    grid_home(&grid);
    grid_move_to_point(&grid, 700, 700);

    systick_delay_blocking(100);

    uint16_t offset_x = 400;
    uint16_t offset_y = 400;
    uint16_t radius = 200;

    while(1) {
        sensor_read_all_colours(colours);
        serial_printf("C: %d, R: %d, G: %d, B: %d\r\n", colours[0], colours[1], colours[2], colours[3]);

        systick_delay_blocking(25);
    }

    return 0;
}
