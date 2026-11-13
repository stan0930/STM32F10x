#ifndef RTC_H
#define RTC_H

#include "stm32f10x.h"

typedef struct{
    uint16_t Year;//2026
    uint8_t month,day,hour,minute,second;

}RTC_DateTimeTypeDef;

/* Return 1 on success, 0 on RTC timeout or conversion failure. */
uint8_t Dri_RTC_Init(void);
uint8_t Dri_RTC_SetDateTime(RTC_DateTimeTypeDef dateTime);
void Dri_RTC_GetDateTime(RTC_DateTimeTypeDef *dateTime);


#endif
