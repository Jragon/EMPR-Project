#include <lpc17xx_dac.h>
#include <lpc17xx_gpdma.h>

#include "gpdma_dac.h"

void gpdma_config(uint32_t* source, uint16_t size, uint8_t ch, GPDMA_LLI_Type* LLI,
                  GPDMA_Channel_CFG_Type* dma_cfg) {
    LLI->SrcAddr = (uint32_t)source;
    LLI->DstAddr = (uint32_t) & (LPC_DAC->DACR);  // copy to dacr
    LLI->NextLLI = (uint32_t)LLI;  // point to itself so it transfers infinitely
    LLI->Control = size            // dma transfer size bits [bits 0-11]
                   | 2 << 18       // source width 32bit
                   | 2 << 21       // dest width 32bit
                   | 1 << 26;      // increment source by one after each transfer
    dma_cfg->ChannelNum = ch;
    dma_cfg->SrcMemAddr = (uint32_t)source;
    dma_cfg->DstMemAddr = (uint32_t) & (LPC_DAC->DACR);
    dma_cfg->DMALLI = (uint32_t)LLI;
    dma_cfg->TransferSize = size;
    dma_cfg->TransferType = GPDMA_TRANSFERTYPE_M2P;  // mem to peripheral
    dma_cfg->SrcConn = 0;
    dma_cfg->DstConn = GPDMA_CONN_DAC;  // use the dac timer
    GPDMA_Init();
    GPDMA_Setup(dma_cfg);
}

void gpdma_dac_config(uint16_t size, uint32_t transfer_freq) {
    DAC_CONVERTER_CFG_Type dac_cfg;
    dac_cfg.CNT_ENA = SET;
    dac_cfg.DMA_ENA = SET;
    dac_init();
    gpdma_dac_config_timeout(size, transfer_freq);
    DAC_ConfigDAConverterControl(LPC_DAC, &dac_cfg);
}

void gpdma_dac_config_timeout(uint16_t size, uint32_t transfer_freq) {
    DAC_SetDMATimeOut(LPC_DAC, DAC_CLK_MHZ / (transfer_freq * size));
}

void gpdma_dac_start(uint8_t ch) {
    GPDMA_ChannelCmd(ch, ENABLE);
}

void gpdma_dac_stop(uint8_t ch) {
    GPDMA_ChannelCmd(ch, DISABLE);
}
