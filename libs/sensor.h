#pragma once

#include <lpc_types.h>

uint8_t sensor_read_register(uint8_t address);
void sensor_read_multiple_registers(uint8_t start_address, uint8_t* buffer, uint32_t len);
void sensor_write_register(uint8_t address, uint8_t data);
void sensor_enable();
void sensor_read_all_colours(uint16_t* colours);
uint16_t sensor_read_16bit(uint8_t address);
uint16_t sensor_read_clear();