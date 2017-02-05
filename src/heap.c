#include "core.h"
#include "heap.h"
#include "main.h"

extern uint64_t _vma_heap_;
extern uint64_t _vma_eheap_;

static const uint8_t heap_t_8b_aligned = ((sizeof(heap_t) + sizeof(uint64_t) - 1)
						/ sizeof(uint64_t)) * sizeof(uint64_t);
static heap_t *heap_head = (heap_t *)&_vma_heap_;

void HEAP_Init()
{
	heap_head->status.size = (&_vma_eheap_ - &_vma_heap_) * sizeof(uint64_t) - heap_t_8b_aligned;
	heap_head->status.avail = true;

	// The address of the allocated space is 4 byte aligned
	heap_head->space = &_vma_heap_ + heap_t_8b_aligned / sizeof(uint64_t);
	heap_head->prev = heap_head->next = NULL;
}

void *malloc(uint32_t size)
{
	void *result;

	// Places parameters in corresponding registers, and then makes a system call
	__asm__ volatile("MOV	R0, %3	\n\t"
			 "MOV	R1, %1	\n\t"
			 "SVC	%2	\n\t"
        		 "MOV	%0, R0	\n\t"
	: "=r" (result) : "r" (size), "I" (__MEM), "I" (__MEM_ALLOCATE) : "r0", "r1");

        return result;
}

void *__malloc(uint32_t size)
{
	// 4 byte alignment
	size = ((size + sizeof(uint64_t) - 1) / sizeof(uint64_t)) * sizeof(uint64_t);

	// Serches for the fittest
	heap_t **probe = &heap_head;
	heap_t *target = NULL;
	uint32_t min = UINT32_MAX;

	while(*probe)
	{
		if((*probe)->status.size >= size && min > (*probe)->status.size && (*probe)->status.avail)
		{
			min = (*probe)->status.size;
			target = *probe;
		}
		probe = &(*probe)->next;
	}

	if(!target)
		return NULL;
	else
	{
		target->status.avail = 0;

		int32_t remain_space = target->status.size - size - heap_t_8b_aligned;

		// If the space is even larger than needed, it would be seperated into two segments
		if(remain_space > 0)
		{
			target->status.size = size;

			// Creates a heap_t
			heap_t *nnext = target->next;

			heap_t *next = target->next = (heap_t *)(target->space + size / sizeof(int64_t));
			next->status.size = remain_space;
			next->status.avail = true;
			next->space = (uint64_t *)(next) + heap_t_8b_aligned / sizeof(uint64_t);
			next->prev = target;
			next->next = nnext;
		}
	}

	return target->space;
}

void __free(void *addr)
{
	// Places parameters in corresponding registers, and then makes a system call
	__asm__ volatile("MOV	R0, %2	\n\t"
			 "MOV   R1, %0  \n\t"
			 "SVC	%1	\n\t"
	:: "r" (addr), "I" (__MEM), "I" (__MEM_FREE) : "r0", "r1");
}

void free(void *addr)
{
	// Serches for the target to be freed
	heap_t **probe = &heap_head;
	heap_t *target = NULL;

	while(*probe)
	{
		if((*probe)->space == addr)
		{
			target = *probe;
			break;
		}
		probe = &(*probe)->next;
	}

	if(!target) return;

	// Merges fragments if possible
	if(target->prev && target->prev->status.avail)
	{
		if(target->next && target->next->status.avail)
		{
			target->prev->status.size += target->status.size + target->next->status.size + 2 * heap_t_8b_aligned;
			target->prev->next = target->next->next;
		}
		else
		{
			target->prev->status.size += target->status.size + heap_t_8b_aligned;
			target->prev->next = target->next;
		}
	}
	else if(target->next && target->next->status.avail)
	{
		target->status.avail = true;
		target->status.size += target->next->status.size + heap_t_8b_aligned;
		target->next = target->next->next;
	}
	else
		target->status.avail = true;
}
