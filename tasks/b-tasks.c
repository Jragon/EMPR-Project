#include <lpc_types.h>

#include "../libs/keypad.h"
#include "../libs/serial.h"
#include "../libs/grid.h"
#include "../libs/lcd.h"
#include "../libs/motors.h"
#include "../libs/systick_delay.h"
#include "../libs/sensor.h"

uint8_t _manmove(Grid_t *grid) {
    char k = keypad_read();
    switch (k)
    {
    case '1':
        grid_x_steps(grid, 8);
        break;
    case '4':
        grid_x_steps(grid, -8);
        break;
    case '2':
        grid_y_steps(grid, 8);
        break;
    case '5':
        grid_y_steps(grid, -8);
        break;
    case '3':
        movez(5);
        break;
    case '6':
        movez(-5);
        break;
    default:
        return -1;
        break;
    }

    return 0;
}

void b1_xyz_move_rgb() {
    serial_printf("b1_xyz_move_rgb\r\n");
    lcd_clear_display();

    lcd_printf(0x00, "homing");

    // perhaps make grid a singleton
    Grid_t grid = {
        700, 700, 10, 260, 0, 0
    };
    grid_home(&grid);

    uint16_t rgb_vals[4] = {0};
    sensor_read_all_colours(rgb_vals);

    lcd_printf(0x00, "Press 1-6");
    lcd_printf(0x40, "(%3d, %3d, %3d)", rgb_vals[1], rgb_vals[2], rgb_vals[3]);

    systick_delay_flag_reset();

    while (1) {
        if (systick_flag() == 0) {
            continue;
        }

        if (_manmove(&grid) == -1) {
            continue;
        }

        systick_delay_flag_reset();

        sensor_read_all_colours(rgb_vals);
        lcd_printf(0x40, "(%3d, %3d, %3d)", rgb_vals[1] >> 8, rgb_vals[2] >> 8, rgb_vals[3] >> 8);
    }
}