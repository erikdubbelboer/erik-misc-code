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
// Compile using: g++ -std=c++0x -march=native -O3 -DNDEBUG speed.cpp -o speed
//

#include <stdio.h>
#include <stdlib.h>  // srand(), rand()
#include <time.h>    // clock()
#include <string.h>  // strcmp()
#include <string>
#include <map>
#include <unordered_map>

#include "lrucache0x.h"


template<template<typename...> class Container>
double TestInts(int cacheSize) {
  LRUCache<int, int, Container> cache(cacheSize);

  srand(0);

  clock_t start = clock();

  for (int i = 0; i < 1000000; ++i) {
    int k = rand() % (cacheSize * 2);
    int v = rand();

    if (cache.Get(k) == 0) {
      cache.Set(k, v);
    }
  }

  clock_t end = clock();

  printf("%d\n", cache.Get(0));

  return (end - start) / (CLOCKS_PER_SEC / 1000);
}


const char* RandomString(size_t length) {
  static char buffer[10240];

  for (int i = 0; i < length; ++i) {
    buffer[i] = (rand() % 92) + 32;  // ' ' - '|'
  }

  buffer[length] = 0;

  return buffer;
}


template<template<typename...> class Container>
double TestStrings(int cacheSize, int keySize, int valueSize) {
  LRUCache<std::string, std::string, Container> cache(cacheSize);
  std::string                                   keys[cacheSize * 2];
  std::string                                   values[cacheSize * 2];
  
  srand(0);

  for (int i = 0; i < cacheSize * 2; ++i) {
    keys[i]   = RandomString(keySize);
    values[i] = RandomString(valueSize);
  }
  
  clock_t start = clock();

  for (int i = 0; i < 1000000; ++i) {
    std::string k = keys[rand() % (sizeof(keys) / sizeof(keys[0]))];
    std::string v = values[rand() % (sizeof(values) / sizeof(values[0]))];

    if (cache.Get(k).empty()) {
      cache.Set(k, v);
    }
  }

  clock_t end = clock();

  printf("%s\n", cache.Get("0").c_str());

  return (end - start) / (CLOCKS_PER_SEC / 1000);
}


int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Usage: %s data.dat mode\n\n", argv[0]);
    printf("Available modes: string-keys, string-values, string-cache, int-cache\n");
  } else {
    FILE* fp = fopen(argv[1], "w");

    if (strcmp(argv[2], "string-keys") == 0) {
      for (int i = 10; i < 4000; i += 50) {
        printf("%d ----\n", i);
        fprintf(fp, "%d\t%.2f\t%.2f\n", i, TestStrings<std::map>(1024, i, 1024), TestStrings<std::unordered_map>(1024, i, 1024));
        fflush(fp);
      }
    } else if (strcmp(argv[2], "string-values") == 0) {
      for (int i = 10; i < 8000; i += 100) {
        printf("%d ----\n", i);
        fprintf(fp, "%d\t%.2f\t%.2f\n", i, TestStrings<std::map>(1024, 32, i), TestStrings<std::unordered_map>(1024, 32, i));
        fflush(fp);
      }
    } else if (strcmp(argv[2], "string-cache") == 0) {
      for (int i = 10; i < 2000; i += 25) {
        printf("%d ----\n", i);
        fprintf(fp, "%d\t%.2f\t%.2f\n", i, TestStrings<std::map>(i, 1024, 1024), TestStrings<std::unordered_map>(i, 1024, 1024));
        fflush(fp);
      }
    } else {
      for (int i = 10; i < 2000; i += 25) {
        printf("%d ----\n", i);
        fprintf(fp, "%d\t%.2f\t%.2f\n", i, TestInts<std::map>(i), TestInts<std::unordered_map>(i));
        fflush(fp);
      }
    }
  
    fclose(fp);
  }

  return 0;
}

