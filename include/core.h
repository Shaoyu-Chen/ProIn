#ifndef __PROIN_CORE__
#define __PROIN_CORE__

#include <stdint.h>


// SysTick Register Address
#define SYSTICK_CTRL	(*((volatile uint32_t *) 0xE000E010))
#define SYSTICK_LOAD	(*((volatile uint32_t *) 0xE000E014))
#define SYSTICK_VAL	(*((volatile uint32_t *) 0xE000E018))

// SCB
#define SCB_BASE	0xE000ED00
#define SCB_ICSR	(*((volatile uint32_t *) (SCB_BASE + 0x04)))
#define SCB_AIRCR	(*((volatile uint32_t *) (SCB_BASE + 0x0C)))
#define SCB_SHP		((volatile uint8_t *) (SCB_BASE + 0x18))

// Clock
#define HSI_VALUE	16000000
#define HSE_VALUE	8000000

#define PLL_M		8	// If HSI is used for PLL, PLL_M set to 16
#define PLL_N		336
#define PLL_P		2
#define PLL_Q		7

#define SYSTICK_RATE	100

// Exception priority
// Default PRIGROUP, 4-bit priority width implemented
#define PRIO_HIGHEST	0
#define PRIO_HIGH	1 << 4
#define PRIO_MEDIUM	2 << 4
#define PRIO_LOW	3 << 4
#define PRIO_LOWEST	4 << 4

// System Call #num
#define OS_INIT		0
#define THD_CREATE	1
#define	MEM_ALLOCATE	2
#define MEM_FREE	3

enum stack_frame
{
        R0 = 0,
        R1,
        R2,
        R3,
        R12,
        LR,
        PC,
        xPSR
};

void SysTick_Init(uint32_t systick_rate);

#endif
