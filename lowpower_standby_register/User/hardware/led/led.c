#include "led.h"

void Int_LED_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL |= GPIO_CRL_MODE0;
    GPIOA->CRL &= ~GPIO_CRL_CNF0;

    GPIOA->ODR &=~GPIO_ODR_ODR0;

}
void Int_LED_Toggle(void){
    GPIOA->ODR ^= GPIO_ODR_ODR0;
}