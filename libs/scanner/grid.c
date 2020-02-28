#include "grid.h"

#include <lpc_types.h>

#include <LPC17xx.h>

#include <string.h>

#include "libs/lcd.h"
#include "libs/serial.h"
#include "libs/timer.h"

#include "motors.h"
#include "sensor.h"

Grid_t grid = {GRID_MAX_X, GRID_MAX_Y, GRID_X_OFFSET, GRID_Y_OFFSET, 0, 0, 0};

void grid_home() {
    home_x();
    home_y();
    home_z();
    movex(grid.x_offset);
    movey(grid.y_offset);
    grid.x = 0;
    grid.y = 0;
    grid.z = 0;
}

void _run_until_thresh(Direction_t comp_dir, uint16_t thresh, uint16_t int_time) {
    // wait for first int time cycle
    // serial_printf("a;lskdj)");
    timer_block(int_time << 3);
    uint16_t last_c = sensor_read_clear();
    uint32_t last_time = timer_get();

    motor_wake();
    while (motor_running()) {
        if (timer_get() - last_time < int_time << 2) {
            continue;
        }

        __disable_irq();  // critical section
        last_c = sensor_read_clear();
        if ((comp_dir == DIR_POSITIVE && last_c > thresh) ||
            (comp_dir == DIR_NEGATIVE && last_c < thresh)) {
            motor_sleep();
            __enable_irq();
            break;
        }

        last_time = timer_get();
        lcd_printf(0x40, "cdata: %d     ", last_c);
        __enable_irq();
    }
}

void grid_calibrate() {
    // reset grid before calibrating
    memset(&grid, 0, sizeof(grid));
    grid_home();

    serial_printf("calibrating grid\r\n");

    // fix dodgy timings
    timer_block(2);
    // set the gain high since all we're interested in is whether the sensor reads nothing
    // or anything (ie on the platform or not on the platform).
    sensor_set_gain(SENSOR_GAIN_60X);

    // low int time needs high gain otherwise the values take to long to read and things
    // are slow.
    sensor_set_int_time(8);
    uint16_t int_time = sensor_get_int_time();

    // move at 1/4 the int time
    // ie the motor does 4 steps each time the sensor is read
    uint32_t motor_tick = int_time * 250;
    motor_set_tick(motor_tick);

    serial_printf("find y offset\r\n");

    motor_set(0, 250, 0);
    _run_until_thresh(DIR_POSITIVE, 3800, int_time);
    grid.y_offset = 250 - motors.y_steps;
    serial_printf("y offset: %d\r\n", grid.y_offset);

    serial_printf("find y max\r\n");

    // carry on moving along the y axis until we fall off the platform
    motor_set(0, 1500, 0);
    _run_until_thresh(DIR_NEGATIVE, 4000, int_time);
    grid.max_y = 1500 - motors.y_steps - 100;
    serial_printf("y max: %d\r\n", grid.max_y);

    motor_reset_tick();
    grid_home();
    grid_move_to_point(0, grid.max_y / 2);
    motor_set_tick(motor_tick);

    // it is always zero idc
    grid.x_offset = 0;
    serial_printf("x offset: %d\r\n", grid.x_offset);

    serial_printf("find x max\r\n");
    motor_set(1100, 0, 0);
    _run_until_thresh(DIR_NEGATIVE, 4000, int_time);
    grid.max_x = 1100 - motors.x_steps - 100;
    serial_printf("x max: %d\r\n", grid.max_x);

    motor_reset_tick();
    grid_home();
}

void grid_move_to_point(uint16_t x, uint16_t y) {
    if (x > grid.max_x) {
        x = grid.max_x;
    }

    if (y > grid.max_y) {
        y = grid.max_y;
    }

    motor_move_blocking(x - grid.x, y - grid.y, 0);

    grid.x = x;
    grid.y = y;
}

void grid_step_to_point(uint16_t x, uint16_t y, uint16_t step) {
    uint16_t movex, movey;

    if (x < grid.x) {
        // neg dir
        movex = grid.x < step ? 0 : grid.x - step;
    } else {
        movex = (grid.x + step < x) ? grid.x + step : x;
    }

    if (y < grid.y) {
        // neg dir
        movey = grid.y < step ? 0 : grid.y - step;
    } else {
        movey = (grid.y + step < y) ? grid.y + step : y;
    }

    grid_move_to_point(movex, movey);
}

void grid_x_steps(int steps) {
    motor_move_blocking(steps, 0, 0);
    if (motor_get_lims() & X_LIM) {
        grid.x = 0;
    } else {
        grid.x += steps;
    }
}

void grid_y_steps(int steps) {
    motor_move_blocking(0, steps, 0);
    if (motor_get_lims() & Y_LIM) {
        grid.y = 0;
    } else {
        grid.y += steps;
    }
}

void grid_z_steps(int steps) {
    motor_move_blocking(0, 0, steps);
    if (motor_get_lims() & Z_LIM) {
        grid.z = 0;
    } else {
        grid.z += steps;
    }
}

uint32_t grid_get_x() {
    return grid.x;
}

uint32_t grid_get_y() {
    return grid.y;
}

uint32_t grid_get_z() {
    return grid.z;
}
