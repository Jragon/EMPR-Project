#include "menu.h"

#include <string.h>

#include "lcd.h"
#include "serial.h"
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
