//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin A Cache is an interface that maps keys to values.  It has internal
//ticoin synchronization and may be safely accessed concurrently from
//ticoin multiple threads.  It may automatically evict entries to make room
//ticoin for new entries.  Values have a specified charge against the cache
//ticoin capacity.  For example, a cache where the values are variable
//ticoin length strings, may use the length of the string as the charge for
//ticoin the string.
//
//ticoin A builtin cache implementation with a least-recently-used eviction
//ticoin policy is provided.  Clients may use their own implementations if
//ticoin they want something more sophisticated (like scan-resistance, a
//ticoin custom eviction policy, variable cache sizing, etc.)

#ifndef STORAGE_LEVELDB_INCLUDE_CACHE_H_
#define STORAGE_LEVELDB_INCLUDE_CACHE_H_

#include <stdint.h>
#include "leveldb/slice.h"

namespace leveldb {

class Cache;

//ticoin Create a new cache with a fixed size capacity.  This implementation
//ticoin of Cache uses a least-recently-used eviction policy.
extern Cache* NewLRUCache(size_t capacity);

class Cache {
 public:
  Cache() { }

  //ticoin Destroys all existing entries by calling the "deleter"
  //ticoin function that was passed to the constructor.
  virtual ~Cache();

  //ticoin Opaque handle to an entry stored in the cache.
  struct Handle { };

  //ticoin Insert a mapping from key->value into the cache and assign it
  //ticoin the specified charge against the total cache capacity.
  //
  //ticoin Returns a handle that corresponds to the mapping.  The caller
  //ticoin must call this->Release(handle) when the returned mapping is no
  //ticoin longer needed.
  //
  //ticoin When the inserted entry is no longer needed, the key and
  //ticoin value will be passed to "deleter".
  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                         void (*deleter)(const Slice& key, void* value)) = 0;

  //ticoin If the cache has no mapping for "key", returns NULL.
  //
  //ticoin Else return a handle that corresponds to the mapping.  The caller
  //ticoin must call this->Release(handle) when the returned mapping is no
  //ticoin longer needed.
  virtual Handle* Lookup(const Slice& key) = 0;

  //ticoin Release a mapping returned by a previous Lookup().
  //ticoin REQUIRES: handle must not have been released yet.
  //ticoin REQUIRES: handle must have been returned by a method on *this.
  virtual void Release(Handle* handle) = 0;

  //ticoin Return the value encapsulated in a handle returned by a
  //ticoin successful Lookup().
  //ticoin REQUIRES: handle must not have been released yet.
  //ticoin REQUIRES: handle must have been returned by a method on *this.
  virtual void* Value(Handle* handle) = 0;

  //ticoin If the cache contains entry for key, erase it.  Note that the
  //ticoin underlying entry will be kept around until all existing handles
  //ticoin to it have been released.
  virtual void Erase(const Slice& key) = 0;

  //ticoin Return a new numeric id.  May be used by multiple clients who are
  //ticoin sharing the same cache to partition the key space.  Typically the
  //ticoin client will allocate a new id at startup and prepend the id to
  //ticoin its cache keys.
  virtual uint64_t NewId() = 0;

 private:
  void LRU_Remove(Handle* e);
  void LRU_Append(Handle* e);
  void Unref(Handle* e);

  struct Rep;
  Rep* rep_;

  //ticoin No copying allowed
  Cache(const Cache&);
  void operator=(const Cache&);
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_UTIL_CACHE_H_
