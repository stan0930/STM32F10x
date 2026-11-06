#ifndef W25Q32_H
#define W25Q32_H
#include "stm32f10x.h"

void Int_W25Q32_Init(void);
void Int_W25Q32_ReadId(uint8_t *mid, uint16_t *did);
void Int_W25Q32_WriteEnable(void);
void Int_W25Q32_WriteDisanable(void);
void Int_W25Q32_SectorErase(uint8_t block, uint8_t sector);
void Int_W25Q32_WritePage(uint8_t block, uint8_t sector, uint8_t page, 
uint8_t *data, uint16_t len);
void Int_W25Q32_Read(uint8_t block, uint8_t sector, uint8_t page, 
uint8_t *data, uint16_t len);
void Int_W25Q32_WaitNotBusy(void);


#endif