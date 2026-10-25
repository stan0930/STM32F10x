#include "delay.h"


void Delay_us(uint16_t us)
{
    SysTick->LOAD = 72 * us;

    SysTick->CTRL = 0x05;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG) == 0 
            &&(SysTick->CTRL & SysTick_CTRL_ENABLE))
    {
    }

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

void Delay_ms(uint16_t ms)
{
    while (ms--)
    {
        Delay_us(1000);
    }
}
