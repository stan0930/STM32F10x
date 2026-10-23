#include "led.h"
#include "Delay.h"

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
void Delay_s(uint16_t s);


int main(){

		//1.初始化LED
		LED_Init();
		//2.初始化按键
		key_init();

		while(1)
		{
		}
}
