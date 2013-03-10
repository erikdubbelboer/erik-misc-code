
#include <string.h>  // memset()
#include <stdlib.h>  // malloc()
#include <assert.h>  // assert()

#include "hashtable.h"


void hashtable_init(hashtable_t* hash, uint64_t size) {
  assert(size > 0);

  memset(hash, 0, sizeof(hashtable_t));

  hash->bucket_count = (size + 3) / 4;  // Round up.
  hash->entries      = malloc(sizeof(entry_t ) * size);
  hash->buckets      = malloc(sizeof(entry_t*) * hash->bucket_count);
  
  // Complete the whole chain.
  for (uint64_t i = 0; i < size; ++i) {
    hash->entries[i].id = 0;

    // Make a chain of 1 entry.
    hash->entries[i].next = &hash->entries[i];
    hash->entries[i].prev = &hash->entries[i];

    hash->entries[i].newer = &hash->entries[(i + size + 1) % size];
    hash->entries[i].older = &hash->entries[(i + size - 1) % size];
  }

  hash->newest = &hash->entries[size - 1];
  hash->oldest = &hash->entries[0];

  for (uint64_t i = 0; i < hash->bucket_count; ++i) {
    hash->buckets[i] = 0;
  }
}


void hashtable_free(hashtable_t* hash) {
  free(hash->entries);
  free(hash->buckets);
}


entry_t* hashtable_find(hashtable_t* hash, uint64_t id) {
  assert(id != 0);

  uint64_t bucket = id % hash->bucket_count;
  entry_t* h      = hash->buckets[bucket];

  if (!h) {
    return 0;
  }

  entry_t* e = h;

  do {
    if (e->id == id) {
      // Move the entry to the head of the bucket chain.
      // It it's not already the head.
      if (e != h) {
        if (e != h->prev) {
          // Remove it from it's current position.
          e->next->prev = e->prev;
          e->prev->next = e->next;

          e->prev = h->prev;
          e->next = h;

          h->prev       = e;
          e->prev->next = e;
        }

        hash->buckets[bucket] = e;
      }


      entry_t* n = hash->newest;

      if (e != n) {
        if (e != n->newer) {
          e->newer->older = e->older;
          e->older->newer = e->newer;

          e->newer = n->newer;
          e->older = n;

          n->newer        = e;
          e->newer->older = e;
        }

        hash->newest = e;
      }

      return e;
    }

    e = e->next;

    // Do some assertions to make sure the bucket chain is still correct.
    assert(e != 0);
    assert(e->next->prev == e);
    assert(e->prev->next == e);
  } while (e != h);

  return 0;
}


static void hashtable_free_entry(hashtable_t* hash, entry_t* e) {
  // If the entry wasn't free we need to remove it from it's chain.
  if (e->id != 0) {
    uint64_t bucket = e->id % hash->bucket_count;

    if (hash->buckets[bucket] == e) {
      // If it's the head of the bucket chain it's also the only entry.
      hash->buckets[bucket] = 0;
    } else if (e->next != e) {
      e->next->prev = e->prev;
      e->prev->next = e->next;
    }
  
    e->id = 0;

    e->next = e;
    e->prev = e;
  }
}


// Return a free entry which will also be moved to the head of the age chain.
static entry_t* hashtable_alloc(hashtable_t* hash) {
  entry_t* e = hash->newest->newer;

  // Make the entry the newest.
  hash->newest = e;

  if (hash->oldest->id == 0) {
    hash->oldest = e;
  } else if (hash->oldest == e) {
    hash->oldest = e->newer;
  }

  hashtable_free_entry(hash, e);
  
  return e;
}


entry_t* hashtable_find_or_add(hashtable_t* hash, uint64_t id) {
  assert(id != 0);

  entry_t* e = hashtable_find(hash, id);
  
  if (!e) {
    uint64_t bucket = id % hash->bucket_count;
    entry_t* h      = hash->buckets[bucket];
    
    e = hashtable_alloc(hash);

    e->id = id;

    // Is there already a chain in our bucket?
    if (!!h) {
      e->next = h;
      e->prev = h->prev;

      h->prev = e;
      e->prev->next = e;
    } else {
      e->next = e;
      e->prev = e;
    }

    hash->buckets[bucket] = e;
  }

  return e;
}


// Evict the oldest entry.
void hashtable_evict(hashtable_t* hash) {
  if (hash->oldest == 0) {
    // Nothing to evict.
    return;
  }

  entry_t* e = hash->oldest;

  hash->oldest = e->newer;

  hashtable_free_entry(hash, e);
}

