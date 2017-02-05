#include "heap.h"
#include "main.h"

extern uint32_t _vma_heap_;
extern uint32_t _vma_eheap_;

static const uint8_t heap_t_4b_aligned = ((sizeof(heap_t) + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);
static heap_t *heap_head = (heap_t *)&_vma_heap_;

void HEAP_Init()
{
	heap_head->status.size = (&_vma_eheap_ - &_vma_heap_) * sizeof(uint32_t) - heap_t_4b_aligned;
	heap_head->status.avail = 1;

	// The address of the allocated space is 4 byte aligned
	heap_head->space = &_vma_heap_ + heap_t_4b_aligned / sizeof(uint32_t);
	heap_head->prev = heap_head->next = NULL;
}

void *malloc(uint32_t size)
{
	void *result;

	// Places parameters in corresponding registers, and then makes a system call
	__asm__ volatile("MOV	R0, %0	\n\t"
			 "MOV	R1, #0	\n\t" :: "r" (size) : "r0", "r1");

	__asm__ volatile("svc	2	\n\t");

        __asm__ volatile("MOV	%0, R0	\n\t" : "=r" (result));

        return result;
}

void *__malloc(uint32_t size)
{
	// 4 byte alignment
	size = ((size + sizeof(uint32_t) - 1) / sizeof(uint32_t)) * sizeof(uint32_t);

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

		int32_t remain_space = target->status.size - size - heap_t_4b_aligned;

		// If the space is even larger than needed, it would be seperated into two segments
		if(remain_space > 0)
		{
			target->status.size = size;

			// Creates a heap_t
			heap_t *nnext = target->next;

			heap_t *next = target->next = (heap_t *)(target->space + size / sizeof(int32_t));
			next->status.size = remain_space;
			next->status.avail = 1;
			next->space = (uint32_t *)(next) + heap_t_4b_aligned / sizeof(uint32_t);
			next->prev = target;
			next->next = nnext;
		}
	}

	return target->space;
}

void __free(void *addr)
{
	// Places parameters in corresponding registers, and then makes a system call
	__asm__ volatile("MOV	R0, %0	\n\t"
			 "MOV   R1, #1  \n\t" :: "r" (addr) : "r0");

	__asm__ volatile("svc	2	\n\t");
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
			target->prev->status.size += target->status.size + target->next->status.size + 2 * heap_t_4b_aligned;
			target->prev->next = target->next->next;
		}
		else
		{
			target->prev->status.size += target->status.size + heap_t_4b_aligned;
			target->prev->next = target->next;
		}
	}
	else if(target->next && target->next->status.avail)
	{
		target->status.avail = 1;
		target->status.size += target->next->status.size + heap_t_4b_aligned;
		target->next = target->next->next;
	}
	else
		target->status.avail = 1;
}
