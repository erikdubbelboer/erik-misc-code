
#include <stdio.h>   // printf()
#include <stdlib.h>  // rand()
#include <assert.h>  // assert()

#include "hashtable.h"


void test_find() {
  hashtable_t t;

  hashtable_init(&t, 1);

  assert(hashtable_find(&t, 1) == 0);

  hashtable_free(&t);
}


void test_find_or_add() {
  hashtable_t t;

  hashtable_init(&t, 2);

  hashtable_find_or_add(&t, 1);

  entry_t* e = hashtable_find(&t, 1);

  assert(e != 0);
  assert(e->id == 1);

  assert(hashtable_find(&t, 5) == 0);
  
  hashtable_free(&t);
}


void test_chain() {
  hashtable_t t;
  entry_t*    e;

  hashtable_init(&t, 4);

  hashtable_find_or_add(&t, 1);
  hashtable_find_or_add(&t, 5);
  
  e = hashtable_find(&t, 5);
  
  assert(e     != 0);
  assert(e->id == 5);

  e = hashtable_find(&t, 1);
  
  assert(e     != 0);
  assert(e->id == 1);

  e = hashtable_find(&t, 5);

  assert(e != 0);
  assert(e->id == 5);
  
  assert(t.newest->id == 5);
  
  hashtable_free(&t);
}


void test_overflow() {
  hashtable_t t;

  hashtable_init(&t, 10);

  for (int i = 1; i < 12; ++i) {
    hashtable_find_or_add(&t, i);
  }

  assert(hashtable_find(&t, 1) == 0);

  for (uint64_t i = 2; i < 12; ++i) {
    entry_t* e = hashtable_find(&t, i);

    assert(e     != 0);
    assert(e->id == i);
  }
  
  hashtable_free(&t);
}


void test_evict_simple() {
  hashtable_t t;

  hashtable_init(&t, 2);
  
  // Evict on an empty hashtable.
  hashtable_evict(&t);

  hashtable_find_or_add(&t, 1);

  hashtable_evict(&t);

  assert(hashtable_find(&t, 1) == 0);

  // Do another evict to make sure it doesn't crash.
  hashtable_evict(&t);
  
  hashtable_free(&t);
}


void test_evict_chain() {
  hashtable_t t;

  hashtable_init(&t, 4);

  hashtable_find_or_add(&t, 1);
  hashtable_find_or_add(&t, 5);

  assert(t.newest->id == 5);

  hashtable_evict(&t);

  assert(hashtable_find(&t, 1) == 0);

  entry_t* e = hashtable_find(&t, 5);

  assert(e != 0);
  assert(e->id == 5);
  
  hashtable_free(&t);
}


void test_random() {
  hashtable_t t;

  srand(0);

  for (int test = 0; test < 1000; ++test) {
    printf("random test %d\n", test);

    int entries = 1 + (rand() % 10000);

    hashtable_init(&t, entries);

    for (int i = 0; i < 1000000; ++i) {
      int action = rand() % 10;
      int id     = 1 + (rand() % entries);

      if (action < 3) { // 30%
        hashtable_find_or_add(&t, id);
      } else if (action == 4) { // 10%
        hashtable_evict(&t);
      } else { // 60%
        hashtable_find(&t, id);
      }
    }
    
    hashtable_free(&t);
  }
}


int main(int argc, char* argv[]) {
  printf("testing...\n");

  test_find();
  test_find_or_add();
  test_chain();
  test_overflow();
  test_evict_simple();
  test_evict_chain();
  test_random();

  printf("done.\n");

  return 0;
}

