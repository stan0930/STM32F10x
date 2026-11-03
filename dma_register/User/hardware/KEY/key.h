#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include "delay.h"
extern volatile uint8_t isOn;
extern volatile uint8_t key3_event;

void Dri_KEY_Init(void);



#endif /* __KEY_H__ */

