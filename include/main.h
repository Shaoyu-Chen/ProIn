#ifndef __PROIN_MAIN__
#define __PROIN_MAIN__

#include <stdint.h>

#ifndef NULL
#define NULL		((void *) 0)
#endif


// Clock
#define HSI_VALUE	16000000
#define HSE_VALUE	8000000

#define PLL_M		8		// If HSI is used for PLL, PLL_M set to 16
#define PLL_N		336
#define PLL_P		2
#define PLL_Q		7

#define SYSTICK_RATE	2


// Structure hoding the current time configuration
struct __RCC_CLK__
{
        uint32_t sysclk;
        uint32_t hclk;
        uint32_t pclk1;
        uint32_t pclk2;
};


__attribute__((naked)) int main();
void SystemInit();
void SystemCoreClockUpdate();

#endif
