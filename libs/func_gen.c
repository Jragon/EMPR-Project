#include <math.h>

#include "func_gen.h"

#define PI (float)(3.141592654)

void func_gen_sinewave(int32_t* sinewave, int32_t samples, float amplitude_v) {
    uint32_t amplitude = (1023 / 3.3) * amplitude_v;
    amplitude = (amplitude > 512) ? 512 : amplitude;

    for (int i = 0; i < samples; i++) {
        sinewave[i] = amplitude * sin(2 * PI * i / samples);
        sinewave[i] = ((sinewave[i] + 512)  // offset by 512 (half of DAC val)
                       << 6)                // DACR bit value is 6-15
                      | 1 << 16;            // Set DACR bias bit (bit 16)
    }
}
