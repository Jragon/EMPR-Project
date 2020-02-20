#include "menu.h"

#include <stdlib.h>
#include <string.h>

#include "keypad.h"
#include "lcd.h"
#include "serial.h"
#include "systick_delay.h"
#include "util_macros.h"

menu_item menu[64];
int8_t menu_index;

void menu_init() {
    memset(menu, 0, sizeof(menu));
}

void menu_print_items() {
    for (int i = 0; i < LEN(menu); i++) {
        if (menu[i].name[0] == '\0') {
            continue;
        }
        serial_printf("%i - %s\r\n", i, menu[i].name);
    }
    serial_printf("done\r\n");
}

void menu_add_option(char* name, int8_t index, void (*func)()) {
    strcpy(menu[index].name, name);
    menu[index].callback = func;
}

void pc_control_mode() {
    lcd_clear_display();
    char buf[32];
    while (strcmp(buf, "hello") != 0) {
        memset(buf, 0, sizeof(buf));
        serial_read_blocking(buf, 5);
    }

    serial_printf("[Menu]: Sending menu items\r\n");
    menu_print_items();

    memset(buf, 0, sizeof(buf));
    serial_read_blocking(buf, 11);
    lcd_printf(0x0, "%s", buf);
    char dest[4];
    memset(dest, 0, sizeof(dest));
    strcpy(dest, buf + 8);
    int item = atoi(dest);
    lcd_clear_display();
    menu_run_callback(item);
}

void menu_run_callback(int8_t index) {
    menu[index].callback();
}

void menu_draw(int8_t index) {
    index = index < 0 ? 0 : index;
    menu_index = index;
    serial_printf("[Menu]: menu drawn at %i\r\n", index);
    lcd_clear_display();
    lcd_printf(0x00, "^");
    lcd_printf(0x40, "}");
    lcd_printf(0x00 + 2, "%s", menu[index].name);
    lcd_printf(0x40 + 2, "%s", menu[index + 1].name);
}

void menu_loop() {
    menu_draw(0);
    while (1) {
        if (keypad_flag() == 0 || systick_flag() == 0) {
            continue;
        }

        char k = keypad_read();
        serial_printf("[Menu]: Read '%c'\r\n", k);

        switch (k) {
            case 'A':
                menu_draw(menu_index - 1);
                break;

            case 'B':
                menu_draw(menu_index + 1);
                break;

            case '#':
                serial_printf("[Menu]: Called menu item %i\r\n", menu_index);
                menu_run_callback(menu_index);
                menu_draw(0);

            default:
                break;
        }

        keypad_set_as_inputs();
        systick_delay_flag_reset();
        keypad_reset_flag();
    }
}
