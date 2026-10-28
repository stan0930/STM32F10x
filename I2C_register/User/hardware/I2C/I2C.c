#include "I2C.h"


// //宏定义
// #define ACK 0
// #define NACK 1

// //控制SCL，SDA的输出高低电平
// #define SCL_HIGH (GPIOB->ODR |= GPIO_ODR_ODR10)
// #define SCL_LOW (GPIOB->ODR &= ~GPIO_ODR_ODR10)
// #define SDA_HIGH (GPIOB->ODR |= GPIO_ODR_ODR11)
// #define SDA_LOW (GPIOB->ODR &= ~GPIO_ODR_ODR11)

// //读取操作
// #define READ_SDA (GPIOB->IDR & GPIO_IDR_IDR11)

// //定义操作的基本延迟
// #define I2C_DELAY (Dri_Delay_Us(10))


//PB10 - SCL
//PB11 - SDA
//这两个引脚都是复用开漏输出模式 MODE:11, CNF:11


//初始化
void Dri_I2C_Init(void)
{
    //1.配时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    //2.GPIO工作模式配置：复用开漏输出 CNF-01,MODE-11
    GPIOB->CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_MODE11|GPIO_CRH_CNF10|GPIO_CRH_CNF11);
    //3.设定当前硬件用于i2c
    //0 - i2c协议  1 - smbus协议 默认是0
    I2C2->CR1 &= ~ I2C_CR1_SMBUS;

    //4.设置时钟频率 36M
    I2C2->CR2 |= 0x24;

    //5.设置I2C2 在标准模式下工作，默认值0 S:0-100kbit/s F:1-400kbit/s
    I2C2->CCR &=~ I2C_CCR_FS;

    //6.设置时钟控制分频系数：CCR
    //当前是标准模式，因此占空比是50%：速率是100kbit/s
    //传输1bit需要10us，所以可以反推出时钟线一个周期是10us，高电平是5us
    //根据公式计算：CCR = Thigh / Tp1=5us * 36 =180

    I2C2->CCR |= 180;

    //7.设置TRISE时长
    //I2C2在APB1总线上，频率是36M，因此周期是1/36 us
    //高低电平切换需要的时长是1us。因此理论上要36个周期才能完成电平切换，保险起见我们给个37
    I2C2->TRISE |= 37;

    //8.使能i2c
    I2C2->CR1 |= I2C_CR1_PE;



}

//发出起始信号
uint8_t Dri_I2C_Start(void)
{
    //将CR1中的START位设置为1，表示我想发送一个起始位
    I2C2->CR1 |= I2C_CR1_START;

    uint16_t timeout = UINT16_MAX;
    while (((I2C2->SR1 & I2C_SR1_SB)==0)&& timeout){
        timeout--;
    }
    if(timeout == 0){
        return FAIL;
    }
    return OK;
    
}

//发出停止信号
void Dri_I2C_Stop(void)
{
    /* Receiver ADDR stays pending until ACK/NACK has been selected. */
    if (I2C2->SR1 & I2C_SR1_ADDR)
    {
        volatile uint32_t status = I2C2->SR1;
        status = I2C2->SR2;
        (void)status;
    }
    I2C2->CR1 |= I2C_CR1_STOP;
}



//主机发送一个字节
uint8_t Dri_I2C_SendByte(uint8_t byte)
{

    //1.等待发送缓冲区为空
    uint16_t timeout = UINT16_MAX;
    while (((I2C2->SR1 & I2C_SR1_TXE) == 0)&& timeout){
        timeout--;
    }

    if(timeout){
        //2.将待发送的字节赋值给DR
        I2C2->DR = byte;

        //3.等待传输完成
        timeout = UINT16_MAX;
        while(((I2C2->SR1 & I2C_SR1_BTF) == 0)&& timeout){
            timeout--;
        }

        return timeout? OK:FAIL;

    }
    return FAIL;
    
}

uint8_t Dri_I2C_SendDev(uint8_t dev)
{
    //1.将设备地址存入到DR寄存器
    //思考：此处给DR赋值，为什么不用考虑TXE？因为发送地址的上一步是起始信号
    I2C2->DR = dev;

    //2.等待设备地址发送完成
    uint16_t timeout = UINT16_MAX;
    while (((I2C2->SR1 & I2C_SR1_ADDR) == 0)&& timeout){
        timeout--;
    }


    //3.清除发送设备地址标志位：先读SR1 ，再读SR2
    if (timeout)
    {   
        //I2C2->SR1;
        I2C2->SR2;
    }

    return timeout? OK:FAIL;
}

/* SendDev/SendByte already wait for the hardware acknowledgement.
 * For sequential reception, release ADDR after enabling ACK.
 * The single-byte path still selects NACK before Stop releases ADDR.
 */
uint16_t Dri_I2C_ReceiveAck(void)
{
    uint32_t status = I2C2->SR1;
    if (status & I2C_SR1_AF)
    {
        I2C2->SR1 &= ~I2C_SR1_AF;
        return NACK;
    }
    if (status & I2C_SR1_ADDR)
    {
        I2C2->CR1 |= I2C_CR1_ACK;
        (void)I2C2->SR2;
    }
    return ACK;
}

//主机从EEPROM接收一个字节
uint8_t Dri_I2C_ReceiveByte(void)
{
    uint8_t byte = 0;
    uint16_t timeout = UINT16_MAX;
    while (((I2C2->SR1 & I2C_SR1_RXNE) == 0)&& timeout){
        timeout--;
    }
    if(timeout){
        byte = I2C2->DR;
        return byte;
    }
    return 0;
}

//主机发送应答：ACK 为低电平，NACK 为高电平
void Dri_I2C_SendAck(uint8_t ack)
{
    //发送ACK，起始就是将CR1里面的ACK这一位置1或者置0
    //置1表示：ACK ，置0表示：NACK
    if(ack ==0){
        I2C2->CR1 |= I2C_CR1_ACK;
    }else{
        I2C2->CR1 &= ~I2C_CR1_ACK;
    }
}

