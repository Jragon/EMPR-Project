#include <lpc_types.h>

#include <math.h>

#include "../libs/keypad.h"
#include "../libs/lcd.h"
#include "../libs/scanner/grid.h"
#include "../libs/scanner/motors.h"
#include "../libs/scanner/sensor.h"
#include "../libs/serial.h"
#include "../libs/systick_delay.h"

void task_A1a_circle() {
    serial_printf("[Task]: Drawing a circle\r\n");
    grid_home();
    serial_printf("Homed\r\n");
    uint16_t offset_x = 400;
    uint16_t offset_y = 400;
    uint16_t radius = 300;

    for (int i = 0; i < 3; i++) {
        for (int theta = 0; theta < 360; theta++) {
            double rad = theta * 3.14 / 180;
            int x = offset_x + (radius * sin(rad));
            int y = offset_y + (radius * cos(rad));
            grid_move_to_point(x, y);
            // serial_printf("x: %3d, y: %3d\r\n", x, y);
        }
    }
}

void task_A1b_square() {
    serial_printf("[Task]: Drawing a square\r\n");
    grid_home();
    grid_move_to_point(50, 50);
    grid_move_to_point(50, 650);
    grid_move_to_point(650, 650);
    grid_move_to_point(650, 50);
    grid_move_to_point(50, 50);
}

void task_A1c_z_axis() {
    serial_printf("[Task]: Moving the Z axis\r\n");
    grid_home();
    grid_z_steps(50);
    grid_z_steps(-50 * 8);
    grid_z_steps(50);
    grid_z_steps(-50 * 8);
}

void task_A2_edge_detection() {
    serial_printf("[Task]: Edge Detection\r\n");
    lcd_clear_display();

    grid_calibrate();
}

void task_A3_manual_move() {
    serial_printf("[Task]: Manual move mode\r\n");
    keypad_set_as_inputs();
    lcd_clear_display();
    lcd_printf(0x00, "Homing");
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

            case '#':
                return;

            default:
                continue;
                break;
        }

        systick_delay_flag_reset();
        lcd_printf(0x40, "(%3d, %3d, %3d)", grid_get_x(), grid_get_y(), grid_get_z());
    }
}
