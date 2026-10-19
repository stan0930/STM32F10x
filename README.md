# STM32 工程

用于保存 STM32 学习过程中编写的实验工程，各实验放在独立目录。

## 当前工程

| 目录             | 内容                            | 芯片        |
| ---------------- | ------------------------------- | ----------- |
| `led_register` | 使用寄存器配置 GPIO 的 LED 实验 | STM32F103ZE |

## 打开与编译

使用 Keil µVision 打开 `led_register/led_register.uvprojx`，安装工程需要的 STM32F1 设备支持包和 ARM 编译器后重新构建。

当前工程配置使用 ARM Compiler 5.06 update 6、设备支持包 `Keil.STM32F1xx_DFP.2.4.1`。调试和下载设置需与实际开发板及调试器匹配。

源码、启动文件、`.uvprojx`、`.uvoptx` 及调试配置纳入版本管理。`Objects`、`Listings` 等生成目录和个人界面布局由 `.gitignore` 排除，克隆后需要重新编译。
