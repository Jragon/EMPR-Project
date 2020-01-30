#include <lpc_types.h>

#include "grid.h"
#include "motors.h"

static Grid_t grid = {GRID_MAX_X, GRID_MAX_Y, GRID_X_OFFSET, GRID_Y_OFFSET, 0, 0};

void grid_home() {
    home_x();
    home_y();

    movex(grid.x_offset);
    movey(grid.y_offset);

    grid.x = 0;
    grid.y = 0;
}

void grid_move_to_point(uint32_t x, uint32_t y) {
    if (x > grid.max_x) {
        x = grid.max_x;
    }

    if (y > grid.max_y) {
        y = grid.max_y;
    }

    movexy_together(x - grid.x, y - grid.y);
    grid.x = x;
    grid.y = y;
}

void grid_x_steps(int steps) {
    grid.x += steps;
    movex(steps);
}

void grid_y_steps(int steps) {
    grid.y += steps;
    movey(steps);
}

uint32_t grid_get_x() {
    return grid.x;
}

uint32_t grid_get_y() {
    return grid.y;
}