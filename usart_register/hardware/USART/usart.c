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
    //3.串口配置
    //3.1波特率设置
    USART1->BRR = 0x271;
    //3.2收发使能和模块使能
    USART1->CR1 |= USART_CR1_RE;
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_UE;
    //3.3其他配置，设置数据帧格式
    USART1->CR1 &=~USART_CR1_M;
    USART1->CR1 &=~USART_CR1_PCE;
    USART1->CR2 &=~USART_CR2_STOP;
    //3.4 开启中断使能，接收的
    USART1->CR1 |= USART_CR1_RXNEIE;
    USART1->CR1 |= USART_CR1_IDLEIE;

    //4.NVIC配置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(USART1_IRQn,3);
    NVIC_EnableIRQ(USART1_IRQn);



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

    while((USART1->SR & USART_SR_IDLE) == 0)//最合理的判断结束的就是IDLE位
    {
        buffer[i] = USART_ReceiveChar();
        i++;
    }
    //清除IDLE

    //可写可不写USART1->SR;
    USART1->DR;

    *size = --i;
    
}

//中断服务程序
void USART1_IRQHandler(void){
    //先判断中断类型,通过状态位
    if(USART1->SR & USART_SR_RXNE){
        //接收完成一个字符
        buffer[size] = USART1->DR;
        size++;
    }
    else if(USART1->SR & USART_SR_IDLE){
        //字符串接收完毕
        //清除IDLE
        USART1->DR;
        //直接发送字符串到电脑
        //USART_SendString(buffer,size);核心逻辑不在中断服务程序中，而是通过标志位，在主函数中调用
        //清 0 size
        // size = 0;
        isOver = 1;
    }
}
