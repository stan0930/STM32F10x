#include "led.h"
#include "Delay.h"

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
void Delay_s(uint16_t s);


int main(){


		LED_Init();

		key_init();

		while(1)
		{
		}
}
