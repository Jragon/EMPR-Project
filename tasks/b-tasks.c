#include <lpc_types.h>

#include "../libs/grid.h"
#include "../libs/keypad.h"
#include "../libs/lcd.h"
#include "../libs/motors.h"
#include "../libs/sensor.h"
#include "../libs/serial.h"
#include "../libs/systick_delay.h"

uint8_t _manmove() {
    char k = keypad_read();
    switch (k) {
        case '1':
            grid_x_steps(8);
            break;
        case '4':
            grid_x_steps(-8);
            break;
        case '2':
            grid_y_steps(8);
            break;
        case '5':
            grid_y_steps(-8);
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
    grid_home();

    uint16_t rgb_vals[4] = {0};
    sensor_read_all_colours(rgb_vals);

    lcd_printf(0x00, "Press 1-6");
    lcd_printf(0x40, "(%3d, %3d, %3d)", rgb_vals[1], rgb_vals[2], rgb_vals[3]);

    systick_delay_flag_reset();

    while (1) {
        if (systick_flag() == 0) {
            continue;
        }

        if (_manmove() == -1) {
            continue;
        }

        systick_delay_flag_reset();

        sensor_read_all_colours(rgb_vals);
        lcd_printf(0x40, "(%3d, %3d, %3d)", rgb_vals[1] >> 8, rgb_vals[2] >> 8,
                   rgb_vals[3] >> 8);
    }
}