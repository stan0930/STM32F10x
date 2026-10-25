#include "usart.h"
#include "delay.h"

int main(){
	USART_Init();
	USART_SendChar('a');
	USART_SendChar('t');

	while(1){
		USART_SendChar('a');
		USART_SendChar('t');
		Delay_ms(1000);
	}

}
