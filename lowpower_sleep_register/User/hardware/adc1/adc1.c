#include "adc1.h"


//ADC1_IN10   PC0
void Dri_ADC1_Init(void){
    //ADC1 RCC 使能
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    //ADC1 预分频(不能超过14Mhz) ADCPRE 10 - 6分频
    RCC->CFGR |= RCC_CFGR_ADCPRE_1;
    RCC->CFGR &=~ RCC_CFGR_ADCPRE_0;

    //GPIOC RCC使能
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    //GPIOC工作模式：模拟输入  MODE0:00  CNF0:00
    GPIOC->CRL &=~ (GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    //ADC1的设置
    //1.设置扫描模式 当前只有一个通道需要转换，因此scan可以不设置
    ADC1->CR1 |= ADC_CR1_SCAN;
    //2.开启规则通道的外部触发转换模式
    ADC1->CR2 |= ADC_CR2_EXTTRIG;
    //3.设置选择启动规则通道组转换的外部事件 111-使用软件方式启动
    ADC1->CR2 |= ADC_CR2_EXTSEL;
    //4.通道转换之后的结果存储到数据寄存器（DR）,转换结果是12位，默认右对齐
    ADC1->CR2 &=~ ADC_CR2_ALIGN;
    //5.设置规则通道组是连续模式 - continuous
    ADC1->CR2 |= ADC_CR2_CONT;

    //6.设置ADC1——IN10通道的采样时间 - 000 - 1.5周期
    //采样时间有8种选择，我们使用3位就可以表示8种，因此每一个通道的采样时间需要使用3位来表示
    //因此我们需要两个寄存器来描述18个通道的采样时间
    ADC1->SMPR1 &=~ ADC_SMPR1_SMP10;

    //7.将ADC1_IN10通道放入到规则通道组的第一个位置
    //通道的号码范围：0-17，号码范围需要使用5位来表示，32：5*6 因此一个寄存器只能容纳6个通道号码
    //因此一个寄存器只能容纳6个通道号码，因此我们需要3个寄存器 6(SQR3)+6(SQR2)+4(SQR1)
    //SQR1里面除了存放4个通道号之外，还记录了长度L
    ADC1->SQR3 &=~ ADC_SQR3_SQ1;//先将对应位置清0
    ADC1->SQR3 |= 10;           //再赋值10
    //8.指定当前规则通道
    ADC1->SQR1 &=~ ADC_SQR1_L; //0000表示有一个通道待转换




}
void Dri_ADC1_StartConvert(void){
    //1. 上电：当该位为‘0’时，写入‘1’将把ADC从断电模式下唤醒
    ADC1->CR2 |= ADC_CR2_ADON;

    //2.校准：0校准完成 1正在校准
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);

    //3.开始转换
    ADC1->CR2 |= ADC_CR2_SWSTART;

    //4.等待转换完成
    while ((ADC1->SR & ADC_SR_EOC)==0);
    
}