#include "I2C.h"
#include "m24c02.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    uint8_t txChar = 'A';
    uint8_t rxChar;
    uint8_t txText[] = "STM32I2C";
    uint8_t rxText[sizeof(txText)] = {0};
    uint8_t textLength = (uint8_t)(sizeof(txText) - 1);
    uint8_t readLength;

    Dri_USART_Init();
    Int_M24C02_Init();
    Dri_Delay_Ms(10);

    printf("\r\n=== M24C02 read/write test ===\r\n");
    printf("WARNING: overwrites 0x00 and 0x10-0x17.\r\n");

    Int_M24C02_WriteByte(0x00, txChar);
    rxChar = Int_M24C02_ReadByte(0x00);
    printf("Single: TX=0x%02X, RX=0x%02X, %s\r\n",
           (unsigned int)txChar, (unsigned int)rxChar,
           rxChar == txChar ? "PASS" : "FAIL");

    /* M24C02 page writes must stay within one 8-byte page. */
    Int_M24C02_WriteBytes(0x10, txText, textLength);
    readLength = Int_M24C02_ReadBytes(0x10, rxText, textLength);
    rxText[textLength] = '\0';

    printf("Multi: TX=\"%s\", RX=\"%s\"\r\n",
           (char *)txText, (char *)rxText);
    printf("RX hex:");
    for (uint8_t i = 0; i < textLength; i++)
    {
        printf(" %02X", (unsigned int)rxText[i]);
    }
    printf("\r\nMulti: %s\r\n",
           (readLength == textLength &&
            memcmp(txText, rxText, textLength) == 0) ? "PASS" : "FAIL");
    printf("Test finished. Reset to run again.\r\n");

    /* Do not repeatedly write EEPROM in the main loop. */
    while (1)
    {
    }
}
