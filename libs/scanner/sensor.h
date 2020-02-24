#pragma once

#include <lpc_types.h>

/**
 * @brief maximum integration time
 * the int time can range between 2.27ms and 2.56ms. As a defualt it's set to 2.4ms -- so
 * the max is around 3ms, useful for the timer_get func
 */
#define MAX_INT_TIME 3

/** Integration time settings for TCS34725 */
typedef enum {
    SENSOR_INT_TIME_2_4MS = 0xFF,
    SENSOR_INT_TIME_24MS = 0xF6,
    SENSOR_INT_TIME_50MS = 0xEB,
    SENSOR_INT_TIME_101MS = 0xD5,
    SENSOR_INT_TIME_154MS = 0xC0,
    SENSOR_INT_TIME_700MS = 0x00
} SensorIntTime_t;

typedef enum {
    SENSOR_GAIN_1X = 0x00,
    SENSOR_GAIN_4X = 0x01,
    SENSOR_GAIN_16X = 0x02,
    SENSOR_GAIN_60X = 0x03
} SensorGain_t;

uint8_t sensor_read_register(uint8_t address);
void sensor_read_multiple_registers(uint8_t start_address, uint8_t* buffer, uint32_t len);
void sensor_write_register(uint8_t address, uint8_t data);
void sensor_enable();
void sensor_read_all_colours(uint16_t* colours);
uint16_t sensor_read_16bit(uint8_t address);
uint16_t sensor_read_clear();
void sensor_set_int_time(uint16_t ms);
uint16_t sensor_get_int_time();
void sensor_set_gain(SensorGain_t gain);
void sensor_read_rgb(uint16_t* red, uint16_t* green, uint16_t* blue);
uint8_t sensor_ready();