#include <lpc17xx_i2c.h>
#include <lpc_types.h>

#include <stdarg.h>
#include <stdio.h>

#include "i2c.h"
#include "lcd.h"
#include "util_macros.h"

uint8_t cursor_address = 0x00;

void lcd_init() {
    uint8_t init_cmds[] = {
        LCDCMD_ControlByte(0, 0),   LCDCMD_StandardFunctionSet, LCDCMD_DisplayCtl(1, 1, 0),
        LCDCMD_EntryModeSet(1, 0),  LCDCMD_ExtendedFunctionSet, LCDCMD_DispConf(0, 0),
        LCDCMD_DefaultTempCtl,      LCDCMD_DefaultHVGen,        LCDCMD_DefaultVLCDSet,
        LCDCMD_StandardFunctionSet, LCDCMD_ReturnHome
    };
    lcd_send_data(init_cmds, LEN(init_cmds));
}

void lcd_send_data(uint8_t* data, uint32_t length) {
    I2C_M_SETUP_Type cfg;
    cfg.sl_addr7bit = LCD_ADDRESS;
    cfg.rx_data = NULL;
    cfg.tx_data = data;
    cfg.tx_length = length;
    cfg.retransmissions_max = 2;
    I2C_MasterTransferData(I2C1DEV, &cfg, I2C_TRANSFER_POLLING);
}

void lcd_clear_display() {
    uint8_t resetInstructions[] = {
        LCDCMD_ControlByte(0, 0), LCDCMD_DisplayCtl(0, 0, 0),  // turn the display off
        LCDCMD_SetDDRAM(0)                                     // start at 0
    };
    uint8_t onInstructions[] = {LCDCMD_ControlByte(0, 0), LCDCMD_DisplayCtl(1, 1, 0),
                                LCDCMD_ReturnHome
                               };
    uint8_t data[] = {
        LCDCMD_ControlByte(0, 1),
        [1 ... 80] = 0x91  // blank char, see LCD_CHARSET
    };
    lcd_send_data(resetInstructions, LEN(resetInstructions));
    lcd_send_data(data, LEN(data));
    lcd_send_data(onInstructions, LEN(onInstructions));
    lcd_set_cursor_addr(0);
}

void lcd_send_char(char ch, uint8_t addr) {
    uint8_t instructions[] = {LCDCMD_ControlByte(0, 0), LCDCMD_SetDDRAM(addr)};
    uint8_t data[] = {LCDCMD_ControlByte(0, 1), lcd_char_map(ch)};
    lcd_send_data(instructions, 2);
    lcd_send_data(data, 2);
}

void lcd_write_char_at_cursor(char ch) {
    lcd_send_char(ch, cursor_address);

    if (cursor_address >= 0x0F && cursor_address < 0x40) {
        cursor_address = 0x40;

    } else if (cursor_address >= 0x4F) {
        cursor_address = 0x00;

    } else {
        cursor_address++;
    }
}

void lcd_set_cursor_addr(uint8_t addr) {
    cursor_address = addr;
}

void lcd_send_string(char* string, uint8_t start_addr) {
    uint8_t instructions[] = {LCDCMD_ControlByte(0, 0), LCDCMD_SetDDRAM(start_addr)};
    uint8_t data[DDRAM_SIZE + 1] = {LCDCMD_ControlByte(0, 1)};
    uint8_t i = 0;

    while (i <= DDRAM_SIZE && string[i] != '\0') {
        data[i + 1] = lcd_char_map(string[i]);
        i++;
    }

    lcd_send_data(instructions, LEN(instructions));
    lcd_send_data(data, i + 1);
}

uint8_t lcd_char_map(uint8_t index) {
    static const uint8_t LCD_CHARSET[128] = {
        [0 ... 127] = 0xE0,  // flipped ? as default symbol
        [' '] = 0x91,       ['!'] = 0xA1, ['"'] = 0xA2,  ['#'] = 0xA3, ['$'] = 0x82,
        ['%'] = 0xA5,       ['&'] = 0xA6, ['\''] = 0xA7, ['('] = 0xA8, [')'] = 0xA9,
        ['*'] = 0xAA,       ['+'] = 0xAB, [','] = 0xAC,  ['-'] = 0xAD, ['.'] = 0xAE,
        ['/'] = 0xAF,       ['0'] = 0xB0, ['1'] = 0xB1,  ['2'] = 0xB2, ['3'] = 0xB3,
        ['4'] = 0xB4,       ['5'] = 0xB5, ['6'] = 0xB6,  ['7'] = 0xB7, ['8'] = 0xB8,
        ['9'] = 0xB9,       [':'] = 0xBA, [';'] = 0xBB,  ['<'] = 0xBC, ['='] = 0xBD,
        ['>'] = 0xBE,       ['?'] = 0xBF, ['@'] = 0x80,  ['A'] = 0xC1, ['B'] = 0xC2,
        ['C'] = 0xC3,       ['D'] = 0xC4, ['E'] = 0xC5,  ['F'] = 0xC6, ['G'] = 0xC7,
        ['H'] = 0xC8,       ['Y'] = 0xC9, ['J'] = 0xCA,  ['K'] = 0xCB, ['L'] = 0xCC,
        ['M'] = 0xCD,       ['N'] = 0xCE, ['O'] = 0xCF,  ['P'] = 0xD0, ['Q'] = 0xD1,
        ['R'] = 0xD2,       ['S'] = 0xD3, ['T'] = 0xD4,  ['U'] = 0xD5, ['V'] = 0xD6,
        ['W'] = 0xD7,       ['X'] = 0xD8, ['Y'] = 0xD9,  ['Z'] = 0xDA, ['['] = 0x8A,
        [']'] = 0x54,       ['_'] = 0x5A, ['a'] = 0xE1,  ['b'] = 0xE2, ['c'] = 0xE3,
        ['d'] = 0xE4,       ['e'] = 0xE5, ['f'] = 0xE6,  ['g'] = 0xE7, ['h'] = 0xE8,
        ['i'] = 0xE9,       ['j'] = 0xEA, ['k'] = 0xEB,  ['l'] = 0xEC, ['m'] = 0xED,
        ['n'] = 0xEE,       ['o'] = 0xEF, ['p'] = 0xF0,  ['q'] = 0xF1, ['r'] = 0xF2,
        ['s'] = 0xF3,       ['t'] = 0xF4, ['u'] = 0xF5,  ['v'] = 0xF6, ['w'] = 0xF7,
        ['x'] = 0xF8,       ['y'] = 0xF9, ['z'] = 0xFA
    };
    return LCD_CHARSET[index <= 128 ? index : 0];
}

int lcd_printf(uint8_t start_addr, const char* format, ...) {
    va_list arg;
    char buf[40];
    int done;
    va_start(arg, format);
    done = vsprintf((char*)&buf, format, arg);
    va_end(arg);

    if (done > 0)
        lcd_send_string((char*)&buf, start_addr);

    return done;
}
