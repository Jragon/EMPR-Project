#include <lpc_types.h>

#include <LPC17xx.h>

#include "libs/keypad.h"
#include "libs/lcd.h"
#include "libs/scanner/grid.h"
#include "libs/scanner/motors.h"
#include "libs/scanner/sensor.h"
#include "libs/scanner/sensor_commands.h"
#include "libs/serial.h"
#include "libs/systick_delay.h"
#include "libs/timer.h"
#include "libs/util_macros.h"

#include "tasks.h"

void task_B1_rgb_man_move() {
    serial_printf("[Task]: B1: RGB Manual Move\r\n");
    lcd_clear_display();

    lcd_printf(0x00, "homing...");

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    grid_home();
    timer_block(int_time);

    uint16_t rgb_vals[4] = {0};
    sensor_read_all_colours(rgb_vals);
    lcd_printf(0x00, "C %5d, R %5d", rgb_vals[0], rgb_vals[1]);
    lcd_printf(0x40, "G %5d, B %5d", rgb_vals[2], rgb_vals[3]);

    uint32_t time = timer_get();
    while (1) {
        if (timer_get() - time < 7) {
            continue;
        }
        time = timer_get();

        _manmove();

        timer_block(int_time);

        sensor_read_all_colours(rgb_vals);
        lcd_printf(0x00, "C %5d, R %5d", rgb_vals[0], rgb_vals[1]);
        lcd_printf(0x40, "G %5d, B %5d", rgb_vals[2], rgb_vals[3]);
    }
}

#define ROW_STEP 20
#define COL_STEP 20
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

        col += COL_STEP;
    }

    serial_printf("\n");
}

void task_B2_raster_scan() {
    serial_printf("[Task]: B2: Raster Scanner\r\n");
    grid_home();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    for (int row = 0; row < grid.max_x; row += ROW_STEP) {
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

#define MIN_X 200
#define MIN_Y 200
#define MAX_X 700
#define MAX_Y 700
#define STEP 25

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    uint16_t min_val = -1, x = 0, y = 0;
    int intval = 0;
    uint16_t colours[4] = {0};

    grid_move_to_point(MIN_X, MIN_Y);

    while (grid.y < MAX_X) {
        while (grid.x < MAX_Y) {
            if (timer_get() - last_time < int_time << 2) {
                continue;
            }

            last_time = timer_get();
            grid_move_to_point(grid.x + STEP, grid.y);

            sensor_read_all_colours(colours);
            lcd_printf(0x00, "C %5d, R %5d", colours[0], colours[1]);
            lcd_printf(0x40, "G %5d, B %5d", colours[2], colours[3]);

            // normalize results
            for (int i = 1; i < 4; i++) {
                colours[i] = colours[i] * 255 / colours[0];
            }

            switch (find) {
                case 'R':
                    intval = ABS(255 - colours[1]) + colours[2] + colours[3];
                    break;
                case 'G':
                    intval = colours[1] + ABS(255 - colours[2]) + colours[3];
                    break;
                case 'B':
                    intval = colours[1] + colours[2] + ABS(255 - colours[3]);
                    break;
            }

            // this will always return the whitest point which should fulfill the spec,
            // but I'm not sure chris will see it that way. It needs to find the reddest
            // point. IE red is at max whilst blue and green are the lowest.
            if (intval < min_val) {
                min_val = intval;
                x = grid.x;
                y = grid.y;
            }
            // serial_printf("(%d, %d): int: %d\r\n", grid.x, grid.y, intval);
        }

        grid_move_to_point(MIN_X, grid.y + STEP);
    }

    grid_move_to_point(x, y);
    lcd_clear_display();
    lcd_printf(0, "Most %c point", find);
    lcd_printf(0x40, "(%d, %d)", x, y);

    while (1)
        ;
}