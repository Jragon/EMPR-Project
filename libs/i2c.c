#include <lpc17xx_i2c.h>
#include <lpc17xx_pinsel.h>
#include <lpc_types.h>

#include "i2c.h"

void i2c_init() {
    // only initilize once
    static Bool initilized = FALSE;

    if (initilized == TRUE) {
        return;
    }

    PINSEL_CFG_Type PinCfg;
    // initial i2c pins, on P0.0/P0.1
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Funcnum = 3;
    PinCfg.Portnum = 0;
    PinCfg.Pinnum = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 1;
    PINSEL_ConfigPin(&PinCfg);
    I2C_Init(I2C1DEV, 100000);
    I2C_Cmd(I2C1DEV, ENABLE);
    initilized = TRUE;
}

void i2c_send_data(uint8_t address, uint8_t* data, uint32_t length) {
    I2C_M_SETUP_Type cfg;
    cfg.sl_addr7bit = address;
    cfg.rx_data = NULL;
    cfg.tx_data = data;
    cfg.tx_length = length;
    cfg.retransmissions_max = 2;
    I2C_MasterTransferData(I2C1DEV, &cfg, I2C_TRANSFER_POLLING);
}

void i2c_recieve_data(uint8_t address, uint8_t* data, uint32_t length) {
    I2C_M_SETUP_Type cfg;
    cfg.sl_addr7bit = address;
    cfg.tx_data = NULL;
    cfg.rx_data = data;
    cfg.rx_length = length;
    cfg.retransmissions_max = 2;
    I2C_MasterTransferData(I2C1DEV, &cfg, I2C_TRANSFER_POLLING);
}

void i2c_duplex(uint8_t address, uint8_t* tx, uint32_t tx_len, uint8_t* rx, uint32_t rx_len) {
    I2C_M_SETUP_Type cfg;
    cfg.sl_addr7bit = address;
    cfg.tx_data = tx;
    cfg.tx_length = tx_len;
    cfg.rx_data = rx;
    cfg.rx_length = rx_len;
    cfg.retransmissions_max = 2;
    I2C_MasterTransferData(I2C1DEV, &cfg, I2C_TRANSFER_POLLING);
}