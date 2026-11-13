#include "rtc.h"
#include "delay.h"
#include <time.h>

/* RTC 没有时钟时不能无限等待标志位。 */
static uint8_t Dri_RTC_WaitForFlag(uint16_t flag)
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

uint8_t Dri_RTC_Init(void)
{
    uint16_t timeout = 5000;

    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;

    /* 仅在需要切换已有时钟源时复位后备域，此操作会清除后备域数据。 */
    if (((RCC->BDCR & RCC_BDCR_RTCSEL) != RCC_BDCR_RTCSEL_NOCLOCK) &&
        ((RCC->BDCR & RCC_BDCR_RTCSEL) != RCC_BDCR_RTCSEL_LSE))
    {
        RCC->BDCR |= RCC_BDCR_BDRST;
        RCC->BDCR &= ~RCC_BDCR_BDRST;
    }

    /* 先开启 32.768 kHz 晶振，再选择并使能 RTC 时钟。 */
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

    RTC->CRL &= ~RTC_CRL_RSF;
    if (!Dri_RTC_WaitForFlag(RTC_CRL_RSF) ||
        !Dri_RTC_WaitForFlag(RTC_CRL_RTOFF))
    {
        return 0;
    }

    RTC->CRL |= RTC_CRL_CNF;
    RTC->PRLH = 0;
    RTC->PRLL = 32768 - 1;
    RTC->CRL &= ~RTC_CRL_CNF;
    return Dri_RTC_WaitForFlag(RTC_CRL_RTOFF);
}

uint8_t Dri_RTC_SetDateTime(RTC_DateTimeTypeDef datetime)
{
    struct tm t = {0};
    time_t timestamp;

    t.tm_year = datetime.Year - 1900;
    t.tm_mon = datetime.month - 1;
    t.tm_mday = datetime.day;
    t.tm_hour = datetime.hour;
    t.tm_min = datetime.minute;
    t.tm_sec = datetime.second;
    timestamp = mktime(&t);
    if ((timestamp == (time_t)-1) || !Dri_RTC_WaitForFlag(RTC_CRL_RTOFF))
    {
        return 0;
    }

    RTC->CRL |= RTC_CRL_CNF;
    /* 配置模式中直接写计数器，不需要等待 SECF。 */
    RTC->CNTH = (uint16_t)(timestamp >> 16);
    RTC->CNTL = (uint16_t)timestamp;
    RTC->CRL &= ~RTC_CRL_CNF;
    return Dri_RTC_WaitForFlag(RTC_CRL_RTOFF);
}

void Dri_RTC_GetDateTime(RTC_DateTimeTypeDef *datetime)
{
    uint16_t high, low;
    time_t timestamp;
    struct tm *now;

    /* 防止读取两个半字期间低 16 位溢出，拼出不一致的时间戳。 */
    do
    {
        high = RTC->CNTH;
        low = RTC->CNTL;
    } while (high != RTC->CNTH);
    timestamp = ((uint32_t)high << 16) | low;
    now = localtime(&timestamp);
    if (now == NULL)
    {
        return;
    }
    datetime->Year = 1900 + now->tm_year;
    datetime->month = now->tm_mon + 1;
    datetime->day = now->tm_mday;
    datetime->hour = now->tm_hour;
    datetime->minute = now->tm_min;
    datetime->second = now->tm_sec;
}
