#include <lpc_types.h>
#include <lpc17xx_i2c.h>

#include "motors.h"
#include "i2c.h"
#include "systick_delay.h"

void setup_switches() {
    uint8_t data = 0xF;
    i2c_send_data(SWITCH_ADDRESS, &data, 1);
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

    if (~data & mask) {
        return 1;
    } else {
        return 0;
    }
}

int move(uint8_t *seq, uint8_t mask, unsigned int steps) {
    for (int i = 0, j = 0; i < steps; i++, j = (j + 1) % 4) {
        i2c_send_data(MOTORS_XY_LATCH_ADDRESS, seq + j, 1);

        if (check_switch(mask)) {
            return -1;
        }
    }

    return 1;
}

int movex(unsigned int steps) {
    uint8_t data[] = MOTOR_STEPX;
    return move(data, SWITCH_X_MASK, steps);
}

int movey(unsigned int steps) {
    uint8_t data[] = MOTOR_STEPY;
    return move(data, SWITCH_Y_MASK, steps);
}

int home(uint8_t *steps, uint8_t mask) {
    int i = 0;
    int j = 0;
    while (1) {
        i2c_send_data(MOTORS_XY_LATCH_ADDRESS, steps + i, 1);
        // i2c_send_data(MOTORS_XY_LATCH_ADDRESS, steps + (i % 4), 1);

        if (check_switch(mask)) {
            break;
        }

        // i++;
        i = (i + 1) % 4;
        j++;
        systick_delay_blocking(MOTOR_DELAY);
    }

    return j;
}

int home_x() {
    uint8_t data[] = MOTOR_STEPX_REVERSE;
    return home(data, SWITCH_X_MASK);
}

int home_y() {
    uint8_t data[] = MOTOR_STEPY_REVERSE;
    return home(data, SWITCH_Y_MASK);
}