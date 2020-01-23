#include <lpc_types.h>

typedef struct Grid_t {
    uint32_t max_x;
    uint32_t max_y;
    uint32_t x_offset;
    uint32_t y_offset;

    uint32_t x;
    uint32_t y;
} Grid_t;

void grid_home(Grid_t *grid);
void grid_move_to_point(Grid_t *grid, uint32_t x, uint32_t y);
