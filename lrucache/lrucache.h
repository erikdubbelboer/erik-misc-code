//
// Copyright Erik Dubbelboer and other contributors. All rights reserved.
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
// Based loosely on http://timday.bitbucket.org/lru.html by Tim Day.
//
// Possible improvements:
//  - return an interator instead of a value.
//

#ifndef SRC_LRUCACHE_H_
#define SRC_LRUCACHE_H_


#include <list>
#include <map>
#include <assert.h>


/**
 * Simple Least Recently Used (LRU) cache.
 *
 * This cache will discards the least recently used item when space is needed.
 * All methods (except for resizing) are O(log n).
 *
 * Both KeyType and ItemType can be any type.
 * Keep in mind that internally an std::map is used to when using strings as keys use std::string.
 * When ItemType is a pointer the destructor will be called on eviction.
 */
template <class KeyType, class ItemType>
class LRUCache {
 private:
  typedef std::list<KeyType> KeyTypeList;
  typedef std::map<KeyType, std::pair<ItemType, typename KeyTypeList::iterator> > KeyItemMap;

  KeyTypeList lru;    // List of recently accessed items. The last item is the most recent.
  KeyItemMap  cache;  // Map of cached items.
  size_t      capacity;


  /**
   * These two classes are helper classes. They help make sure destructors are
   * called when ItemType is a pointer type.
   * The compiler will select the correct class to use based on ItemType.
   */
  template<typename T>
  class Pointer {
   public:
    static void Delete(const T v) {
      // Not a pointer so do nothing.
    }
  };

  template<typename T>
  class Pointer<T*> {
   public:
    static void Delete(T* v) {
      delete v;
    }
  };


  /**
   * Remove the least recently accessed element.
   *
   * O(log n)
   */
  void Evict() {
    const typename KeyItemMap::iterator i = cache.find(lru.front());  // O(log n)

    assert(i != cache.end());

    cache.erase(i);   // O(1)
    lru.pop_front();  // O(1)

    // Make sure the destructor is called.
    Pointer<ItemType>::Delete(i->second.first);
  }


 public:
  explicit LRUCache(size_t capacity) : capacity(capacity) {
  }


  size_t Capacity() {
    return capacity;
  }


  /**
   * Resize the cache.
   *
   * O(m log n)
   * where m is the number of items to evict.
   */
  void Resize(size_t capacity) {
    while (cache.size() > capacity) {
      Evict();
    }

    this->capacity = capacity;
  }


  /**
   * Get the entry at with the specified key.
   *
   * If the key was not set the default value for ItemType is returned.
   * For example for pointers this will be a NULL pointer. For integers
   * it will be 0.
   *
   * O(log n)
   */
  ItemType Get(const KeyType& key) {
    const typename KeyItemMap::iterator i = cache.find(key);  // O(log n)

    if (i == cache.end()) {
      return ItemType();  // Return the default value for ItemType.
    } else {
      // Move the key back to the end of the lru (making it the most recently visited).
      lru.splice(lru.end(), lru, i->second.second);  // O(1)

      return i->second.first;
    }
  }


  /**
   * Set the entry at key to value.
   * Removing the least recently accessed element if needed.
   *
   * O(log n)
   */
  void Set(const KeyType& key, const ItemType value) {
    // Setting the capacity to 0 will disable the cache.
    if (capacity == 0) {
      return;
    }

    const typename KeyItemMap::iterator i = cache.find(key);  // O(log n)

    if (i == cache.end()) {
      // Make sure we have room.
      if (cache.size() == capacity) {
        Evict();
      }

      // Insert the key into the back of the lru list (making it the most recently visited).
      // We need a pointer to the entry for the map.
      const typename KeyTypeList::iterator j = lru.insert(lru.end(), key);  // O(1)

      // Insert the value into the map.
      cache.insert(std::make_pair(key, std::make_pair(value, j)));  // O(log n)
    } else {
      // Make sure the destructor is called.
      Pointer<ItemType>::Delete(i->second.first);

      i->second.first = value;

      // Move the key back to the end of the lru (making it the most recently visited).
      lru.splice(lru.end(), lru, i->second.second);  // O(1)
    }
  }


  /**
   * Remove the element with the specified key.
   *
   * O(log n)
   */
  void Remove(const KeyType& key) {
    const typename KeyItemMap::iterator i = cache.find(key);  // O(log n)

    if (i == cache.end()) {
      return;
    }

    cache.erase(i);   // O(1)
    lru.remove(key);  // O(1)

    // Make sure the destructor is called.
    Pointer<ItemType>::Delete(i->second.first);
  }


  /**
   * Return a list of keys in the order of most to least recently used.
   */
  KeyTypeList List() {
    return KeyTypeList(lru.rbegin(), lru.rend());
  }
};


#endif  // SRC_LRUCACHE_H_

