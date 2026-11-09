#ifndef DMA_H
#define DMA_H
#include "stm32f10x.h"

void Dri_DMA1_Trans_Init(void);
void Dri_DMA1_Rec_Init(void);
void Dri_DMA1_TransmitData(uint32_t scrAddr, uint32_t desAddr,uint16_t dataLen);
void Dri_DMA1_ReceiveData(uint32_t scrAddr, uint32_t desAddr,uint16_t dataLen);
#endif