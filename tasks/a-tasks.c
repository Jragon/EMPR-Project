#include <lpc_types.h>

#include "../libs/keypad.h"
#include "../libs/serial.h"
#include "../libs/grid.h"
#include "../libs/lcd.h"
#include "../libs/motors.h"
#include "../libs/systick_delay.h"

void a3_manual_move() {
    serial_printf("a3 manual move\r\n");
    keypad_set_as_inputs();
    lcd_clear_display();

    lcd_printf(0x00, "homing");

    // perhaps make grid a singleton
    Grid_t grid = {
        700, 700, 10, 260, 0, 0
    };
    grid_home(&grid);

    lcd_printf(0x00, "Press 1-6");
    lcd_printf(0x40, "(%3d, %3d, %3d)", 0, 0, 0);

    systick_delay_flag_reset();
    while (1) {
        if (systick_flag() == 0) {
            continue;
        }

        char k = keypad_read();
        switch (k)
        {
        case '1':
            grid_x_steps(&grid, 8);
            break;
        case '4':
            grid_x_steps(&grid, -8);
            break;
        case '2':
            grid_y_steps(&grid, 8);
            break;
        case '5':
            grid_y_steps(&grid, -8);
            break;
        case '3':
            movez(5);
            break;
        case '6':
            movez(-5);
            break;
        default:
            continue;
            break;
        }

        systick_delay_flag_reset();
        lcd_printf(0x40, "(%3d, %3d, %3d)", grid.x, grid.y, 0);
    }
}