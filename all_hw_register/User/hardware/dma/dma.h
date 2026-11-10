//USART1 DMA：通道4发送、通道5接收，先初始化USART，再初始化DMA。
#ifndef DMA_H
#define DMA_H
#include "stm32f10x.h"

//接收完成标志，中断置1，主循环处理后清0
extern volatile uint8_t tcFlag;

void Dri_DMA1_Trans_Init(void);
void Dri_DMA1_Rec_Init(void);
void Dri_DMA1_TransmitData(uint32_t scrAddr, uint32_t desAddr,uint16_t dataLen);
void Dri_DMA1_ReceiveData(uint32_t scrAddr, uint32_t desAddr,uint16_t dataLen);
#endif

//实验一：RAM → 串口，DMA 发送
/*
#include "dma.h"
#include "usart.h"

int main(void){
    //发送数组，末尾的字符串结束符不发送
    static uint8_t str[] = "hello world\r\n";

    //1.初始化串口
    Dri_USART_Init();

    //2.初始化DMA发送：DMA1通道4
    Dri_DMA1_Trans_Init();

    //3.设置内存地址、外设地址、发送数量，并启动传输
    Dri_DMA1_TransmitData(
        (uint32_t)str,
        (uint32_t)&USART1->DR,
        sizeof(str) - 1
    );

    while(1){
        //DMA自动搬运，完成后进入通道4中断
    }
}
*/


//实验二：串口 → RAM，DMA 接收
//串口助手发送 → USART1->DR → DMA1通道5 → arr数组→ 收满10字节 → 完成中断 → tcFlag=1 → main打印
/*
#include "dma.h"
#include "usart.h"
#include <stdio.h>

int main(void){
    //接收10字节，多留1字节放字符串结束符
    static uint8_t arr[11] = {0};

    //1.初始化串口
    Dri_USART_Init();

    //2.初始化DMA接收：DMA1通道5
    Dri_DMA1_Rec_Init();

    //3.清除接收完成标志
    tcFlag = 0;

    //4.设置外设地址、内存地址、接收数量，并启动接收
    Dri_DMA1_ReceiveData(
        (uint32_t)&USART1->DR,
        (uint32_t)arr,
        10
    );

    while(1){
        //收到10字节后，中断会将tcFlag置1
        if(tcFlag){
            tcFlag = 0;

            //补上结束符，才能按字符串打印
            arr[10] = '\0';
            printf("收到：%s\r\n", arr);
        }
    }
}
*/