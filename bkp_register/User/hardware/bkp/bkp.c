#include "bkp.h"
#include "stm32f10x.h"

void Dri_BKP_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;
    //经过以上三步后备寄存器才允许我们访问

}