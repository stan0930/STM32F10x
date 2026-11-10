#include "adc1.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include <stdio.h>  

void enter_sleep(){
    __WFI();
}

int main(void){
    Dri_USART_Init();
    Int_LED_Init();
    printf("低功耗-睡眠模式-寄存器版本...\n");


    while (1)
    {
    printf("3秒之后进入睡眠模式\n");
    Dri_Delay_Ms(1000);
    printf("睡眠模式已进入\n");
    Dri_Delay_Ms(1);
    //进入睡眠模式
    enter_sleep();


    printf("睡眠模式唤醒\n");

    Int_LED_Toggle();
    }
    
}