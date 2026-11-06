#include "spi.h"


//SCK PA5 通用推挽输出 MODE:11 CNF:00
//MISO PA6 浮空输入 MODE:00 CNF:01
//MOSI PA7 通用推挽输出 MODE:11 CNF:00
//CS PC13 通用推挽输出 MODE:11 CNF:00

//SPI有四种模式：当前使用MODE0  极性：0 相位：0

#define CS_HIGH (GPIOC->ODR |= GPIO_ODR_ODR13)
#define CS_LOW  (GPIOC->ODR &=~GPIO_ODR_ODR13)

#define SCK_HIGH (GPIOA->ODR |= GPIO_ODR_ODR5)
#define SCK_LOW (GPIOA->ODR &=~GPIO_ODR_ODR5)


#define MOSI_HIGH (GPIOA->ODR |= GPIO_ODR_ODR7)
#define MOSI_LOW (GPIOA->ODR &=~GPIO_ODR_ODR7)

#define READ_MISO (GPIOA->IDR & GPIO_IDR_IDR6)




void Dri_SPI_init(void){
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
void Dri_SPI_Start(void){
    CS_LOW;
}
void Dri_SPI_Stop(void){
    CS_HIGH;
}
uint8_t Dri_SPI_SwapByte(uint8_t byte){
    uint8_t recevieByte = 0;

    for(uint8_t i = 0;i<8;i++){
        (byte &0x80) ? MOSI_HIGH : MOSI_LOW;
        byte<<=1;

        SCK_HIGH;

        recevieByte<<=1;
        if(READ_MISO){
            recevieByte |= 0x01;
        }

        SCK_LOW;
    }
    return recevieByte;
}