#ifndef SPI_H
#define SPI_H
#include "stm32f10x.h"

void Dri_SPI_init(void);
void Dri_SPI_Start(void);
void Dri_SPI_Stop(void);
uint8_t Dri_SPI_SwapByte(uint8_t byte);

#endif