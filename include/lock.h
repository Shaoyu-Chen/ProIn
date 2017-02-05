#ifndef __PROIN_LOCK__
#define __PROIN_LOCK__

#include "stdint.h"

#define __SPINLOCK	0


// Spin lock
typedef struct __SPINLOCK_T__
{
	uint32_t locked;
} spinlock_t;

#define DEFINE_SPINLOCK(x)	spinlock_t __attribute__((section (".spinlock"))) x

void spinlock_init(spinlock_t *lock);
void __spinlock_lock(spinlock_t *lock);
void __spinlock_unlock(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);

#endif
