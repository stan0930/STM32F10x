#include "spi.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>  
#include "w25q32.h"
#include "adc1.h"
#include "tim5.h"
#include "tim1.h"

// volatile double v;

// int main(){
//     Dri_USART_Init();
//     Dri_ADC1_Init();
//     Dri_ADC1_StartConvert();
//     Dri_Tim5_Init();
//     Dri_Tim5_Start();
//     while (1)
//     {
//         double v = Dri_ADC1_ReadV();
//         //Dri_Delay_Ms(100);
//         //printf("v = %f\n", v);
//         uint8_t duty = (uint8_t)(v / 3.3 * 100);
//         //printf("duty = %d\n", duty);
//         //Dri_Delay_Ms(100);
//         Dri_Tim5_SetDuty(duty);
//     }
// }

volatile double v;
int main(){
    Dri_USART_Init();
    Dri_ADC1_Init();
    Dri_ADC1_StartConvert();
    Dri_Tim1_Init();
    Dri_Tim1_Start();
    while (1)
    {
        double v = Dri_ADC1_ReadV();
        // Dri_Delay_Ms(100);
        // printf("v = %f\n", v);
        uint16_t duty = (uint16_t)(v / 3.3 * 5000);
        // printf("duty = %d\n", duty);
        // Dri_Delay_Ms(100);
        Dri_Tim1_SetDuty(duty);
    }
}

// int main(){
//     Dri_USART_Init();
//     Dri_ADC1_Init();
//     Dri_ADC1_StartConvert();
//     Dri_Tim1_Init();
//     Dri_Tim1_Start();
//     while (1)
//     {
//         double v = Dri_ADC1_ReadV();
//         // Dri_Delay_Ms(100);
//         // printf("v = %f\n", v);
//         uint16_t duty = (uint16_t)(v / 3.3 * 100);
//         // printf("duty = %d\n", duty);
//         // Dri_Delay_Ms(100);
//         Dri_Tim1_SetDuty(duty);
//     }
// }

