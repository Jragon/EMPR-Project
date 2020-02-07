#include <lpc_types.h>

#define GRID_MAX_X 700
#define GRID_MAX_Y 700
#define GRID_X_OFFSET 10
#define GRID_Y_OFFSET 260

typedef struct Grid_t {
    uint32_t max_x;
    uint32_t max_y;
    uint32_t x_offset;
    uint32_t y_offset;

    uint32_t x;
    uint32_t y;
    uint32_t z;
} Grid_t;

extern Grid_t grid;

void grid_home();
void grid_calibrate();
void grid_move_to_point(uint16_t x, uint16_t y);

void grid_x_steps(int steps);
void grid_y_steps(int steps);
void grid_z_steps(int steps);

uint32_t grid_get_x();
uint32_t grid_get_y();
uint32_t grid_get_z();