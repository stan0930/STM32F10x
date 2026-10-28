#include "I2C.h"
#include "m24c02.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>

// EEPROM中保存速度模式的地址
#define EEPROM_SPEED_ADDR 0x00
// 速度模式：0为正常速度，1为快速速度
#define SPEED_NORMAL       0
#define SPEED_FAST         1
// 两种速度对应的LED移动间隔，单位：毫秒
#define DELAY_NORMAL       500
#define DELAY_FAST         150

int main(void)
{
       uint8_t speed;          // 当前速度模式
       uint16_t delay_ms;      // 当前LED移动间隔
       uint8_t led = 0;        // 当前点亮的LED编号：0、1、2

       // 初始化串口、LED、按键外部中断和硬件I2C
       Dri_USART_Init();
       Dri_LED_Init();
       Dri_KEY_Init();
       Int_M24C02_Init();

       // 上电时从EEPROM读取上次保存的速度
       speed = Int_M24C02_ReadByte(EEPROM_SPEED_ADDR);

       // EEPROM中不是有效速度值时，使用正常速度
       if (speed != SPEED_FAST)
       {
           speed = SPEED_NORMAL;
       }

       // 根据速度模式确定LED流水间隔
       delay_ms = (speed == SPEED_FAST) ? DELAY_FAST : DELAY_NORMAL;

       while (1)
       {
           // KEY3按下后，外部中断会将key3_event置1
           if (key3_event)
           {
               // 清除按键事件标志，准备接收下一次按键
               key3_event = 0;

               // 在正常速度和快速速度之间切换
               speed = (speed == SPEED_NORMAL) ? SPEED_FAST : SPEED_NORMAL;
               delay_ms = (speed == SPEED_FAST) ? DELAY_FAST : DELAY_NORMAL;

               // 将新的速度保存到EEPROM，复位后可以恢复
               Int_M24C02_WriteByte(EEPROM_SPEED_ADDR, speed);
           }

           // 先关闭三个LED，保证每次只有一个LED点亮
           Dri_LED_Off(LED1 | LED2 | LED3);

           // 依次点亮LED1、LED2、LED3
           if (led == 0)
               Dri_LED_On(LED1);
           else if (led == 1)
               Dri_LED_On(LED2);
           else
               Dri_LED_On(LED3);

           // 使用当前速度延时，延时越小，流水灯越快
           Dri_Delay_Ms(delay_ms);

           // 移动到下一个LED，超过LED3后重新回到LED1
           led++;
           if (led >= 3)
               led = 0;
       }

}
