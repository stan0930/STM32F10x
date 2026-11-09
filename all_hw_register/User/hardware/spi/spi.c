#include "spi.h"

/* SPI1: SCK PA5, MISO PA6, MOSI PA7; software CS: PC13. */
#define CS_HIGH (GPIOC->ODR |= GPIO_ODR_ODR13)
#define CS_LOW  (GPIOC->ODR &= ~GPIO_ODR_ODR13)

void Dri_SPI_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |
                    RCC_APB2ENR_IOPCEN |
                    RCC_APB2ENR_SPI1EN;

    /* PA5/PA7: 50 MHz alternate-function push-pull outputs. */
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOA->CRL |=  (GPIO_CRL_MODE5 | GPIO_CRL_CNF5_1 |
                    GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1);

    /* PA6: floating input. */
    GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
    GPIOA->CRL |= GPIO_CRL_CNF6_0;

    /* PC13: 2 MHz general-purpose push-pull output, inactive high. */
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    CS_HIGH;

    /* SPI mode 0, 8-bit, master, software NSS, MSB first. */
    SPI1->CR1 = SPI_CR1_SSM |
                SPI_CR1_SSI |
                SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_BR_0;       /* PCLK2 / 4 */
    SPI1->CR1 &= ~(SPI_CR1_CPOL |
                   SPI_CR1_CPHA |
                   SPI_CR1_LSBFIRST |
                   SPI_CR1_DFF |
                   SPI_CR1_BIDIMODE);
    SPI1->CR2 &= ~SPI_CR2_SSOE;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void Dri_SPI_Start(void)
{
    CS_LOW;
}

void Dri_SPI_Stop(void)
{
    /* Do not release the flash while the final byte is still shifting. */
    while (SPI1->SR & SPI_SR_BSY) ;
    CS_HIGH;
}

uint8_t Dri_SPI_SwapByte(uint8_t byte)
{
    while (!(SPI1->SR & SPI_SR_TXE)) {
    }
    SPI1->DR = byte;
    while (!(SPI1->SR & SPI_SR_RXNE)) {
    }
    return (uint8_t)SPI1->DR;
}
