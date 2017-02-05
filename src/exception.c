#include "Config.h"
#include "core.h"
#include "heap.h"
#include "lock.h"
#include "main.h"
#include "peripheral.h"
#include "thread.h"

/*
 * Customed prologue and epilogue for exceptions.
 * LR need to be stored for function call to work correctly
 */
#define isr_pro()	__asm__ volatile("PUSH	{R4-R11, LR}	\n\t")
#define isr_epi()	__asm__ volatile("POP	{R4-R11, LR}	\n\t"	\
					 "BX	LR		\n\t")

extern uint32_t _lma_etext_;
extern uint32_t _vma_data_;
extern uint32_t _vma_edata_;
extern spinlock_t _vma_spinlock_;
extern spinlock_t _vma_espinlock_;
extern uint32_t _vma_bss_;
extern uint32_t _vma_ebss_;
extern uint32_t _empty_sp_;

extern tcb *current_thd;
extern tcb_list_t tcb_list;

uint8_t init_flag = 0;

void __attribute__((naked)) Reset_Handler()
{
	uint32_t *pData, *pStart, *pEnd;

	// Copy .data section to SRAM
	pData = &_lma_etext_;
	pStart = &_vma_data_;
	pEnd = &_vma_edata_;
	while(pStart < pEnd)
		*(pStart++) = *(pData++);

	for(spinlock_t *ps = &_vma_spinlock_; ps != &_vma_espinlock_; ++ps)
		ps->locked = false;

	// Zero .bss section
	pStart = &_vma_bss_;
	pEnd = &_vma_ebss_;
	while(pStart < pEnd)
		*(pStart++) = 0;


	SystemInit();
	main();
}

void SVC_routine_c(uint32_t *stack_pointer)
{
	// Extracts the number hardcoded in svc instruction and serves the request
	uint8_t svc_call = ((uint8_t *)(stack_pointer[PC]))[-2];

	switch(svc_call)
	{
		case __OS_INIT:
		{
			// The supervisor call is invoked only one time after all the setup is completed
			if(init_flag)
				break;
			else
				init_flag = !init_flag;

			// Discards the stack frame of initialization thread
			__asm__ volatile("MOV   R0, %0          \n\t"
					 "MSR   MSP, R0         \n\t" :: "r" (&_empty_sp_) : "r0");

			// Initializes the sp of first thread to PSP and return to thread mode with PSP as sp
			current_thd = tcb_list.head;
			if(!current_thd)
			{
				print("No thread to be run.\r\n");
				while(true);
			}

			// Enables Systick
			SysTick_Init(SYSTICK_RATE);

			__asm__ volatile("MOV	R0, %0		\n\t"
					 "MRS	R1, CONTROL	\n\t"
					 "ORR	R0, R0, R1	\n\t"
					 "MSR	CONTROL, R0	\n\t" :: "r" (current_thd->state.nPriv) : "r0", "r1");

			__asm__ volatile("MOV   R2, %0          \n\t"
					 "MSR   PSP, R2         \n\t" :: "r" (current_thd->SP) : "r2");

			__asm__ volatile("MOV   LR, #0xFFFFFFFD \n\t"
					 "BX    LR              \n\t");
			break;
		}
		case __THD:
		{
			switch(stack_pointer[R3])
			{
			case __THD_CREATE:
				stack_pointer[R0] = __thread_create((void *)stack_pointer[R0], 
							(uint32_t *)stack_pointer[R1], (uint8_t)stack_pointer[R2]);
				break;
			default:
				print("Invalid param.\r\n");
				break;
			}
			break;
		}
		case __MEM:
		{
			switch(stack_pointer[R1])
			{
			case __MEM_ALLOCATE:
				stack_pointer[R0] = (uint32_t)__malloc((uint32_t)stack_pointer[R0]);
				break;
			case __MEM_FREE:
				__free((void *)stack_pointer[R0]);
				break;
			default:
				print("Invalid param.\r\n");
				break;
			}
			break;
		}
		case __LOCK:
		{
			switch(stack_pointer[R1])
			{
			case __SPINLOCK:
				__spinlock_lock((spinlock_t *)stack_pointer[R0]);
				break;
			default:
				print("Invalid param.\r\n");
				break;
			}
			break;
		}
		default:
		{
			print("svc_call #num invalid.\r\n");
			break;
		}
	}
}

void __attribute__((naked)) SVC_Handler()
{
	// Finds which sp is used
	// Needs to be done before the target SP is modified
	__asm__ volatile("TST	LR, #4		\n\t"
			 "ITE	EQ		\n\t"
			 "MRSEQ	R0, MSP		\n\t"
			 "MRSNE R0, PSP		\n\t" ::: "r0");
	isr_pro();

	__asm__ volatile("BL	SVC_routine_c	\n\t");
	
	isr_epi();
}

void __attribute__((naked)) PendSV_Handler()
{
	// If not running out of time slices, the courrent thread continues
	__asm__ volatile("MOV	R0, %0			\n\t"
			 "CMP	R0, #0			\n\t"
			 "IT	EQ			\n\t"
			 "BEQ	Context_Switch		\n\t"
			 "SUB	R0, R0, #1		\n\t"
			 "MOV	%0, R0			\n\t" : "+r" (current_thd->state.time_slices) :: "r0");
	__asm__ volatile("BX	LR			\n\t");

	__asm__ volatile("Context_Switch:		\n\t");
	// Context stored
	__asm__ volatile("	MRS	R0, CONTROL	\n\t"
			 "	AND	%0, R0, #1	\n\t" : "=r" (current_thd->state.nPriv) :: "r0");

	__asm__ volatile("	MOV	R1, %0		\n\t"
        		 "	STM	R1, {R4-R11}	\n\t" :: "r" (current_thd->R4_R11) : "r1");

	__asm__ volatile("	MRS	R2, PSP		\n\t"
			 "	MOV	%0, R2		\n\t" : "=r" (current_thd->SP) :: "r2");

	__asm__ volatile("	MOV	%0, LR		\n\t" : "=r" (current_thd->LR));

	// Scheduler
	__asm__ volatile("	BL	scheduler	\n\t");

	// Context recovered
	__asm__	volatile("	MRS	R0, CONTROL	\n\t"
			 "	ORR	R0, R0, %0	\n\t"
			 "	MSR	CONTROL, R0	\n\t" :: "r" (current_thd->state.nPriv) : "r0");

	__asm__ volatile("	MOV	R1, %0		\n\t"
			 "	LDM	R1, {R4-R11}	\n\t" :: "r" (current_thd->R4_R11) : "r1");

	__asm__ volatile("	MOV	R2, %0		\n\t"
			 "	MSR	PSP, R2		\n\t" :: "r" (current_thd->SP) : "r2");

	__asm__ volatile("	MOV	LR, %0		\n\t" :: "r" (current_thd->LR));


	// EXC_RETURN	
	__asm__ volatile("	BX	LR		\n\t");
}


void __attribute__((naked)) SysTick_Handler()
{
	isr_pro();

	// PENDSVSET
	LED_off(LED4);
	SCB_ICSR |= 1 << 28;

	isr_epi();
}

