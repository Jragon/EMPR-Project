#include "grid.h"

#include <lpc_types.h>

#include "../serial.h"

#include "motors.h"

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
