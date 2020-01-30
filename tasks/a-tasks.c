#include <lpc_types.h>

#include "../libs/grid.h"
#include "../libs/keypad.h"
#include "../libs/lcd.h"
#include "../libs/motors.h"
#include "../libs/sensor.h"
#include "../libs/serial.h"
#include "../libs/systick_delay.h"

void a2_edge_detection() {
    serial_printf("A2 Edge Detection \r\n");
    lcd_clear_display();

    lcd_printf(0x00, "homing ...");

    home_x();
    home_y();
    // home_z();

    lcd_clear_display();

    uint16_t last_intensity = sensor_read_clear();
    uint16_t high_bound = 10000;

    /* order:
       scan y +ve;
       scan x +ve;
       scan y -ve;
       scan x -ve -- to limit switches probably

       probs do inside of the grid
    */

    while (last_intensity < high_bound) {
        lcd_printf(0x00, "int: %d", last_intensity);
        movey(16);
        last_intensity = sensor_read_clear();
    }
}

void a3_manual_move() {
    serial_printf("a3 manual move\r\n");
    keypad_set_as_inputs();
    lcd_clear_display();

    lcd_printf(0x00, "homing");

    grid_home();

    lcd_printf(0x00, "Press 1-6");
    lcd_printf(0x40, "(%3d, %3d, %3d)", 0, 0, 0);

    systick_delay_flag_reset();
    while (1) {
        if (systick_flag() == 0) {
            continue;
        }

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
                continue;
                break;
        }

        systick_delay_flag_reset();
        lcd_printf(0x40, "(%3d, %3d, %3d)", grid_get_x(), grid_get_y(), grid_get_z());
    }
}