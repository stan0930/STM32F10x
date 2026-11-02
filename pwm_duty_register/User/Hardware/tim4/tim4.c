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
    
    // //5.设置Tim4的捕获中断使能
    // TIM4->DIER |= TIM_DIER_CC1IE;

    //6.设置IC1的来源：TI1 - 01
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_1;

    //设置IC2的来源：TI1 - 10
    TIM4->CCMR1 |=TIM_CCMR1_CC2S_1;
    TIM4->CCMR1 &= ~TIM_CCMR1_CC2S_0;

    //7.当前Tim4的信号来源：不滤波，不预分频
    TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC);
    //CH2也不预分频
    TIM4->CCMR1 &= ~TIM_CCMR1_IC2PSC;

    //8. Tim4的CH1输入通道配置为上升沿捕获 - 极性设置
    TIM4->CCER &=~ TIM_CCER_CC1P;
    //Tim4的CH2输入通道配置为下降沿捕获
    TIM4->CCER |= TIM_CCER_CC2P;

    //9. Tim4的输入通道（CH1和CH2）捕获使能
    TIM4->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

    //Tim4的时基单元配置
    //进行72分频，从而时基单元的工作频率是1mhz
    //因此每计数一次就是1us

    TIM4 -> PSC = 72-1; 
    TIM4 -> CNT = 0;
    //Tim4的ARR设置为最大值，目的尽量不要溢出
    //例如：第一次捕获CNT=1；第二次捕获CNT=101，
    //如果没有溢出的情况，因此周期就是100，但是如果有溢出，则周期不确定
    TIM4 -> ARR = UINT16_MAX;

    //当前Tim4可以接受触发信号，因此Tim4是从定时器，出发后需要做复位指令。因此需要设置SMR中的TS和SMS
    //触发信号来源的设置 触发信号来源于TI1FP1 - 101
    TIM4->SMCR |= (TIM_SMCR_TS_2 | TIM_SMCR_SMS_0);
    TIM4->SMCR &=~ TIM_SMCR_TS_1;

    //从模式选择：触发信号到来时CNT复位 SMS - 100
    TIM4->SMCR |= TIM_SMCR_SMS_2;
    TIM4->SMCR &=~ (TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);

    

}

void Dri_Tim4_Start(void){
    TIM4->CR1 |= TIM_CR1_CEN;
}

