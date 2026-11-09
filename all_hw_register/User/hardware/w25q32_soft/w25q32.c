#include "w25q32.h"
#include "spi.h"


void Int_W25Q32_Init(void){
    Dri_SPI_init();
}
void Int_W25Q32_ReadId(uint8_t *mid, uint16_t *did){
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x9f);//发送9f命令，读取ID
    *mid = Dri_SPI_SwapByte(0xFF);//服务商ID
    uint8_t did1 = Dri_SPI_SwapByte(0x00);//设备ID1
    uint8_t did2 = Dri_SPI_SwapByte(0x00);//设备ID2
    *did = (did1<<8) | did2;
    Dri_SPI_Stop();
}
void Int_W25Q32_WriteEnable(void){
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x06);//发送06命令，使能写入
    Dri_SPI_Stop();
}
void Int_W25Q32_WaitNotBusy(void){
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x05);//发送ff命令，读取状态寄存器
    while (Dri_SPI_SwapByte(0xFF) & 0x01);//可以一直读取状态寄存器，直到写入完成位为0
    Dri_SPI_Stop();
}
void Int_W25Q32_WriteDisanable(void){
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x04);//发送04命令，关闭写使能
    Dri_SPI_Stop();
}
void Int_W25Q32_SectorErase(uint8_t block, uint8_t sector){
    //1.等待空闲
    Int_W25Q32_WaitNotBusy();
    //2.写使能
    Int_W25Q32_WriteEnable();
    //3.开始进行段擦除
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x20);
    Dri_SPI_SwapByte(block);
    Dri_SPI_SwapByte(sector<<4);//发送扇区号
    Dri_SPI_SwapByte(0xFF);
    Dri_SPI_Stop();
    //4.关闭写使能
    Int_W25Q32_WriteDisanable();
}
void Int_W25Q32_WritePage(uint8_t block, uint8_t sector, uint8_t page, 
uint8_t *data, uint16_t len){
    //1.等待空闲
    Int_W25Q32_WaitNotBusy();
    //2.写使能
    Int_W25Q32_WriteEnable();
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x02);//发送02命令，写入页
    Dri_SPI_SwapByte(block);
    Dri_SPI_SwapByte(sector<<4 | page);//发送扇区号
    Dri_SPI_SwapByte(0x00);
    for (uint16_t i = 0; i < len; i++){
        Dri_SPI_SwapByte(data[i]);
    }

    Dri_SPI_Stop();
    Int_W25Q32_WriteDisanable();
}
void Int_W25Q32_Read(uint8_t block, uint8_t sector, uint8_t page, 
uint8_t *data, uint16_t len){
    Int_W25Q32_WaitNotBusy();
    Dri_SPI_Start();
    Dri_SPI_SwapByte(0x03);//发送03命令，读取页
    Dri_SPI_SwapByte(block);
    Dri_SPI_SwapByte(sector<<4 | page);//发送扇区号
    Dri_SPI_SwapByte(0x00);
    for (uint16_t i = 0; i < len; i++){
        data[i] = Dri_SPI_SwapByte(0xFF);
    }
    Dri_SPI_Stop();
}