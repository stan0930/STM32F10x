//TIM6：约1秒更新中断，CNT溢出后自动回零；Callback仍在中断里执行。
#ifndef TIM6_H
#define TIM6_H

#include "stm32f10x.h"

void Dri_Tim6_Init(void);
void Dri_Tim6_Start(void);
void Dri_Tim6_Callback(void);

#endif
