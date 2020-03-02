#include <lpc_types.h>

#include <math.h>

#include "libs/keypad.h"
#include "libs/lcd.h"
#include "libs/scanner/grid.h"
#include "libs/scanner/motors.h"
#include "libs/scanner/sensor.h"
#include "libs/serial.h"
#include "libs/systick_delay.h"
#include "libs/timer.h"
#include "libs/util_macros.h"

#include "flags.h"

uint16_t _sum_start_end(uint16_t* lst, uint16_t start, uint16_t end) {
    uint16_t sum = 0;
    for (int i = start; i < end; i++) {
        sum += lst[i];
    }

    return sum;
}

uint16_t _sum_colours(uint16_t* colours) {
    return _sum_start_end(colours, 1, 4) * 255 / colours[0];
}

uint8_t _step_until_edge(uint16_t start_x, uint16_t start_y, uint16_t end_x,
                         uint16_t end_y, uint16_t step, uint32_t inttime, uint16_t diff) {
    grid_move_to_point(start_x, start_y);
    timer_block(inttime << 3);
    uint16_t colours[4] = {0};
    sensor_read_all_colours(colours);

    uint16_t sum, last_sum = _sum_colours(colours);

    while (grid.x != end_x || grid.y != end_y) {
        grid_step_to_point(end_x, end_y, step);
        timer_block(inttime);

        sensor_read_all_colours(colours);
        sum = _sum_colours(colours);

        // serial_printf("%d - %d = %d\r\n", last_sum, sum, ABS(last_sum - sum));

        if (ABS(last_sum - sum) > diff) {
            return 1;
            break;
        }

        last_sum = sum;
    }

    return 0;
}

void _detect_flag(uint16_t* min_x, uint16_t* min_y, uint16_t step, uint32_t inttime) {
    // detects the two edges of the flags -- assumed positioned at MAXX MAXY
    grid_home();
    serial_printf("[Flag]: Detecting Flag\r\n");
    lcd_clear_display();
    lcd_printf(0x00, "Finding edges");

    if (_step_until_edge(0, 700, grid.max_x, 700, 50, inttime, 25) == 0) {
        serial_printf("[Flag]: FAILED TO FIND X EDGE! :(\r\n");
        lcd_clear_display();
        lcd_printf(0x00, "Failed to");
        lcd_printf(0x40, "find x edge :(");
        while (1)
            ;
    }

    *min_x = grid.x;
    serial_printf("[Flag]: X edge detected at %d\r\n", *min_x);

    if (_step_until_edge(700, 50, 700, grid.max_y, 10, inttime, 25) == 0) {
        serial_printf("[Flag]: FAILED TO FIND Y EDGE! :(\r\n");
        lcd_clear_display();
        lcd_printf(0x00, "Failed to");
        lcd_printf(0x40, "find y edge :(");
        while (1)
            ;
    }

    *min_y = grid.y;
    serial_printf("[Flag]: Y edge detected at %d\r\n", *min_y);
}

#define POINTS 3
uint16_t _box_scan_sum(uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y,
                       uint32_t inttime) {
    uint16_t width = max_x - min_x;
    uint16_t height = max_y - min_y;
    uint16_t x_step = width / (POINTS + 1);
    uint16_t y_step = height / (POINTS + 1);

    uint16_t colours[4] = {0};
    sensor_read_all_colours(colours);

    uint32_t sum = 0;

    // uint16_t flag[9] = {327, 391, 275, 420, 516, 271, 342, 437, 259};
    uint16_t flagi = 0;

    for (int i = 1; i < POINTS + 1; i++) {
        for (int j = 1; j < POINTS + 1; j++) {
            if (width < height) {
                grid_move_to_point(min_x + x_step * j, min_y + y_step * i);
            } else {
                grid_move_to_point(min_x + x_step * i, min_y + y_step * j);
            }
            timer_block(inttime);
            sensor_read_all_colours(colours);

            sum += _sum_colours(colours);
            // serial_printf("%d - %d = %d \r\n", flag[flagi], _sum_colours(colours),
            //               flag[flagi] - _sum_colours(colours));
            // flagi++;
        }
    }

    return sum;
}

void flag_scan() {
    serial_printf("[Task]: Flag Scan\r\n");

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t inttime = sensor_get_int_time();

    uint16_t min_x = 0, min_y = 0;

    _detect_flag(&min_x, &min_y, 100, inttime);
    uint32_t sum = _box_scan_sum(min_x, min_y, grid.max_x, grid.max_y, inttime);
    serial_printf("[Flag]: scan hash: %d\r\n", sum);
}