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

		
		// TCB initialization
		newborn->tid = tcb_list.num_of_thd + 1;

		if(prio >= PRIO_LEVEL)
			prio = PRIO_LEVEL - 1;

		newborn->status.prio = prio;
		tcb_list.slices_sum += PRIO_LEVEL - prio;
		newborn->status.time_slices = (PRIO_LEVEL - prio) * TARGET_LATENCY / tcb_list.slices_sum;

		newborn->status.runable = 1;

		newborn->sp = newborn->stack + THD_STK;
		newborn->sp -= STK_FR_SIZE;

		newborn->next = NULL;

		// Context initialization
		if(args)				// argc, argv
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
		newborn->sp[xPSR] = (1 << 24);		// Always in Thumb state


		// Inserts into tcb_list
		tcb_list.num_of_thd += 1;

		if(!tcb_list.head)
			tcb_list.head = newborn;
		else
		{
			tcb **probe = &tcb_list.head;

			do
			{
				if((*probe)->status.prio >= newborn->status.prio)
					break;

				probe = &(*probe)->next;

			} while(*probe);

			newborn->next = *probe;
			*probe = newborn;
		}

		return 0;
	}

	return -1;
}

void scheduler()
{
	current_thd = current_thd->next;
	
	// Update time_slices
	if(!current_thd)
	{
		tcb *probe = tcb_list.head;

		// If all threads reach 0 slices, RELOAD
		do
		{
			probe->status.time_slices = (PRIO_LEVEL - probe->status.prio) * TARGET_LATENCY / tcb_list.slices_sum;
			(probe->status.time_slices == 0) ? (probe->status.time_slices = 1) : 0;

			probe = probe->next;
		} while(probe);

		current_thd = tcb_list.head;
	}
}
