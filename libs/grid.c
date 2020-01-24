#include <lpc_types.h>

#include "motors.h"
#include "grid.h"

void grid_home(Grid_t *grid) {
    home_x();
    home_y();

    movex(grid->x_offset);
    movey(grid->y_offset);

    grid->x = 0;
    grid->y = 0;
}

void grid_move_to_point(Grid_t *grid, uint32_t x, uint32_t y) {
    if (x > grid->max_x) {
        x = grid->max_x;
    }

    if (y > grid->max_y) {
        y = grid->max_y;
    }

    movexy_together(x - grid->x, y - grid->y);
    grid->x = x;
    grid->y = y;
}
