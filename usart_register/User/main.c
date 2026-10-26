#include "usart.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"

uint8_t buffer[100]={0};
uint8_t size = 0;

int main(){
	USART_Init();
	USART_SendChar('a');
	USART_SendChar('t');

	uint8_t str[] = "hello world";
	USART_SendString(str,strlen(str));

	while(1){
		// USART_SendChar('a');
		// USART_SendChar('t');
		// Delay_ms(1000);

		//接收字符串，再发回来
		USART_ReceiveString(buffer,&size);
		USART_SendString(buffer,size);
	}

}
