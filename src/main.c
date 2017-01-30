#include "main.h"
#include "core.h"
#include "peripheral.h"
#include "thread.h"
#include "lock.h"
#include "heap.h"

struct __RCC_CLK__ RCC_CLK;

uint8_t AHB_prescaler[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
uint8_t APB_prescaler[] = {0, 0, 0, 0, 1, 2, 3, 4};

void thread_1(uint32_t argc, void *argv)
{
	while(1)
		LED_on(LED3);
}

void thread_2(uint32_t argc, void *argv)
{
	while(1)
		LED_off(LED3);
}

void thread_3(uint32_t argc, void *argv)
{
	while(1)
		LED_on(LED6);
}

void thread_4(uint32_t argc, void *argv)
{
	while(1)
		LED_off(LED6);
}

int main()
{
	// Sets exception priorities (default PRIGROUP)
	SCB_SHP[7] = PRIO_LOW;		// SVC
	SCB_SHP[10] = PRIO_LOWEST;	// PendSV
	SCB_SHP[11] = PRIO_MEDIUM;	// SysTick

	LED_Init();
	USART3_Init();
	HEAP_Init();

	if(thread_create(thread_1, NULL, 3) || thread_create(thread_2, NULL, 3) ||
		thread_create(thread_3, NULL, 3) || thread_create(thread_4, NULL, 3))
	{
		print("Thread create failed.\n");
		while(1);
	}

	__asm__ volatile("svc 0");

	while(1);
}

void SystemInit()
{
	// Initial clock source: HSI
	RCC_CLK.sysclk = 16000000;

	/*********** Configure PLL as clock source ***********/
	uint32_t RDY_flag;

	// HSE enable
	RCC_CR |= 1 << 16;              // HSEON
	RDY_flag = 1 << 17;
	while(!(RCC_CR & RDY_flag));

	// Voltage Scaling Sutput: Scale 2 mode
	RCC_APB1ENR |= 1 << 28;
	PWR_CR &= ~(1 << 14);

	// Prescaler: AHB divided by 1 (default)
	RCC_CFGR |= 5 << 10;            // APB1 divided by 4
	RCC_CFGR |= 4 << 13;            // APB2 dibided by 2

	// PLL
	RCC_PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) | (1 << 22) | (PLL_Q << 24);
	RCC_CR |= 1 << 24;              // PLLON
	RDY_flag = 1 << 25;
	while(!(RCC_CR & RDY_flag));

	// Flash: Instruction cache, Data cache, 5 Wait State
	FLASH_ACR |= (1 << 9) | (1 << 10) | 5;

	// PLL seleted as system clock
	RCC_CFGR |= 2;                                  // SW
	while((RCC_CFGR & (3 << 2)) != (2 << 2));       // SWS

	SystemCoreClockUpdate();
}

void SystemCoreClockUpdate()
{
	switch(RCC_CFGR & (3 << 2))
	{
		// HSI used
		case 0x00:
			RCC_CLK.sysclk = HSI_VALUE;
			break;
		// HSE used
		case 0x04:
			RCC_CLK.sysclk = HSE_VALUE;
			break;
		// PLL used
		case 0x08:
		{
			// Input clock / PLL_M * PLL_N / PLL_P
			uint32_t pll_m = RCC_PLLCFGR & 0x3F;
			uint32_t pll_n = (RCC_PLLCFGR & 0x7FC0) >> 6;
			uint32_t pll_p = (((RCC_PLLCFGR & 0x30000) >> 16) + 1) * 2;

			if(RCC_PLLCFGR & (1 << 22))
				RCC_CLK.sysclk = HSE_VALUE / pll_m * pll_n / pll_p;
			else
				RCC_CLK.sysclk = HSI_VALUE / pll_m * pll_n / pll_p;

			break;
		}
		default:
			RCC_CLK.sysclk = HSI_VALUE;
			break;
	}

	RCC_CLK.hclk = RCC_CLK.sysclk >> AHB_prescaler[(RCC_CFGR & 0xF0) >> 4];
	RCC_CLK.pclk1 = RCC_CLK.sysclk >> APB_prescaler[(RCC_CFGR & 0x1C00) >> 10];
	RCC_CLK.pclk2 = RCC_CLK.sysclk >> APB_prescaler[(RCC_CFGR & 0xE000) >> 13];
}
