#include "delay.h"
#include "usart.h"
#include "alarm.h"
#include <stdio.h>  

void enter_sleep(){
    __WFI();
}


void enter_standby(){
    PWR->CR |= PWR_CR_CWUF;
    PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    __DSB();
    __WFI();
}

int main(void)
{
    Dri_USART_Init();
    printf("[BOOT] USART1 ready (115200 8N1)\r\n");
    // PA0 用于 WKUP，不调用将 PA0 配置为输出的 Int_LED_Init()。
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    if (PWR->CSR & PWR_CSR_SBF)
    {
        // 清除待机历史标志，保留 WUF 供下面判断。
        PWR->CR |= PWR_CR_CSBF;

        if (PWR->CSR & PWR_CSR_WUF)
        {
            // WUF 也可能由 RTC 闹钟置位，不能单独证明是 WKUP 引脚。
            printf("待机后检测到唤醒事件（WKUP/RTC闹钟）...\r\n");
            PWR->CR |= PWR_CR_CWUF;
        }
        else
        {
            // 例如 RESET 按键复位；仅凭 SBF/WUF 不能确定具体复位源。
            printf("待机后复位启动，未检测到唤醒事件...\r\n");
        }
    }
    else
    {
        printf("非待机模式启动...\r\n");
    }

    printf("低功耗-闹钟唤醒-寄存器版本...\r\n");
    printf("[RTC] Starting LSE...\r\n");
    if (!Dri_Alarm_Init())
    {
        printf("[RTC] Init timeout: check 32.768 kHz LSE, RTC clock and sync.\r\n");
        while (1)
        {
        }
    }
    printf("3秒之后进入待机模式\r\n");
    Dri_Delay_Ms(3000);
    printf("进入待机模式,5秒后闹钟唤醒\r\n");
    if (!Dri_Alarm_SetAlarm(5))
    {
        printf("[RTC] Alarm setup timeout.\r\n");
        while (1)
        {
        }
    }
    /* 等最后一个字符的停止位发完，再关闭主电源域。 */
    while ((USART1->SR & USART_SR_TC) == 0)
    {
    }


    enter_standby();

    while (1)
    {
    }
}
