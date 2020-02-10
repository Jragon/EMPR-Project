#include <lpc_types.h>

#include <LPC17xx.h>

#include "../libs/keypad.h"
#include "../libs/lcd.h"
#include "../libs/scanner/grid.h"
#include "../libs/scanner/motors.h"
#include "../libs/scanner/sensor.h"
#include "../libs/scanner/sensor_commands.h"
#include "../libs/serial.h"
#include "../libs/systick_delay.h"
#include "../libs/timer.h"

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
            grid_z_steps(8);
            break;
        case '6':
            grid_z_steps(-8);
            break;
        default:
            return -1;
            break;
    }

    return 0;
}

void task_B1_rgb_man_move() {
    serial_printf("[Task]: B1: RGB Manual Move\r\n");
    lcd_clear_display();

    lcd_printf(0x00, "homing...");

    grid_home();

    uint16_t rgb_vals[4] = {0};
    sensor_read_all_colours(rgb_vals);

    lcd_printf(0x00, "Press 1-6(R,G,B)");
    lcd_printf(0x40, "(%3d, %3d, %3d)   ", rgb_vals[1], rgb_vals[2], rgb_vals[3]);
    uint32_t time = timer_get();

    while (1) {
        if (timer_get() - time < 5) {
            continue;
        }
        time = timer_get();

        if (_manmove() == -1) {
            continue;
        }

        systick_delay_flag_reset();

        sensor_read_all_colours(rgb_vals);
        lcd_printf(0x40, "(%3d, %3d, %3d)  ", rgb_vals[1] >> 8, rgb_vals[2] >> 8,
                   rgb_vals[3] >> 8);
    }
}

void _scan_row(uint16_t row, uint16_t int_time) {
    uint16_t col = 0;
    uint16_t red, green, blue;

    grid_move_to_point(row, col);

    uint32_t last_time = timer_get();
    while (col < grid.max_y) {
        if (timer_get() - last_time < int_time << 2) {
            continue;
        }

        sensor_read_rgb(&red, &green, &blue);
        last_time = timer_get();
        grid_move_to_point(row, col);
        serial_printf("%d %d %d;", red, green, blue);

        col += 10;
    }

    serial_printf("\n");
    grid.y = grid.max_y - motors.y_steps;
}

void task_B2_raster_scan() {
    serial_printf("[Task]: B2: Raster Scanner\r\n");
    grid_home();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    for (int row = 0; row < grid.max_x; row += 25) {
        _scan_row(row, int_time);
    }

    serial_printf("end scan\r\n");
}