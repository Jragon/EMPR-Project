#include <lpc_types.h>

#include "data_len.h"
#define COLS 4
#define ROWS 4
struct data {
    char name[16];
    uint16_t vals[COLS][ROWS][3];
    uint16_t width;
    uint16_t height;
    // multiplied by 1000
    int errors[4];
    uint8_t square;
} typedef data_t;

extern data_t data[];