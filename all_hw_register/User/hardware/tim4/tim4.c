//TIM4：PB6输入捕获，PA1可接PB6；周期/占空比两种初始化选一个。
#include "tim4.h"
#include "stm32f10x.h"

//周期测量：每次上升沿捕获后清CNT，下一次CCR1就是两次边沿之间的计数
void Dri_Tim4_Init(void){
    //1.RCC时钟使能
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
   
    //2.GPIOB工作模式 浮空输入 MODE:00 CNF:01
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1);
    GPIOB->CRL |= GPIO_CRL_CNF6_0;

    //补充：切换回周期测量时，关闭旧的PWM输入模式
    TIM4->CR1 &=~ TIM_CR1_CEN;
    TIM4->CCER &=~ (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC2P);
    TIM4->SMCR &=~ (TIM_SMCR_TS | TIM_SMCR_SMS);

    //Tim4设置
    //3.不需要自动重装载的预加载使能：向上计数：禁用单脉冲模式
    TIM4->CR1 &= ~(TIM_CR1_ARPE | TIM_CR1_DIR | TIM_CR1_OPM);

    //4.设置TI1的来源,直接来源于CH1
    TIM4->CR2 &= ~TIM_CR2_TI1S;
    
    //5.设置Tim4的捕获中断使能
    TIM4->DIER |= TIM_DIER_CC1IE;
    //6.设置IC1的来源：TI1 - 01
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_1;

    //7.当前Tim4的信号来源：不滤波，不预分频
    TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC);

    //8. Tim4输入通道配置为上升沿捕获
    TIM4->CCER &=~ TIM_CCER_CC1P;
    //9. Tim4输入通道配置为捕获模式
    TIM4->CCER |= TIM_CCER_CC1E;

    //Tim4的时基单元配置
    //进行72分频，从而时基单元的工作频率是1mhz
    //因此每计数一次就是1us

    TIM4 -> PSC = 72-1; 
    TIM4 -> CNT = 0;
    //Tim4的ARR设置为最大值，目的尽量不要溢出
    //例如：第一次捕获CNT=1；第二次捕获CNT=101，
    //如果没有溢出的情况，因此周期就是100，但是如果有溢出，则周期不确定
    TIM4 -> ARR = UINT16_MAX;

    //补充：让PSC立即生效，清掉旧标志和旧捕获值
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR = 0;
    TIM4->CCR1 = 0;

    //当前Tim4是输入捕获模式，不需要设置CCR1的值

    //Tim4我们设置了捕获使能，因此需要进行NVIC的设置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(TIM4_IRQn, 3);
    NVIC_EnableIRQ(TIM4_IRQn);

}

void Dri_Tim4_Start(void){
    TIM4->CR1 |= TIM_CR1_CEN;
}

void TIM4_IRQHandler(void){
    //清除中断标志位
    TIM4->SR &= ~TIM_SR_CC1IF;

    TIM4->CNT = 0;

}

//占空比测量：保留原pwm_duty工程的初始化写法，只改函数名
void Dri_Tim4_PWMInput_Init(void){
    //1.RCC时钟使能
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
   
    //2.GPIOB工作模式 浮空输入 MODE:00 CNF:01
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1);
    GPIOB->CRL |= GPIO_CRL_CNF6_0;

    //补充：这个模式不使用捕获中断，切换前先关闭旧配置
    NVIC_DisableIRQ(TIM4_IRQn);
    TIM4->CR1 &=~ TIM_CR1_CEN;
    TIM4->DIER &=~ TIM_DIER_CC1IE;
    TIM4->CCER &=~ (TIM_CCER_CC1E | TIM_CCER_CC2E);

    //Tim4设置
    //3.不需要自动重装载的预加载使能：向上计数：禁用单脉冲模式
    TIM4->CR1 &= ~(TIM_CR1_ARPE | TIM_CR1_DIR | TIM_CR1_OPM);

    //4.设置TI1的来源,直接来源于CH1
    TIM4->CR2 &= ~TIM_CR2_TI1S;
    
    // //5.设置Tim4的捕获中断使能
    // TIM4->DIER |= TIM_DIER_CC1IE;

    //6.设置IC1的来源：TI1 - 01
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM4->CCMR1 &= ~TIM_CCMR1_CC1S_1;

    //设置IC2的来源：TI1 - 10
    TIM4->CCMR1 |=TIM_CCMR1_CC2S_1;
    TIM4->CCMR1 &= ~TIM_CCMR1_CC2S_0;

    //7.当前Tim4的信号来源：不滤波，不预分频
    TIM4->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC1PSC);
    //CH2也不预分频
    TIM4->CCMR1 &= ~TIM_CCMR1_IC2PSC;

    //8. Tim4的CH1输入通道配置为上升沿捕获 - 极性设置
    TIM4->CCER &=~ TIM_CCER_CC1P;
    //Tim4的CH2输入通道配置为下降沿捕获
    TIM4->CCER |= TIM_CCER_CC2P;

    //9. Tim4的输入通道（CH1和CH2）捕获使能
    TIM4->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

    //Tim4的时基单元配置
    //进行72分频，从而时基单元的工作频率是1mhz
    //因此每计数一次就是1us

    TIM4 -> PSC = 72-1; 
    TIM4 -> CNT = 0;
    //Tim4的ARR设置为最大值，目的尽量不要溢出
    //例如：第一次捕获CNT=1；第二次捕获CNT=101，
    //如果没有溢出的情况，因此周期就是100，但是如果有溢出，则周期不确定
    TIM4 -> ARR = UINT16_MAX;
    //补充：装载PSC，清旧标志和旧捕获值
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR = 0;
    TIM4->CCR1 = 0;
    TIM4->CCR2 = 0;

    //当前Tim4可以接受触发信号，因此Tim4是从定时器，出发后需要做复位指令。因此需要设置SMR中的TS和SMS
    //触发信号来源的设置 触发信号来源于TI1FP1  TS- 101
    TIM4->SMCR |= (TIM_SMCR_TS_2 | TIM_SMCR_TS_0);
    TIM4->SMCR &=~ TIM_SMCR_TS_1;

    //从模式选择：触发信号到来时CNT复位 SMS - 100
    TIM4->SMCR |= TIM_SMCR_SMS_2;
    TIM4->SMCR &=~ (TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);

    

}

void Dri_Tim4_Stop(void){
    TIM4->CR1 &=~ TIM_CR1_CEN;
}

//周期：每次计数1us，换算成ms
double Dri_Tim4_GetPWMCycle(void){
    return 1.0*TIM4->CCR1 / 1000;
}

//频率：先检查有没有捕获值，防止除0
double Dri_Tim4_GetPWMFreq(void){
    if(TIM4->CCR1 == 0) return 0;
    return 1000000.0 / TIM4->CCR1;
}

//占空比：只在PWMInput模式下使用，返回0~1，乘100得到百分数
double Dri_Tim4_GetDutyCycle(void){
    if(TIM4->CCR1 == 0) return 0;
    return 1.0*TIM4->CCR2 / TIM4->CCR1;
}
