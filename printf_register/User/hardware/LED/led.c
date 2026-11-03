#include "led.h"

void Dri_LED_Init(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;


	GPIOA->CRL &= ~GPIO_CRL_CNF0;
	GPIOA->CRL |= GPIO_CRL_MODE0;

	GPIOA->CRL &= ~GPIO_CRL_CNF1;
	GPIOA->CRL |= GPIO_CRL_MODE1;

	GPIOA->CRH &= ~GPIO_CRH_CNF8;
	GPIOA->CRH |= GPIO_CRH_MODE8;


	Dri_LED_Off(LED1);
	Dri_LED_Off(LED2);
	Dri_LED_Off(LED3);
}

void Dri_LED_On(uint16_t led)
{
    GPIOA->ODR &= ~led;
}
void Dri_LED_Off(uint16_t led)
{
    GPIOA->ODR |= led;
}

void Dri_LED_Toggle(uint16_t led)
{
    if((GPIOA->IDR & led) == 0)
    {
        Dri_LED_Off(led);
    }
    else
    {
        Dri_LED_On(led);
    }
}

void Dri_LED_OnAll(uint16_t leds[],uint8_t size)
{
    for(uint8_t i = 0;i<size;i++)
    {
        Dri_LED_On(leds[i]);
    }
}
void Dri_LED_OffAll(uint16_t leds[],uint8_t size)
{
    for(uint8_t i = 0;i<size;i++)
    {
        Dri_LED_Off(leds[i]);
    }
}
