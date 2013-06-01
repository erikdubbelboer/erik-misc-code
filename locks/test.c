
#include <sys/time.h>  // gettimeofday()
#include <stdint.h>    // uint64_t
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "spinlock.h"
#include "ticketlock.h"


static int64_t ustime() {
  struct timeval tv;
  int64_t ust;

  gettimeofday(&tv, NULL);

  ust = ((int64_t)tv.tv_sec) * 1000000;
  ust += tv.tv_usec;

  return ust;
}


void busysleep(int usec) {
  int64_t t = ustime() + usec;

  while (t > ustime()) {
    ; // Nothing.
  }
}
    

#define WORK()                              \
  do {                                      \
    for (volatile int i = 0; i < 10; ++i) { \
      ;                                     \
    }                                       \
  } while (0);


static spinlock_t   simple_spin;
static ticketlock_t simple_ticket;


#define ITERATIONS 2000000


static void* simple_spinlock(void* ignore) {
  (void)ignore;

  for (uint64_t i = 0; i < ITERATIONS; ++i) {
    spinlock_lock(&simple_spin);
    WORK();
    spinlock_unlock(&simple_spin);
  }

  return 0;
}


static void* simple_spinlock_yield(void* ignore) {
  (void)ignore;

  for (uint64_t i = 0; i < ITERATIONS; ++i) {
    spinlock_yieldlock(&simple_spin);
    WORK();
    spinlock_unlock(&simple_spin);
  }

  return 0;
}


static void* simple_ticketlock(void* ignore) {
  (void)ignore;

  for (uint64_t i = 0; i < ITERATIONS; ++i) {
    ticketlock_lock(&simple_ticket);
    WORK();
    ticketlock_unlock(&simple_ticket);
  }

  return 0;
}


static void* simple_ticketlock_yield(void* ignore) {
  (void)ignore;

  for (uint64_t i = 0; i < ITERATIONS; ++i) {
    ticketlock_yieldlock(&simple_ticket);
    WORK();
    ticketlock_unlock(&simple_ticket);
  }

  return 0;
}


static void test_splinlock_speed() {
  spinlock_init(&simple_spin);
  ticketlock_init(&simple_ticket);

  pthread_t threads[10];

  printf("spinlock speed test (lock):\n");

  for (int num = 1; num <= 16; num *= 2) {
    printf("%2d threads: ", num);
    fflush(stdout);

    spinlock_lock(&simple_spin);

    for (int t = 0; t < num; ++t) {
      pthread_create(&threads[t], 0, simple_spinlock, 0);
    }
    
    int64_t start = ustime();

    spinlock_unlock(&simple_spin);

    for (int t = 0; t < num; ++t) {
      pthread_join(threads[t], 0);
    }

    int64_t end = ustime();

    printf("%6ld milliseconds\n", (end - start) / 1000);
  }
  
  printf("spinlock speed test (yieldock):\n");
  
  for (int num = 1; num <= 16; num *= 2) {
    printf("%2d threads: ", num);
    fflush(stdout);

    spinlock_lock(&simple_spin);

    for (int t = 0; t < num; ++t) {
      pthread_create(&threads[t], 0, simple_spinlock_yield, 0);
    }
    
    int64_t start = ustime();

    spinlock_unlock(&simple_spin);

    for (int t = 0; t < num; ++t) {
      pthread_join(threads[t], 0);
    }

    int64_t end = ustime();

    printf("%6ld milliseconds\n", (end - start) / 1000);
  }
  
  printf("ticketlock speed test (lock):\n");
  
  for (int num = 1; num <= 8; num *= 2) {
    printf("%2d threads: ", num);
    fflush(stdout);

    ticketlock_lock(&simple_ticket);

    for (int t = 0; t < num; ++t) {
      pthread_create(&threads[t], 0, simple_ticketlock, 0);
    }
    
    int64_t start = ustime();

    ticketlock_unlock(&simple_ticket);

    for (int t = 0; t < num; ++t) {
      pthread_join(threads[t], 0);
    }

    int64_t end = ustime();

    printf("%6ld milliseconds\n", (end - start) / 1000);
  }
  
  printf("ticketlock speed test (yieldlock):\n");
  
  for (int num = 1; num <= 8; num *= 2) {
    printf("%2d threads: ", num);
    fflush(stdout);

    ticketlock_lock(&simple_ticket);

    for (int t = 0; t < num; ++t) {
      pthread_create(&threads[t], 0, simple_ticketlock_yield, 0);
    }
    
    int64_t start = ustime();

    ticketlock_unlock(&simple_ticket);

    for (int t = 0; t < num; ++t) {
      pthread_join(threads[t], 0);
    }

    int64_t end = ustime();

    printf("%6ld milliseconds\n", (end - start) / 1000);
  }
}


int main() {
  printf("testing...\n");


  test_splinlock_speed();


  printf("done.\n");

  return 0;
}

