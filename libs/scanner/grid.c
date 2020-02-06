#include "grid.h"

#include <lpc_types.h>

#include <LPC17xx.h>

#include <string.h>

#include "../serial.h"
#include "../timer.h"

#include "motors.h"
#include "sensor.h"

static Grid_t grid = {GRID_MAX_X, GRID_MAX_Y, GRID_X_OFFSET, GRID_Y_OFFSET, 0, 0, 0};

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
    uint32_t last_time = timer_get();
    while (timer_get() - last_time < int_time << 1)
        ;

    uint16_t last_c = sensor_read_clear(), cur_c;

    motor_wake();
    last_time = timer_get();
    while (motor_running()) {
        if (timer_get() - last_time < int_time) {
            continue;
        }

        __disable_irq();  // critical section
        cur_c = sensor_read_clear();

        if ((comp_dir == DIR_POSITIVE && (cur_c - last_c) > thresh) ||
            (comp_dir == DIR_NEGATIVE && (last_c - cur_c) > thresh)) {
            motor_sleep();
            __enable_irq();
            break;
        }

        // take avg of last 4 values
        last_c = last_c - (last_c >> 2) + (cur_c >> 2);
        last_time = timer_get();
        __enable_irq();
    }
}

void grid_calibrate() {
    // reset grid before calibrating
    memset(&grid, 0, sizeof(grid));
    grid_home();

    // set the gain high since all we're interested in is whether the sensor reads nothing
    // or anything (ie on the platform or not on the platform).
    sensor_set_gain(SENSOR_GAIN_60X);
    // low int time needs high gain otherwise the values take to long to read and things
    // are slow.
    sensor_set_int_time(SENSOR_INT_TIME_24MS);
    uint16_t int_time = sensor_get_int_time();

    // move at 1/4 the int time
    // ie the motor does 4 steps each time the sensor is read
    uint32_t motor_tick = int_time * 250;
    motor_set_tick(motor_tick);

    motor_set(0, 1000, 0);
    _run_until_thresh(DIR_POSITIVE, 500, int_time);
    grid.y_offset = 1000 - motors.y_steps + 64;
    serial_printf("y offset: %d\r\n", grid.y_offset);

    // carry on moving along the y axis until we fall off the platform
    motor_set(0, 1500, 0);
    _run_until_thresh(DIR_NEGATIVE, 500, int_time);
    grid.max_y = 1500 - motors.y_steps;
    serial_printf("y max: %d\r\n", grid.max_y);

    motor_reset_tick();
    grid_home();
    motor_set_tick(motor_tick);

    // motor_set(100, 0, 0);
    // _run_until_thresh(DIR_POSITIVE, 300, int_time);
    // grid.x_offset = 100 - motors.x_steps;
    // it is always zero idc
    grid.x_offset = 0;
    serial_printf("x offset: %d\r\n", grid.x_offset);

    motor_set(1500, 0, 0);
    _run_until_thresh(DIR_NEGATIVE, 300, int_time);
    grid.max_x = 1500 - motors.x_steps;
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
