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
    while((USART1->SR & USART_SR_RXNE) == 0){};
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

    //外层循环，不停的接收下一个字符
    while(1){
        //内层循环，判断当前数据帧是否接收完毕
        while((USART1->SR & USART_SR_RXNE) == 0){
            //额外增加一个条件，判断当前是否检测到空闲帧
            if((USART1->SR & USART_SR_IDLE) != 0){
                //字符串接收完毕
                *size = i;
                return;
            }
        }
        //接收当前字符
        buffer[i] = USART1->DR;
        i++;
    }
}