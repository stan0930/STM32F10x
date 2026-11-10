//TIM5：PA1输出100Hz PWM，SetDuty传0~100，板上LED低电平点亮。
#ifndef TIM5_H
#define TIM5_H
#include "stm32f10x.h"
void Dri_Tim5_Init(void);
void Dri_Tim5_Start(void);
void Dri_Tim5_Stop(void);
void Dri_Tim5_SetDuty(uint8_t duty);

#endif
/*
#include "tim5.h"
#include "delay.h"

int main(void)
{
    Dri_Tim5_Init();
    Dri_Tim5_Start();

    uint8_t duty = 1;
    uint8_t direction = 1;

    while (1)
    {
        Dri_Tim5_SetDuty(duty);
        Dri_Delay_Ms(20);

        if (direction == 1)
        {
            duty++;

            if (duty >= 99)
            {
                duty = 99;
                direction = 0;
            }
        }
        else
        {
            duty--;

            if (duty <= 1)
            {
                duty = 1;
                direction = 1;
            }
        }
    }
}

*/