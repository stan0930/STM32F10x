//TIM4：PB6输入捕获，PA1可接PB6；周期/占空比两种初始化选一个。
#ifndef TIM4_H
#define TIM4_H

#include "stm32f10x.h"

//PB6接PWM输入，计数周期1us；两个初始化选一个，不要同时调用
void Dri_Tim4_Init(void);             //原周期测量，捕获中断里清CNT
void Dri_Tim4_PWMInput_Init(void);     //原占空比测量，硬件触发复位CNT
void Dri_Tim4_Start(void);
void Dri_Tim4_Stop(void);

//启动后至少等两个上升沿再读；单周期必须小于65.536ms
double Dri_Tim4_GetPWMCycle(void);     //周期，ms
double Dri_Tim4_GetPWMFreq(void);      //频率，Hz
double Dri_Tim4_GetDutyCycle(void);    //高电平占空比，0~1

#endif

//PA1（TIM5_CH2 输出）→ PB6（TIM4_CH1 输入）

//任务一：测量 PWM 周期和频率
/*
#include "tim4.h"
#include "tim5.h"
#include "usart.h"
#include "delay.h"
#include <stdio.h>

int main(void){
    //1.初始化串口
    Dri_USART_Init();

    //2.TIM5输出PWM，TIM4测量周期
    Dri_Tim5_Init();
    Dri_Tim4_Init();

    //3.先启动测量，再启动输出
    Dri_Tim4_Start();
    Dri_Tim5_Start();

    //4.等待完整的PWM周期，跳过第一次不完整的捕获
    Dri_Delay_Ms(100);

    while(1){
        double cycle = Dri_Tim4_GetPWMCycle();
        double freq = Dri_Tim4_GetPWMFreq();

        printf("T=%.2f ms, F=%.2f Hz\r\n", cycle, freq);

        Dri_Delay_Ms(1000);
    }
}
*/

//任务二：同时测量周期、频率和占空比
/*
#include "tim4.h"
#include "tim5.h"
#include "usart.h"
#include "delay.h"
#include <stdio.h>

int main(void){
    //1.初始化串口
    Dri_USART_Init();

    //2.TIM5输出PWM
    Dri_Tim5_Init();

    //3.TIM4使用PWM输入模式，同时测周期和高电平时间
    Dri_Tim4_PWMInput_Init();

    //4.先启动测量，再启动输出
    Dri_Tim4_Start();
    Dri_Tim5_Start();

    //5.等待完整的PWM周期
    Dri_Delay_Ms(100);

    while(1){
        double cycle = Dri_Tim4_GetPWMCycle();
        double freq = Dri_Tim4_GetPWMFreq();
        double duty = Dri_Tim4_GetDutyCycle() * 100;

        printf("T=%.2f ms, F=%.2f Hz, duty=%.2f%%\r\n",
               cycle, freq, duty);

        Dri_Delay_Ms(1000);
    }
}
*/