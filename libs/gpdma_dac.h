#pragma once

#include <lpc17xx_gpdma.h>

#include "dac.h"

#define DAC_CLK_MHZ 25000000

/**
    @brief configs the GPDMA transfer to DAC
    for some reason you need to declare the config structs in main :shrug:

    @param source data source to transfer
    @param size size of data
    @param ch DMA channel to use
    @param LLI pointer to LLI cfg
    @param dma_cfg pointer to DMA cfg
*/
void gpdma_config(uint32_t* source, uint16_t size, uint8_t ch, GPDMA_LLI_Type* LLI,
                  GPDMA_Channel_CFG_Type* dma_cfg);

/**
    @brief configures the DAC to use the DMA and transfer at given freq

    @param size size of data
    @param transfer_freq how many bytes to transfer per second (configs the DAC counter)
*/
void gpdma_dac_config(uint16_t size, uint32_t transfer_freq);

/**
    @brief starts the transfer

    @param ch dma channel
*/
void gpdma_dac_start(uint8_t ch);

/**
    @brief stops the transfer

    @param ch dma channel
*/
void gpdma_dac_stop(uint8_t ch);

/**
    @brief configures the DMA counter

    @param size size of data
    @param transfer_freq
*/
void gpdma_dac_config_timeout(uint16_t size, uint32_t transfer_freq);
