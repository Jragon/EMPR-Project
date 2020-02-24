#include "libs/keypad.h"
#include "libs/scanner/grid.h"

uint8_t _manmove() {
    char k = keypad_read();
    switch (k) {
        case '1':
            grid_x_steps(8);
            break;
        case '4':
            grid_x_steps(-8);
            break;
        case '2':
            grid_y_steps(8);
            break;
        case '5':
            grid_y_steps(-8);
            break;
        case '3':
            grid_z_steps(8);
            break;
        case '6':
            grid_z_steps(-8);
            break;
        default:
            return -1;
            break;
    }

    return 0;
}