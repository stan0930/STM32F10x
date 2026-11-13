//TIM1：PA8输出有限个PWM，单脉冲和中断停机方案选一个。
#ifndef TIM1_H
#define TIM1_H
#include "stm32f10x.h"

//PA8，2Hz，RCR=4，对应5个周期；两种初始化选一个
void Dri_Tim1_Init(void);       //原单脉冲方案
void Dri_Tim1_Start(void);
void Dri_Tim1_IT_Init(void);    //原中断停机方案
void Dri_Tim1_IT_Start(void);
void Dri_Tim1_Stop(void);
void Dri_Tim1_SetDuty(uint16_t duty);

#endif

/*
#include "tim1.h"

int main(void){
    //1.初始化TIM1，使用更新中断停止计数
    Dri_Tim1_IT_Init();

    //2.启动PWM输出
    Dri_Tim1_IT_Start();

    while(1){
        //输出5个周期后，中断里自动停止
    }
}
    
*/
