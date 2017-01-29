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
