#include "m24c02.h"

#define DEV ADDR
#define Delay_ms Dri_Delay_Ms

void Int_M24C02_Init(void){
    Dri_I2C_Init();
}

void Int_M24C02_WriteByte(uint8_t addr , uint8_t byte ){
    Dri_I2C_Start();
    Dri_I2C_SendDev(DEV);
    Dri_I2C_SendByte(addr);
    Dri_I2C_SendByte(byte);
    Dri_I2C_Stop();
    // 不要忘记写周期 5ms
    Delay_ms(5);
}

uint8_t Int_M24C02_ReadByte(uint8_t addr){
    Dri_I2C_Start();
    Dri_I2C_SendDev(DEV);
    Dri_I2C_SendByte(addr);
    Dri_I2C_Start();
    Dri_I2C_SendDev(DEV+1);
    uint8_t byte = Dri_I2C_ReceiveByte();
    // 当前I2C采用的是硬件实现。SendAck方法实现是：将CR1里面的ack置位
    Dri_I2C_SendAck(NACK);
    // 当前I2C采用的是硬件实现。stop方法实现是：将CR1里面的stop置位
    Dri_I2C_Stop();
    return byte;
}

/**
 * @brief 从指定的字地址开始写入指定长度的字节
 *
 * @param addr 字地址
 * @param bytes 待写入的字节数组
 * @param len 待写入的字节数组的长度
 */
void Int_M24C02_WriteBytes(uint8_t addr , uint8_t *bytes , uint8_t len){
    Dri_I2C_Start();
    Dri_I2C_SendDev(DEV);
    Dri_I2C_SendByte(addr);
    for(uint8_t i = 0; i < len; i++){
        Dri_I2C_SendByte(bytes[i]);
    }
    Dri_I2C_Stop();

    Delay_ms(5);
}

/**
 * @brief 从指定的字地址开始读取指定长度的字节
 *
 * @param addr 字地址
 * @param bytes 待读取的字节存入这个数组
 * @param len 待读取的字节长度
 */
void Int_M24C02_ReadBytes(uint8_t addr , uint8_t *bytes , uint8_t len){
    Dri_I2C_Start();
    Dri_I2C_SendDev(DEV);
    Dri_I2C_SendByte(addr);
    Dri_I2C_Start();
    Dri_I2C_SendDev(DEV+1);
    for(uint8_t i = 0; i < len; i++){
        if(i == (len-1)){
            Dri_I2C_SendAck(NACK);
            Dri_I2C_Stop();
        }else{
            Dri_I2C_SendAck(ACK);
        }

        bytes[i] = Dri_I2C_ReceiveByte();

    }
}
