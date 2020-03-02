#pragma once

/**
    gets the length of an array
*/
#define LEN(x) (sizeof(x) / sizeof((x)[0]))
#define ABS(x) ((x < 0) ? -(x) : (x))