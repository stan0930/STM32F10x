#ifndef USART_H
#define USART_H

#include "stm32f10x.h"
extern uint8_t buffer[100];
extern volatile uint8_t size;
extern volatile uint8_t isOver;

void USART_Init(void);
void USART_SendChar(uint8_t ch);
uint8_t USART_ReceiveChar(void);
void USART_SendString(uint8_t *str,uint8_t size);
//void USART_ReceiveString(uint8_t buffer[],uint8_t *size);

#endif
