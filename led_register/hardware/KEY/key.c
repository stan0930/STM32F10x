#include "key.h"
#include "led.h"

void key_init(void)
{
    // 1.配置时钟
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;//要去stm32f10x.h中把#define STM32F10X_HD打开才有F端口
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;//开启引脚复用选择器

    //2.GPIO工作模式配置，PF10：CNF - 10， MODE - 00
    GPIOF->CRH &= ~GPIO_CRH_MODE10;//清除PF10的MODE位  0 0
    GPIOF->CRH |= GPIO_CRH_CNF10_1;//设置PF10的CNF位  1 0
    GPIOF->CRH &= ~GPIO_CRH_CNF10_0;

    GPIOF->ODR &= ~GPIO_ODR_ODR10;//设置PF10为低电平

    //3.AFIO配置引脚复用选择
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PF;//选择PF10为EXTI10

    //4.配置EXTI
    EXTI->RTSR |= EXTI_RTSR_TR10;//设置EXTI10为上升沿触发
    EXTI->IMR |= EXTI_IMR_MR10;//中断屏蔽寄存器要配1

    //5.NVIC配置
    NVIC_SetPriorityGrouping(3);//全部都是抢占优先级
    NVIC_SetPriority(EXTI15_10_IRQn,3);//设置EXTI10_IRQn的优先级为0
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

//中断服务程序
void EXTI15_10_IRQHandler(void)
{
    //挂起位清零
    EXTI->PR |= EXTI_PR_PR10;//置1

    //防抖延时
    Delay_ms(10);

    //判断如果依然保持高电平，就翻转LED1
    if((GPIOF->IDR & GPIO_IDR_IDR10) !=0)//如果PF10为低电平
    {
        LED_Toggle(LED1);//翻转LED1
    }
}
