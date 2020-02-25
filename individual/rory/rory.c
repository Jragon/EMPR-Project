#include "rory.h"

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

#include "data.h"

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

        // serial_printf("\tC %5d, R %5d, G: %5d, B: %5d\r\n", colours[0], colours[1],
        //               colours[2], colours[3]);

        for (int i = 0; i < 4; i++) {
            // any change greater than thresh on any channel
            uint16_t diff = ABS((int)(last_colours[i] - colours[i]));
            if (diff > thresh) {
                found = 1;
                // lcd_printf(0x00, "C %5d, R %5d", colours[0], colours[1]);
                // lcd_printf(0x40, "G %5d, B %5d", colours[2], colours[3]);
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

    grid_move_to_point(GRID_HALF_X, grid.max_y - 50);

    _step_to_thresh(GRID_HALF_X, 0, colours, last_colours, int_time, THRESH, STEP);
    *max_y = grid.y;

    grid_move_to_point(0, GRID_HALF_Y);
    _step_to_thresh(grid.max_x, GRID_HALF_Y, colours, last_colours, int_time, THRESH,
                    STEP);
    *min_x = grid.x;

    grid_move_to_point(grid.max_x - 25, GRID_HALF_Y);
    _step_to_thresh(0, GRID_HALF_Y, colours, last_colours, int_time, THRESH, STEP);
    *max_x = grid.x;
}

#define POINTS 4
void _get_edge_data(scan_t* scan) {
    detect_edges(&(scan->min_x), &(scan->max_x), &(scan->min_y), &(scan->max_y));
    serial_printf("\tMin: (%d, %d); Max: (%d, %d)\r\n", scan->min_x, scan->min_y,
                  scan->max_x, scan->max_y);

    scan->width = scan->max_x - scan->min_x;
    scan->height = scan->max_y - scan->min_y;
    scan->widthOffset = scan->width / 20;
    scan->heightOffset = scan->height / 20;
    scan->widthStep = (scan->width - 2 * scan->widthOffset) / (POINTS + 1);
    scan->heightStep = (scan->height - 2 * scan->heightOffset) / (POINTS + 1);
    scan->startX = scan->min_x + scan->widthOffset;
    scan->startY = scan->min_y + scan->heightOffset;

    serial_printf("\tW: %d, H: %d, wStep: %d, hStep: %d\r\n", scan->width, scan->height,
                  scan->widthStep, scan->heightStep);
    serial_printf("\twOffset: %d, hOffset: %d, startX: %d, startY: %d\r\n",
                  scan->widthOffset, scan->heightOffset, scan->startX, scan->startY);
}

void detect_scan() {
    serial_printf("[Rory]: Scan Image\r\n");

    scan_t scan;
    _get_edge_data(&scan);

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    serial_printf("[Data]: .width = %d, .height = %d, .vals = {", scan.width,
                  scan.height);

    uint16_t colours[4] = {0};
    sensor_read_all_colours(colours);
    for (int i = 1; i <= POINTS; i++) {
        for (int j = 1; j <= POINTS; j++) {
            grid_move_to_point(scan.startX + scan.widthStep * j,
                               scan.startY + scan.heightStep * i);
            timer_block(int_time);
            while (sensor_ready() == 0) {
                serial_printf("Sensor not ready\r\n");
                timer_block(int_time);
            }
            sensor_read_all_colours(colours);
            serial_printf("{%d, %d, %d}%c ", colours[1], colours[2], colours[3],
                          (i == POINTS && j == POINTS) ? '}' : ',');
        }
    }
    serial_printf("\r\n");
    grid_move_to_point(grid.max_y, grid.max_y);
}

int _get_min_error() {
    int min_index = 0;
    int min = data[min_index].errors[0];
    serial_printf("Errors: \r\n");
    for (uint8_t scan_index = 0; scan_index < DATA_LEN; scan_index++) {
        serial_printf("\t%-12s ", data[scan_index].name);

        for (int i = 0; i < 4; i++) {
            if (data[scan_index].errors[i] > 0) {
                if (data[scan_index].errors[i] < min) {
                    min_index = scan_index;
                    min = data[scan_index].errors[i];
                }

                serial_printf(" %c %-5d", i == 0 ? ' ' : '|', data[scan_index].errors[i]);
            }
        }
        serial_printf("\r\n");
    }

    return min_index;
}

void _reset_errors() {
    for (uint8_t scan_index = 0; scan_index < DATA_LEN; scan_index++) {
        for (int i = 0; i < 3; i++) {
            data[scan_index].errors[i] = 0;
        }
    }
}

#define SQUARE_TOLERANCE 50
void recognise() {
    serial_printf("[Rory]: Recognise Image\r\n");
    lcd_clear_display();
    lcd_printf(0x00, "Detecting image");

    uint32_t scan_timer = timer_get();
    scan_t scan;
    _get_edge_data(&scan);
    uint32_t detect_time = timer_get() - scan_timer;

    lcd_clear_display();
    lcd_printf(0x00, "Scanning image");
    lcd_printf(0x40, "%d x %d", scan.width, scan.height);

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t int_time = sensor_get_int_time();

    serial_printf("[Rory]: Starting Scan\r\n");
    _reset_errors();

    uint16_t colours[4] = {0};
    uint8_t point_index = 0;
    uint8_t flipped = scan.width < scan.height ? 0 : 1;
    uint8_t square =
      ((ABS((int)(scan.width - scan.height))) < (SQUARE_TOLERANCE)) ? 1 : 0;
    serial_printf("[Data]: .width = %d, .height = %d, .square = %d, .vals = {",
                  scan.width, scan.height, square);
    sensor_read_all_colours(colours);
    for (int col = 1; col <= COLS; col++) {
        serial_printf("{");
        for (int row = 1; row <= ROWS; row++) {
            if (flipped == 0) {
                grid_move_to_point(scan.startX + scan.widthStep * row,
                                   scan.startY + scan.heightStep * col);
            } else {
                grid_move_to_point(scan.startX + scan.widthStep * col,
                                   scan.startY + scan.heightStep * row);
            }

            timer_block(int_time);
            while (sensor_ready() == 0) {
                serial_printf("Sensor not ready\r\n");
                timer_block(int_time);
            }

            sensor_read_all_colours(colours);

            serial_printf("{%d, %d, %d}%s", colours[1], colours[2], colours[3],
                          (row == ROWS) ? "}" : ", ");

            // compare current results an store error
            for (uint8_t scan_index = 0; scan_index < DATA_LEN; scan_index++) {
                for (uint8_t k = 0; k < 3; k++) {
                    data[scan_index].errors[0] += ABS(
                      (int)(data[scan_index].vals[col - 1][row - 1][k] - colours[k + 1]));
                    data[scan_index].errors[1] +=
                      ABS((int)(data[scan_index].vals[COLS - col][ROWS - row][k] -
                                colours[k + 1]));
                    if (square == 1 || data[scan_index].square == 1) {
                        // calculate transpose errors
                        data[scan_index].errors[2] +=
                          ABS((int)(data[scan_index].vals[row - 1][col - 1][k] -
                                    colours[k + 1]));
                        data[scan_index].errors[3] +=
                          ABS((int)(data[scan_index].vals[ROWS - row][COLS - col][k] -
                                    colours[k + 1]));
                    }
                }
            }

            point_index++;
        }

        serial_printf(col == COLS ? "}\r\n" : ", ");
    }
    serial_printf("\r\n");

    int min_index = _get_min_error();
    // float e =
    //   100.0 - (float)(MIN(data[min_index].error, data[min_index].error_rev) / 1000);
    data[min_index].errors[0] = 100 - data[min_index].errors[0] / 1000;
    data[min_index].errors[1] = 100 - data[min_index].errors[1] / 1000;

    serial_printf("%s: %d%% forward / %d%% reverse match\r\n", data[min_index].name,
                  data[min_index].errors[0], data[min_index].errors[1]);
    if (data[min_index].square == 1) {
        data[min_index].errors[2] = 100 - data[min_index].errors[2] / 1000;
        data[min_index].errors[3] = 100 - data[min_index].errors[3] / 1000;

        serial_printf("%s: %d%% forward / %d%% reverse match -- transpose\r\n",
                      data[min_index].name, data[min_index].errors[2],
                      data[min_index].errors[3]);
    }

    uint8_t e_fwd = MAX(data[min_index].errors[0], data[min_index].errors[2]);
    uint8_t e_rev = MAX(data[min_index].errors[1], data[min_index].errors[3]);

    scan_timer = timer_get() - scan_timer;
    serial_printf("Detection in %dms, scan in %dms, %dms in total\r\n", detect_time,
                  scan_timer - detect_time, scan_timer);
    lcd_clear_display();
    lcd_printf(0, "%s", data[min_index].name);
    lcd_printf(0x40, "%d%% match", MAX(e_fwd, e_rev));
    grid_move_to_point(grid.max_y, grid.max_y);

    while (1)
        ;
}