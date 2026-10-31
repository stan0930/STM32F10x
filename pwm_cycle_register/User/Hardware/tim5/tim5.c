#include "tim5.h"
#include "stm32f10x.h"

void Dri_Tim5_Init(void){
    //1.RCC时钟使能
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    //2.PA1工作模式 复用推挽输出 MODE：11 CNF：10
    GPIOA->CRL |= (GPIO_CRL_MODE1 | GPIO_CRL_CNF1_1);
    GPIOA->CRL &= ~(GPIO_CRL_CNF1_0);

    //Tim5设置
    //3.自动重装载的预装载
    TIM5->CR1 &=~ TIM_CR1_ARPE;
    //4.向上计数
    TIM5->CR1 &=~ TIM_CR1_DIR;
    //5.不需要单脉冲
    TIM5->CR1 &=~ TIM_CR1_OPM;
    //6.Tim5配置为比较输出模式
    TIM5->CCMR1 &=~ TIM_CCMR1_CC2S;
    //7.Tim5输出模式选择PWM模式1-110
    TIM5->CCMR1 |= TIM_CCMR1_OC2M_2;
    TIM5->CCMR1 |= TIM_CCMR1_OC2M_1;
    TIM5->CCMR1 &=~ TIM_CCMR1_OC2M_0;
    //8.Tims 极性选择 0 - 有效电平是高电平
    TIM5->CCER &=~ TIM_CCER_CC2P;
    //9.Tim5 通道2输出使能
    TIM5->CCER |= TIM_CCER_CC2E;


    // Time5 时基单元配置
    TIM5->CNT = 0;
    TIM5->PSC = 7200-1;
    TIM5->ARR = 100-1;
    TIM5->CCR2 = 60;
}  

void Dri_Tim5_Start(void){
    TIM5->CR1 |= TIM_CR1_CEN;
}
