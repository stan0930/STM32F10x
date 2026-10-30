#include "tim5.h"


void Dri_Tim5_Init(void){
    //RCC使能
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL |= (GPIO_CRL_MODE1|GPIO_CRL_CNF1_1);
    //CNF：10
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0;




    //自动重装载预装载使能 默认不需要
    TIM5->CR1 &= ~TIM_CR1_ARPE;
    //CNT向上计数
    TIM5->CR1 &= ~TIM_CR1_DIR;
    //禁用单脉冲模式
    TIM5->CR1 &= ~TIM_CR1_OPM;
    //禁用CNT溢出中断，默认就是不开启
    TIM5->DIER &= ~TIM_DIER_UIE;
    //禁用捕获比较器2中断，默认就是不开启
    TIM5->DIER &= ~TIM_DIER_CC2IE;

    //设置tim5的ch2的模式：比较输出模式，PWM1模式
    TIM5->CCMR1 &= ~TIM_CCMR1_CC2S;
    TIM5->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
    TIM5->CCMR1 &= ~TIM_CCMR1_OC2M_0;//110设置好了

    //设置tim5的ch2通道的比较输出极性
    TIM5->CCER &=~ TIM_CCER_CC2P;
    //输出使能（打开输出通道）
    TIM5->CCER |= TIM_CCER_CC2E;

    //时基单元的配置
    TIM5->PSC = 7200-1;
    TIM5->CNT = 0;
    TIM5->ARR = 100-1;

    //设置Tim5的CH2的捕获比较寄存器中的值
    TIM5->CCR2 = 60;


   }
void Dri_Tim5_Start(void){
    TIM5->CR1 |= TIM_CR1_CEN;
}
void Dri_Tim5_Stop(void){

}

void Dri_Tim5_SetDuty(uint8_t duty)
{
    TIM5->CCR2 = duty;
}
