#ifndef __PROIN_PERIPHERAL__
#define __PROIN_PERIPHERAL__

#include <stdint.h>

/*
 * Peripheral Register Address Definition
 */
// RCC
#define RCC_BASE	0x40023800
#define RCC_CR		(*((volatile uint32_t *) (RCC_BASE + 0x00)))
#define RCC_PLLCFGR	(*((volatile uint32_t *) (RCC_BASE + 0x04)))
#define RCC_CFGR	(*((volatile uint32_t *) (RCC_BASE + 0x08)))
#define RCC_AHB1ENR	(*((volatile uint32_t *) (RCC_BASE + 0x30)))
#define RCC_APB1ENR	(*((volatile uint32_t *) (RCC_BASE + 0x40)))

// PWR
#define PWR_BASE	0x40007000
#define PWR_CR		(*((volatile uint32_t *) (PWR_BASE + 0x00)))

// FLASH
#define FLASH_BASE	0x40023C00
#define FLASH_ACR	(*((volatile uint32_t *) (FLASH_BASE + 0x00)))

// USART3
#define USART3_BASE	0x40004800
#define USART3_SR	(*((volatile uint32_t *) (USART3_BASE + 0x00)))
#define USART3_DR	(*((volatile uint32_t *) (USART3_BASE + 0x04)))
#define USART3_BRR	(*((volatile uint32_t *) (USART3_BASE + 0x08)))
#define USART3_CR1	(*((volatile uint32_t *) (USART3_BASE + 0x0C)))
#define USART3_CR2	(*((volatile uint32_t *) (USART3_BASE + 0x10)))
#define USART3_CR3	(*((volatile uint32_t *) (USART3_BASE + 0x14)))

// GPIOB
#define GPIOB_BASE	0x40020400
#define GPIOB_MODER	(*((volatile uint32_t *) (GPIOB_BASE + 0x00)))
#define GPIOB_OTYPER	(*((volatile uint32_t *) (GPIOB_BASE + 0x04)))
#define GPIOB_SPEEDR	(*((volatile uint32_t *) (GPIOB_BASE + 0x08)))
#define GPIOB_PUPDR	(*((volatile uint32_t *) (GPIOB_BASE + 0x0C)))
#define GPIOB_IDR	(*((volatile uint32_t *) (GPIOB_BASE + 0x10)))
#define GPIOB_ODR	(*((volatile uint32_t *) (GPIOB_BASE + 0x14)))
#define GPIOB_BSRR	(*((volatile uint32_t *) (GPIOB_BASE + 0x18)))
#define GPIOB_LCKR	(*((volatile uint32_t *) (GPIOB_BASE + 0x1C)))
#define GPIOB_AFRL	(*((volatile uint32_t *) (GPIOB_BASE + 0x20)))
#define GPIOB_AFRH	(*((volatile uint32_t *) (GPIOB_BASE + 0x24)))

// GPIOD
#define GPIOD_BASE	0x40020C00
#define GPIOD_MODER	(*((volatile uint32_t *) (GPIOD_BASE + 0x00)))
#define GPIOD_OTYPER	(*((volatile uint32_t *) (GPIOD_BASE + 0x04)))
#define GPIOD_SPEEDR	(*((volatile uint32_t *) (GPIOD_BASE + 0x08)))
#define GPIOD_PUPDR	(*((volatile uint32_t *) (GPIOD_BASE + 0x0C)))
#define GPIOD_IDR	(*((volatile uint32_t *) (GPIOD_BASE + 0x10)))
#define GPIOD_ODR	(*((volatile uint32_t *) (GPIOD_BASE + 0x14)))
#define GPIOD_BSRR	(*((volatile uint32_t *) (GPIOD_BASE + 0x18)))
#define GPIOD_LCKR	(*((volatile uint32_t *) (GPIOD_BASE + 0x1C)))
#define GPIOD_AFRL	(*((volatile uint32_t *) (GPIOD_BASE + 0x20)))
#define GPIOD_AFRH	(*((volatile uint32_t *) (GPIOD_BASE + 0x24)))

// LED
#define LED3	8192
#define LED4	4096
#define LED5	16384
#define LED6	32768

/*
 * Function Declaration
 */
// LED
void LED_Init();
void LED_on(uint32_t led);
void LED_off(uint32_t led);


// USART3
void USART3_Init();
void print(char *);

#endif
