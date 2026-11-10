//SPI：保留原硬件和软件实现，用初始化函数选择，只有一份spi.h。
#ifndef SPI_H
#define SPI_H
#include "stm32f10x.h"

//PA5/PA6/PA7，片选PC13，两种初始化选一个
void Dri_SPI_init(void);        //原硬件SPI
void Dri_SPI_Soft_Init(void);   //原软件SPI
void Dri_SPI_Start(void);
void Dri_SPI_Stop(void);
//发一个字节，同时返回收到的一个字节
uint8_t Dri_SPI_SwapByte(uint8_t byte);

#endif
