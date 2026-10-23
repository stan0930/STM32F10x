#ifndef __LED_H__
#define __LED_H__

#include "gpio.h"

#define LED uint16_t

void LED_On(LED led);
void LED_Off(LED led);
void LED_Toggle(LED led);

void LED_OnAll(LED leds[], uint8_t size);
void LED_OffAll(LED leds[], uint8_t size);



#endif /* __LED_H__ */