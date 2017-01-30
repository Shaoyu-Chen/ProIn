#include "core.h"
#include "heap.h"
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
		case OS_INIT:
		{
			// The supervisor call is invoked only one time after all the setup is completed
			if(init_flag)
				break;
			else
				init_flag = !init_flag;

			// Enables Systick
			SysTick_Init(SYSTICK_RATE);

			// Discards the stack frame of initialization thread
			__asm__ volatile("MOV   R2, %0          \n\t" :: "r" (&_empty_sp_) : "r2");
			__asm__ volatile("MSR   MSP, R2         \n\t");

			// Initializes the sp of first thread to PSP and return to thread mode with PSP as sp
			current_thd = *tcb_list.head;

			__asm__ volatile("MOV   R2, %0          \n\t" :: "r" (current_thd->sp) : "r2");
			__asm__ volatile("MSR   PSP, R2         \n\t");
			__asm__ volatile("MOV   LR, #0xFFFFFFFD \n\t");
			__asm__ volatile("BX    LR              \n\t");
			break;
		}
		case THD_CREATE:
		{
			stack_pointer[R0] = __thread_create((void *)stack_pointer[R0], 
						(uint32_t *)stack_pointer[R1], (uint8_t)stack_pointer[R2]);
			break;
		}
		case MEM_ALLOCATE:
		{
			stack_pointer[R0] = (uint32_t)__malloc((uint32_t)stack_pointer[R0]);
			break;
		}
		case MEM_FREE:
		{
			__free((void *)stack_pointer[R0]);
			break;
		}
		default:
		{
			print("svc_call invalid.\r\n");
			break;
		}
	}
}

void __attribute__((naked)) SVC_Handler()
{
	// Finds which sp is used
	// Needs to be done before the target SP is modified
	__asm__ volatile("TST	LR, #4		\n\t");
	__asm__ volatile("ITE	EQ		\n\t");
	__asm__ volatile("MRSEQ	R0, MSP		\n\t"
			 "MRSNE R0, PSP		\n\t" ::: "r0");
	isr_pro();

	__asm__ volatile("BL	SVC_routine_c	\n\t");
	
	isr_epi();
}

void __attribute__((naked)) PendSV_Handler()
{
	// Saves callee-saved registers
	__asm__ volatile("MRS   R0, PSP		\n\t" ::: "r0");
	__asm__ volatile("MOV	%0, R0		\n\t" : "=r" (current_thd->sp));
	__asm__ volatile("MOV	R0, %0		\n\t" :: "r" (current_thd->r4_r11) : "r0");
	__asm__ volatile("STM	R0, {R4-R11}	\n\t");

	// Scheduler
	__asm__ volatile("BL	scheduler	\n\t");

	// Context recovered
	__asm__ volatile("MOV	R0, %0		\n\t" :: "r" (current_thd->r4_r11) : "r0");
	__asm__ volatile("LDM	R0, {R4-R11}	\n\t");

	__asm__ volatile("MOV   R2, %0          \n\t" :: "r" (current_thd->sp) : "r2");
	__asm__ volatile("MSR   PSP, R2         \n\t");

	// Return to Thread Mode with PSP as sp
	__asm__ volatile("MOV   LR, #0xFFFFFFFD \n\t");
	__asm__ volatile("BX	LR		\n\t");
}


void __attribute__((naked)) SysTick_Handler()
{
	isr_pro();

	// PENDSVSET
	SCB_ICSR |= 1 << 28;

	isr_epi();
}

