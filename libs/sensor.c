#include <lpc_types.h>

#include "i2c.h"
#include "sensor.h"
#include "sensor_commands.h"
#include "serial.h"
#include "systick_delay.h"

uint8_t sensor_read_register(uint8_t address) {
    uint8_t in;
    uint8_t tx = SENSOR_CMD_REG(1, 0, address);
    i2c_duplex(SENSOR_ADDRESS, &tx, 1, &in, 1);

    return in;
}

void sensor_read_multiple_registers(uint8_t start_address, uint8_t* buffer,
                                    uint32_t len) {
    uint8_t tx = SENSOR_CMD_REG(1, 1, start_address);
    i2c_duplex(SENSOR_ADDRESS, &tx, 1, buffer, len);
}

void sensor_write_register(uint8_t address, uint8_t data) {
    uint8_t tx[] = {SENSOR_CMD_REG(1, 0, address), data};
    i2c_send_data(SENSOR_ADDRESS, tx, 2);
}

void sensor_enable() {
    uint8_t en = sensor_read_register(SENSOR_ENABLE_ADDRESS);
    en |= SENSOR_ENABLE_REG(0, 0, 1, 1);

    // need to give the chip time to wake up :shrug:
    systick_delay_blocking(8);
    sensor_write_register(SENSOR_ENABLE_ADDRESS, en);

    // use ~101 ms to start
    // sensor_write_register(SENSOR_TIMING_ADDRESS, 0xD5);
}

void sensor_read_all_colours(uint16_t* colours) {
    uint8_t buffer[8] = {0};
    sensor_read_multiple_registers(SENSOR_CDATA, buffer, 8);

    for (int i = 0; i < 4; i++) {
        colours[i] = buffer[i * 2 + 1] << 8 | buffer[i * 2];
    }
}

uint16_t sensor_read_16bit(uint8_t address) {
    uint8_t buffer[2] = {0};
    sensor_read_multiple_registers(address, buffer, 2);

    return buffer[1] << 8 | buffer[0];
}

uint16_t sensor_read_clear() {
    return sensor_read_16bit(SENSOR_CDATA);
}