#include <string.h>

#include "libs/i2c.h"
#include "libs/serial.h"
#include "libs/pinsel.h"

void I2C_SweepBus() {
    pinsel_enable(PINSEL_PORT_0, PINSEL_PORT_0, PINSEL_FUNC_3);
    pinsel_enable(PINSEL_PORT_0, PINSEL_PORT_1, PINSEL_FUNC_3);
    unsigned char buf;
    I2C_M_SETUP_Type t;
    int j = 0;

    for (int i = 0; i < 128; i++) {
        t.sl_addr7bit = i & 0b1111111;
        t.tx_data = &buf;
        t.tx_length = sizeof(buf);
        t.retransmissions_max = 2;
        t.rx_length = 0;
        t.rx_data = NULL;

        if (I2C_MasterTransferData(LPC_I2C1, &t, I2C_TRANSFER_POLLING) == SUCCESS) {
            serial_printf("There is a device on %i", i);
            j++;
        }
    }

    serial_printf("There are %i devices connected to the I2C bus.\r\n", j);
}

int main(int argc, char* argv[]) {
    serial_printf("reee");
    i2c1_init();
    serial_printf("eeeeer");
    I2C_SweepBus();
}
