#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include "delay.h"
extern volatile uint8_t isOn;
// KEY3（PF10）产生的按键事件标志
extern volatile uint8_t key3_event;

void Dri_KEY_Init(void);



#endif /* __KEY_H__ */

