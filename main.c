#include <lpc17xx_gpio.h>

#include <math.h>
#include <string.h>

#include <lpc17xx_gpio.h>

#include "libs/i2c.h"
#include "libs/lcd.h"
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
        serial_printf("[Menu]: keypad int\r\n");
        keypad_pressed_flag = 1;
    }
}

int main() {
    extern int8_t menu_index;
    serial_init();
    i2c_init();
    lcd_init();
    menu_init();
    systick_init();
    serial_printf("hello\r\n");
    GPIO_IntCmd(0, 1 << 23, 1);
    NVIC_EnableIRQ(EINT3_IRQn);
    keypad_set_as_inputs();
    menu_add_option("Opt1", 0, NULL);
    menu_add_option("Opt2", 1, NULL);
    menu_add_option("Opt3", 2, NULL);
    menu_add_option("Opt4", 3, NULL);
    menu_add_option("Opt5", 4, NULL);
    menu_draw(0);
    keypad_pressed_flag = 0;
    systick_delay_flag_init(5);

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
        keypad_pressed_flag = 0;
    }
}
