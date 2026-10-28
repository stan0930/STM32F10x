#include "tim6.h"
#include "led.h"

void Dri_Tim6_Init(void)
{
    // 初始化TIM6
    //Tim6 RCC 使能TIM6时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    // ARPE 使能自动重装载预分加载使能
    //当前这一位不重要，因为我们不会中途改变ARR的值
    TIM6->CR1 &= ~TIM_CR1_ARPE;
    //单脉冲：CNT计数器溢出之后要不要机械循环
    //当前肯定不是单脉冲
    TIM6->CR1 &= ~TIM_CR1_OPM;
    //当溢出时需要产生中断
    TIM6->DIER |= TIM_DIER_UIE;

    //时基单元设置
    //预分频设置
    //内部时钟是72MHz，预分频设置为7200-1，因此可以得到100us的时钟周期
    TIM6->PSC = 7200 - 1;
    //自动重装载：已知定时器周期是100us，我们需要产生一个1s的溢出周期，因此ARR需要设置为10000-1
    TIM6->ARR = 10000 - 1;
    //CNT计数器初始值
    TIM6->CNT = 0;

    //NVIC设置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(TIM6_IRQn, 3);
    NVIC_EnableIRQ(TIM6_IRQn);
}

void Dri_Tim6_Start(void)
{
    // 启动TIM6
    TIM6->CR1 |= TIM_CR1_CEN;
}

__weak void Dri_Tim6_Callback(void)
{
}

void TIM6_IRQHandler(){
    // 清除中断标志位
    TIM6->SR &= ~TIM_SR_UIF;
    Dri_Tim6_Callback();

}