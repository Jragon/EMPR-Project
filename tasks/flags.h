#include <lpc_types.h>

#define STEP_X 150
#define STEP_Y 150
#define FLAGS_LEN 10

struct flag {
    char name[10];
    uint8_t data[3][3][3];
    uint16_t error[2];
} typedef flag_t;

extern flag_t flags[FLAGS_LEN];