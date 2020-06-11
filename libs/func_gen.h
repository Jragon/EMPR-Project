#pragma once

#include <lpc_types.h>

/**
    @brief gens a sinewave for the DAC centered around 1.165V
    shifts the bits into the right pattern for putting directly in to the DACR register

    @param sinewave pointer to buffer
    @param samples size of buffer
    @param amplitude_v amplitude voltage
*/
void func_gen_sinewave(int32_t* sinewave, int32_t samples, float amplitude_v);
