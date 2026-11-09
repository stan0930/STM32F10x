#include "tim4.h"
#include "stm32f10x.h"

void Dri_Tim4_Init(void){
    //1.RCC时钟使能
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
   
    //2.GPIOB工作模式 浮空输入 MODE:00 CNF:01
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1);
    GPIOB->CRL |= GPIO_CRL_CNF6_0;

    //Tim4设置
    //3.不需要自动重装载的预加载使能：向上计数：禁用单脉冲模式
    TIM4->CR1 &= ~(TIM_CR1_ARPE | TIM_CR1_DIR | TIM_CR1_OPM);

    //4.设置TI1的来源,直接来源于CH1
    TIM4->CR2 &= ~TIM_CR2_TI1S;
    
    //5.设置Tim4的捕获中断使能
    TIM4->DIER |= TIM_DIER_CC1IE;
    //6.设置IC1的来源：TI1 - 01
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_1;

    //7.当前Tim4的信号来源：不滤波，不预分频
    TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC);

    //8. Tim4输入通道配置为上升沿捕获
    TIM4->CCER &=~ TIM_CCER_CC1P;
    //9. Tim4输入通道配置为捕获模式
    TIM4->CCER |= TIM_CCER_CC1E;

    //Tim4的时基单元配置
    //进行72分频，从而时基单元的工作频率是1mhz
    //因此每计数一次就是1us

    TIM4 -> PSC = 72-1; 
    TIM4 -> CNT = 0;
    //Tim4的ARR设置为最大值，目的尽量不要溢出
    //例如：第一次捕获CNT=1；第二次捕获CNT=101，
    //如果没有溢出的情况，因此周期就是100，但是如果有溢出，则周期不确定
    TIM4 -> ARR = UINT16_MAX;

    //当前Tim4是输入捕获模式，不需要设置CCR1的值

    //Tim4我们设置了捕获使能，因此需要进行NVIC的设置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(TIM4_IRQn, 3);
    NVIC_EnableIRQ(TIM4_IRQn);

}

void Dri_Tim4_Start(void){
    TIM4->CR1 |= TIM_CR1_CEN;
}


void TIM4_IRQHandler(void){
    //清除中断标志位
    TIM4->SR &= ~TIM_SR_CC1IF;

    TIM4->CNT = 0;

}
