#include "adc1.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>  


int main(void){
    Dri_USART_Init();
    Dri_ADC1_Init();
    Dri_ADC1_StartConvert();

    while (1)
    {
        double v = 3.3*ADC1->DR/4095;
        printf("v = %.2f\n",v);
        Dri_Delay_Ms(1000);
    }
    
}