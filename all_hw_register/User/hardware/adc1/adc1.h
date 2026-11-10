//ADC1：单通道和DMA采集放在一份文件，函数名区分，两个模式选一个。
#ifndef ADC1_H
#define ADC1_H

#include "stm32f10x.h"

//单通道：PC0，初始化后启动连续转换
void Dri_ADC1_Init(void);
void Dri_ADC1_StartConvert(void);
double Dri_ADC1_ReadV(void);           //返回电压，V

//DMA：PC0、PC2，两个初始化方案选一个
void Dri_ADC1_DMA_Init(void);
//desAddr传(uint32_t)data，data是持续有效的uint16_t data[2]，len传2
void Dri_ADC1_DMA_Start(uint32_t desAddr, uint8_t len);

#endif
