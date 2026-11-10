//SPI：保留原硬件和软件实现，用初始化函数选择，只有一份spi.h。
#include "spi.h"
#include "delay.h"

//补充：0用原硬件SPI，1用原软件SPI，Flash仍调用同一组函数
static uint8_t spi_soft_mode = 0;
static uint8_t SPI_Soft_SwapByte(uint8_t byte);

//SPI1：SCK PA5，MISO PA6，MOSI PA7，CS PC13
#define CS_HIGH (GPIOC->ODR |= GPIO_ODR_ODR13)
#define CS_LOW  (GPIOC->ODR &= ~GPIO_ODR_ODR13)

void Dri_SPI_init(void){
    spi_soft_mode = 0;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |
                    RCC_APB2ENR_IOPCEN |
                    RCC_APB2ENR_SPI1EN;

    //PA5和PA7：复用推挽输出，50MHz速度等级
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |=  (GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1);

    //PA6：浮空输入
    GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
    GPIOA->CRL |= GPIO_CRL_CNF6_0;

    //PC13：普通推挽输出，2MHz，默认高电平不选中
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    CS_HIGH;

    //SPI模式0，8位，主机，软件管理NSS，最高位先发
    SPI1->CR1 = SPI_CR1_SSM |
                SPI_CR1_SSI |
                SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_BR_0;       //PCLK2四分频，18MHz
    SPI1->CR1 &= ~(SPI_CR1_CPOL |
                   SPI_CR1_CPHA |
                   SPI_CR1_LSBFIRST |
                   SPI_CR1_DFF |
                   SPI_CR1_BIDIMODE);
    SPI1->CR2 &= ~SPI_CR2_SSOE;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void Dri_SPI_Start(void)
{
    CS_LOW;
}

void Dri_SPI_Stop(void)
{
    //等最后一位发完再拉高片选
    if(spi_soft_mode == 0){
        while (SPI1->SR & SPI_SR_BSY);
    }
    CS_HIGH;
}

uint8_t Dri_SPI_SwapByte(uint8_t byte){
    if(spi_soft_mode){
        return SPI_Soft_SwapByte(byte);
    }
    while (!(SPI1->SR & SPI_SR_TXE)) {
    }
    SPI1->DR = byte;
    while (!(SPI1->SR & SPI_SR_RXNE)) {
    }
    return (uint8_t)SPI1->DR;
}

//下面保留原软件模拟SPI的写法
//SCK PA5 通用推挽输出 MODE:11 CNF:00
//MISO PA6 浮空输入 MODE:00 CNF:01
//MOSI PA7 通用推挽输出 MODE:11 CNF:00
//CS PC13 通用推挽输出 MODE:11 CNF:00

//SPI有四种模式：当前使用MODE0  极性：0 相位：0

#define SCK_HIGH (GPIOA->ODR |= GPIO_ODR_ODR5)
#define SCK_LOW (GPIOA->ODR &=~GPIO_ODR_ODR5)

#define MOSI_HIGH (GPIOA->ODR |= GPIO_ODR_ODR7)
#define MOSI_LOW (GPIOA->ODR &=~GPIO_ODR_ODR7)

#define READ_MISO (GPIOA->IDR & GPIO_IDR_IDR6)

void Dri_SPI_Soft_Init(void){
    //补充：同一个spi.h选择软件模式
    spi_soft_mode = 1;
    if(RCC->APB2ENR & RCC_APB2ENR_SPI1EN){
        SPI1->CR1 &=~ SPI_CR1_SPE;
    }
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);

    GPIOA->CRL |= (GPIO_CRL_MODE5 | GPIO_CRL_MODE7);
    GPIOA->CRL &=~(GPIO_CRL_CNF5 | GPIO_CRL_CNF7);
    GPIOC->CRH |= GPIO_CRH_MODE13;
    GPIOC->CRH &=~ GPIO_CRH_CNF13;
    GPIOA->CRL &=~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1);
    GPIOA->CRL |= GPIO_CRL_CNF6_0;

    SCK_LOW;
    CS_HIGH;

}
static uint8_t SPI_Soft_SwapByte(uint8_t byte){
    uint8_t recevieByte = 0;

    for(uint8_t i = 0;i<8;i++){
        (byte &0x80) ? MOSI_HIGH : MOSI_LOW;
        byte<<=1;

        Dri_Delay_Us(5);
        SCK_HIGH;
        Dri_Delay_Us(5);

        recevieByte<<=1;
        if(READ_MISO){
            recevieByte |= 0x01;
        }

        SCK_LOW;
    }
    return recevieByte;
}
