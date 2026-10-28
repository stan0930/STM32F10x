
#include "m24c02.h"


//初始化
void M24C02_Init(void)
{
    I2C_Init();
}

//向EEPROM写入一个字节
void M24C02_WriteByte(uint8_t innerAddr, uint8_t byte)
{
    //1.发送起始信号
    I2C_Start();
    //2.发送写地址
    I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    uint16_t ack = I2C_Wait4Ack();
    if(ack == ACK)
    {
        //4.发送内部地址
        I2C_SendByte(innerAddr);

        //5.等待EEPROM应答
        I2C_Wait4Ack();

        //6.发送数据
        I2C_SendByte(byte);
        //7.等待EEPROM应答
        I2C_Wait4Ack();
        //8.发送停止信号
        I2C_Stop();
    }

    //延迟等待写入周期结束
    Delay_ms(5);
}

//从EEPROM读取一个字节
uint8_t M24C02_ReadByte(uint8_t innerAddr)
{
    //1.发送起始信号
    I2C_Start();
    //2.发送假写地址
    I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    I2C_Wait4Ack();
    //4.发送内部地址
    I2C_SendByte(innerAddr);
    //5.等待应答
    I2C_Wait4Ack();
    //6.发出开始信号
    I2C_Start();
    //7.发送真读地址
    I2C_SendByte(R_ADDR);
    //8.等待应答
    I2C_Wait4Ack();
    //9.接收数据
    uint8_t byte = I2C_ReadByte();
    //10.发送非应答信号
    I2C_Nack();
    //11.发送停止信号
    I2C_Stop();
    return byte;

}

//连续写入多个字节（页写）
void M24C02_WriteBytes(uint8_t innerAddr, uint8_t *bytes, uint8_t size)
{
    //1.发送起始信号
    I2C_Start();
    //2.发送写地址
    I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    uint16_t ack = I2C_Wait4Ack();
    if(ack == ACK)
    {
        //4.发送内部地址
        I2C_SendByte(innerAddr);

        //5.等待EEPROM应答
        I2C_Wait4Ack();

        for(uint8_t i = 0; i < size; i++)
        {
            I2C_SendByte(bytes[i]);
            I2C_Wait4Ack();
        }
        //8.发送停止信号
        I2C_Stop();
    }

    //延迟等待写入周期结束
    Delay_ms(5);
}

//连续读取多个字节（页读）
uint8_t M24C02_ReadBytes(uint8_t innerAddr, uint8_t *buffer, uint8_t size)
{
    if (size == 0)
    {
        return 0;
    }

    //1.发送起始信号
    I2C_Start();
    //2.发送假写地址
    I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    I2C_Wait4Ack();
    //4.发送内部地址
    I2C_SendByte(innerAddr);
    //5.等待应答
    I2C_Wait4Ack();
    //6.发出开始信号
    I2C_Start();
    //7.发送真读地址
    I2C_SendByte(R_ADDR);
    //8.等待应答
    I2C_Wait4Ack();
    //利用循环连续接收取数据
    for(uint8_t i = 0; i < size; i++)
    {
        buffer[i] = I2C_ReadByte();
        if(i == size - 1)
        {
            I2C_Nack();
        }
        else
        {
            I2C_Ack();
        }
    }
    //11.发送停止信号
    I2C_Stop();
    return size;
}
