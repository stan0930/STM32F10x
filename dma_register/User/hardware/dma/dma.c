#include "dma.h"

//将SRAM中的数据发送到USART1 uasrt1_tx发送-ch4
void Dri_DMA1_Trans_Init(void){
    //1.RCC使能
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    //DMA设置
    //2.设置 非存储器到存储器模式 -0
    DMA1_Channel4->CCR &=~ DMA_CCR4_MEM2MEM;
    //3.当前通道优先级可以不设置，使用默认
    //4.存储器数据宽度和外设数据宽度都设置为8位 - 00
    DMA1_Channel4->CCR &=~ (DMA_CCR4_PSIZE | DMA_CCR4_MSIZE);
    //5.存储器地址自增
    DMA1_Channel4->CCR |= DMA_CCR4_MINC;
    //6.外设地址自增
    DMA1_Channel4->CCR &=~ DMA_CCR4_PINC;
    //7.不需要循环模式
    DMA1_Channel4->CCR &=~ DMA_CCR4_CIRC;
    //8.从存储器读
    DMA1_Channel4->CCR |= DMA_CCR4_DIR;
    //9.传输完成中断使能
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE;
    //10. 开启USART1中的DMA发送使能
    USART1->CR3 |= USART_CR3_DMAT;

    //11.NVIC设置
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(DMA1_Channel4_IRQn,3);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

//读uasrt1接收的数据 usart1_rx接收 ch5
void Dri_DMA1_Rec_Init(void){
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CCR &=~ DMA_CCR5_MEM2MEM;
    DMA1_Channel5->CCR &=~ (DMA_CCR5_PSIZE | DMA_CCR4_MSIZE);
    DMA1_Channel5->CCR |= DMA_CCR5_MINC;
    DMA1_Channel5->CCR &=~ DMA_CCR5_PINC;
    DMA1_Channel5->CCR &=~ DMA_CCR5_CIRC;
    //方向从USART读到存储器,ch5才是接收usart1的
    DMA1_Channel5->CCR &=~ DMA_CCR5_DIR;
    //中断使能
    DMA1_Channel5->CCR |= DMA_CCR5_TCIE;
    //开启USART1中的DMA接收使能
    USART1->CR3 |= USART_CR3_DMAR;

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(DMA1_Channel5_IRQn,3);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);


}

void Dri_DMA1_TransmitData(uint32_t scrAddr, uint32_t desAddr,uint16_t dataLen){
    //设置数据源地址
    DMA1_Channel4->CMAR = scrAddr;
    //设置数据目的地址
    DMA1_Channel4->CPAR = desAddr;
    //设置数据传输长度
    DMA1_Channel4->CNDTR = dataLen;
    //使能通道
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
}

void Dri_DMA1_ReceiveData(uint32_t scrAddr, uint32_t desAddr, uint16_t dataLen){
    DMA1_Channel5->CMAR = desAddr;
    DMA1_Channel5->CPAR = scrAddr;
    DMA1_Channel5->CNDTR = dataLen;
    //使能通道
    DMA1_Channel5->CCR |= DMA_CCR5_EN;
}

uint8_t tcFlag =0;
void DMA1_Channel5_IRQHandler(void){
    if(DMA1->ISR & DMA_ISR_TCIF5){
        //清除标志位
        DMA1->IFCR |= DMA_IFCR_CTCIF5;
        tcFlag = 1;
    }
}

// void DMA1_Channel4_IRQHandler(void)
// {
//     if (DMA1->ISR & DMA_ISR_TCIF4)
//     {
//         DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
//         DMA1->IFCR = DMA_IFCR_CTCIF4;
//     }
// }