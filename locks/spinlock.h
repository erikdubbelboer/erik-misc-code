
#include <sched.h>


typedef volatile int spinlock_t;


inline void spinlock_init(spinlock_t* s) {
  __sync_lock_release(s);
}


inline void spinlock_lock(spinlock_t* s) {
  while (!__sync_bool_compare_and_swap(s, 0, 1)) {
    while (*s) {
      asm volatile("pause\n": : :"memory");
    }
  }
}


inline void spinlock_yieldlock(spinlock_t* s) {
  while (!__sync_bool_compare_and_swap(s, 0, 1)) {
    for (int tries = 0; *s; ++tries) {
      if (tries == 100) {
        tries = 0;
        sched_yield();
      }
      
      asm volatile("pause\n": : :"memory");
    }
  }
}


inline int spinlock_trylock(spinlock_t* s) {
  if (__sync_bool_compare_and_swap(s, 0, 1)) {
    return 1;
  }

  return 0;
}


inline int spinlock_locked(spinlock_t* s) {
  return *s;
}


inline void spinlock_unlock(spinlock_t* s) {
  __sync_lock_release(s);
}

