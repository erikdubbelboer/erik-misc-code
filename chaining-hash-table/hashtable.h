
#ifndef SRC_HASHTABLE_
#define SRC_HASHTABLE_

#include <stdint.h>  // uint64_t


typedef struct entry_s {
  uint64_t id;

  // Bucket chain.
  struct entry_s* next;
  struct entry_s* prev;

  // Age chain.
  struct entry_s* newer;
  struct entry_s* older;
} entry_t;


typedef struct hashtable_s {
  uint64_t bucket_count;

  entry_t*  entries;
  entry_t** buckets;

  entry_t* newest;
  entry_t* oldest;
} hashtable_t;


void     hashtable_init(hashtable_t* hash, uint64_t size);
void     hashtable_free(hashtable_t* hash);
entry_t* hashtable_find(hashtable_t* hash, uint64_t id);
entry_t* hashtable_find_or_add(hashtable_t* hash, uint64_t id);
void     hashtable_evict(hashtable_t* hash);


#endif // SRC_HASHTABLE_

