#include "usart.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"

uint8_t buffer[100]={0};
uint8_t size = 0;

//定义标志位
uint8_t isOver = 0;

int main(){
	USART_Init();
	USART_SendChar('a');
	USART_SendChar('t');

	uint8_t str[] = "hello world";
	USART_SendString(str,strlen(str));

	while(1){
		if (isOver)
		{
			USART_SendString(buffer,size);
			isOver = 0;
			size = 0;
		}
		
	}

}
