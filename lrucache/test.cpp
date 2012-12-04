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
// Compile using: g++ -g -Wall test.cpp -o test
//

#include <stdio.h>
#include <assert.h>
#include <list>
#include <string>

#include "lrucache.h"


void TestInts() {
  LRUCache<int, int>       cache(4);
  std::list<int>           l;
  std::list<int>::iterator i;
  int                      n;

  int three = 3;

  cache.Set(0, three);
  cache.Set(1, 2);
  cache.Set(2, 1);
  cache.Set(3, 0);

  // The order of the keys in the LRU list is now 3,2,1,0.
  // So the values are 0,1,2,3.
  l = cache.List();
  n = 0;
  for (i = l.begin(); i != l.end(); ++i, ++n) {
    if (cache.Get(*i) != n) {
      printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(*i), n);
    }
  }
  
  // Since we just accessed each element in order the order is reversed.
  // The order of the keys in the LRU list is now 0,1,2,3.
  // So the values are 3,2,1,0.
  l = cache.List();
  n = 3;
  for (i = l.begin(); i != l.end(); ++i, --n) {
    if (cache.Get(*i) != n) {
      printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(*i), n);
    }
  }

  // Since we just accessed each element again the least recently used item is now key 0 (value 3).
  cache.Set(4, 4);

  // Make sure key 0 was evicted.
  if (cache.Get(0) != 0) {
    printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(0), 0);
  }
  
  // Make sure key 4 was added properly.
  if (cache.Get(4) != 4) {
    printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(4), 4);
  }

  cache.Remove(4);

  // Make sure key 4 was removed.
  if (cache.Get(4) != 0) {
    printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(4), 0);
  }
}


// We keep track if the destructor of one of our items is called.
static int destructorCalled = 0;

class pointerTest {
 public:
  int value;

  pointerTest(int v) : value(v) {
  }

  ~pointerTest() {
    ++destructorCalled;
  }
};


void TestPointers() {
  LRUCache<int, pointerTest*> cache(4);
  std::list<int>              l;
  std::list<int>::iterator    i;
  int                         n;

  pointerTest* three = new pointerTest(3);

  cache.Set(0, three);
  cache.Set(1, new pointerTest(2));
  cache.Set(2, new pointerTest(1));
  cache.Set(3, new pointerTest(0));
  
  // The order of the keys in the LRU list is now 3,2,1,0.
  // So the values are 0,1,2,3.
  l = cache.List();
  n = 0;
  for (i = l.begin(); i != l.end(); ++i, ++n) {
    if (cache.Get(*i) == NULL) {
      printf("%s:%d: %d == NULL\n",  __FILE__, __LINE__, *i);
    }

    if (cache.Get(*i)->value != n) {
      printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(*i)->value, n);
    }
  }
  
  // Since we just accessed each element in order the order is reversed.
  // The order of the keys in the LRU list is now 0,1,2,3.
  // So the values are 3,2,1,0.
  l = cache.List();
  n = 3;
  for (i = l.begin(); i != l.end(); ++i, --n) {
    if (cache.Get(*i) == NULL) {
      printf("%s:%d: %d == NULL\n",  __FILE__, __LINE__, *i);
    }

    if (cache.Get(*i)->value != n) {
      printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(*i)->value, n);
    }
  }

  // Since we just accessed each element again the least recently used item is now key 0 (value 3).
  cache.Set(4, new pointerTest(4));

  // Make sure the destructor was called.
  if (destructorCalled != 1) {
    printf("%s:%d: destructor called %d times!\n", __FILE__, __LINE__, destructorCalled);
  }

  // Make sure key 0 was evicted.
  if (cache.Get(0) != NULL) {
    printf("%s:%d: %p != NULL\n", __FILE__, __LINE__, cache.Get(0));
  }
  
  // Make sure key 4 was added properly.
  if (cache.Get(4) == NULL) {
     printf("%s:%d: %d == NULL\n",  __FILE__, __LINE__, 4);
  }

  // Make sure key 4 was added properly.
  if (cache.Get(4)->value != 4) {
    printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(4)->value, 4);
  }

  cache.Remove(4);

  // Make sure key 4 was removed.
  if (cache.Get(4) != NULL) {
    printf("%s:%d: %d != %d\n", __FILE__, __LINE__, cache.Get(4)->value, 0);
  }

  // Make sure the destructor was called.
  if (destructorCalled != 2) {
    printf("%s:%d: destructor called %d times!\n", __FILE__, __LINE__, destructorCalled);
  }
}


void TestStdStrings() {
  LRUCache<std::string, std::string> cache(4);
  std::list<std::string>             l;
  std::list<std::string>::iterator   i;
  int                                n;

  std::string three = "3";

  cache.Set("0", three);
  cache.Set("1", "2");
  cache.Set("2", "1");
  cache.Set("3", "0");

  std::string values[] = {
    "0",
    "1",
    "2",
    three
  };

  // The order of the keys in the LRU list is now "3","2","1","0".
  // So the values are "0","1","2","3".
  l = cache.List();
  n = 0;
  for (i = l.begin(); i != l.end(); ++i, ++n) {
    if (cache.Get(*i).compare(values[n]) != 0) {
      printf("%s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, cache.Get(*i).c_str(), values[n].c_str());
    }
  }
  
  // Since we just accessed each element in order the order is reversed.
  // The order of the keys in the LRU list is now 0,1,2,3.
  // So the values are 3,2,1,0.
  l = cache.List();
  n = 3;
  for (i = l.begin(); i != l.end(); ++i, --n) {
    if (cache.Get(*i).compare(values[n]) != 0) {
      printf("%s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, cache.Get(*i).c_str(), values[n].c_str());
    }
  }

  // Since we just accessed each element again the least recently used item is now key 0 (value 3).
  cache.Set("4", "4");

  // Make sure key 0 was evicted.
  if (!cache.Get("0").empty()) {
    printf("%s:%d: \"%s\" != \"\"\n", __FILE__, __LINE__, cache.Get("0").c_str());
  }
  
  // Make sure key 4 was added properly.
  if (cache.Get("4").compare("4") != 0) {
    printf("%s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__, cache.Get("4").c_str(), "4");
  }

  cache.Remove("4");

  // Make sure key 4 was removed.
  if (!cache.Get("4").empty()) {
    printf("%s:%d: \"%s\" != \"\"\n", __FILE__, __LINE__, cache.Get("4").c_str());
  }
}


int main() {
  TestInts();
  TestPointers();
  TestStdStrings();

  printf("All tests done.\n");

  return 0;
}

