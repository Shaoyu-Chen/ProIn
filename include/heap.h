#ifndef __PROIN_HEAP__
#define __PROIN_HEAP__

#include "stdint.h"

#define __MEM_ALLOCATE	0
#define __MEM_FREE	1

typedef struct __HEAP_T__
{
	struct
	{
		uint32_t size : 17;
		uint32_t avail: 1;
	} status;

	uint32_t *space;

	struct __HEAP_T__ *prev;
	struct __HEAP_T__ *next;
} heap_t;

void HEAP_Init();

void *malloc(uint32_t size);
void *__malloc(uint32_t size);

void free(void *addr);
void __free(void *addr);

#endif
