#ifndef M24C02_H
#define M24C02_H

#include "I2C.h"

//宏定义
#define W_ADDR 0xA0
#define R_ADDR 0xA1

//初始化
void Int_M24C02_Init(void);

//向EEPROM写入一个字节
void Int_M24C02_WriteByte(uint8_t innerAddr, uint8_t data);

//从EEPROM读取一个字节
uint8_t Int_M24C02_ReadByte(uint8_t innerAddr);

//连续写入多个字节（页写）
void Int_M24C02_WriteBytes(uint8_t innerAddr, uint8_t *bytes, uint8_t size);

//连续读取多个字节（页读）
uint8_t Int_M24C02_ReadBytes(uint8_t innerAddr, uint8_t *buffer, uint8_t size);

#endif

