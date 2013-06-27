//
// Copyright Erik Dubbelboer. and other contributors. All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Compile using: gcc -g -Wall test.c -o test
//

#include <stdio.h>  /* printf() */
#include <assert.h>

#include "lru.h"


struct test_s;

static int  lru_compare(const struct test_s* a, const struct test_s* b);
static void lru_free   (struct test_s* entry);


struct test_s {
  LRU_ENTRY(test_s) lru;

  int id;
};

LRU_GENERATE_STATIC(test_s, lru_compare, lru_free, lru)

static int free_calls = 0;

  
static int lru_compare(const struct test_s* a, const struct test_s* b) {
  if (a->id == b->id) {
    return 0;
  }

  /* The order is not really important as long as we have one. */
  return (a->id < b->id) ? 1 : -1;
}


static void lru_free(struct test_s* entry) {
  ++free_calls;
}


void bug1() {
  struct test_s a1  = { .id = 1 };
  struct test_s a2  = { .id = 2 };
  struct test_s a3  = { .id = 3 };

  LRU_TYPE(test_s) cache;

  LRU_INIT(&cache, 4);
  
  LRU_INSERT(test_s, &cache, &a1);
  LRU_INSERT(test_s, &cache, &a2);
  LRU_INSERT(test_s, &cache, &a3);
  LRU_FIND(test_s, &cache, &a2);
  LRU_FIND(test_s, &cache, &a1);
  
  LRU_REMOVE(test_s, &cache, LRU_HEAD(&cache));
  LRU_REMOVE(test_s, &cache, LRU_HEAD(&cache));
  LRU_REMOVE(test_s, &cache, LRU_HEAD(&cache));
  assert(LRU_HEAD(&cache) == 0);
}


int main() {
  struct test_s a1  = { .id = 1 };
  struct test_s a2  = { .id = 2 };
  struct test_s a3  = { .id = 3 };
  struct test_s a4  = { .id = 4 };
  struct test_s a5  = { .id = 5 };
  struct test_s a12 = { .id = 1 };

  LRU_TYPE(test_s) cache;

  LRU_INIT(&cache, 4);

  assert(LRU_FIND(test_s, &cache, &a1) == 0);

  LRU_INSERT(test_s, &cache, &a1);
  assert(LRU_FIND(test_s, &cache, &a1) == &a1);

  LRU_INSERT(test_s, &cache, &a2);
  LRU_INSERT(test_s, &cache, &a3);
  LRU_INSERT(test_s, &cache, &a4);
  LRU_INSERT(test_s, &cache, &a5);
  assert(free_calls == 1);
  assert(LRU_FIND(test_s, &cache, &a1) == 0  );
  assert(LRU_FIND(test_s, &cache, &a2) == &a2);
  assert(LRU_FIND(test_s, &cache, &a3) == &a3);
  assert(LRU_FIND(test_s, &cache, &a4) == &a4);
  assert(LRU_FIND(test_s, &cache, &a5) == &a5);
  
  LRU_REMOVE(test_s, &cache, &a5);
  assert(free_calls == 2);
  assert(LRU_FIND(test_s, &cache, &a1) == 0  );
  assert(LRU_FIND(test_s, &cache, &a2) == &a2);
  assert(LRU_FIND(test_s, &cache, &a3) == &a3);
  assert(LRU_FIND(test_s, &cache, &a4) == &a4);
  assert(LRU_FIND(test_s, &cache, &a5) == 0  );
  
  LRU_INSERT(test_s, &cache, &a1);
  assert(free_calls == 2);
  assert(LRU_FIND(test_s, &cache, &a1 ) == &a1 );
  assert(LRU_FIND(test_s, &cache, &a12) != &a12);
  assert(LRU_FIND(test_s, &cache, &a2 ) == &a2 );
  assert(LRU_FIND(test_s, &cache, &a3 ) == &a3 );
  assert(LRU_FIND(test_s, &cache, &a4 ) == &a4 );
  assert(LRU_FIND(test_s, &cache, &a5 ) == 0   );
  LRU_INSERT(test_s, &cache, &a12);
  assert(free_calls == 3);
  assert(LRU_FIND(test_s, &cache, &a1 ) != &a1 );
  assert(LRU_FIND(test_s, &cache, &a12) == &a12);
  assert(LRU_FIND(test_s, &cache, &a2 ) == &a2 );
  assert(LRU_FIND(test_s, &cache, &a3 ) == &a3 );
  assert(LRU_FIND(test_s, &cache, &a4 ) == &a4 );
  assert(LRU_FIND(test_s, &cache, &a5 ) == 0   );

  struct test_s* p;
  while ((p = LRU_HEAD(&cache))) {
    LRU_REMOVE(test_s, &cache, p);
  }
  assert(free_calls == 7);  /* 1, 2, 3 and 4 were still in there so +4. */

  assert(LRU_FIND(test_s, &cache, &a1) == 0);
  assert(LRU_FIND(test_s, &cache, &a2) == 0);
  assert(LRU_FIND(test_s, &cache, &a3) == 0);
  assert(LRU_FIND(test_s, &cache, &a4) == 0);
  assert(LRU_FIND(test_s, &cache, &a5) == 0);


  bug1();


  printf("All tests passed.\n");

  return 0;
}

