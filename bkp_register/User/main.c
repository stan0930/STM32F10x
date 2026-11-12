#include "delay.h"
#include "usart.h"
#include "led.h"
#include <stdio.h>  
#include "bkp.h"

int main(void)
{
    Dri_USART_Init();
    Dri_BKP_Init();

    // BKP->DR1= 99;
    // BKP->DR2= 88;

    uint16_t d1 = BKP->DR1;
    uint16_t d2 = BKP->DR2;
    printf("d1 = %d, d2 = %d\n", d1, d2);

    while (1)
    {
    }
}
