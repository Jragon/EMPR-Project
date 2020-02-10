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

void _scan_row(uint16_t row, uint16_t int_time, uint8_t lcd, uint8_t serial) {
    uint16_t col = 0;
    uint16_t red, green, blue;

    grid_move_to_point(row, col);

    uint32_t last_time = timer_get();
    while (col < grid.max_y) {
        if (timer_get() - last_time < int_time) {
            continue;
        }

        sensor_read_rgb(&red, &green, &blue);
        last_time = timer_get();
        grid_move_to_point(row, col);

        if (serial == 1) {
            serial_printf("%d %d %d;", red, green, blue);
        }
        if (lcd == 1) {
            lcd_printf(0x00, "R %5d       ", red);
            lcd_printf(0x40, "G %5d B %5d", green, blue);
        }

        col += 10;
    }

    serial_printf("\n");
}

void task_B2_raster_scan() {
    serial_printf("[Task]: B2: Raster Scanner\r\n");
    grid_home();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    for (int row = 0; row < grid.max_x; row += 25) {
        _scan_row(row, int_time, 1, 1);
    }

    serial_printf("end scan\r\n");
}

void task_B3_color_search() {
    grid_home();
    lcd_clear_display();
    lcd_printf(0x00, "Colour Search");

    char find = 'R';
    lcd_printf(0x40, "Find Biggest: %c", find);
    uint32_t last_time = timer_get();
    uint8_t go = 0;
    while (go == 0) {
        if (timer_get() - last_time < 3) {
            continue;
        }

        char k = keypad_read();
        switch (k) {
            case 'A':
                find = 'R';
                break;
            case 'B':
                find = 'G';
                break;
            case 'C':
                find = 'B';
                break;
            case '#':
                go = 1;
                break;
        }

        lcd_printf(0x40, "Find Biggest: %c", find);
        last_time = timer_get();
    }

#define MIN_X 400
#define MIN_Y 400
#define MAX_X 800
#define MAX_Y 800
#define STEP 10

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    uint16_t max_val = 0, x = 0, y = 0, intval;
    uint16_t red, green, blue;

    grid_move_to_point(MIN_X, MIN_Y);

    while (grid.x < MAX_X) {
        while (grid.y < MAX_Y) {
            if (timer_get() - last_time < int_time << 2) {
                continue;
            }

            sensor_read_rgb(&red, &green, &blue);
            last_time = timer_get();
            grid_move_to_point(grid.x, grid.y + STEP);

            lcd_printf(0x00, "R %5d       ", red);
            lcd_printf(0x40, "G %5d B %5d", green, blue);

            switch (find) {
                case 'R':
                    intval = red;
                    break;
                case 'B':
                    intval = blue;
                    break;
                case 'G':
                    intval = green;
                    break;
            }

            // this will always return the whitest point which should fulfill the spec,
            // but I'm not sure chris will see it that way. It needs to find the reddest
            // point. IE red is at max whilst blue and green are the lowest.
            if (intval > max_val) {
                max_val = intval;
                x = grid.x;
                y = grid.y;
            }
        }

        grid_move_to_point(grid.x + STEP, MIN_Y);
    }

    grid_move_to_point(x, y);
}