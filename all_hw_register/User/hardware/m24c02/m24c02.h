//M24C02：I2C读写，地址0~255，页写不能跨8字节页，写后等5ms。
#ifndef __INF_M24C02_H
#define __INF_M24C02_H

#include "I2C.h"
#include "string.h"
#define ADDR 0xA0
void Int_M24C02_Init(void);
void Int_M24C02_WriteByte(uint8_t innerAddr, uint8_t byte);
uint8_t Int_M24C02_ReadByte(uint8_t innerAddr);
void Int_M24C02_WriteBytes(uint8_t innerAddr, uint8_t *bytes, uint8_t len);
void Int_M24C02_ReadBytes(uint8_t innerAddr, uint8_t *bytes, uint8_t len);

#endif
