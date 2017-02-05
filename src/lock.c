#include "lock.h"
#include "main.h"
#include "thread.h"

extern tcb *current_thd;

void spinlock_init(spinlock_t *lock)
{
	lock->locked = false;
}

void spinlock_lock(spinlock_t *lock)
{
	__asm__ volatile("MOV	R0, #0	\n\t"
			 "MOV	R1, %0	\n\t"
			 "SVC	3	\n\t" :: "r" (lock) : "r0", "r1");
}

void __spinlock_lock(spinlock_t *lock)
{
	__asm__ volatile("MOV	R0, #1			\n\t"
			 "L1:				\n\t"
			 "	MOV	R1, %0		\n\t"
			 "	LDREX	R2, [R1]	\n\t"
			 "	CMP	R2, #1		\n\t"
			 "	BEQ	L1		\n\t"
			 "	CPSID	I		\n\t"
			 "	STREX	R2, R0, [R1]	\n\t"
			 "	CMP	R2, R0		\n\t"
			 "	BEQ	L2		\n\t"
			 "	DMB			\n\t"
			 "	B	L3		\n\t"

			 "L2:				\n\t"
			 "	CPSIE	I		\n\t"
			 "	B	L1		\n\t"

			 // Thread promoted to privileged access
			 "L3:				\n\t"
			 "	MRS	R0, CONTROL	\n\t"
			 "	BIC	R0, R0, #1	\n\t"
			 "	MSR	CONTROL, R0	\n\t"  :: "r" (&lock->locked) : "r0", "r1", "r2");

	current_thd->state.nPriv = false;
}

void __spinlock_unlock(spinlock_t *lock)
{
	lock->locked = 0;
	current_thd->state.nPriv = true;

	__asm__ volatile("CPSIE	I		\n\t"
			 "MOV	R0, #3		\n\t"
			 "MSR	CONTROL, R0	\n\t" ::: "r0");
}

void spinlock_unlock(spinlock_t *lock) __attribute__ ((alias ("__spinlock_unlock")));
