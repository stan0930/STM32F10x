#ifndef I2C_H
#define I2C_H

#include "stm32f10x.h"
#include "delay.h"

//宏定义
#define ACK 0
#define NACK 1

//控制SCL，SDA的输出高低电平
#define SCL_HIGH (GPIOB->ODR |= GPIO_ODR_ODR10)
#define SCL_LOW (GPIOB->ODR &= ~GPIO_ODR_ODR10)
#define SDA_HIGH (GPIOB->ODR |= GPIO_ODR_ODR11)
#define SDA_LOW (GPIOB->ODR &= ~GPIO_ODR_ODR11)

//读取操作
#define READ_SDA (GPIOB->IDR & GPIO_IDR_IDR11)

//定义操作的基本延迟
#define I2C_DELAY (Dri_Delay_Us(10))

//初始化
void Dri_I2C_Init(void);

//发出起始信号
void Dri_I2C_Start(void);

//发出停止信号
void Dri_I2C_Stop(void);

//主机发送 ACK(0) 或 NACK(1)
void Dri_I2C_SendAck(uint8_t ack);

//主机等待从设备发来应答信号
uint16_t Dri_I2C_ReceiveAck(void);

//主机发送一个字节
void Dri_I2C_SendByte(uint8_t byte);

//主机从EEPROM接收一个字节
uint8_t Dri_I2C_ReceiveByte(void);


#endif

