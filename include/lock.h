#ifndef __PROIN_LOCK__
#define __PROIN_LOCK__

#include "stdint.h"

typedef struct __SPINLOCK_T__
{
	uint32_t locked;
	uint8_t owner_id;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
int8_t spinlock_unlock(spinlock_t *lock);

#endif
