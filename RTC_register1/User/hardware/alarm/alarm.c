#include "alarm.h"
#include "delay.h"

/* RTC 寄存器同步/写操作也要有超时，避免时钟异常时卡死。 */
static uint8_t Dri_Alarm_WaitForFlag(uint16_t flag)
{
    uint16_t timeout = 1000;

    while ((RTC->CRL & flag) == 0)
    {
        if (timeout == 0)
        {
            return 0;
        }
        timeout--;
        Dri_Delay_Ms(1);
    }
    return 1;
}

uint8_t Dri_Alarm_Init(void)
{
    uint16_t timeout = 5000;

    /* 允许访问后备域。 */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;

    /* 已选其他时钟时，必须复位后备域才能切换；这会清除后备域数据。 */
    if (((RCC->BDCR & RCC_BDCR_RTCSEL) != RCC_BDCR_RTCSEL_NOCLOCK) &&
        ((RCC->BDCR & RCC_BDCR_RTCSEL) != RCC_BDCR_RTCSEL_LSE))
    {
        RCC->BDCR |= RCC_BDCR_BDRST;
        RCC->BDCR &= ~RCC_BDCR_BDRST;
    }

    /* 使用板上的 32.768 kHz 外部低速晶振。 */
    RCC->BDCR |= RCC_BDCR_LSEON;
    while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0)
    {
        if (timeout == 0)
        {
            return 0;
        }
        timeout--;
        Dri_Delay_Ms(1);
    }
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE | RCC_BDCR_RTCEN;

    /* 上电或待机唤醒后，先等待 APB 接口与 RTC 时钟域同步。 */
    RTC->CRL &= ~RTC_CRL_RSF;
    if (!Dri_Alarm_WaitForFlag(RTC_CRL_RSF) ||
        !Dri_Alarm_WaitForFlag(RTC_CRL_RTOFF))
    {
        return 0;
    }

    RTC->CRL |= RTC_CRL_CNF;
    RTC->PRLH = 0;
    RTC->PRLL = 32768 - 1;     /* 32768 分频，RTC 每秒计数一次。 */
    RTC->CRL &= ~RTC_CRL_CNF;

    return Dri_Alarm_WaitForFlag(RTC_CRL_RTOFF);
}

uint8_t Dri_Alarm_SetAlarm(uint32_t second)
{
    if ((second == 0) || !Dri_Alarm_WaitForFlag(RTC_CRL_RTOFF))
    {
        return 0;
    }

    /* 重设期间先关闭闹钟请求，清除上一次的闹钟标志。 */
    RTC->CRH &= ~RTC_CRH_ALRIE;
    if (!Dri_Alarm_WaitForFlag(RTC_CRL_RTOFF))
    {
        return 0;
    }
    RTC->CRL &= ~RTC_CRL_ALRF;
    EXTI->PR = EXTI_PR_PR17;

    RTC->CRL |= RTC_CRL_CNF;
    RTC->CNTH = 0;
    RTC->CNTL = 0;
    /* 从 0 数到 second 时触发，不应再减 1。 */
    RTC->ALRH = (uint16_t)(second >> 16);
    RTC->ALRL = (uint16_t)second;
    RTC->CRL &= ~RTC_CRL_CNF;
    if (!Dri_Alarm_WaitForFlag(RTC_CRL_RTOFF))
    {
        return 0;
    }

    /* 待机闹钟唤醒需要 ALRIE；无需开启 NVIC 中断。 */
    RTC->CRH |= RTC_CRH_ALRIE;
    return Dri_Alarm_WaitForFlag(RTC_CRL_RTOFF);
}
