#ifndef ADC1_H
#define ADC1_H

#include "stm32f10x.h"

void Dri_ADC1_Init(void);
void Dri_ADC1_StartConvert(uint32_t desAddr, uint8_t len);

#endif