//ADC1：单通道和DMA采集放在一份文件，函数名区分，两个模式选一个。
#include "adc1.h"
#include "delay.h"

//ADC1_IN10   PC0
void Dri_ADC1_Init(void){
    //ADC1 RCC 使能
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    //补充：切回单通道前先关闭转换和DMA请求
    ADC1->CR2 &=~ (ADC_CR2_ADON | ADC_CR2_DMA);
    //ADC1 预分频(不能超过14Mhz) ADCPRE 10 - 6分频
    RCC->CFGR |= RCC_CFGR_ADCPRE_1;
    RCC->CFGR &=~ RCC_CFGR_ADCPRE_0;

    //GPIOC RCC使能
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    //GPIOC工作模式：模拟输入  MODE0:00  CNF0:00
    GPIOC->CRL &=~ (GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    //ADC1的设置
    //1.设置扫描模式 当前只有一个通道需要转换，因此scan可以不设置
    ADC1->CR1 &=~ ADC_CR1_SCAN;
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
    //补充：上电后先等稳定，再复位校准寄存器
    Dri_Delay_Us(2);
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while(ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);

    //3.开始转换
    ADC1->CR2 |= ADC_CR2_SWSTART;

    //4.等待转换完成
    while ((ADC1->SR & ADC_SR_EOC)==0);
    
}
//下面是原多通道DMA写法，函数改名，按Word使用PC0/PC2两路

//ADC1_IN10   PC0
//ADC1_IN12   PC2
void Dri_ADC1_DMA_Init(void){
    //ADC1 RCC 使能
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    //补充：配置前关闭ADC，DMA通道也要先停再设置
    ADC1->CR2 &=~ (ADC_CR2_ADON | ADC_CR2_DMA);
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel1->CCR &=~ DMA_CCR1_EN;
    //ADC1 预分频(不能超过14Mhz) ADCPRE 10 - 6分频
    RCC->CFGR |= RCC_CFGR_ADCPRE_1;
    RCC->CFGR &=~ RCC_CFGR_ADCPRE_0;

    //GPIOC RCC使能
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    //GPIOC工作模式：模拟输入  MODE0:00  CNF0:00
    GPIOC->CRL &=~ (GPIO_CRL_MODE0 | GPIO_CRL_CNF0);//C0
    GPIOC->CRL &=~ (GPIO_CRL_MODE2 | GPIO_CRL_CNF2);//C2

    //ADC1的设置
    //1.设置扫描模式 当前有两个通道需要转换，因此开启scan
    ADC1->CR1 |= ADC_CR1_SCAN;
    //2.开启规则通道的外部触发转换模式
    ADC1->CR2 |= ADC_CR2_EXTTRIG;
    //3.设置选择启动规则通道组转换的外部事件 111-使用软件方式启动
    ADC1->CR2 |= ADC_CR2_EXTSEL;
    //4.通道转换之后的结果存储到数据寄存器（DR）,转换结果是12位，默认右对齐
    ADC1->CR2 &=~ ADC_CR2_ALIGN;
    //5.设置规则通道组是连续模式 - continuous
    ADC1->CR2 |= ADC_CR2_CONT;

    //6.设置ADC1——IN10通道的采样时间 - 010 - 13.5周期
    //采样时间有8种选择，我们使用3位就可以表示8种，因此每一个通道的采样时间需要使用3位来表示
    //因此我们需要两个寄存器来描述18个通道的采样时间
    ADC1->SMPR1 &=~ ADC_SMPR1_SMP10;
    ADC1->SMPR1 &=~ ADC_SMPR1_SMP12;
    ADC1->SMPR1 |= (ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP12_1);
    //7.将ADC1_IN10通道放入到规则通道组的第一个位置
    //通道的号码范围：0-17，号码范围需要使用5位来表示，32：5*6 因此一个寄存器只能容纳6个通道号码
    //因此一个寄存器只能容纳6个通道号码，因此我们需要3个寄存器 6(SQR3)+6(SQR2)+4(SQR1)
    //SQR1里面除了存放4个通道号之外，还记录了长度L
    ADC1->SQR3 &=~ ADC_SQR3_SQ1;//先将对应位置清0
    ADC1->SQR3 |= 10;           //再赋值10
    ADC1->SQR3 &=~ ADC_SQR3_SQ2;//先将对应位置清0
    ADC1->SQR3 |= 12<<5;           //第二个通道IN12，PC2
    ADC1->SQR3 &=~ ADC_SQR3_SQ3;//先将对应位置清0

    //8.指定当前规则通道组中的待转换通道的个数
    //0001表示有2个通道待转换
    ADC1->SQR1 &=~ ADC_SQR1_L;
    ADC1->SQR1|=ADC_SQR1_L_0;

    //DMA设置
    //RCC DMA使能
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    ADC1->CR2 |= ADC_CR2_DMA;
    //1.非存储器到存储器模式
    DMA1_Channel1->CCR &=~ DMA_CCR1_MEM2MEM;
    //2.设置MSIZE和PSIZE - 16位 - 01
    DMA1_Channel1->CCR &=~ DMA_CCR1_MSIZE;
    DMA1_Channel1->CCR |= DMA_CCR1_MSIZE_0;
    DMA1_Channel1->CCR &=~ DMA_CCR1_PSIZE;
    DMA1_Channel1->CCR |= DMA_CCR1_PSIZE_0;
    //3.设置地址是否自增长 MINC-增长 PINC-不增长
    DMA1_Channel1->CCR |= DMA_CCR1_MINC;
    DMA1_Channel1->CCR &=~ DMA_CCR1_PINC;
    //4.设置数据传输方向 0-从外设读（从ADC外设的DR里读）
    DMA1_Channel1->CCR &=~ DMA_CCR1_DIR;
    //5.每搬完2个数据，自动从数组开头继续搬运
    DMA1_Channel1->CCR |= DMA_CCR1_CIRC;

}
void Dri_ADC1_DMA_Start(uint32_t desAddr, uint8_t len){

    //补充：当前规则序列固定两路，数组长度传2
    if(desAddr == 0 || len != 2) return;
    DMA1_Channel1->CCR &=~ DMA_CCR1_EN;
    DMA1->IFCR = DMA_IFCR_CGIF1;

    //DMA设置:
    //1.外设地址
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    //2.存储器的地址
    DMA1_Channel1->CMAR = desAddr;
    //3.设置传输的数量
    DMA1_Channel1->CNDTR = len;
    //EN
    DMA1_Channel1->CCR |= DMA_CCR1_EN;

    //1. 上电：当该位为‘0’时，写入‘1’将把ADC从断电模式下唤醒
    ADC1->CR2 |= ADC_CR2_ADON;

    //2.校准：0校准完成 1正在校准
    //补充：上电后先等稳定，再复位校准寄存器
    Dri_Delay_Us(2);
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while(ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);

    //3.开始转换
    ADC1->CR2 |= ADC_CR2_SWSTART;

    //补充：DMA会读DR并清EOC，这里不再等待EOC，数据直接看数组
    
}
//单通道读取电压，参考电压按3.3V换算
double Dri_ADC1_ReadV(void){
    return 3.3*ADC1->DR/4095;
}
