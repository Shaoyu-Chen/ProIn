#ifndef __PROIN_THREAD__
#define __PROIN_THREAD__

#include <stdint.h>

#define MAX_THD_NUM	8
#define STK_FR_SIZE	8
#define THD_STK		128

#define PRIO_LEVEL	256

typedef struct __TCB_T__
{
	uint32_t tid;

	struct {
		uint32_t prio : 8;
		uint32_t slices : 8;
		uint32_t runable : 4;
	} status;

	uint32_t r4_r11[8];

	uint32_t *sp;
	uint32_t stack[THD_STK];

	struct __TCB_T__ *next;
} tcb;

typedef struct __TCB_LIST_T__
{
	uint32_t num_of_thd;
	tcb **head;
} tcb_list_t;

int32_t thread_create(void *func, uint32_t *args, uint8_t prio);

int32_t __thread_create(void *func, uint32_t *args, uint8_t prio);

void scheduler();
#endif
