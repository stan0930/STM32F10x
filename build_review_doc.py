from pathlib import Path
import re
from docx import Document
from docx.shared import Cm, Pt, RGBColor
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT

BASE=Path(__file__).resolve().parent
OUT=BASE/'all_hw_register'/'STM32第1至4章代码复习顺序表.docx'
TASKS=[]
def task(ch,title,files,goal,rows,flow,formula,pitfall,check):
    TASKS.append(dict(ch=ch,title=title,files=files,goal=goal,rows=rows,flow=flow,formula=formula,pitfall=pitfall,check=check))

task(1,'SysTick 延时',['UTIL/delay.c','UTIL/delay.h'],
 '从最底层的微秒延时开始，理解计数器如何产生时间间隔。',[
 ('Dri_Delay_Us()','先看 us=0 的处理，再看 LOAD、VAL、CTRL 和 COUNTFLAG。72MHz 下 72 个时钟约为 1μs。'),
 ('Dri_Delay_Ms()','一毫秒调用一次 1000μs 延时，理解 while(ms--) 的循环次数。'),
 ('Dri_Delay_S()','一秒调用一次 1000ms 延时，理解三层函数的复用关系。')],
 'main 调用毫秒或秒延时 → 拆成微秒延时 → LOAD 装入计数值 → VAL 清零 → 启动 SysTick → 轮询 COUNTFLAG → 关闭计数器。',
 'LOAD = 72 × us − 1；CTRL=0x05：CLKSOURCE=1、TICKINT=0、ENABLE=1。',
 '这是忙等延时。SysTick 与 TIM5/TIM6 是独立硬件，但当前函数会覆盖 SysTick 配置，不能和 HAL 的 SysTick 时基并用。手册的 SysTick 中断闪灯是另一种用法：设置 TICKINT 并实现 SysTick_Handler；当前目录没有该驱动。',
 '能解释为什么 LOAD 减 1、为什么写 VAL=0，以及为什么 CPU 延时期间 TIM5 仍能输出 PWM。')

task(1,'TIM6 定时中断',['tim6/tim6.c','tim6/tim6.h','led/led.c'],
 '用基本定时器每隔约 1s 触发一次中断，通过回调完成 LED 翻转。',[
 ('Dri_LED_Init()','先看 PA0/PA1/PA8 的推挽输出和低电平点亮，为 LED 实验做准备。'),
 ('Dri_Tim6_Init()','开启时钟；设置 PSC、ARR、DIER.UIE；UG 装载分频值后清 UIF；配置 NVIC。'),
 ('Dri_Tim6_Start()','设置 CR1.CEN=1，计数器开始工作。'),
 ('TIM6_IRQHandler()','更新事件产生后进入中断，清 UIF，再调用业务回调；不需要再次清 CNT。'),
 ('Dri_Tim6_Callback()','驱动提供空的弱定义；在 main 中定义同名普通函数，如调用 Dri_LED_Toggle(LED1)。')],
 '初始化 LED 和 TIM6 → Start → CNT 数到 ARR → 下一计数时钟硬件回到 0 并置 UIF → NVIC 响应 → IRQHandler → Callback → LED 翻转。',
 'T = (PSC+1) × (ARR+1) / 72MHz = 7200 × 10000 / 72MHz = 1s。',
 'TIM6 没有 CCR/PWM 通道。回调仍在中断上下文中；复杂业务只置标志，交给主循环。不要把 __weak 加到 main 中的覆盖实现上。',
 '能说清“外设允许中断 UIE”和“NVIC 允许中断”的区别，并能说明 CNT 自动归零与清 UIF 是两件事。')

task(1,'TIM5 PWM 与呼吸灯',['tim5/tim5.c','tim5/tim5.h','UTIL/delay.c'],
 '让 TIM5 自动输出 PWM，主循环逐步修改比较值形成明暗变化。',[
 ('Dri_Tim5_Init()','PA1 配复用推挽；CC2S=00；OC2M=110 为 PWM1；CC2P=0；CC2E=1；设置 PSC、ARR、CCR2。'),
 ('Dri_Tim5_Start()','CEN=1 后硬件连续计数和输出，不需要主循环逐次翻转 GPIO。'),
 ('Dri_Tim5_SetDuty()','传入 0~100，函数将参数写入 CCR2；当前 ARR=99，可直接理解为高电平占空比百分数。'),
 ('Dri_Delay_Ms()','每次调整后等待约 20ms，让占空比缓慢变化；延时不会停止硬件 PWM。'),
 ('Dri_Tim5_Stop()','清 CEN 停止计数；没有强制引脚回到 LED 熄灭电平。')],
 'Init → Start → 设置 CCR2 → 等待 20ms → CCR2 逐步增大 → 到上限反向 → 逐步减小 → 到下限反向，循环。',
 'f = 72MHz / (7200 × 100) = 100Hz；高电平占空比 = CCR2 / (ARR+1)。',
 'PWM1 向上计数、正常极性时 CNT<CCR2 输出高。板上 LED 是低电平点亮，所以 CCR2 增大时 LED 变暗。GPIO 的 50MHz 速度等级不是 PWM 频率。',
 '能解释 MODE/CNF、CC2S、OC2M、CC2P、CC2E，能说明 CNT 到 CCR2 只改变输出，到 ARR 溢出才归零。')

task(1,'TIM4 测量周期和频率',['tim4/tim4.c','tim4/tim4.h','tim5/tim5.c'],
 '先读当前正在复习的上升沿中断方案，理解 CNT 与 CCR1 的关系。',[
 ('Dri_Tim4_Init()','选择上升沿捕获，开启通道 1 捕获中断，配置 NVIC。'),
 ('Dri_Tim4_Init()','继续按原顺序看：PB6 浮空输入，PSC=71，每次计数 1μs；ARR 取最大值，最后配置 NVIC。'),
 ('Dri_Tim4_Start()','CEN=1 开始计数；测试信号由 Dri_Tim5_Init()/Start() 输出。'),
 ('TIM4_IRQHandler()','恢复原写法：每次上升沿锁存 CCR1；中断里清捕获标志并清 CNT，重新开始计时。'),
 ('Dri_Tim4_GetPWMCycle()','周期计数除以 1000，得到 ms。'),
 ('Dri_Tim4_GetPWMFreq()','1000000 除以周期计数，得到 Hz；计数为 0 时返回 0，防止除零。')],
 'PA1 输出 PWM → 接 PB6 → 上升沿锁存 CCR1 → 中断清 CNT → 下一上升沿再次锁存 CCR1 → main 直接读 CCR1 换算。',
 '例如 CCR1=10000：周期约 10ms，频率约 100Hz。',
 '首次捕获不是完整周期，至少等待两个上升沿再读。软件清 CNT 有中断延迟误差。周期需小于 65.536ms，未实现溢出扩展；断线后旧值不会自动清零。',
 '能区分“硬件捕获 CCR1”和“CPU 进入中断”，能说明为什么这里需要软件清 CNT，而 TIM6 更新中断不需要。')

task(1,'TIM4 同时测量占空比',['tim4/tim4.c','tim4/tim4.h'],
 '在同一根 PB6 信号上用两个捕获通道，得到周期与高电平持续时间。',[
 ('Dri_Tim4_PWMInput_Init()','用这个函数替代 Dri_Tim4_Init，选择硬件 PWM 输入模式。'),
 ('Dri_Tim4_PWMInput_Init()','本函数直接配置 PB6 和 1MHz 计数时基；关闭旧中断和通道，清旧模式与状态。'),
 ('Dri_Tim4_PWMInput_Init()','继续看 CC1S=01、CC2S=10，两路都映射到 TI1；CH1 上升沿、CH2 下降沿；TS=101、SMS=100。'),
 ('Dri_Tim4_Start()','启动后由输入上升沿硬件复位 CNT；该模式不依赖 TIM4_IRQHandler。'),
 ('Dri_Tim4_GetPWMCycle()','从 CCR1 读取周期；GetPWMFreq 再计算频率。'),
 ('Dri_Tim4_GetDutyCycle()','返回 CCR2/CCR1，即高电平占空比；乘 100 得到百分数。')],
 '上升沿 → CCR1 锁存前一周期计数，同时硬件复位 CNT → 下降沿 → CCR2 锁存高电平持续时间 → 下一上升沿 → 更新周期 → main 查询。',
 'CCR1=10000、CCR2=6000：周期 10ms、频率 100Hz、高电平占空比约 60%。',
 '接线仍为 PA1→PB6，不需要第二根信号线。Init 和 PWMInput_Init 选一种。需等完整波形后再读；当前实现不保证两次寄存器读取来自同一周期，不支持恒高/恒低的 100%/0% 测量。',
 '能说清 IC1 与 IC2 为什么都能来自 PB6，以及这里 CNT 复位是触发从模式实现，不是“到 CCR 自动归零”。')

task(1,'TIM1 输出有限个 PWM 周期',['tim1/tim1.c','tim1/tim1.h'],
 '复习高级定时器的 RCR、主输出使能 MOE，以及两种停止方式。',[
 ('Dri_Tim1_IT_Init()','先看原中断停机方案：直接配置全部寄存器，关闭 OPM，开启更新中断。'),
 ('Dri_Tim1_IT_Init()','继续看 PA8 复用推挽，PSC=7199、ARR=4999、CCR1=2500、RCR=4、BDTR.MOE=1，UG 后等 UIF 并清除。'),
 ('Dri_Tim1_IT_Start()','调用 Start 设置 CEN，开始输出 2Hz PWM。'),
 ('TIM1_UP_IRQHandler()','RCR 使每 5 次计数溢出产生一次更新；中断里直接清 UIF 和 CEN。'),
 ('Dri_Tim1_Stop()','清 CEN 停止，达到有限周期输出。'),
 ('Dri_Tim1_Init()','再对照单脉冲方案：直接用 Init 与 Start，保留 OPM=1，由硬件在更新事件时自动停止。')],
 '中断方案：IT_Init → IT_Start → 输出 PWM → 5 次溢出形成更新 → IRQHandler 直接清 CEN。单脉冲方案：Init → Start → 更新事件硬件自动清 CEN。',
 'f = 72MHz / (7200 × 5000) = 2Hz；RCR=4 对应 5 个周期，约 2.5s。',
 'RCR 不会令 CNT 只在第 5 次才回零，CNT 每个周期都溢出回零；RCR 改变更新事件的节奏。高级定时器还必须 MOE=1。两种初始化方案选一种。',
 '能解释为什么 UG 后要清 UIF，以及 OPM 和更新中断停机的差别。')

task(2,'DMA 从 RAM 发送到串口',['dma/dma.c','dma/dma.h','USART/usart.c'],
 '建立“源在 RAM、目的为外设 DR”的数据搬运概念。',[
 ('Dri_USART_Init()','先初始化 USART1，保证串口时钟、引脚和波特率已配置。'),
 ('Dri_DMA1_Trans_Init()','选择 DMA1 通道 4；DIR=1、MINC=1、PINC=0、8 位宽度、非循环；打开 USART.DMAT 与 DMA 完成中断。'),
 ('Dri_DMA1_TransmitData()','先关通道、清旧标志，再设 CMAR 为源数组、CPAR 为 USART1->DR、CNDTR 为字节数，最后 EN=1。'),
 ('DMA1_Channel4_IRQHandler()','TCIF4 表示指定数量已搬完；清标志并关 DMA 通道。'),
 ('Dri_USART_SendChar()','对照 CPU 轮询写 DR：理解普通发送由 CPU 搬，而 DMA 发送由硬件响应请求搬。')],
 'main 准备持续有效的发送数组 → 配好 USART/DMA → Start 传输 → USART 发出 DMA 请求 → DMA 从 RAM 搬到 DR → 串口逐位发出 → DMA 完成中断。',
 '源地址：(uint32_t)str；目的地址：(uint32_t)&USART1->DR；长度：实际待发送字节数，不一定包含字符串结尾的 0。',
 '普通模式每次发送都要重新设置数量并使能。数据搬运完成不代表串口最后一位已经发完，线路完成看 USART.TC。未完成前不要修改或释放发送数组。',
 '能区分 CMAR 与 CPAR、MINC 与 PINC，以及 DMA 的 TCIF4 与串口 TC。')

task(2,'DMA 从串口接收到 RAM',['dma/dma.c','dma/dma.h','USART/usart.c'],
 '理解 DMA 接收、指定长度完成中断和主循环处理标志的配合。',[
 ('Dri_USART_Init()','初始化串口。本函数默认开 RXNE/IDLE 中断，下一步 DMA 初始化会将它们关闭。'),
 ('Dri_DMA1_Rec_Init()','DMA1 通道 5；DIR=0、MINC=1、PINC=0；开 USART.DMAR、TCIE 和 NVIC，避免 CPU 中断同时读 DR。'),
 ('Dri_DMA1_ReceiveData()','CPAR 指向 USART1->DR，CMAR 指向接收数组，CNDTR 指定接收字节数，EN=1 开始等待数据。'),
 ('DMA1_Channel5_IRQHandler()','收到指定数量后 TCIF5=1；清标志，设置 volatile tcFlag=1。'),
 ('main 中处理 tcFlag','主循环发现标志后处理数据并清零；需要再次接收时重新调用 ReceiveData。')],
 '串口收到字节 → USART RXNE 产生 DMA 请求 → DMA 自动读取 DR 并写入数组 → CNDTR 递减 → 到 0 触发完成中断 → tcFlag=1 → main 处理。',
 '例如数组至少 10 字节，CNDTR=10，就要收到 10 字节才触发完成；这不是按 IDLE 判断一帧。',
 '数组不会自动添加字符串结尾的 0。用 %s 打印时应额外预留并补零，或按长度输出。当前不是循环接收，不能只初始化一次就无限收。',
 '能说明为什么 DMA 接收时要关闭 RXNE 中断，以及 volatile 标志为什么适合中断与主循环之间通信。')

task(3,'ADC 单通道采集',['adc1/adc1.c','adc1/adc1.h','USART/usart.c'],
 '把 PC0 的模拟电压转换为 12 位数字，再换算电压并打印。',[
 ('Dri_USART_Init()','准备打印通道；需要间隔打印时复用 Dri_Delay_Ms。'),
 ('Dri_ADC1_Init()','PC0 模拟输入，ADC 时钟 12MHz；关闭扫描、连续转换、右对齐；SQR3 首位放 IN10，SQR1.L=0。'),
 ('Dri_ADC1_StartConvert()','ADON 上电，等待稳定，RSTCAL 复位校准，CAL 校准，SWSTART 启动，等待首次 EOC。'),
 ('Dri_ADC1_ReadV()','从 DR 读当前转换结果，按 3.3V 参考值换算电压。'),
 ('Dri_Delay_Ms()','主循环定期读取和 printf，例如每 1000ms 输出一次；连续采集由 ADC 自行进行。')],
 'PC0 输入电压 → 模拟采样 → ADC 转换 → EOC 置位、结果写 DR → main 读取 DR → 换算 V → 串口输出。',
 'V = ADC原始值 × 3.3 / 4095；12 位右对齐范围 0~4095。当前单通道采样时间为 1.5 个 ADC 周期。',
 '此处 CONT=1，Start 后连续转换，main 不必每轮重新校准。结果精度受参考电压、采样时间和信号源阻抗影响；3.3 是换算假设。',
 '能解释 SCAN、CONT、ALIGN、SMPR、SQR 的用途，以及 IN10 与 PC0 的对应关系。')

task(3,'ADC 双通道 DMA 采集',['adc1/adc1.c','adc1/adc1.h'],
 '按手册在 PC0、PC2 上采集两路电压，用 DMA 保存完整序列。',[
 ('Dri_ADC1_DMA_Init()','先看 ADC：PC0/PC2 模拟输入，SCAN=1、CONT=1、右对齐，采样 13.5 周期，序列为 IN10→IN12，L=1。'),
 ('Dri_ADC1_DMA_Init()','接着看 DMA1 通道 1：PSIZE/MSIZE=16 位，PINC=0、MINC=1、DIR=0、CIRC=1；打开 ADC.DMA。'),
 ('main 中准备 data[2]','使用全局或 static uint16_t data[2]；数组必须持续有效，不能小于两个元素。'),
 ('Dri_ADC1_DMA_Start()','传 (uint32_t)data 与 len=2；设 CPAR=&ADC1->DR、CMAR、CNDTR，再使能 DMA 和 ADC 转换。'),
 ('main 中读取数组','data[0] 是 PC0，data[1] 是 PC2；分别乘 3.3/4095 换算，再 printf。')],
 'PC0 转换完成 → DMA 写 data[0] → PC2 转换完成 → DMA 写 data[1] → 循环模式重装数量和内存指针 → 下一组采集。',
 '两通道序列长度 L=通道数−1=1；DMA len 是 16 位数据项数量 2，不是字节数 4。',
 'DMA 会读 DR 并清 EOC，启动 DMA 模式后不要再靠轮询 EOC 等待。两路是依次采样而非同时采样，main 读取数组也不是原子快照。不要混用单通道 Init/Start。',
 '能说明为什么多个通道只靠 DR 容易覆盖数据，以及 DMA 的 CIRC 和 ADC 的 CONT 分别控制什么。')

task(4,'软件模拟 SPI 字节交换',['spi/spi.c','spi/spi.h','UTIL/delay.c'],
 '先用 GPIO 时序理解模式 0 的采样边沿、位顺序和全双工。',[
 ('Dri_SPI_Soft_Init()','PA5 SCK、PA7 MOSI 普通推挽，PA6 MISO 输入，PC13 CS；选择软件模式，SCK 空闲低。'),
 ('Dri_SPI_Start()','CS 拉低，选中从设备，开始一条事务。'),
 ('Dri_SPI_SwapByte()','公共接口检测当前模式，软件模式进入内部 SPI_Soft_SwapByte。'),
 ('SPI_Soft_SwapByte()','最高位先放到 MOSI；延时后拉高 SCK 并采 MISO；拉低 SCK，循环 8 次，返回接收字节。'),
 ('Dri_SPI_Stop()','CS 拉高，结束事务。')],
 'CS 低 → MOSI 放一位 → SCK 上升沿采样 → SCK 下降沿准备下一位 → 重复 8 次 → 返回接收字节 → CS 高。',
 'CPOL=0：时钟空闲低；CPHA=0：第一个边沿采样。MSB first：先发 bit7。每半周期延时约 5μs，另有软件开销。',
 'SPI 读数据也必须发字节来产生时钟，常发 0xFF 作占位。SPI_Soft_SwapByte 是 static 内部函数，main 只调用公共接口。',
 '能在纸上画出一位 MOSI 与 SCK 的先后关系，说明为什么发送一个字节会同时收到一个字节。')

task(4,'硬件 SPI1 字节交换',['spi/spi.c','spi/spi.h'],
 '把上一任务的 GPIO 时序换成硬件 SPI，比较相同接口的两种实现。',[
 ('Dri_SPI_init()','打开 SPI1/GPIO 时钟；PA5/PA7 复用推挽、PA6 输入、PC13 片选；配置主机模式。注意原函数名 init 为小写。'),
 ('Dri_SPI_init()','继续看 SSM/SSI、CPOL/CPHA、BR、DFF、LSBFIRST，最后 SPE=1。当前 72MHz/4≈18MHz。'),
 ('Dri_SPI_Start()','用 GPIO 拉低 CS；NSS 使用软件管理。'),
 ('Dri_SPI_SwapByte()','等待 TXE→写 DR→等待 RXNE→读 DR 返回，硬件完成时钟与位移。'),
 ('Dri_SPI_Stop()','先等 BSY=0，确认最后一位移出，再拉高 CS。')],
 '配置 SPI1 → CS 低 → TXE 可写 → 写 DR → 硬件移位并产生时钟 → RXNE 置位 → 读 DR → BSY 清零 → CS 高。',
 'TXE 表示发送缓冲可写，RXNE 表示收到数据，BSY 表示总线仍在忙；三个标志含义不同。',
 '默认硬件模式与软件模式共用引脚和公共函数，一次选择一种。不要看到 TXE 就马上结束片选。当前等待没有软件超时。',
 '能说清 PA5/PA7 为什么要改成复用推挽，以及 SSM、SSI 与外部 CS 的区别。')

task(4,'W25Q32 识别擦除写入读取',['w25q32/w25q32.c','w25q32/w25q32.h','spi/spi.c'],
 '把字节交换组合成完整 Flash 命令，按先识别、再擦写、后读取校验复习。',[
 ('Int_W25Q32_Init()','默认初始化硬件 SPI；软件实验在其后调用 Dri_SPI_Soft_Init，后续 Flash API 相同。'),
 ('Int_W25Q32_ReadId()','CS 低后发 0x9F，读取厂商与设备 ID；先确认通信，W25Q32 常见 EF/4016。'),
 ('Int_W25Q32_WaitNotBusy()','发 0x05 读取状态寄存器，轮询 BUSY 位为 0；当前没有超时。'),
 ('Int_W25Q32_SectorErase()','内部先等空闲，再调用 WriteEnable 发 0x06，然后发 0x20 和 24 位地址，擦一个 4KB 扇区。'),
 ('Int_W25Q32_WritePage()','再次等空闲、写使能，发 0x02、地址和数据；当前从页首写，长度不超过 256B。'),
 ('Int_W25Q32_Read()','等空闲后发 0x03 和地址，连续发占位字节接收数据；按长度与原数据比较。'),
 ('Int_W25Q32_WriteDisanable()','查看原工程保留的拼写：发 0x04 关闭写使能；它不表示等待擦写完成。')],
 'Init → ReadId → EraseSector → WritePage → Read → 比较数据。擦写内部共用流程：等 BUSY=0 → WriteEnable → CS 低 → 指令+地址+数据 → CS 高。',
 '页首地址=(block<<16)|(sector<<12)|(page<<8)；block 0~63、sector/page 0~15。每页 256B，每扇区 4KB，容量 4MB。',
 '写入只能把 1 改成 0，恢复为 1 需要擦除。擦写返回时可能仍忙，下一次命令前需等待；读函数已经等待。读出的原始字节不保证有字符串结束符，不能直接假定可用 %s。',
 '能说明为什么每次擦写都要 WriteEnable，为什么不能跨页编程，以及页号与扇区号如何拼成地址。')

# 防止文档中误写当前工程不存在的方法名。
sources='\n'.join(p.read_text(encoding='utf-8-sig',errors='replace') for p in (BASE/'all_hw_register/User/hardware').rglob('*.c'))
for t in TASKS:
 for f in t['files']: assert (BASE/'all_hw_register/User/hardware'/f).is_file(),f
 for method,_ in t['rows']:
  name=method.split('(')[0]
  if '(' in method: assert re.search(r'\b'+re.escape(name)+r'\s*\(',sources),name

doc=Document()
sec=doc.sections[0]
sec.page_width=Cm(21); sec.page_height=Cm(29.7)
sec.top_margin=Cm(1.65); sec.bottom_margin=Cm(1.6)
sec.left_margin=Cm(1.65); sec.right_margin=Cm(1.65)
for name in ['Normal','Title','Subtitle','Heading 1','Heading 2','Heading 3']:
 st=doc.styles[name]; st.font.name='Microsoft YaHei'; st.font.color.rgb=RGBColor(0,0,0)
 st.element.get_or_add_rPr().get_or_add_rFonts().set(qn('w:eastAsia'),'Microsoft YaHei')
doc.styles['Normal'].font.size=Pt(10)
doc.styles['Normal'].paragraph_format.space_after=Pt(5)
doc.styles['Normal'].paragraph_format.line_spacing=1.08
doc.styles['Title'].font.size=Pt(22)
doc.styles['Heading 1'].font.size=Pt(16)
doc.styles['Heading 1'].paragraph_format.space_before=Pt(6)
doc.styles['Heading 1'].paragraph_format.space_after=Pt(10)
doc.styles['Heading 2'].font.size=Pt(11)
doc.styles['Heading 2'].paragraph_format.space_before=Pt(8)
doc.styles['Heading 2'].paragraph_format.space_after=Pt(4)

def para(text,boldlead=None):
 p=doc.add_paragraph()
 if boldlead:
  p.add_run(boldlead).bold=True; p.add_run(text)
 else: p.add_run(text)
 return p

def table(headers,rows,widths,codecol=None):
 tb=doc.add_table(rows=1,cols=len(headers)); tb.alignment=WD_TABLE_ALIGNMENT.CENTER; tb.autofit=False
 for col,w in zip(tb.columns,widths): col.width=Cm(w)
 pr=tb._tbl.tblPr
 borders=OxmlElement('w:tblBorders')
 for edge in ['top','left','bottom','right','insideH','insideV']:
  v=OxmlElement('w:'+edge); v.set(qn('w:val'),'single'); v.set(qn('w:sz'),'4'); v.set(qn('w:color'),'D9D9D9'); borders.append(v)
 pr.append(borders)
 for ri,values in enumerate([headers]+list(rows)):
  row=tb.rows[0] if ri==0 else tb.add_row()
  trpr=row._tr.get_or_add_trPr(); trpr.append(OxmlElement('w:cantSplit'))
  if ri==0: trpr.append(OxmlElement('w:tblHeader'))
  for ci,(cell,value,w) in enumerate(zip(row.cells,values,widths)):
   cell.width=Cm(w); cell.vertical_alignment=WD_CELL_VERTICAL_ALIGNMENT.CENTER
   cp=cell._tc.get_or_add_tcPr()
   mar=OxmlElement('w:tcMar')
   for edge in ['top','bottom','left','right']:
    x=OxmlElement('w:'+edge); x.set(qn('w:w'),'65'); x.set(qn('w:type'),'dxa'); mar.append(x)
   cp.append(mar)
   shade=OxmlElement('w:shd'); shade.set(qn('w:fill'),'24476A' if ri==0 else ('F3F6F9' if ri%2==0 else 'FFFFFF'));cp.append(shade)
   p=cell.paragraphs[0]; p.paragraph_format.space_after=Pt(0);p.paragraph_format.line_spacing=1.12
   if ci==0:p.alignment=WD_ALIGN_PARAGRAPH.CENTER
   r=p.add_run(str(value)); r.font.size=Pt(9.5)
   if ri==0:r.bold=True;r.font.color.rgb=RGBColor(255,255,255)
   elif ci==codecol:
    r.font.name='Consolas';r.font.size=Pt(9)
 return tb

doc.add_paragraph('STM32 第1至4章代码复习顺序表',style='Title')
para('按当前 all_hw_register 寄存器工程复习定时器、DMA、ADC 与 SPI。每个任务按“先找文件，再顺着方法读，最后解释运行流程”的方式学习；表格中的顺序是阅读顺序，不一定是 main 的逐行调用顺序。')
doc.add_heading('使用方法',level=2)
para('所有文件路径均相对 all_hw_register/User/hardware。每个模块只保留一套 .c/.h；在 main 中包含对应头文件并调用公共方法。static 方法只供驱动内部使用，中断入口由硬件事件触发。')
para('时钟计算按当前工程：SYSCLK=72MHz，PCLK2=72MHz，PCLK1=36MHz；APB1 预分频不为 1 时，TIM4/5/6 的定时器输入时钟为 72MHz。改时钟后需重新计算 PSC、波特率和延时。')
doc.add_heading('任务总览',level=2)
table(['任务','章节','复习内容'],[(str(i),'第'+str(t['ch'])+'章',t['title']) for i,t in enumerate(TASKS,1)],[1.4,2.1,14.2])
doc.add_heading('复习完成的标准',level=2)
para('能不看注释说清：引脚接在哪里，初始化配置哪些寄存器，谁启动硬件，事件发生后数据放在哪里，以及 main 最后如何得到结果。不要只背函数名。')
para('课程对照：尚硅谷 STM32 单片机进阶篇 V2.1.2，第1至4章。方法名称与具体参数以当前 all_hw_register 工程为准。')

chapter_names={1:'定时器',2:'DMA 直接存储访问',3:'ADC 模数转换',4:'SPI 通信'}
for i,t in enumerate(TASKS,1):
 doc.add_page_break()
 para('第'+str(t['ch'])+'章  '+chapter_names[t['ch']])
 doc.add_heading(str(i)+'  '+t['title'],level=1)
 para(t['goal'])
 para('；'.join(t['files']),boldlead='打开文件  ')
 doc.add_heading('方法阅读顺序',level=2)
 table(['顺序','方法','重点'],[(str(j),name,focus) for j,(name,focus) in enumerate(t['rows'],1)],[1.05,6.4,10.25],codecol=1)
 doc.add_heading('运行流程',level=2);para(t['flow'])
 doc.add_heading('关键参数与换算',level=2);para(t['formula'])
 doc.add_heading('容易混淆的地方',level=2);para(t['pitfall'])
 doc.add_heading('复习自检',level=2);para(t['check'])

# 页码帮助任务总览和打印本检索，页脚不放额外装饰。
f=sec.footer.paragraphs[0];f.alignment=WD_ALIGN_PARAGRAPH.CENTER
r=f.add_run();r.font.size=Pt(9)
fld=OxmlElement('w:fldSimple');fld.set(qn('w:instr'),'PAGE');r._r.addnext(fld)
for st in doc.styles:
 pp=st.element.find(qn('w:pPr'))
 if pp is not None:
  for border in list(pp.findall(qn('w:pBdr'))): pp.remove(border)
for pp in doc.paragraphs:
 pr=pp._p.pPr
 if pr is not None:
  for border in list(pr.findall(qn('w:pBdr'))): pr.remove(border)
doc.core_properties.title='STM32 第1至4章代码复习顺序表'
doc.core_properties.subject='定时器 DMA ADC SPI 的代码阅读顺序和运行流程'
doc.core_properties.author=''
OUT.parent.mkdir(exist_ok=True)
doc.save(OUT)
print(OUT)
print('tasks',len(TASKS),'tables',len(doc.tables))
