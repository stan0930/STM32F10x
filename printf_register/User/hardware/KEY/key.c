#include "key.h"
#include "led.h"

volatile uint8_t isOn = 0;
// KEY3（PF10）按键事件标志：中断发生时置 1，主循环处理后清 0
volatile uint8_t key3_event = 0;

void Dri_KEY_Init(void)
{
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    GPIOF->CRH &= ~GPIO_CRH_MODE10;
    GPIOF->CRH |= GPIO_CRH_CNF10_1;
    GPIOF->CRH &= ~GPIO_CRH_CNF10_0;
    GPIOF->ODR &= ~GPIO_ODR_ODR10;

    GPIOF->CRH &= ~GPIO_CRH_MODE8;
    GPIOF->CRH |= GPIO_CRH_CNF8_1;
    GPIOF->CRH &= ~GPIO_CRH_CNF8_0;
    GPIOF->ODR |= GPIO_ODR_ODR8;

    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PF;
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PF;



    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->IMR |= EXTI_IMR_MR10;

    EXTI->FTSR |= EXTI_FTSR_TR8;
    EXTI->IMR |= EXTI_IMR_MR8;

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(EXTI15_10_IRQn,3);
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn,3);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
    // PF10 对应 EXTI10，EXTI10~EXTI15 共用这个中断入口
    if (EXTI->PR & EXTI_PR_PR10)
    {
        // 写 1 清除 EXTI10 挂起标志
        EXTI->PR = EXTI_PR_PR10;

        // 中断中只记录按键事件，EEPROM 读写放在主循环完成
        key3_event = 1;
    }
}












void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR8)
    {
        EXTI->PR = EXTI_PR_PR8;
        Dri_Delay_Ms(10);

        if ((GPIOF->IDR & GPIO_IDR_IDR8) == 0)
        {
        //Dri_LED_Toggle(LED3);
            isOn = !isOn;
        }
    }
}
