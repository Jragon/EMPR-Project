#include <lpc_types.h>

/**
 * @brief initilises timer 3 to tick at 1ms
 *
 */
void timer_init();

/**
 * @brief gets the current timer value
 * N.B. the timer will overflow after 4,294,967,295ms
 * that is ~50 days (i think -- my maths is not amazing)
 *
 * @returns milliseconds since timer was inited
 */
uint32_t timer_millis();