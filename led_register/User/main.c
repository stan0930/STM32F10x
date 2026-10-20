
#include "stm32f10x.h"

void Delay_ms(uint16_t ms);
void Delay_us(uint16_t us);
void Delay_s(uint16_t s);


int main(){
	
		// 1. 时钟配置
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

		// 2. GPIO工作模式配置
		GPIOA->CRL &= ~GPIO_CRL_CNF0;
		GPIOA->CRL |= GPIO_CRL_MODE0;

		GPIOA->CRL &= ~GPIO_CRL_CNF1;
		GPIOA->CRL |= GPIO_CRL_MODE1;

		GPIOA->CRH &= ~GPIO_CRH_CNF8;
		GPIOA->CRH |= GPIO_CRH_MODE8;

		// 3. PA0、PA1、PA8输出高电平，默认关灯
		GPIOA->ODR |= GPIO_ODR_ODR0;
		GPIOA->ODR |= GPIO_ODR_ODR1;
		GPIOA->ODR |= GPIO_ODR_ODR8;

		// 4. 在循环中执行流水灯，依次点亮每个灯，保持一段时间再关闭
		while(1)
		{
		    // 4.1 LED1
			GPIOA->ODR &= ~GPIO_ODR_ODR0;//置0
			Delay_ms(500);				//延时500ms
			GPIOA->ODR |= GPIO_ODR_ODR0;//置1

 		    // 4.2 LED2
			GPIOA->ODR &= ~GPIO_ODR_ODR1;//置0
			Delay_ms(500);				//延时500ms
			GPIOA->ODR |= GPIO_ODR_ODR1;//置1

			 // 4.3 LED3
			GPIOA->ODR &= ~GPIO_ODR_ODR8;//置0
			Delay_ms(500);				//延时500ms
			GPIOA->ODR |= GPIO_ODR_ODR8;//置1

		}
}

//延时函数，微秒作为单位,利用系统的嘀嗒定时器,72MHz,一次嘀嗒1/72 us
void Delay_us(uint16_t us)
{
	//1.装载一个计数值,72 * us
	SysTick->LOAD = 72 * us;

	//2.配置，使用系统时钟（1），计数结束不产生中断（0），使能定时器（1）
	SysTick->CTRL |= 0x05;

	//3.等待计数值变为0，判断CTRL标志位COUNTFLAG是否为1，为1则计数结束
	while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG)==0)
	{}

	//4.关闭定时器
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE;

}

void Delay_ms(uint16_t ms)
{
	while(ms--){
		Delay_us(1000);
	}
}
