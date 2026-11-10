//SysTick延时：按72MHz计算，CPU忙等；不影响TIM，但不能和其他SysTick时基并用。
#include "delay.h"

void Dri_Delay_Us(uint16_t us)
{
    //补充：零延时直接返回；LOAD=N-1，VAL清零后重新计数
    if(us == 0) return;
    SysTick->LOAD = 72 * us - 1;
    SysTick->VAL = 0;

    SysTick->CTRL = 0x05;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG) == 0 
            &&(SysTick->CTRL & SysTick_CTRL_ENABLE))
    {
    }

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

void Dri_Delay_Ms(uint16_t ms)
{
    while (ms--)
    {
        Dri_Delay_Us(1000);
    }
}

void Dri_Delay_S(uint16_t s)
{
    while (s--)
    {
        Dri_Delay_Ms(1000);
    }
}
