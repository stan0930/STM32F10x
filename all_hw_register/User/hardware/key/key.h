//按键：PF10上升沿，PF8下降沿，事件标志在主循环处理。
#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include "delay.h"
extern volatile uint8_t isOn;
extern volatile uint8_t key3_event;

void Dri_KEY_Init(void);

#endif /* __KEY_H__ */
