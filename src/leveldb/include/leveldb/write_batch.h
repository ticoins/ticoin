//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin WriteBatch holds a collection of updates to apply atomically to a DB.
//
//ticoin The updates are applied in the order in which they are added
//ticoin to the WriteBatch.  For example, the value of "key" will be "v3"
//ticoin after the following batch is written:
//
//ticoin    batch.Put("key", "v1");
//ticoin    batch.Delete("key");
//ticoin    batch.Put("key", "v2");
//ticoin    batch.Put("key", "v3");
//
//ticoin Multiple threads can invoke const methods on a WriteBatch without
//ticoin external synchronization, but if any of the threads may call a
//ticoin non-const method, all threads accessing the same WriteBatch must use
//ticoin external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_WRITE_BATCH_H_
#define STORAGE_LEVELDB_INCLUDE_WRITE_BATCH_H_

#include <string>
#include "leveldb/status.h"

namespace leveldb {

class Slice;

class WriteBatch {
 public:
  WriteBatch();
  ~WriteBatch();

  //ticoin Store the mapping "key->value" in the database.
  void Put(const Slice& key, const Slice& value);

  //ticoin If the database contains a mapping for "key", erase it.  Else do nothing.
  void Delete(const Slice& key);

  //ticoin Clear all updates buffered in this batch.
  void Clear();

  //ticoin Support for iterating over the contents of a batch.
  class Handler {
   public:
    virtual ~Handler();
    virtual void Put(const Slice& key, const Slice& value) = 0;
    virtual void Delete(const Slice& key) = 0;
  };
  Status Iterate(Handler* handler) const;

 private:
  friend class WriteBatchInternal;

  std::string rep_;  //ticoin See comment in write_batch.cc for the format of rep_

  //ticoin Intentionally copyable
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_WRITE_BATCH_H_
