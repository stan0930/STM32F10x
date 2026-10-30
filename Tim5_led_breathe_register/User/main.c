#include "tim5.h"
#include "led.h"
#include "delay.h"



int main(void){
    Dri_Tim5_Init();

    Dri_Tim5_Start();

    uint8_t duty = 1;
    uint8_t flag = 1;
    while(1){
        if(flag == 1){
            duty++;
        }else{
            duty--;
        }
        Dri_Tim5_SetDuty(duty);
        Dri_Delay_Ms(20);
        if (duty ==99)
        {
            flag = 0;
        }else if(duty == 1){
            flag = 1;
        }
        
    }

}
