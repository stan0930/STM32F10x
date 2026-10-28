#include "I2C.h"

//初始化
void I2C_Init(void)
{
    //1.配时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    //2.GPIO工作模式配置：通用开漏输出 CNF-01,MODE-11
    GPIOB->CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_MODE11);
    GPIOB->CRH &= ~(GPIO_CRH_CNF10_1 | GPIO_CRH_CNF11_1);
    GPIOB->CRH |= (GPIO_CRH_CNF10_0 | GPIO_CRH_CNF11_0);
    SDA_HIGH;
    SCL_HIGH;





}

//发出起始信号
void I2C_Start(void)
{
    /* Change SDA while SCL is low before a repeated START. */
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;
    // 1.SCL拉高，SDA拉高
    SCL_HIGH;
    SDA_HIGH;
    I2C_DELAY;//延时10us，确保SDA和SCL都拉高

    //2.SCL保持不变，SDA拉低
    SDA_LOW;
    I2C_DELAY;//延时10us，确保SDA拉低



}

//发出停止信号
void I2C_Stop(void)
{
    /* Establish SDA low before raising SCL for STOP. */
    SCL_LOW;
    SDA_LOW;
    I2C_DELAY;
    // 1.SCL拉高，SDA拉低
    SCL_HIGH;
    SDA_LOW;
    I2C_DELAY;//延时10us，确保SDA拉低

    //2.SCL保持不变，SDA拉高
    SDA_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高
}

//主机发出应答信号
void I2C_Ack(void)
{
    //1.SCL初始的时候先拉低
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高

    //2.SCL保持不变，SDA拉低,输出应答 0是应答 1是非应答
    SDA_LOW;
    I2C_DELAY;//延时10us，确保SDA拉低

    //3.SDA保持不变，SCL拉高，开始数据线上信号采样
    SCL_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高

    //4.SDA保持不变，SCL拉低，开始数据线上信号采样
    SCL_LOW;
    I2C_DELAY;//延时10us，确保SDA拉低

    //5.SDA拉高，释放数据总线
    SDA_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高



}

//主机发出非应答信号
void I2C_Nack(void)
{
    //1.SCL初始的时候先拉低
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高

    //2.SDA保持不变，SCL拉高，输出应答 0是应答 1是非应答
    SCL_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高

    //3.SDA保持不变，SCL拉低，开始数据线上信号采样
    SCL_LOW;
    I2C_DELAY;//延时10us，确保SDA拉低

}

//主机等待从设备发来应答信号
uint16_t I2C_Wait4Ack(void)
{
    //1.SDA拉高，SCL拉低，释放数据总线
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高

    //2.SCL拉高，开始数据采样
    SCL_HIGH;
    I2C_DELAY;//延时10us，确保SDA拉高

    //3.读取SDA数据线上的电平
    uint16_t ack = READ_SDA;

    //4.SCL拉低，结束数据采样
    SCL_LOW;
    I2C_DELAY;//延时10us，确保SDA拉低

    return ack?NACK:ACK;// ack是0是应答，>1是非应答

}

//主机发送一个字节
void I2C_SendByte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        //1.SCL,SDA都拉低，等待数据翻转
        SCL_LOW;
        SDA_LOW;
        I2C_DELAY;//延时10us，确保SDA和SCL都拉低

        //2.取字节的最高位
        if(byte & 0x80)
        {
            SDA_HIGH;
        }
        else
        {
            SDA_LOW;
        }
        I2C_DELAY;//延时10us，确保SDA和SCL都拉高

        //3.SCL拉高，开始数据采样
        SCL_HIGH;
        I2C_DELAY;//延时10us，确保SDA和SCL都拉高

        //4.SDA保持不变，SCL拉低，开始数据线上信号采样
        SCL_LOW;
        I2C_DELAY;//延时10us，确保SDA和SCL都拉低

        //5.将SDA的电平左移一位
        byte <<= 1;

    }
    
}

//主机从EEPROM接收一个字节
uint8_t I2C_ReadByte(void)
{
    /* Release the open-drain SDA output so the EEPROM can drive it. */
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;
    //定义一个变量，用来保存接收的数据
    uint8_t data = 0;

    //循环处理每一位
    for (uint8_t i = 0; i < 8; i++)
    {
        //1.SCL拉低，等待数据翻转
        SCL_LOW;
        I2C_DELAY;//延时10us，确保SDA和SCL都拉低

        //2.SCL拉高开始采样
        SCL_HIGH;
        I2C_DELAY;//延时10us，确保SDA和SCL都拉高

        //3.数据采样，读取SDA
        data <<=1;     //先做左移，新存入的位永远是最低位    
        if(READ_SDA)
        {
            data |= 0x01; //先存入最低位，然后每次左移1位
        }

        //4.SCL拉低，结束数据采样
        SCL_LOW;
        I2C_DELAY;//延时10us，确保SDA和SCL都拉低


    }

    return data;
    
}
