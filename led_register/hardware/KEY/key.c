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

    //2.GPIO工作模式配置，PF8：CNF - 10， MODE - 00
    GPIOF->CRH &= ~GPIO_CRH_MODE8;//清除PF8的MODE位  0 0
    GPIOF->CRH |= GPIO_CRH_CNF8_1;//设置PF8的CNF位  1 0
    GPIOF->CRH &= ~GPIO_CRH_CNF8_0;
    GPIOF->ODR |= GPIO_ODR_ODR8;//设置PF8为高电平

    //3.AFIO配置引脚复用选择
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PF;//选择PF10为EXTI10
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PF;//选择PF8为EXTI8



    //4.配置EXTI
    EXTI->RTSR |= EXTI_RTSR_TR10;//设置EXTI10为上升沿触发RTSR
    EXTI->IMR |= EXTI_IMR_MR10;//中断屏蔽寄存器要配1

    EXTI->FTSR |= EXTI_FTSR_TR8;//设置EXTI8为下降沿触发FTSR
    EXTI->IMR |= EXTI_IMR_MR8;//中断屏蔽寄存器要配1

    //5.NVIC配置
    NVIC_SetPriorityGrouping(3);//全部都是抢占优先级
    NVIC_SetPriority(EXTI15_10_IRQn,3);//设置EXTI10_IRQn的优先级为0
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn,3);//设置EXTI8_IRQn的优先级为0
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}
//去startup_stm32f10x_hd.s查中断服务程序的名字

//中断服务程序
// void EXTI15_10_IRQHandler(void)
// {
//     //挂起位清零
//     EXTI->PR |= EXTI_PR_PR8;//置1

//     //防抖延时
//     Delay_ms(10);

//     //判断如果依然保持低电平，就翻转LED3（按下为低电平）
//     if((GPIOF->IDR & GPIO_IDR_IDR8) ==0)//如果PF8为低电平
//     {
//         LED_Toggle(LED3);//翻转LED3
//     }
// }


// //中断服务程序
// void EXTI9_5_IRQHandler(void)
// {
//     //挂起位清零
//     EXTI->PR |= EXTI_PR_PR8;//置1

//     //防抖延时
//     Delay_ms(10);

//     //判断如果依然保持高电平，就翻转LED1
//     if((GPIOF->IDR & GPIO_IDR_IDR10) !=0)//如果PF10为低电平
//     {
//         LED_Toggle(LED1);//翻转LED1
//     }
// }

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR10)
    {
        EXTI->PR = EXTI_PR_PR10;
        Delay_ms(10);

        if (GPIOF->IDR & GPIO_IDR_IDR10)
        {
            LED_Toggle(LED1);
        }
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR8)
    {
        EXTI->PR = EXTI_PR_PR8;
        Delay_ms(10);

        if ((GPIOF->IDR & GPIO_IDR_IDR8) == 0)
        {
            LED_Toggle(LED3);
        }
    }
}