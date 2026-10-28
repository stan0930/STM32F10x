#ifndef TIM6_H
#define TIM6_H

#include "stm32f10x.h"

void Dri_Tim6_Init(void);
void Dri_Tim6_Start(void);
__weak void Dri_Tim6_Callback(void);

#endif
