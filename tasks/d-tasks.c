#include <lpc_types.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "libs/keypad.h"
#include "libs/lcd.h"
#include "libs/scanner/grid.h"
#include "libs/scanner/motors.h"
#include "libs/scanner/sensor.h"
#include "libs/serial.h"
#include "libs/systick_delay.h"
#include "libs/timer.h"

void _move_print_rgb(uint16_t x, uint16_t y, uint16_t step, uint16_t int_time) {
    uint16_t red, green, blue;
    uint32_t last_time = timer_get();
    uint16_t movex, movey;
    uint8_t tw = 0;
    while (x != grid.x || y != grid.y) {
        if (timer_get() - last_time < int_time << 2) {
            continue;
        }

        sensor_read_rgb(&red, &green, &blue);

        // if (tw == 0 && red != 2048 && green != 2048 && blue != 2048) {
        //     systick_delay_blocking(500);
        //     tw = 1;
        // }

        // if (tw == 1 && red == 2048 && green == 2048 && blue == 2048) {
        //     systick_delay_blocking(500);
        //     tw = 0;
        // }

        grid_step_to_point(x, y, step);

        last_time = timer_get();

        lcd_printf(0x00, "R %5d       ", red);
        lcd_printf(0x40, "G %5d B %5d", green, blue);
    }
}

void flag_edge_detect() {
    serial_printf("[Task]: Flag Edge Detect\r\n");
    grid_home();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    // do three horizontal scans and three vertical scans
    // then take max value

    // x axis scan
    // uint16_t x_scans[3] = {100, 450, 700};
    // for (int i = 0; i < 3; i++) {
    //     grid_move_to_point(x_scans[i], 0);
    //     systick_delay_blocking(100);
    //     _move_print_rgb(x_scans[i], grid.max_y, 20, int_time);
    //     systick_delay_blocking(100);
    // }

    // uint16_t y_scans[3] = {100, 450, 700};
    // for (int i = 0; i < 3; i++) {
    //     grid_move_to_point(0, y_scans[i]);
    //     // systick_delay_blocking(100);
    //     _move_print_rgb(grid.max_x, y_scans[i], 5, int_time);
    //     // systick_delay_blocking(100);
    // }

    // diagonal scan
    while (1) {
        grid_move_to_point(0, 0);
        _move_print_rgb(grid.max_x, grid.max_y, 5, int_time);

        grid_move_to_point(0, grid.max_y);
        _move_print_rgb(grid.max_x, 0, 5, int_time);
    }
}

void task_D2_pc_coodrinate_colour() {
    serial_printf("[Task]: D2 Move and Measure\r\n");
    grid_home();

    lcd_clear_display();
    lcd_printf(0x00, "Waiting for");
    lcd_printf(0x40, "input");

    char bufs[8][8];
    memset(bufs, 0, sizeof(bufs));

    serial_read_blocking(bufs[0], 4);
    serial_read_blocking(bufs[1], 4);

    int coords[2];
    coords[0] = atoi(bufs[0]);
    coords[1] = atoi(bufs[1]);

    lcd_clear_display();
    lcd_printf(0x0, "POS: (%3d, %3d)", coords[0], coords[1]);
    grid_move_to_point(coords[0], coords[1]);

    uint16_t colours[4];
    sensor_read_all_colours(colours);
    sensor_normalize_colours(colours);

    serial_printf("[Task D2]: Sending rgb values\r\n");
    serial_printf("%i|%i|%i\r\n", colours[1], colours[2], colours[3]);
    lcd_printf(0x40, "(%3d, %3d, %3d)", colours[1], colours[2], colours[3]);

    keypad_wait_key('#', 10);
}
