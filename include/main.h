#ifndef __PROIN_MAIN__
#define __PROIN_MAIN__

#include <stdint.h>

#ifndef NULL
#define NULL	((void *) 0)
#endif

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

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
