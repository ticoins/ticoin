//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <vector>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace leveldb {

class Arena {
 public:
  Arena();
  ~Arena();

  //ticoin Return a pointer to a newly allocated memory block of "bytes" bytes.
  char* Allocate(size_t bytes);

  //ticoin Allocate memory with the normal alignment guarantees provided by malloc
  char* AllocateAligned(size_t bytes);

  //ticoin Returns an estimate of the total memory usage of data allocated
  //ticoin by the arena (including space allocated but not yet used for user
  //ticoin allocations).
  size_t MemoryUsage() const {
    return blocks_memory_ + blocks_.capacity() * sizeof(char*);
  }

 private:
  char* AllocateFallback(size_t bytes);
  char* AllocateNewBlock(size_t block_bytes);

  //ticoin Allocation state
  char* alloc_ptr_;
  size_t alloc_bytes_remaining_;

  //ticoin Array of new[] allocated memory blocks
  std::vector<char*> blocks_;

  //ticoin Bytes of memory in blocks allocated so far
  size_t blocks_memory_;

  //ticoin No copying allowed
  Arena(const Arena&);
  void operator=(const Arena&);
};

inline char* Arena::Allocate(size_t bytes) {
  //ticoin The semantics of what to return are a bit messy if we allow
  //ticoin 0-byte allocations, so we disallow them here (we don't need
  //ticoin them for our internal use).
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }
  return AllocateFallback(bytes);
}

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_UTIL_ARENA_H_
