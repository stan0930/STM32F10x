#include "spi.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>  
#include "w25q32.h"


int main(){
    Dri_USART_Init();
    Int_W25Q32_Init();
    printf("测试spi\n");
    // uint8_t mid = 0;
    // uint16_t did = 0;
    // Int_W25Q32_ReadId(&mid, &did);
    // printf("服务商ID = %X, 设备ID = %X\n",mid,did);
    // Dri_SPI_Stop();
    Int_W25Q32_SectorErase(0x00,0x01);
    Int_W25Q32_WritePage(0x00,0x01,0x00,"helloworld",10);
    uint8_t arr[11];
    Int_W25Q32_Read(0x00,0x01,0x00,arr,10);
    printf("数据：");
    for (uint8_t i = 0; i < 10; i++) {
    printf("%02X ", arr[i]);
    }
    printf("\r\n");
    while (1)
    {
        
    }
}
