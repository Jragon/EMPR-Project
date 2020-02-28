#include <lpc_types.h>

#include <math.h>

#include "libs/keypad.h"
#include "libs/lcd.h"
#include "libs/scanner/grid.h"
#include "libs/scanner/motors.h"
#include "libs/scanner/sensor.h"
#include "libs/serial.h"
#include "libs/systick_delay.h"
#include "libs/timer.h"

#include "tasks.h"

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
    grid_calibrate();

    for (int i = 0; i < 3; i++) {
        grid_move_to_point(grid.max_x, 0);
        grid_move_to_point(grid.max_x, grid.max_y);
        grid_move_to_point(0, grid.max_y);
        grid_move_to_point(0, 0);
    }
}

#define DISTANCE 1000
void task_A1c_z_axis() {
    serial_printf("[Task]: Moving the Z axis\r\n");
    grid_home();
    grid_z_steps(DISTANCE);
    grid_z_steps(-DISTANCE * 8);
    grid_z_steps(DISTANCE);
    grid_z_steps(-DISTANCE * 8);
}

void task_A2_edge_detection() {
    serial_printf("[Task]: Edge Detection\r\n");
    lcd_clear_display();

    grid_calibrate();

    lcd_clear_display();
    lcd_printf(0x00, "Max: (%3d, %3d)", grid.max_x, grid.max_y);
    lcd_printf(0x40, "Min: (%3d, %3d)", grid.x_offset, grid.y_offset);

    while (1)
        ;
}

void task_A3_manual_move() {
    serial_printf("[Task]: A1: Manual Move\r\n");
    lcd_clear_display();

    lcd_printf(0x00, "homing...");

    grid_home();

    lcd_printf(0x00, "X: %4d, Y: %4d", grid.x, grid.y);
    lcd_printf(0x40, "Z: %4d", grid.z);

    uint32_t time = timer_get();
    while (1) {
        if (timer_get() - time < 7) {
            continue;
        }
        time = timer_get();

        if (_manmove() == 0) {
            continue;
        }

        lcd_printf(0x00, "X: %4d, Y: %4d", grid.x, grid.y);
        lcd_printf(0x40, "Z: %4d", grid.z);
    }
}
