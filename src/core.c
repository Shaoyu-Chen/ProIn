#include "core.h"
#include "main.h"

extern struct __RCC_CLK__ RCC_CLK;

void SysTick_Init(uint32_t systick_rate)
{
	SYSTICK_LOAD = (RCC_CLK.hclk / 8 / systick_rate) - 1;
	SYSTICK_VAL = 0;
	SYSTICK_CTRL = 3;
}
