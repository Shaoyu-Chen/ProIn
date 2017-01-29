#include "core.h"
#include "heap.h"
#include "main.h"
#include "thread.h"

tcb *current_thd;
tcb_list_t tcb_list;

int32_t thread_create(void *func, uint32_t *args, uint8_t prio)
{
	int result;

	// Places parameters in corresponding registers, and then makes a system call
	__asm__ volatile("MOV	R0, %0	\n\t" :: "r" (func) : "r0");
	__asm__ volatile("MOV	R1, %0	\n\t" :: "r" (args) : "r1");
	__asm__ volatile("MOV   R2, %0  \n\t" :: "r" (prio) : "r1");
	__asm__ volatile("svc	1	\n\t");

	__asm__ volatile("MOV	%0, R0	\n\t" : "=r" (result) :: "r0");

	return result;
}

int32_t __thread_create(void *func, uint32_t *args, uint8_t prio)
{
	if(tcb_list.num_of_thd < MAX_THD_NUM)
	{
		tcb *newborn = __malloc(sizeof(tcb));
		
		if(!newborn) return -1;

		newborn->tid = tcb_list.num_of_thd + 1;
		newborn->status.prio = prio;
		newborn->status.slices = PRIO_LEVEL - prio;
		newborn->status.runable = 1;

		newborn->sp = newborn->stack + THD_STK;
		newborn->sp -= STK_FR_SIZE;

		if(args)
		{
			newborn->sp[R0] = args[0];
			newborn->sp[R1] = args[1];
		}
		else
		{
			newborn->sp[R0] = 0;
                        newborn->sp[R1] = 0;
		}

		newborn->sp[PC] = (uint32_t) func;

		// Always in Thumb state
		newborn->sp[xPSR] = (1 << 24);

		tcb_list.num_of_thd += 1;

		// Inserts into tcb_list
		if(tcb_list.head)
		{
			newborn->next = *tcb_list.head;
			*tcb_list.head = newborn;
		}
		else
		{
			tcb_list.head = &newborn->next;
                        *tcb_list.head = newborn;
		}

		return 0;
	}

	return -1;
}

void scheduler()
{
	tcb *probe = *tcb_list.head;
	uint8_t max_slices = 0;

	// Searches for the thread with max slices
	do
	{
		if(probe->status.slices > max_slices)
		{
			max_slices = probe->status.slices;
			current_thd = probe;
		}
		probe = probe->next;

	} while(probe != *tcb_list.head);


	// Update slices
	if(!max_slices)
	{
		// If all threads reach 0 slices, RELOAD
		do
		{
			probe->status.slices = PRIO_LEVEL - probe->status.prio;
			(probe->status.slices > max_slices) ? current_thd = probe : 0;
			probe = probe->next;

		} while(probe != *tcb_list.head);
	}

	--current_thd->status.slices;
}
