#include "lock.h"
#include "thread.h"

extern tcb *current_thd;

void spinlock_init(spinlock_t *lock)
{
	lock->locked = 0;
	lock->owner_id = 0;
}

void spinlock_lock(spinlock_t *lock)
{
	__asm__ volatile("MOV	R0, #1			\n\t" ::: "r0");
	__asm__ volatile("L1:				\n\t");
	__asm__ volatile("	MOV	R1, %0		\n\t" :: "r" (&lock->locked) : "r1");
	__asm__ volatile("	LDREX	R2, [R1]	\n\t" ::: "r2");
	__asm__ volatile("	CMP	R2, #1		\n\t"
			 "	BEQ	L1		\n\t"
			 "	STREX	R2, R0, [R1]	\n\t"
			 "	CMP	R2, R0		\n\t"
			 "	BEQ	L1		\n\t"
			 "	DMB			\n\t");

	// Spinlock only released by the owner
	lock->owner_id = current_thd->tid;
}

int8_t spinlock_unlock(spinlock_t *lock)
{
	if(current_thd->tid == lock->owner_id)
	{
		lock->locked = 0;
		return 0;
	}
	else
		return -1;
}
