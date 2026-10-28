#ifndef I2C_H
#define I2C_H

#include "stm32f10x.h"
#include "delay.h"

#define OK  1
#define FAIL 0
#define ACK 0
#define NACK 1


//初始化
void Dri_I2C_Init(void);

//发出起始信号
uint8_t Dri_I2C_Start(void);

//发出停止信号
void Dri_I2C_Stop(void);

//主机发送 ACK(0) 或 NACK(1)
void Dri_I2C_SendAck(uint8_t ack);

//主机等待从设备发来应答信号
uint16_t Dri_I2C_ReceiveAck(void);

//主机发送一个字节
uint8_t Dri_I2C_SendByte(uint8_t byte);

//主机发送设备地址+读写表示
uint8_t Dri_I2C_SendDev(uint8_t dev);

//主机从EEPROM接收一个字节
uint8_t Dri_I2C_ReceiveByte(void);


#endif

