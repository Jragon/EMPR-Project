#pragma once

#include <lpc_types.h>

#include "lcd_commands.h"

#define LCD_ADDRESS 0x3b
#define DDRAM_SIZE 0x50

/**
    @brief initialises the lcd screen with cursor
*/
void lcd_init();

/**
    @brief Sends data to the lcd address: LCD_ADDRESS in lcd.h
    @param data array of 8 bit ints to send
    @param length lenth of the array. use LEN() macro from utils
*/
void lcd_send_data(uint8_t* data, uint32_t length);

/**
    @brief sends an ascii string to the 2x16 lcd screen.
    for non-shift mode line 1 is addr 0x00 to 0x0F, line 2 is 0x40 to 0x4F
    NB the max length is DDRAM is 80 registers
    @param string \0 terminated string (array of chars or uint8)
    @param start_addr starting address for the DDRAM - 0x40 is the start second line
*/
void lcd_send_string(char* string, uint8_t start_addr);

/**
    @brief Clears the lcd display by overwriting each cell with ' '
*/
void lcd_clear_display();

/**
    @brief sends character with DDRAM as start addr

    @param ch character to send
    @param start_addr DDRAM cell to write
*/
void lcd_send_char(char ch, uint8_t start_addr);

/**
    @brief writes character at the cursor, incrementing cursor
    When the cursor reaches the end of the line it goes to the next,
    or it goes back to the first

    @param ch character to send
*/
void lcd_write_char_at_cursor(char ch);

/**
    @brief sets the internal cursor variable

    @param addr DDRAM address
*/
void lcd_set_cursor_addr(uint8_t addr);

/**
    @brief maps a char to the R character set of the lcd screen.
    chars not in the charset are mapped to upside-down question mark.

    @param index
    @return uint8_t
*/
uint8_t lcd_char_map(uint8_t index);

/**
    @brief printf to lcd

    @param start_addr starting addr to write to, 0x40 is start of line 2
    @param format printf format string
    @param ... format args
    @return int number of chars written
*/
int lcd_printf(uint8_t start_addr, const char* format, ...);
