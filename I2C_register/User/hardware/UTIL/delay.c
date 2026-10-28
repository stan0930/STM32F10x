#include "delay.h"


void Dri_Delay_Us(uint16_t us)
{
    SysTick->LOAD = 72 * us;

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
