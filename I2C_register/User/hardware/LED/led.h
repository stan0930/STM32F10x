#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"
//宏定义
#define LED1 GPIO_ODR_ODR0
#define LED2 GPIO_ODR_ODR1
#define LED3 GPIO_ODR_ODR8

//初始化
void Dri_LED_Init(void);

//控制某个LED的开关
void Dri_LED_On(uint16_t led);
void Dri_LED_Off(uint16_t led);

//反转LED状态
void Dri_LED_Toggle(uint16_t led);

//对一组LED灯，全开全关
void Dri_LED_OnAll(uint16_t leds[],uint8_t size);
void Dri_LED_OffAll(uint16_t leds[],uint8_t size);




#endif /* __LED_H__ */

