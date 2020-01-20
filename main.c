#include <string.h>

#include "libs/i2c.h"
#include "libs/serial.h"
#include "libs/pinsel.h"
#include "libs/systick_delay.h"

#include "motors.h"

void setup_switches() {
    uint8_t data = 0xF;
    I2C_M_SETUP_Type cfg;
    cfg.sl_addr7bit = SWITCH_ADDRESS;
    cfg.rx_length = 0;
    cfg.tx_data = &data;
    cfg.tx_length = 1;
    I2C_MasterTransferData(I2C1DEV, &cfg, I2C_TRANSFER_POLLING);
    serial_printf("Switches initiated \r\n");
}

int check_switch(uint8_t mask) {
    uint8_t data;
    I2C_M_SETUP_Type cfg;
    cfg.sl_addr7bit = SWITCH_ADDRESS;
    cfg.rx_data = &data;
    cfg.rx_length = 1;
    cfg.tx_data = NULL;
    cfg.retransmissions_max = 2;
    I2C_MasterTransferData(I2C1DEV, &cfg, I2C_TRANSFER_POLLING);

    serial_printf("\r\nSwitch: 0x%X\r\n", ~data);
    if (~data & mask) {
        return 1;
    } else {
        return 0;
    }
}


void home(uint8_t *steps, uint8_t mask) {
    int i = 0;
    while (1) {
        i2c_send_data(MOTORS_XY_LATCH_ADDRESS, steps + i, 1);

        if (check_switch(mask)) {
            serial_printf("home!\r\n");
            break;
        }

        i = (i + 1) % 4;
        systick_delay_blocking(5);
    }
}

void home_x() {
    uint8_t data[] = MOTOR_STEPX_REVERSE;
    home(data, SWITCH_X_MASK);
}

void home_y() {
    uint8_t data[] = MOTOR_STEPY_REVERSE;
    home(data, SWITCH_Y_MASK);
}

int main() {
    serial_init();
    systick_init();
    i2c_init();
    serial_printf("hello\r\n");
    setup_switches();

    home_x();
    home_y();

    return 0;
}

