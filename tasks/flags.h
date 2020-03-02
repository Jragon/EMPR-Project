#include <lpc_types.h>

#define STEP_X 150
#define STEP_Y 150
#define FLAGS_LEN 10

struct flag {
    char name[10];
    uint16_t vals[12][3];
    // multiplied by 1000
    uint16_t error;
} typedef flag_t;

extern flag_t flags[FLAGS_LEN];