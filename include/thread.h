#ifndef __PROIN_THREAD__
#define __PROIN_THREAD__

#include <stdint.h>
#include "Config.h"

#define	THD_BLOCK	0
#define THD_READY	1

typedef struct __TCB_T__
{
	uint32_t tid;

	struct {
		uint32_t prio : 4;
		uint32_t time_slices : 16;
		uint32_t state : 4;
	} state;

	// Context register
	uint32_t CTRL;
	uint32_t R4_R11[8];
	uint32_t *SP;
	uint32_t LR;

	uint32_t stack[THD_STK];

	struct __TCB_T__ *next;
} tcb;

typedef struct __TCB_LIST_T__
{
	uint32_t num_of_thd;
	uint32_t slices_sum;
	tcb *head;
} tcb_list_t;

int32_t thread_create(void *func, uint32_t *args, uint8_t prio);

int32_t __thread_create(void *func, uint32_t *args, uint8_t prio);

void scheduler();
#endif
