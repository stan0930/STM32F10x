#ifndef ALARM_H
#define ALARM_H

#include "stm32f10x.h"


/* Return 1 on success, or 0 if the RTC clock/synchronization times out. */
uint8_t Dri_Alarm_Init(void);
uint8_t Dri_Alarm_SetAlarm(uint32_t second);


#endif

