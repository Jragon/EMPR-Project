#include <lpc17xx_gpio.h>

#include <math.h>
#include <string.h>

#include <lpc17xx_gpio.h>

#include "libs/i2c.h"
#include "libs/keypad.h"
#include "libs/serial.h"
#include "libs/pinsel.h"
#include "libs/systick_delay.h"
#include "libs/motors.h"
#include "libs/grid.h"
#include "libs/sensor.h"
#include "libs/sensor_commands.h"

int main() {
    serial_init();
    systick_init();
    i2c_init();

volatile uint8_t keypad_pressed_flag = 0;
volatile uint32_t adc_val;
volatile uint8_t read = 0;

    Grid_t grid = {
        700, 700, 10, 260, 0, 0
    };

    
    grid_home(&grid);
    grid_move_to_point(&grid, 700, 700);

void EINT3_IRQHandler() {
    if (GPIO_GetIntStatus(KEYPAD_INT_PORT, KEYPAD_INT_PIN, KEYPAD_INT_EDGE)) {
        GPIO_ClearInt(KEYPAD_INT_PORT, 1 << KEYPAD_INT_PIN);
        // serial_printf("keypad int\r\n");
        keypad_set_flag();
    }
}

int main() {
    extern int8_t menu_index;
    serial_init();
    i2c_init();
    lcd_init();
    menu_init();
    systick_init();
    sensor_enable();

    GPIO_IntCmd(0, 1 << 23, 1);
    NVIC_EnableIRQ(EINT3_IRQn);
    keypad_set_as_inputs();

    serial_printf("hello\r\n");

    // a2_edge_detection();

    menu_add_option("A2: edge detec", 0, a2_edge_detection);
    menu_add_option("A3: man move", 1, a3_manual_move);
    menu_add_option("B1: CRGB move", 2, b1_xyz_move_rgb);

    menu_draw(0);

    while(1) {
        sensor_read_all_colours(colours);
        serial_printf("C: %d, R: %d, G: %d, B: %d\r\n", colours[0], colours[1], colours[2], colours[3]);

        systick_delay_blocking(25);
    }

            case '#':
                menu_run_callback(menu_index);

            default:
                break;
        }

        keypad_set_as_inputs();
        systick_delay_flag_reset();
        keypad_reset_flag();
    }
}
