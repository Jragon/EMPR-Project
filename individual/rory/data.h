#include <lpc_types.h>

#include "data_len.h"
struct data {
    char name[16];
    uint16_t vals[16][3];
    uint16_t width;
    uint16_t height;
    // multiplied by 1000
    uint16_t error;
    uint16_t error_rev;
} typedef data_t;

extern data_t data[];