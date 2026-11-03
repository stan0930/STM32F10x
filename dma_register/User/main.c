#include "I2C.h"
#include "m24c02.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>  
#include "dma.h"

extern uint8_t tcFlag;
 int main(void){
    Dri_USART_Init();
    Dri_DMA1_Trans_Init();
    Dri_DMA1_Rec_Init();

    uint8_t *str = "hello world";
    uint8_t arr[11]={0};

    Dri_DMA1_TransmitData((uint32_t)str,(uint32_t)&USART1->DR,11);
    Dri_DMA1_ReceiveData((uint32_t)&USART1->DR, (uint32_t)arr, 10);

    while(1){
        if(tcFlag){
            tcFlag = 0;
            printf("%s",arr);
        }
    }
}