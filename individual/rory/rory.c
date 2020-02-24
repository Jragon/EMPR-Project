#include <lpc_types.h>

#include <math.h>

#include "libs/lcd.h"
#include "libs/scanner/grid.h"
#include "libs/scanner/motors.h"
#include "libs/scanner/sensor.h"
#include "libs/serial.h"
#include "libs/systick_delay.h"
#include "libs/timer.h"
#include "libs/util_macros.h"

void _step_to_thresh(uint16_t x, uint16_t y, uint16_t* colours, uint16_t* last_colours,
                     uint16_t int_time, uint16_t thresh, uint16_t step) {
    timer_block(int_time);
    sensor_read_all_colours(last_colours);
    // move diagonally until there is a change
    uint32_t last_time = timer_get();

    uint8_t found = 0;
    while (found == 0 && (x != grid.x || y != grid.y)) {
        if (timer_get() - last_time < int_time + 1) {
            continue;
        }

        sensor_read_all_colours(colours);
        grid_step_to_point(x, y, step);
        last_time = timer_get();

        for (int i = 0; i < 4; i++) {
            // any change greater than thresh on any channel
            uint16_t diff = ABS((int)(last_colours[i] - colours[i]));
            if (diff > thresh) {
                found = 1;
                lcd_printf(0x00, "C %5d, R %5d", colours[0], colours[1]);
                lcd_printf(0x40, "G %5d, B %5d", colours[2], colours[3]);
                break;
            }
            last_colours[i] = colours[i];
        }
    }
}

// change thresh
#define STEP 20
#define THRESH 150
void detect_edges(uint16_t* min_x, uint16_t* max_x, uint16_t* min_y, uint16_t* max_y) {
    serial_printf("[Rory]: Detect edges\r\n");
    grid_home();

    uint16_t colours[4] = {0};
    uint16_t last_colours[4] = {0};

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    grid_move_to_point(GRID_HALF_X, 0);
    _step_to_thresh(GRID_HALF_X, grid.max_y, colours, last_colours, int_time, THRESH,
                    STEP);
    *min_y = grid.y;

    grid_move_to_point(GRID_HALF_X, grid.max_y);
    _step_to_thresh(GRID_HALF_X, 0, colours, last_colours, int_time, THRESH, STEP);
    *max_y = grid.y;

    grid_move_to_point(0, GRID_HALF_Y);
    _step_to_thresh(grid.max_x, GRID_HALF_Y, colours, last_colours, int_time, THRESH,
                    STEP);
    *min_x = grid.x;

    grid_move_to_point(grid.max_x, GRID_HALF_Y);
    _step_to_thresh(0, GRID_HALF_Y, colours, last_colours, int_time, THRESH, STEP);
    *max_x = grid.x;
}

#define POINTS 4
void detect_scan() {
    uint16_t min_x, max_x, min_y, max_y;
    detect_edges(&min_x, &max_x, &min_y, &max_y);

    serial_printf("[Rory]: Scan Image\r\n");

    serial_printf("\tMin: (%d, %d); Max: (%d, %d)\r\n", min_x, min_y, max_x, max_y);

    // scan POINTS
    uint16_t width = max_x - min_x;
    uint16_t height = max_y - min_y;
    uint16_t widthOffset = width / 20;
    uint16_t heightOffset = height / 20;
    uint16_t widthStep = (width - 2 * widthOffset) / (POINTS + 1);
    uint16_t heightStep = (height - 2 * heightOffset) / (POINTS + 1);
    uint16_t start_x = min_x + widthOffset;
    uint16_t start_y = min_y + heightOffset;
    serial_printf("\tW: %d, H: %d, wStep: %d, hStep: %d\r\n", width, height, widthStep,
                  heightStep);
    serial_printf("\twOffset: %d, hOffset: %d, startX: %d, startY: %d\r\n", widthOffset,
                  heightOffset, start_x, start_y);

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    uint16_t red, green, blue;
    serial_printf("[Data]: .error = 0, .width = %d, .height = %d, .vals = {", width,
                  height);
    for (int i = 1; i <= POINTS; i++) {
        for (int j = 1; j <= POINTS; j++) {
            grid_move_to_point(start_x + widthStep * j, start_y + heightStep * i);
            timer_block(int_time);
            sensor_read_rgb(&red, &green, &blue);
            serial_printf("{%d, %d, %d}%c ", red, green, blue,
                          (i == POINTS && j == POINTS) ? '}' : ',');
        }
    }
    serial_printf("\r\n");
}