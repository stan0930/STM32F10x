#include "led.h"
#include "Delay.h"

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
void Delay_s(uint16_t s);

//定义全局变量, 表示是否开启流水灯的标志位
uint8_t isOn = 0;

int main(){


		LED_Init();

		key_init();

		uint8_t size = 3;
		uint16_t leds[] = {LED1, LED2, LED3};

		while(1)
		{
			//判断标志位，开启流水灯
			if(isOn){
				for (uint8_t i = 0; i < size; i++)
				{
					LED_On(leds[i]);
					Delay_ms(500);
					LED_Off(leds[i]);
					Delay_ms(500);
				}
			}else{
				LED_OffAll(leds,size);
			}
		}
}
