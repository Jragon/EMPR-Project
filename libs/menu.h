#pragma once

#include <lpc_types.h>

typedef struct menu_item {
    char name[32];
    void (*callback)();
} menu_item;

/**
    @breif Initialise the menu.
    Needs I2C, lcd and serial init.
*/
void menu_init();

void menu_print_items();

/**
    @breif add an option to the menu

    @param the name of the option to display on the lcd
    @param the index of the option
    @param the callback function pointer
*/
void menu_add_option(char* name, int8_t index, void (*func)());

/**
    @breif run the callback of the given menu item

    @param the index of the menu item
*/
void menu_run_callback(int8_t index);

/**
    @breif draw the menu

    @param the index of item to draw at the top
*/
void menu_draw(int8_t index);
