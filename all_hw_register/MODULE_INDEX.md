# STM32 实操考试代码总索引

所有可复用寄存器驱动都集中在 `User/hardware` 下，每个功能一个文件夹；根目录的 `all_hw_register.uvprojx` 是当前可直接编译的硬件 SPI + W25Q32 示例。

| 功能 | 文件夹 |
|---|---|
| 公共延时 | `User/hardware/UTIL` |
| USART1 | `User/hardware/USART` |
| ADC 单通道 | `User/hardware/adc_single` |
| ADC 多通道 DMA | `User/hardware/adc_dma` |
| USART DMA | `User/hardware/dma_usart` |
| I2C | `User/hardware/i2c` |
| AT24C02 常用实现 | `User/hardware/eeprom_m24c02` |`r`n| AT24C02 接口实现 | `User/hardware/eeprom_m24c02_interface` |
| 按键 | `User/hardware/key` |
| LED | `User/hardware/led` |
| PWM 周期测量 TIM4 | `User/hardware/pwm_cycle_tim4` |
| PWM 周期输出 TIM5 | `User/hardware/pwm_cycle_tim5` |
| PWM 占空比测量 TIM4 | `User/hardware/pwm_duty_tim4` |
| PWM 占空比输出 TIM5 | `User/hardware/pwm_duty_tim5` |
| 软件 SPI | `User/hardware/spi_soft` |
| 硬件 SPI | `User/hardware/spi` |
| W25Q32 硬件 SPI | `User/hardware/w25q32` |`r`n| W25Q32 软件 SPI | `User/hardware/w25q32_soft` |
| TIM1 单脉冲 | `User/hardware/tim1` |
| TIM1 中断 | `User/hardware/tim1` 中的 `tim1_IT.c` |
| TIM5 呼吸灯 | `User/hardware/tim5_breathe` |
| TIM6 基本定时器中断 | `User/hardware/tim6` |

使用时直接打开根工程，根据题目把 `User/main.c` 中的头文件和调用换成对应功能目录即可。不同实现的文件名可能相同，所以目录名称已经区分清楚；同一时刻只把需要的实现加入当前 Target，避免重复定义。

