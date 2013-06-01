
#include <stdint.h>


typedef union __attribute__ ((__packed__)) ticketlock_u {
  uint32_t u;
  struct __attribute__ ((__packed__)) {
    uint16_t ticket;
    uint16_t users;
  } s;
} ticketlock_t;


inline void ticketlock_init(ticketlock_t* t) {
  __sync_lock_release(&t->u);
}


inline void ticketlock_lock(ticketlock_t* t) {
  uint32_t me = __sync_fetch_and_add(&t->s.users, 1);
  
  while (t->s.ticket != me) {
    asm volatile("pause\n": : :"memory");
  }
}


inline void ticketlock_yieldlock(ticketlock_t* t) {
  uint32_t me    = __sync_fetch_and_add(&t->s.users, 1);
  uint32_t memin = me - 1;

  while (t->s.ticket != me) {
    // If I'm not next in line we can let others run.
    if (t->s.ticket < memin) {
      sched_yield();
    }

    asm volatile("pause\n": : :"memory");
  }
}


inline int ticketlock_trylock(ticketlock_t* t) {
  uint16_t me     = t->s.users;
  uint16_t menew  = me + 1;
  uint32_t cmp    = ((uint32_t)me    << 16) + me;
  uint32_t cmpnew = ((uint32_t)menew << 16) + me;

  if (__sync_val_compare_and_swap(&t->u, cmp, cmpnew) == cmp) {
    return 1;
  }

  return 0;
}


inline int ticketlock_locked(ticketlock_t* t) {
  return t->s.ticket != t->s.users;
}


inline void ticketlock_unlock(ticketlock_t* t) {
  asm volatile("": : :"memory");
  t->s.ticket++;
}

