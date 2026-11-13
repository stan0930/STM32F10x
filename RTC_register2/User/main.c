#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include <stdio.h>

int main(void)
{
    RTC_DateTimeTypeDef datetime = {0};

    Dri_USART_Init();
    /* 先打印，RTC 时钟异常也能看到程序已启动。 */
    printf("[BOOT] USART1 ready (115200 8N1)\r\n");
    printf("[RTC] Starting LSE...\r\n");
    if (!Dri_RTC_Init())
    {
        printf("[RTC] Init timeout: check 32.768 kHz LSE and RTC clock/sync.\r\n");
        while (1)
        {
        }
    }

    datetime.Year = 2026;
    datetime.month = 11;
    datetime.day = 13;
    datetime.hour = 18;
    datetime.minute = 46;
    datetime.second = 0;
    if (!Dri_RTC_SetDateTime(datetime))
    {
        printf("[RTC] SetDateTime failed.\r\n");
        while (1)
        {
        }
    }
    printf("实时时钟-寄存器版...\r\n");

    while (1)
    {
        Dri_RTC_GetDateTime(&datetime);
        printf("%04d年%02d月%02d日 %02d:%02d:%02d\r\n",
               datetime.Year, datetime.month, datetime.day,
               datetime.hour, datetime.minute, datetime.second);
        Dri_Delay_Ms(1000);
    }
}
