#include "delay.h"
#include "usart.h"
#include "led.h"
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

void enter_standby(){
    //进入待机模式
    //SLEEPDEEP置1表示进入深度睡眠；
    //深度睡眠有两种情况：1.stop模式 - 停止模式 2.standby模式 - 待机模式
    //PDDS：0-停止模式 1-待机模式
    //PDDS属于STM32，不属于cortex-m3内核
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    PWR->CSR |= PWR_CSR_EWUP;//启用wakeup引脚
    //唤醒标志位清除
    //因为当我们使能wakeup引脚时，可能会导致出现一次上升沿，因此需要手动清除这个标志位

    PWR->CR |= PWR_CR_CWUF;//清除唤醒标志位

    __WFI();
}

int main(void)
{
    Dri_USART_Init();
    // PA0 用于 WKUP，不调用将 PA0 配置为输出的 Int_LED_Init()。
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    if (PWR->CSR & PWR_CSR_SBF)
    {
        // 清除待机历史标志，保留 WUF 供下面判断。
        PWR->CR |= PWR_CR_CSBF;

        if (PWR->CSR & PWR_CSR_WUF)
        {
            // WUF 也可能由 RTC 闹钟置位，不能单独证明是 WKUP 引脚。
            printf("待机后检测到唤醒事件（WKUP/RTC闹钟）...\n");
            PWR->CR |= PWR_CR_CWUF;
        }
        else
        {
            // 例如 RESET 按键复位；仅凭 SBF/WUF 不能确定具体复位源。
            printf("待机后复位启动，未检测到唤醒事件...\n");
        }
    }
    else
    {
        printf("非待机模式启动...\n");
    }

    printf("低功耗-待机模式-寄存器版本...\n");
    printf("3秒之后进入待机模式\n");
    Dri_Delay_Ms(3000);
    printf("准备进入待机模式\n");

    // 等最后一个字符完全发送结束。
    while ((USART1->SR & USART_SR_TC) == 0)
    {
    }

    enter_standby();

    // 成功从 Standby 唤醒会重新启动，不会从这里继续。
    printf("没有成功进入Standby\n");
    while (1)
    {
    }
}
