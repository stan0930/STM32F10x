#include "tim5.h"
#include "tim4.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"

double getCycle(){
    return 1.0*TIM4->CCR1 / 1000;
}

double getFreq(){
    return 1000000.0 / TIM4->CCR1;
}

int main(){
    Dri_Tim5_Init();
    Dri_Tim4_Init();
    Dri_USART_Init();
    printf("测量周期和方波\n");
    Dri_Tim4_Start();
    Dri_Tim5_Start();

    while(1){
        printf("T=%.2f ms,F=%.2f Hz\n",getCycle(),getFreq());
        Dri_Delay_S(1);
    }
}