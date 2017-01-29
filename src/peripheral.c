#include "peripheral.h"
#include "main.h"

extern struct __RCC_CLK__ RCC_CLK;

/*
 * LED
 */
void LED_Init()
{
        RCC_AHB1ENR |= 1 << 3;          // Clock of I/O port D enabled
        GPIOD_MODER |= 0x55000000;      // PD13, PD12, PD14, PD15 for LED3 to LED6 
        GPIOD_SPEEDR |= 0x55000000;     // Medium speed
}

void LED_on(uint32_t led)
{
	if((led & LED3) == LED3)
		GPIOD_BSRR |= LED3;

	if((led & LED4) == LED4)
		GPIOD_BSRR |= LED4;

	if((led & LED5) == LED5)
		GPIOD_BSRR |= LED5;

	if((led & LED6) == LED6)
		GPIOD_BSRR |= LED6;
}

void LED_off(uint32_t led)
{
	if((led & LED3) == LED3)
		GPIOD_BSRR |= LED3 << 16;

	if((led & LED4) == LED4)
		GPIOD_BSRR |= LED4 << 16;

	if((led & LED5) == LED5)
		GPIOD_BSRR |= LED5 << 16;

	if((led & LED6) == LED6)
		GPIOD_BSRR |= LED6 << 16;
}

/*
 * USART3
 */
void USART3_Init()
{
        RCC_AHB1ENR |= 1 << 1;          // Clock of I/O port B enabled
        RCC_APB1ENR |= 1 << 18;         // USART3 clock enable

        // PIN configuration
        GPIOB_MODER |= 0x00A00000;      // PB10 for Tx, PB11 for Rx
        GPIOB_SPEEDR |= 0x00F00000;     // Very high speed
        GPIOB_PUPDR |= 0x00500000;      // Pull up
        GPIOB_AFRH |= 0x00007700;       // Alternate function 7 of PB10 and PB11: USART3

        // USART configuration: 9600/8-n-1
        USART3_CR1 |= 0x0000000C;
        USART3_BRR = ((RCC_CLK.pclk1 / (16 * 9600)) << 4) | ((RCC_CLK.pclk1 / 9600) % 16);
        USART3_CR1 |= 1 << 13;
}

void print(char *str)
{
        uint32_t SR_TXE = 1 << 7;

        while(*str)
        {
                while(!(USART3_SR & SR_TXE));
                USART3_DR = (*str & 0xFF);
                str += 1;
        }
}
