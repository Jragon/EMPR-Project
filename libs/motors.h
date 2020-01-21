#pragma once

#define MOTOR_DELAY 50

#define MOTORS_XY_LATCH_ADDRESS 0x3E

#define MOTOR_STEPX_1 0x09
#define MOTOR_STEPX_2 0x05
#define MOTOR_STEPX_3 0x06
#define MOTOR_STEPX_4 0x0A
#define MOTOR_STEPX         {MOTOR_STEPX_1, MOTOR_STEPX_2, MOTOR_STEPX_3, MOTOR_STEPX_4}
#define MOTOR_STEPX_REVERSE {MOTOR_STEPX_4, MOTOR_STEPX_3, MOTOR_STEPX_2, MOTOR_STEPX_1}

#define MOTOR_STEPY_1 0x90
#define MOTOR_STEPY_2 0x50
#define MOTOR_STEPY_3 0x60
#define MOTOR_STEPY_4 0xA0
#define MOTOR_STEPY         {MOTOR_STEPY_1, MOTOR_STEPY_2, MOTOR_STEPY_3, MOTOR_STEPY_4}
#define MOTOR_STEPY_REVERSE {MOTOR_STEPY_4, MOTOR_STEPY_3, MOTOR_STEPY_2, MOTOR_STEPY_1}


#define SWITCH_ADDRESS 0x3C
#define SWITCH_X_MASK 0x02
#define SWITCH_Y_MASK 0x01
#define SWITCH_Z_MASK 0x03

typedef struct Motor_t {
    uint8_t address;
    uint8_t steps[4];
    uint8_t mask;
    uint8_t step;
} Motor_t;

void setup_switches();
int check_switch(uint8_t mask);
int home(uint8_t *steps, uint8_t mask);
int home_x();
int home_y();
int move(Motor_t *motor, unsigned int steps, uint8_t direction);
int movex(int steps);
int movey(int steps);
