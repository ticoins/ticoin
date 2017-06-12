//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_MEMTABLE_H_
#define STORAGE_LEVELDB_DB_MEMTABLE_H_

#include <string>
#include "leveldb/db.h"
#include "db/dbformat.h"
#include "db/skiplist.h"
#include "util/arena.h"

namespace leveldb {

class InternalKeyComparator;
class Mutex;
class MemTableIterator;

class MemTable {
 public:
  //ticoin MemTables are reference counted.  The initial reference count
  //ticoin is zero and the caller must call Ref() at least once.
  explicit MemTable(const InternalKeyComparator& comparator);

  //ticoin Increase reference count.
  void Ref() { ++refs_; }

  //ticoin Drop reference count.  Delete if no more references exist.
  void Unref() {
    --refs_;
    assert(refs_ >= 0);
    if (refs_ <= 0) {
      delete this;
    }
  }

  //ticoin Returns an estimate of the number of bytes of data in use by this
  //ticoin data structure.
  //
  //ticoin REQUIRES: external synchronization to prevent simultaneous
  //ticoin operations on the same MemTable.
  size_t ApproximateMemoryUsage();

  //ticoin Return an iterator that yields the contents of the memtable.
  //
  //ticoin The caller must ensure that the underlying MemTable remains live
  //ticoin while the returned iterator is live.  The keys returned by this
  //ticoin iterator are internal keys encoded by AppendInternalKey in the
  //ticoin db/format.{h,cc} module.
  Iterator* NewIterator();

  //ticoin Add an entry into memtable that maps key to value at the
  //ticoin specified sequence number and with the specified type.
  //ticoin Typically value will be empty if type==kTypeDeletion.
  void Add(SequenceNumber seq, ValueType type,
           const Slice& key,
           const Slice& value);

  //ticoin If memtable contains a value for key, store it in *value and return true.
  //ticoin If memtable contains a deletion for key, store a NotFound() error
  //ticoin in *status and return true.
  //ticoin Else, return false.
  bool Get(const LookupKey& key, std::string* value, Status* s);

 private:
  ~MemTable();  //ticoin Private since only Unref() should be used to delete it

  struct KeyComparator {
    const InternalKeyComparator comparator;
    explicit KeyComparator(const InternalKeyComparator& c) : comparator(c) { }
    int operator()(const char* a, const char* b) const;
  };
  friend class MemTableIterator;
  friend class MemTableBackwardIterator;

  typedef SkipList<const char*, KeyComparator> Table;

  KeyComparator comparator_;
  int refs_;
  Arena arena_;
  Table table_;

  //ticoin No copying allowed
  MemTable(const MemTable&);
  void operator=(const MemTable&);
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_MEMTABLE_H_
