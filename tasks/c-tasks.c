#include <lpc_types.h>

#include <math.h>
#include <string.h>

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
    return (_sum_start_end(colours, 1, 4) * 255) / colours[0];
}

void _normalize_colours(uint16_t* colours) {
    for (int i = 1; i < 4; i++) {
        colours[i] = (colours[i] * 255) / colours[0];
    }
}

uint8_t _step_until_edge(uint16_t start_x, uint16_t start_y, uint16_t end_x,
                         uint16_t end_y, uint16_t step, uint32_t inttime, uint16_t diff) {
    grid_move_to_point(start_x, start_y);
    timer_block(inttime << 3);
    uint16_t colours[4] = {0};
    uint16_t last_colours[4] = {0};

    sensor_read_all_colours(last_colours);
    sensor_normalize_colours(last_colours);

    while (grid.x != end_x || grid.y != end_y) {
        grid_step_to_point(end_x, end_y, step);
        timer_block(inttime);

        sensor_read_all_colours(colours);
        sensor_normalize_colours(colours);

        serial_printf("(%3d, %3d) ", grid.x, grid.y);
        for (int i = 1; i < 4; i++) {
            serial_printf("; %3d-%3d=%3d", colours[i], last_colours[i],
                          ABS(colours[i] - last_colours[i]));
            if (ABS(colours[i] - last_colours[i]) > diff) {
                return 1;
            }
        }
        serial_printf("\r\n");

        memcpy(last_colours, colours, 8);
    }

    return 0;
}

void _detect_flag(uint16_t* min_x, uint16_t* min_y, uint16_t step, uint32_t inttime) {
    // detects the two edges of the flags -- assumed positioned at MAXX MAXY
    grid_home();
    serial_printf("[Flag]: Detecting Flag\r\n");
    lcd_clear_display();
    lcd_printf(0x00, "Finding edges");

    if (_step_until_edge(50, 700, grid.max_x, 700, 16, inttime, 16) == 0) {
        serial_printf("[Flag]: FAILED TO FIND X EDGE! :(\r\n");
        lcd_clear_display();
        lcd_printf(0x00, "Failed to");
        lcd_printf(0x40, "find x edge :(");
        while (1)
            ;
    }

    *min_x = grid.x;
    serial_printf("[Flag]: X edge detected at %d\r\n", *min_x);
    timer_block(100);

    if (_step_until_edge(700, 20, 700, grid.max_y, 16, inttime, 16) == 0) {
        serial_printf("[Flag]: FAILED TO FIND Y EDGE! :(\r\n");
        lcd_clear_display();
        lcd_printf(0x00, "Failed to");
        lcd_printf(0x40, "find y edge :(");
        while (1)
            ;
    }

    *min_y = grid.y;
    serial_printf("[Flag]: Y edge detected at %d\r\n", *min_y);
    timer_block(100);
}

#define POINTS 3
uint16_t _box_scan(uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y,
                   uint32_t inttime) {
    uint16_t width = max_x - min_x;
    uint16_t height = max_y - min_y;
    uint16_t x_step = width / (POINTS + 1);
    uint16_t y_step = height / (POINTS + 1);

    uint16_t colours[4] = {0};
    sensor_read_all_colours(colours);

    uint32_t sum = 0;

    serial_printf("{ ");
    for (int i = 1; i < POINTS + 1; i++) {
        serial_printf("{ ");
        for (int j = 1; j < POINTS + 1; j++) {
            if (width < height) {
                grid_move_to_point(min_x + x_step * j, min_y + y_step * i);
            } else {
                grid_move_to_point(min_x + x_step * i, min_y + y_step * j);
            }
            timer_block(inttime);
            sensor_read_all_colours(colours);
            sensor_normalize_colours(colours);

            serial_printf("{%d, %d, %d}%s", colours[1], colours[2], colours[3],
                          j == POINTS ? "" : ", ");
        }
        serial_printf(" }%c ", i == POINTS ? ' ' : ',');
    }
    serial_printf("}\r\n");

    return sum;
}

int _hue(int red, int green, int blue) {
    int min = MIN(MIN(red, green), blue);
    int max = MAX(MAX(red, green), blue);

    // serial_printf("\r\nCALCING HUE\r\n\t%d, %d: %d, %d, %d\r\n", min, max, red, green,
    //               blue);
    if (min == max) {
        return 0;
    }

    float hue = 0;
    if (max == red) {
        hue = (float)((float)(green - blue) / (float)(max - min));
    } else if (max == green) {
        hue = 2 + (float)((float)(blue - red) / (float)(max - min));
    } else {
        hue = 4 + (float)((float)(red - green) / (float)(max - min));
    }

    // serial_printf("\tHUE f = %f\r\n", hue);

    hue = hue * 60;
    if (hue < 0) {
        hue = hue + 360;
    }

    return hue;
}

uint16_t _box_scan_comp(uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y,
                        uint32_t inttime) {
    uint16_t width = max_x - min_x;
    uint16_t height = max_y - min_y;
    uint16_t x_step = width / (POINTS + 1);
    uint16_t y_step = height / (POINTS + 1);

    uint16_t colours[4] = {0};
    sensor_read_all_colours(colours);

    uint32_t sum = 0;

    serial_printf("{ ");
    for (int i = 1; i < POINTS + 1; i++) {
        serial_printf("{ ");
        for (int j = 1; j < POINTS + 1; j++) {
            if (width < height) {
                grid_move_to_point(min_x + x_step * j, min_y + y_step * i);
            } else {
                grid_move_to_point(min_x + x_step * i, min_y + y_step * j);
            }
            timer_block(inttime);
            sensor_read_all_colours(colours);
            sensor_normalize_colours(colours);
            serial_printf("{%d, %d, %d}%s", colours[1], colours[2], colours[3],
                          j == POINTS ? "" : ", ");

            // compare against flags and add error
            for (int flagi = 0; flagi < FLAGS_LEN; flagi++) {
                float dist1 = 0;
                float dist2 = 0;
                // serial_printf("\r\n%s", flags[flagi].name);
                for (int k = 0; k < 3; k++) {
                    dist1 += powf(flags[flagi].data[i - 1][j - 1][k] - colours[k + 1], 2);
                    dist2 += powf(
                      flags[flagi].data[POINTS - i][POINTS - j][k] - colours[k + 1], 2);
                    // serial_printf(
                    //   "\r\n\t(%d - %d)^2 = %.2f", flags[flagi].data[i - 1][j -
                    //   1][k], colours[k + 1], powf(flags[flagi].data[i - 1][j -
                    //   1][k] - colours[k + 1], 2));
                }

                flags[flagi].error[0] += sqrtf(dist1);
                flags[flagi].error[1] += sqrtf(dist2);
                // serial_printf("\r\n\tdist: sqrt(%.2f) = %.3f; accum
            }
        }
        serial_printf(" }%c ", i == POINTS ? ' ' : ',');
    }
    serial_printf("}\r\n");

    return sum;
}

void _reset() {
    for (uint8_t flag_index = 0; flag_index < FLAGS_LEN; flag_index++) {
        for (int i = 0; i < 2; i++) {
            flags[flag_index].error[i] = 0;
        }
    }
}

int _min_error() {
    int min_index = 0;
    int min = flags[min_index].error[0];
    serial_printf("Errors: \r\n");
    for (uint8_t flag_index = 0; flag_index < FLAGS_LEN; flag_index++) {
        serial_printf("%s", flags[flag_index].name);
        for (int i = 0; i < 2; i++) {
            serial_printf(" - %d", flags[flag_index].error[i]);
            if (flags[flag_index].error[i] < min) {
                min_index = flag_index;
                min = flags[flag_index].error[i];
            }
        }
        serial_printf("\r\n");
    }

    return min_index;
}

void _raster_scan(uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y,
                  uint16_t step, uint16_t inttime) {
    uint16_t colours[4] = {0};
    grid_move_to_point(min_x, min_y);
    timer_block(inttime);
    sensor_read_all_colours(colours);

    while (max_x != grid.x || max_y != grid.y) {
        timer_block(inttime);
        sensor_read_all_colours(colours);
        sensor_normalize_colours(colours);

        serial_printf("%d %d %d;", colours[1], colours[2], colours[3]);

        grid_step_to_point(max_x, grid.y, step);
        if (grid.x == max_x) {
            serial_printf("\n");
            grid_step_to_point(grid.x, max_y, step + 4);
            if (grid.y != max_y) {
                grid_move_to_point(min_x, grid.y);
            }
        }
    }
    serial_printf("end\r\n");
    serial_printf("at (%d, %d), aiming for (%d, %d)", grid.x, grid.y, grid.max_x,
                  grid.max_y);
}

void flag_raster_scan() {
    serial_printf("[Task]: Flag Raster Scan\r\n");

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t inttime = sensor_get_int_time();

    uint16_t min_x = 0, min_y = 0;

    _detect_flag(&min_x, &min_y, 100, inttime);
    _raster_scan(min_x, min_y, grid.max_x, grid.max_y, 8, inttime);
    grid_home();
}

void flag_scan() {
    serial_printf("[Task]: Flag Scan\r\n");
    _reset();

    sensor_set_gain(SENSOR_GAIN_16X);
    sensor_set_int_time(3);
    uint16_t inttime = sensor_get_int_time();

    uint16_t min_x = 0, min_y = 0;

    _detect_flag(&min_x, &min_y, 100, inttime);
    _box_scan_comp(min_x, min_y, grid.max_x, grid.max_y, inttime);
    int e = _min_error();

    lcd_clear_display();
    lcd_printf(0x00, "%s", flags[e].name);
    lcd_printf(0x40, "%d", MIN(flags[e].error[0], flags[e].error[1]));
    grid_home();

    keypad_wait_key('#', 10);
}