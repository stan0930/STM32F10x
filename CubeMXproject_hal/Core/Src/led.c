#include "led.h"

void LED_On(LED led)
{
    HAL_GPIO_WritePin(GPIOA, led, GPIO_PIN_RESET);
}
void LED_Off(LED led)
{
    HAL_GPIO_WritePin(GPIOA, led, GPIO_PIN_SET);
}
void LED_Toggle(LED led)
{
    HAL_GPIO_TogglePin(GPIOA, led);
}

void LED_OnAll(LED leds[], uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        LED_On(leds[i]);
    }
}
void LED_OffAll(LED leds[], uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        LED_Off(leds[i]);
    }
}