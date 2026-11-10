//SysTick延时：按72MHz计算，CPU忙等；不影响TIM，但不能和其他SysTick时基并用。
#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f10x.h"

void Dri_Delay_Ms(uint16_t ms);
void Dri_Delay_Us(uint16_t us);
void Dri_Delay_S(uint16_t s);

#endif /* __DELAY_H__ */
