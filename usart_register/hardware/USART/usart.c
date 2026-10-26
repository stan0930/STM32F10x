#include "usart.h"

void USART_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    GPIOA->CRH |=GPIO_CRH_MODE9;
    GPIOA->CRH |=GPIO_CRH_CNF9_1;
    GPIOA->CRH &=~GPIO_CRH_CNF9_0;

    GPIOA->CRH &=~GPIO_CRH_MODE10;
    GPIOA->CRH |=GPIO_CRH_CNF10_0;
    GPIOA->CRH &=~GPIO_CRH_CNF10_1;

    USART1->BRR = 0x271;

    USART1->CR1 |= USART_CR1_RE;
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_UE;

    USART1->CR1 &=~USART_CR1_M;
    USART1->CR1 &=~USART_CR1_PCE;
    USART1->CR2 &=~USART_CR2_STOP;

}
void USART_SendChar(uint8_t ch){
    while((USART1->SR & USART_SR_TXE) == 0){};
    USART1->DR = ch;
}
uint8_t USART_ReceiveChar(void){
    while((USART1->SR & USART_SR_RXNE) == 0){
        //我们增加判断空闲帧的条件
        if((USART1->SR & USART_SR_IDLE)){
            //字符串接收完毕
            return '\0';
        }
    }
    return USART1->DR;
}

void USART_SendString(uint8_t *str,uint8_t size){
    for(uint8_t i=0;i<size;i++){
        USART_SendChar(str[i]);
    }
}
void USART_ReceiveString(uint8_t buffer[],uint8_t *size){
    //定义一个变量，用来保存已经接收到的字符个数
    uint8_t i = 0;

    while((USART1->SR & USART_SR_IDLE) == 0)
    {
        buffer[i] = USART_ReceiveChar();
        i++;
    }
    //清除IDLE

    //可写可不写USART1->SR;
    USART1->DR;

    *size = --i;
    
}