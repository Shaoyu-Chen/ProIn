#ifndef __PROIN_CONFIG__
#define __PROIN_CONFIG__


// Clock
#define HSI_VALUE	16000000
#define HSE_VALUE	8000000

#define PLL_M		8	// If HSI is used for PLL, PLL_M set to 16
#define PLL_N		336
#define PLL_P		2
#define PLL_Q		7

#define SYSTICK_RATE	1000
#define TARGET_LATENCY	1000


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
#define MEM_ALLOCATE	2
#define MEM_FREE	3


// Thread
#define MAX_THD_NUM     8
#define STK_FR_SIZE     8
#define THD_STK         128

#define PRIO_LEVEL      8	// Priority level of thread: 0 ~ 7, the loewr the number, the higher the priority


#endif
