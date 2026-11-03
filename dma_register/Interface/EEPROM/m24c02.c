
#include "m24c02.h"


//初始化
void Int_M24C02_Init(void)
{
    Dri_I2C_Init();
}

//向EEPROM写入一个字节
void Int_M24C02_WriteByte(uint8_t innerAddr, uint8_t byte)
{
    //1.发送起始信号
    Dri_I2C_Start();
    //2.发送写地址
    Dri_I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    uint16_t ack = Dri_I2C_ReceiveAck();
    if(ack == ACK)
    {
        //4.发送内部地址
        Dri_I2C_SendByte(innerAddr);

        //5.等待EEPROM应答
        Dri_I2C_ReceiveAck();

        //6.发送数据
        Dri_I2C_SendByte(byte);
        //7.等待EEPROM应答
        Dri_I2C_ReceiveAck();
        //8.发送停止信号
        Dri_I2C_Stop();
    }

    //延迟等待写入周期结束
    Dri_Delay_Ms(5);
}

//从EEPROM读取一个字节
uint8_t Int_M24C02_ReadByte(uint8_t innerAddr)
{
    //1.发送起始信号
    Dri_I2C_Start();
    //2.发送假写地址
    Dri_I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    Dri_I2C_ReceiveAck();
    //4.发送内部地址
    Dri_I2C_SendByte(innerAddr);
    //5.等待应答
    Dri_I2C_ReceiveAck();
    //6.发出开始信号
    Dri_I2C_Start();
    //7.发送真读地址
    Dri_I2C_SendByte(R_ADDR);
    //8.等待应答
    Dri_I2C_ReceiveAck();
    //9.接收数据
    uint8_t byte = Dri_I2C_ReceiveByte();
    //10.发送非应答信号
    Dri_I2C_SendAck(NACK);
    //11.发送停止信号
    Dri_I2C_Stop();
    return byte;

}

//连续写入多个字节（页写）
void Int_M24C02_WriteBytes(uint8_t innerAddr, uint8_t *bytes, uint8_t size)
{
    //1.发送起始信号
    Dri_I2C_Start();
    //2.发送写地址
    Dri_I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    uint16_t ack = Dri_I2C_ReceiveAck();
    if(ack == ACK)
    {
        //4.发送内部地址
        Dri_I2C_SendByte(innerAddr);

        //5.等待EEPROM应答
        Dri_I2C_ReceiveAck();

        for(uint8_t i = 0; i < size; i++)
        {
            Dri_I2C_SendByte(bytes[i]);
            Dri_I2C_ReceiveAck();
        }
        //8.发送停止信号
        Dri_I2C_Stop();
    }

    //延迟等待写入周期结束
    Dri_Delay_Ms(5);
}

//连续读取多个字节（页读）
uint8_t Int_M24C02_ReadBytes(uint8_t innerAddr, uint8_t *buffer, uint8_t size)
{
    if (size == 0)
    {
        return 0;
    }

    //1.发送起始信号
    Dri_I2C_Start();
    //2.发送假写地址
    Dri_I2C_SendByte(W_ADDR);
    //3.等待EEPROM应答
    Dri_I2C_ReceiveAck();
    //4.发送内部地址
    Dri_I2C_SendByte(innerAddr);
    //5.等待应答
    Dri_I2C_ReceiveAck();
    //6.发出开始信号
    Dri_I2C_Start();
    //7.发送真读地址
    Dri_I2C_SendByte(R_ADDR);
    //8.等待应答
    Dri_I2C_ReceiveAck();
    //利用循环连续接收取数据
    for(uint8_t i = 0; i < size; i++)
    {
        buffer[i] = Dri_I2C_ReceiveByte();
        if(i == size - 1)
        {
            Dri_I2C_SendAck(NACK);
        }
        else
        {
            Dri_I2C_SendAck(ACK);
        }
    }
    //11.发送停止信号
    Dri_I2C_Stop();
    return size;
}
