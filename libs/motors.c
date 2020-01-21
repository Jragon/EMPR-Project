#include <lpc_types.h>
#include <lpc17xx_i2c.h>

#include "motors.h"
#include "i2c.h"
#include "systick_delay.h"
#include "serial.h"
#include "util_macros.h"

static Motor_t motor_x = {
    MOTORS_XY_LATCH_ADDRESS,
    MOTOR_STEPX,
    SWITCH_X_MASK,
    0
};

static Motor_t motor_y = {
    MOTORS_XY_LATCH_ADDRESS,
    MOTOR_STEPY,
    SWITCH_Y_MASK,
    0
};

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

int move(Motor_t *motor, unsigned int steps, uint8_t direction) {
    while (steps > 0) {
        if (direction == 1) {
            motor->step = motor->step < 4 ? motor->step + 1 : 0;
        } else {
            motor->step = motor->step > 0 ? motor->step - 1 : 4;
        }

        // serial_printf("step: %d ", motor->step);
        i2c_send_data(motor->address, motor->steps + motor->step, 1);
        // serial_printf("addr: %X, steps: %X\r\n", motor->address, motor->steps + motor->step);

        if (check_switch(motor->mask) == 1) {
            return -1;
        }

        steps--;
        systick_delay_blocking(50);
    }

    return 0;
}

int movex(int steps) {
    uint8_t direction = 1;
    if (steps < 0) {
        direction = 0;
    }

    return move(&motor_x, ABS(steps), direction);
}

int movey(int steps) {
    uint8_t direction = 1;
    if (steps < 0) {
        direction = 0;
    }

    return move(&motor_y, ABS(steps), direction);
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