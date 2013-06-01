
#include <sys/time.h>  // gettimeofday()
#include <time.h>      // nanosleep()
#include <unistd.h>    // usleep()
#include <stdint.h>    // uint64_t
#include <stdio.h>     // printf()
#include <stdlib.h>    // exit()


void do_nanosleep() {
  struct timespec ta, tb;

  ta.tv_sec  = 0;
  ta.tv_nsec = 1;

  if (nanosleep(&ta, &tb) != 0) {
    perror("nanosleep");
    exit(1);
  }
}


void do_usleep() {
  usleep(1);
}


void do_busysleep() {
  struct timeval ta, tb;

  gettimeofday(&ta, 0);

  tb = ta;
  ta.tv_usec++;

  while (tb.tv_usec < ta.tv_usec) {
    gettimeofday(&tb, 0);
  }
}


int main() {
  struct timeval start, end;
  uint64_t       ms;
  int            count = 400000;

#define TEST(func)                                                \
  gettimeofday(&start, 0);                                        \
  for (int i = 0; i < count; ++i) {                               \
    do_##func();                                                  \
  }                                                               \
  gettimeofday(&end, 0);                                          \
  ms = ((uint64_t)end.tv_sec - (uint64_t)start.tv_sec) * 1000000; \
  ms += (uint64_t)end.tv_usec - (uint64_t)start.tv_usec;          \
  printf("%lu microseconds per " #func "\n", ms / count);

  TEST(nanosleep);
  TEST(usleep);
  TEST(busysleep);

#undef TEST

  return 0;
}

