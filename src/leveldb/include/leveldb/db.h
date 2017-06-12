//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_DB_H_
#define STORAGE_LEVELDB_INCLUDE_DB_H_

#include <stdint.h>
#include <stdio.h>
#include "leveldb/iterator.h"
#include "leveldb/options.h"

namespace leveldb {

//ticoin Update Makefile if you change these
static const int kMajorVersion = 1;
static const int kMinorVersion = 17;

struct Options;
struct ReadOptions;
struct WriteOptions;
class WriteBatch;

//ticoin Abstract handle to particular state of a DB.
//ticoin A Snapshot is an immutable object and can therefore be safely
//ticoin accessed from multiple threads without any external synchronization.
class Snapshot {
 protected:
  virtual ~Snapshot();
};

//ticoin A range of keys
struct Range {
  Slice start;          //ticoin Included in the range
  Slice limit;          //ticoin Not included in the range

  Range() { }
  Range(const Slice& s, const Slice& l) : start(s), limit(l) { }
};

//ticoin A DB is a persistent ordered map from keys to values.
//ticoin A DB is safe for concurrent access from multiple threads without
//ticoin any external synchronization.
class DB {
 public:
  //ticoin Open the database with the specified "name".
  //ticoin Stores a pointer to a heap-allocated database in *dbptr and returns
  //ticoin OK on success.
  //ticoin Stores NULL in *dbptr and returns a non-OK status on error.
  //ticoin Caller should delete *dbptr when it is no longer needed.
  static Status Open(const Options& options,
                     const std::string& name,
                     DB** dbptr);

  DB() { }
  virtual ~DB();

  //ticoin Set the database entry for "key" to "value".  Returns OK on success,
  //ticoin and a non-OK status on error.
  //ticoin Note: consider setting options.sync = true.
  virtual Status Put(const WriteOptions& options,
                     const Slice& key,
                     const Slice& value) = 0;

  //ticoin Remove the database entry (if any) for "key".  Returns OK on
  //ticoin success, and a non-OK status on error.  It is not an error if "key"
  //ticoin did not exist in the database.
  //ticoin Note: consider setting options.sync = true.
  virtual Status Delete(const WriteOptions& options, const Slice& key) = 0;

  //ticoin Apply the specified updates to the database.
  //ticoin Returns OK on success, non-OK on failure.
  //ticoin Note: consider setting options.sync = true.
  virtual Status Write(const WriteOptions& options, WriteBatch* updates) = 0;

  //ticoin If the database contains an entry for "key" store the
  //ticoin corresponding value in *value and return OK.
  //
  //ticoin If there is no entry for "key" leave *value unchanged and return
  //ticoin a status for which Status::IsNotFound() returns true.
  //
  //ticoin May return some other Status on an error.
  virtual Status Get(const ReadOptions& options,
                     const Slice& key, std::string* value) = 0;

  //ticoin Return a heap-allocated iterator over the contents of the database.
  //ticoin The result of NewIterator() is initially invalid (caller must
  //ticoin call one of the Seek methods on the iterator before using it).
  //
  //ticoin Caller should delete the iterator when it is no longer needed.
  //ticoin The returned iterator should be deleted before this db is deleted.
  virtual Iterator* NewIterator(const ReadOptions& options) = 0;

  //ticoin Return a handle to the current DB state.  Iterators created with
  //ticoin this handle will all observe a stable snapshot of the current DB
  //ticoin state.  The caller must call ReleaseSnapshot(result) when the
  //ticoin snapshot is no longer needed.
  virtual const Snapshot* GetSnapshot() = 0;

  //ticoin Release a previously acquired snapshot.  The caller must not
  //ticoin use "snapshot" after this call.
  virtual void ReleaseSnapshot(const Snapshot* snapshot) = 0;

  //ticoin DB implementations can export properties about their state
  //ticoin via this method.  If "property" is a valid property understood by this
  //ticoin DB implementation, fills "*value" with its current value and returns
  //ticoin true.  Otherwise returns false.
  //
  //
  //ticoin Valid property names include:
  //
  //ticoin  "leveldb.num-files-at-level<N>" - return the number of files at level <N>,
  //ticoin     where <N> is an ASCII representation of a level number (e.g. "0").
  //ticoin  "leveldb.stats" - returns a multi-line string that describes statistics
  //ticoin     about the internal operation of the DB.
  //ticoin  "leveldb.sstables" - returns a multi-line string that describes all
  //ticoin     of the sstables that make up the db contents.
  virtual bool GetProperty(const Slice& property, std::string* value) = 0;

  //ticoin For each i in [0,n-1], store in "sizes[i]", the approximate
  //ticoin file system space used by keys in "[range[i].start .. range[i].limit)".
  //
  //ticoin Note that the returned sizes measure file system space usage, so
  //ticoin if the user data compresses by a factor of ten, the returned
  //ticoin sizes will be one-tenth the size of the corresponding user data size.
  //
  //ticoin The results may not include the sizes of recently written data.
  virtual void GetApproximateSizes(const Range* range, int n,
                                   uint64_t* sizes) = 0;

  //ticoin Compact the underlying storage for the key range [*begin,*end].
  //ticoin In particular, deleted and overwritten versions are discarded,
  //ticoin and the data is rearranged to reduce the cost of operations
  //ticoin needed to access the data.  This operation should typically only
  //ticoin be invoked by users who understand the underlying implementation.
  //
  //ticoin begin==NULL is treated as a key before all keys in the database.
  //ticoin end==NULL is treated as a key after all keys in the database.
  //ticoin Therefore the following call will compact the entire database:
  //ticoin    db->CompactRange(NULL, NULL);
  virtual void CompactRange(const Slice* begin, const Slice* end) = 0;

 private:
  //ticoin No copying allowed
  DB(const DB&);
  void operator=(const DB&);
};

//ticoin Destroy the contents of the specified database.
//ticoin Be very careful using this method.
Status DestroyDB(const std::string& name, const Options& options);

//ticoin If a DB cannot be opened, you may attempt to call this method to
//ticoin resurrect as much of the contents of the database as possible.
//ticoin Some data may be lost, so be careful when calling this function
//ticoin on a database that contains important information.
Status RepairDB(const std::string& dbname, const Options& options);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_DB_H_
