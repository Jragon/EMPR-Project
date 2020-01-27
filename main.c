#include <math.h>
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

    systick_delay_blocking(100);

    uint16_t offset_x = 400;
    uint16_t offset_y = 400;
    uint16_t radius = 200;

    while(1) {
        for (int theta = 0; theta < 360; theta++) {
            double rad = theta * 3.14/180;
            int x = offset_x + (radius * sin(rad));
            int y = offset_y + (radius * cos(rad));
            grid_move_to_point(&grid, x, y);
            // serial_printf("x: %3d, y: %3d\r\n", x, y);
        }
    }

    // movex(900);

    // raster
    // while (grid.y < grid.max_y) {
    //     grid_move_to_point(&grid, grid.max_x, grid.y);
    //     systick_delay_blocking(100);
    //     grid_move_to_point(&grid, 0, grid.y + 5);
    // }



    return 0;
}
