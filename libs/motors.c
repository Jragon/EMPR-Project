#include <lpc_types.h>
#include <lpc17xx_i2c.h>

#include "motors.h"
#include "i2c.h"
#include "systick_delay.h"
#include "serial.h"
#include "util_macros.h"

static Motor_t motor_x = {
    MOTOR_XY_LATCH_ADDRESS,
    MOTOR_STEPX,
    SWITCH_X_MASK,
    0
};

static Motor_t motor_y = {
    MOTOR_XY_LATCH_ADDRESS,
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

void send_move(Motor_t *motor, uint8_t direction) {
    if (direction == 1) {
        motor->step = motor->step < 4 ? motor->step + 1 : 0;
    } else {
        motor->step = motor->step > 0 ? motor->step - 1 : 4;
    }
    i2c_send_data(motor->address, motor->steps + motor->step, 1);
}

int move(Motor_t *motor, int steps) {
    uint8_t direction = 1;
    if (steps < 0) {
        direction = 0;
    }

    steps = ABS(steps);
    uint8_t delay = MOTOR_MAX_DELAY;

    while (steps > 0) {
        send_move(motor, direction);

        // check switch only if it's going towards the switch
        if (direction == 0 && check_switch(motor->mask) == 1) {
            // move off limit switch
            while (check_switch(motor->mask) == 1) {
                send_move(motor, direction ? 0 : 1);

                systick_delay_blocking(MOTOR_MAX_DELAY);
            }

            return -1;
        }

        steps--;
        systick_delay_blocking(delay);

        if (delay > MOTOR_MIN_DELAY) {
            delay = delay <= MOTOR_RAMP ? MOTOR_MIN_DELAY : delay - MOTOR_RAMP;
        }
    }

    return steps;
}

void movexy_together(int xsteps, int ysteps) {
    uint8_t direction_x = xsteps < 0 ? 0 : 1;
    uint8_t direction_y = ysteps < 0 ? 0 : 1;
    xsteps = ABS(xsteps);
    ysteps = ABS(ysteps);

    uint8_t delay = MOTOR_MAX_DELAY;

    while (xsteps > 0 || ysteps > 0) {
        if (xsteps > 0) {
            if (direction_x == 1) {
                motor_x.step = motor_x.step < 4 ? motor_x.step + 1 : 0;
            } else {
                motor_x.step = motor_x.step > 0 ? motor_x.step - 1 : 4;
            }

            xsteps--;
        }

        if (ysteps > 0) {
            if (direction_y == 1) {
                motor_y.step = motor_y.step < 4 ? motor_y.step + 1 : 0;
            } else {
                motor_y.step = motor_y.step > 0 ? motor_y.step - 1 : 4;
            }

            ysteps--;
        }

        uint8_t data = motor_x.steps[motor_x.step] | motor_y.steps[motor_y.step];
        i2c_send_data(motor_x.address, &data, 1);


        systick_delay_blocking(delay);

        if (delay > MOTOR_MIN_DELAY) {
            delay = delay <= MOTOR_RAMP ? MOTOR_MIN_DELAY : delay - MOTOR_RAMP;
        }
    }
}

int movex(int steps) {
    return move(&motor_x, steps);
}

int movey(int steps) {
    return move(&motor_y, steps);
}

int home(Motor_t *motor) {
    return 1000 - move(motor, -1000);
}

int home_x() {
    return home(&motor_x);
}

int home_y() {
    return home(&motor_y);
}