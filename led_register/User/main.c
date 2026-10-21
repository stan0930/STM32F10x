#include "led.h"
#include "Delay.h"

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
void Delay_s(uint16_t s);


int main(){

		//1.初始化LED
		LED_Init();

		//2.定义一个LED数组
		uint16_t leds[]={LED1,LED2,LED3};

		//3.在循环中执行流水灯，依次点亮每个灯，保持一段时间再关闭
		while(1)
		{	
			for(uint8_t i=0;i<3;i++)
			{
				LED_On(leds[i]);
				Delay_ms(500);
				LED_Off(leds[i]);
			}
		}
}
