#ifndef TIM5_H
#define TIM5_H
#include "stm32f10x.h"
void Dri_Tim5_Init(void);
void Dri_Tim5_Start(void);
void Dri_Tim5_Stop(void);
void Dri_Tim5_SetDuty(uint8_t duty);


#endif