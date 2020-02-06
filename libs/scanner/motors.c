#include "motors.h"

#include <lpc_types.h>

#include <lpc17xx_i2c.h>
#include <lpc17xx_timer.h>

#include "../i2c.h"
#include "../serial.h"
#include "../systick_delay.h"
#include "../timer.h"
#include "../util_macros.h"

static Motor_t motor_x = {MOTOR_XY_LATCH_ADDRESS, MOTOR_STEPX, SWITCH_X_MASK, 0, 1};
static Motor_t motor_y = {MOTOR_XY_LATCH_ADDRESS, MOTOR_STEPY, SWITCH_Y_MASK, 0, 1};
static Motor_t motor_z = {MOTOR_ZPEN_LATCH_ADDRESS, MOTOR_STEPZ, SWITCH_Z_MASK, 0, 10};

#define MIN_TICK 1500

volatile Motors_t motors = {
  .m_x = &motor_x,
  .m_y = &motor_y,
  .m_z = &motor_z,
  .tick_size = MIN_TICK,  // ~1.5ms
};

void motor_init() {
    timer_init();

    // set up switches
    uint8_t data = 0xFF;
    i2c_send_data(SWITCH_ADDRESS, &data, 1);

    TIM_TIMERCFG_Type cfg;
    cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    cfg.PrescaleValue = 1;
    TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &cfg);

    // mcr what to do when mr0 fires
    LPC_TIM1->MCR = 0b011;  // MR0: !stop, reset, interrupt
    LPC_TIM1->MR0 = motors.tick_size;

    // disable capture and external match
    LPC_TIM1->CCR = 0;
    LPC_TIM1->EMR = 0;
    NVIC_EnableIRQ(TIMER1_IRQn);  // Enable Stepper Driver Interrupt
}

void motor_set_tick(uint32_t tick) {
    motors.tick_size = tick;
    LPC_TIM1->MR0 = tick;
}

void motor_reset_tick() {
    motor_set_tick(MIN_TICK);
}

uint8_t motor_running() {
    return LPC_TIM1->TCR == 0b01 &&
           (motors.x_steps != 0 || motors.y_steps != 0 || motors.z_steps != 0);
}

void motor_wake() {
    // reset the timer
    LPC_TIM1->TCR = 0b10;
    // enable timer
    LPC_TIM1->TCR = 0b01;
}

void motor_sleep() {
    LPC_TIM1->TCR = 0;

    // uint8_t off = 0;
    // i2c_send_data(MOTOR_XY_LATCH_ADDRESS, &off, 1);
    // i2c_send_data(MOTOR_ZPEN_LATCH_ADDRESS, &off, 1);
}

uint8_t motor_get_move(Motor_t *motor, uint8_t direction) {
    if (direction == 1) {
        motor->step = motor->step > 0 ? motor->step - 1 : 3;
    } else {
        motor->step = motor->step < 3 ? motor->step + 1 : 0;
    }

    return motor->steps[motor->step];
}

void motor_set(int x_steps, int y_steps, int z_steps) {
    motors.x_steps = ABS(x_steps);
    motors.y_steps = ABS(y_steps);
    motors.z_steps = ABS(z_steps);

    motors.x_dir = x_steps < 0 ? 1 : 0;
    motors.y_dir = y_steps < 0 ? 1 : 0;
    motors.z_dir = z_steps < 0 ? 1 : 0;
}

void motor_move_blocking(int x_steps, int y_steps, int z_steps) {
    motor_set(x_steps, y_steps, z_steps);
    motor_wake();
    while (motor_running())
        ;
}

void motor_goto_lims() {
    motor_set(-1500, -2000, -10000);
    motor_wake();
    while (motors.lims != XYZ_LIM && motor_running())
        ;
}

LimitSwitches_t motor_get_lims() {
    return motors.lims;
}

void TIMER1_IRQHandler() {
    // uint32_t start = timer_get();

    LPC_TIM1->IR = LPC_TIM1->IR;

    uint8_t sw;
    i2c_recieve_data(SWITCH_ADDRESS, &sw, 1);
    motors.lims = (uint8_t)~sw;

    uint8_t x = 0;
    if (motors.x_steps > 0) {
        if (motors.x_dir == 1 && motors.lims & X_LIM) {
            motors.x_steps = 0;
        } else {
            x = motor_get_move(motors.m_x, motors.x_dir);
            motors.x_steps -= 1;
        }
    }

    uint8_t y = 0;
    if (motors.y_steps > 0) {
        if (motors.y_dir == 1 && motors.lims & Y_LIM) {
            motors.y_steps = 0;
        } else {
            y = motor_get_move(motors.m_y, motors.y_dir);
            motors.y_steps -= 1;
        }
    }

    uint8_t xy = x | y;

    uint8_t z = 0;
    if (motors.z_steps > 0) {
        if (motors.z_dir == 1 && motors.lims & Z_LIM) {
            motors.z_steps = 0;
        } else {
            z = motor_get_move(motors.m_z, motors.z_dir);
            i2c_send_data(MOTOR_ZPEN_LATCH_ADDRESS, &z, 1);
            motors.z_steps -= 1;
        }
    }

    if (xy != 0) {
        i2c_send_data(MOTOR_XY_LATCH_ADDRESS, &xy, 1);
    } else if (z == 0) {
        // ie no steps left running
        motor_sleep();
    }

    // serial_printf("time: %d\r\n", timer_get() - start);
}

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
        motor->step = motor->step < 3 ? motor->step + 1 : 0;
    } else {
        motor->step = motor->step > 0 ? motor->step - 1 : 3;
    }
    i2c_send_data(motor->address, motor->steps + motor->step, 1);
}

int move(Motor_t *motor, int steps) {
    uint8_t direction = 1;
    if (steps < 0) {
        direction = 0;
    }

    steps = ABS(steps) * motor->step_scaler;
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

    // turn off motors
    uint8_t off = 0x00;
    i2c_send_data(motor->address, &off, 1);

    return steps;
}

void movexy_together(int xsteps, int ysteps) {
    uint8_t direction_x = xsteps < 0 ? 0 : 1;
    uint8_t direction_y = ysteps < 0 ? 0 : 1;
    xsteps = ABS(xsteps);
    ysteps = ABS(ysteps);

    uint8_t delay = MOTOR_MAX_DELAY;

    while (xsteps > 0 || ysteps > 0) {
        /*
         *serial_printf("xsteps %d, ysteps %d, motorx_step %d, motory_step %d\r\n",
         *xsteps, ysteps, motor_x.step, motor_y.step);
         */

        uint8_t x = 0;
        if (xsteps > 0) {
            if (direction_x == 1) {
                motor_x.step = motor_x.step < 3 ? motor_x.step + 1 : 0;
            } else {
                motor_x.step = motor_x.step > 0 ? motor_x.step - 1 : 3;
            }

            xsteps--;
            x = motor_x.steps[motor_x.step];
        }

        uint8_t y = 0;
        if (ysteps > 0) {
            if (direction_y == 1) {
                motor_y.step = motor_y.step < 3 ? motor_y.step + 1 : 0;
            } else {
                motor_y.step = motor_y.step > 0 ? motor_y.step - 1 : 3;
            }

            ysteps--;
            y = motor_y.steps[motor_y.step];
        }

        uint8_t data = x | y;
        // serial_printf("xstep %d, ystep %d, data: 0x%x \r\n", motor_x.step,
        // motor_y.step, data);
        i2c_send_data(motor_x.address, &data, 1);

        systick_delay_blocking(delay);

        if (delay > MOTOR_MIN_DELAY) {
            delay = delay <= MOTOR_RAMP ? MOTOR_MIN_DELAY : delay - MOTOR_RAMP;
        }
        // serial_printf("delay: %2d; x_steps: %3d; y_steps: %3d\r\n", delay, xsteps,
        // ysteps);
    }

    uint8_t off = 0x00;
    i2c_send_data(motor_x.address, &off, 1);
}

int movex(int steps) {
    return move(&motor_x, steps);
}

int movey(int steps) {
    return move(&motor_y, steps);
}

int movez(int steps) {
    return move(&motor_z, steps);
}

int home(Motor_t *motor) {
    return 10000 - move(motor, -10000);
}

int home_x() {
    return home(&motor_x);
}

int home_y() {
    return home(&motor_y);
}

int home_z() {
    return home(&motor_z);
}
