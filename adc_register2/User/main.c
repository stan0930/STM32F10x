#include "adc1.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>  


int main(void){
    Dri_USART_Init();
    Dri_ADC1_Init();
    uint16_t arr[3];
    Dri_ADC1_StartConvert((uint32_t)arr, 3);

    while (1)
    {
        double v1 = 3.3*arr[0]/4095;
        double v2 = 3.3*arr[1]/4095;
        double v3 = 3.3*arr[2]/4095;
        printf("v1 = %.2f, v2 = %.2f, v3 = %.2f\n",v1,v2,v3);
        Dri_Delay_Ms(1000);
    }
    
}