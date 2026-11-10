#include "adc1.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include <stdio.h>  

void enter_sleep(){
    __WFI();
}

void enter_stop(){
    //进入停止模式
    //SLEEPDEEP置1表示进入深度睡眠；
    //深度睡眠有两种情况：1.stop模式 - 停止模式 2.standby模式 - 待机模式
    //PDDS：0-停止模式 1-待机模式

    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    __WFI();
}

int main(void){
    Dri_USART_Init();
    Dri_KEY_Init();
    Int_LED_Init();
    printf("低功耗-停止模式-寄存器版本...\n");
    printf("3秒之后进入停止模式\n");
    Dri_Delay_Ms(1000);
    printf("停止模式已进入\n");
    Dri_Delay_Ms(1);

    //进入停止模式
    enter_stop();
    //解决时钟恢复72MHz问题
    SystemInit();


    while (1)
    {
        Int_LED_Toggle();
        Dri_Delay_Ms(1000);
        //printf("停止模式唤醒\n");

        printf("停止模式唤醒\n");
    }
    
}