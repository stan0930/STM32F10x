#include "tim1.h"
#include "stm32f10x.h"

//TIM1 CH1 PA8
void Dri_Tim1_Init(void){
    RCC->APB2ENR |= (RCC_APB2ENR_TIM1EN|RCC_APB2ENR_IOPAEN);
    //PA8 复用推挽输出模式 MODE:11 CNF:10
    GPIOA->CRH |= (GPIO_CRH_MODE8 | GPIO_CRH_CNF8_1);
    GPIOA->CRH &=~ GPIO_CRH_CNF8_0;

    //TIM1 设置
    //不需要自动重装载的预加载使能：向上计数
    TIM1 -> CR1 &=~ (TIM_CR1_ARPE | TIM_CR1_DIR);
    //这只单脉冲模式
    TIM1 -> CR1 |= TIM_CR1_OPM;
    //这只TIM1 CH1 是比较输出模式
    TIM1 -> CCMR1 &=~ TIM_CCMR1_CC1S;
    //设置TIM1 CH1 PWM1输出模式
    TIM1->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
    TIM1->CCMR1 &=~ TIM_CCMR1_OC1M_0;

    //设置通道的极性和使能
    TIM1->CCER &=~ TIM_CCER_CC1P;//有效电平是高电平
    TIM1->CCER |= TIM_CCER_CC1E;//使能通道1

    //时基单元
    TIM1->CNT = 0;
    TIM1->PSC = 7200-1; //10kHZ 周期100us
    TIM1->ARR = 5000-1;
    TIM1->CCR1 = 2500; // 小于是有效电平，大于等于是无效电平

    //设置重复计数器
    TIM1->RCR = 4;

    //设置主输出使能"放开手刹"
    TIM1->BDTR |= TIM_BDTR_MOE;

    //最初状态时，REP里面存储的是0，在一轮中，只会重复一次，而tim1被我们设置为单脉冲，所以灯瞬间亮一下就结束了
    //通过软件的方式强制产生一次更新事件
    TIM1->EGR |= TIM_EGR_UG;
}

void Dri_Tim1_Start(void){
    TIM1->CR1 |= TIM_CR1_CEN;

}