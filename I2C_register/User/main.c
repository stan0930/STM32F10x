#include "I2C.h"
#include "m24c02.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
       Dri_USART_Init();
       Int_M24C02_Init();
       printf("I2C Èí¼þÄ£Äâ\r\n");

       Int_M24C02_WriteBytes(0x10, "abcdeHELLOSTM32",15);
       uint8_t str[16] = {0};
       Int_M24C02_ReadBytes(0x10, str, 5);
       printf("%s\n", str);
       while(1);

}
