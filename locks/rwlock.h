
// Simple non-reentrant spinning read-write lock.
// See: http://locklessinc.com/articles/locks/

#include "spinlock.h"


typedef struct rwlock_s {
  spinlock_t   lock;
  volatile int readers;
} rwlock_t;


inline void rwlock_init(rwlock_t* rwlock) {
  spinlock_init(&rwlock->lock);
  rwlock->readers = 0;
}


inline void rwlock_lock_read(rwlock_t* rwlock, int yield) {
  spinlock_lock(&rwlock->lock, yield);

  __sync_add_and_fetch(&rwlock->readers, 1);

  spinlock_unlock(&rwlock->lock);
}

inline int rwlock_try_lock_read(rwlock_t* rwlock) {
  if (!spinlock_try_lock(&rwlock->lock)) {
    return 0;
  }

  __sync_add_and_fetch(&rwlock->readers, 1);
   
  spinlock_unlock(&rwlock->lock);

  return 1;
}

inline int rwlock_locked_read(rwlock_t* rwlock) {
  return (rwlock->readers > 0);
}

inline void rwlock_unlock_read(rwlock_t* rwlock) {
  __sync_sub_and_fetch(&rwlock->readers, 1);
}


inline void rwlock_lock_write(rwlock_t* rwlock, int yield) {
  spinlock_lock(&rwlock->lock, yield);

  int tries = 0;
  while (rwlock->readers > 0) {
    if ((tries++ == 100) && yield) {    
      tries = 0;
      sched_yield();
    }
  }
}

inline int rwlock_try_lock_write(rwlock_t* rwlock) {
  if (!spinlock_try_lock(&rwlock->lock)) {
    return 0;
  }

  if (rwlock->readers > 0) {
    spinlock_unlock(&rwlock->lock);
    return 0;
  }

  return 1;
}

inline int rwlock_locked_write(rwlock_t* rwlock) {
  return spinlock_locked(&rwlock->lock);
}

inline void rwlock_unlock_write(rwlock_t* rwlock) {
  spinlock_unlock(&rwlock->lock);
}

