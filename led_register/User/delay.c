#include "delay.h"


void Delay_us(uint16_t us)
{
    //1.装载一个计数值，72*us
    SysTick->LOAD = 72 * us;

    //配置，使用系统时钟（1），计数结束不产生中断（0），使能定时器（1）
    SysTick->CTRL = 0x05;
    //等待SysTick定时器计数到0
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG) == 0 
            &&(SysTick->CTRL & SysTick_CTRL_ENABLE))
    {
    }

    //关闭SysTick定时器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

void Delay_ms(uint16_t ms)
{
    while (ms--)
    {
        Delay_us(1000);
    }
}