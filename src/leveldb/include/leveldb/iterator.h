//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin An iterator yields a sequence of key/value pairs from a source.
//ticoin The following class defines the interface.  Multiple implementations
//ticoin are provided by this library.  In particular, iterators are provided
//ticoin to access the contents of a Table or a DB.
//
//ticoin Multiple threads can invoke const methods on an Iterator without
//ticoin external synchronization, but if any of the threads may call a
//ticoin non-const method, all threads accessing the same Iterator must use
//ticoin external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_ITERATOR_H_
#define STORAGE_LEVELDB_INCLUDE_ITERATOR_H_

#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace leveldb {

class Iterator {
 public:
  Iterator();
  virtual ~Iterator();

  //ticoin An iterator is either positioned at a key/value pair, or
  //ticoin not valid.  This method returns true iff the iterator is valid.
  virtual bool Valid() const = 0;

  //ticoin Position at the first key in the source.  The iterator is Valid()
  //ticoin after this call iff the source is not empty.
  virtual void SeekToFirst() = 0;

  //ticoin Position at the last key in the source.  The iterator is
  //ticoin Valid() after this call iff the source is not empty.
  virtual void SeekToLast() = 0;

  //ticoin Position at the first key in the source that at or past target
  //ticoin The iterator is Valid() after this call iff the source contains
  //ticoin an entry that comes at or past target.
  virtual void Seek(const Slice& target) = 0;

  //ticoin Moves to the next entry in the source.  After this call, Valid() is
  //ticoin true iff the iterator was not positioned at the last entry in the source.
  //ticoin REQUIRES: Valid()
  virtual void Next() = 0;

  //ticoin Moves to the previous entry in the source.  After this call, Valid() is
  //ticoin true iff the iterator was not positioned at the first entry in source.
  //ticoin REQUIRES: Valid()
  virtual void Prev() = 0;

  //ticoin Return the key for the current entry.  The underlying storage for
  //ticoin the returned slice is valid only until the next modification of
  //ticoin the iterator.
  //ticoin REQUIRES: Valid()
  virtual Slice key() const = 0;

  //ticoin Return the value for the current entry.  The underlying storage for
  //ticoin the returned slice is valid only until the next modification of
  //ticoin the iterator.
  //ticoin REQUIRES: !AtEnd() && !AtStart()
  virtual Slice value() const = 0;

  //ticoin If an error has occurred, return it.  Else return an ok status.
  virtual Status status() const = 0;

  //ticoin Clients are allowed to register function/arg1/arg2 triples that
  //ticoin will be invoked when this iterator is destroyed.
  //
  //ticoin Note that unlike all of the preceding methods, this method is
  //ticoin not abstract and therefore clients should not override it.
  typedef void (*CleanupFunction)(void* arg1, void* arg2);
  void RegisterCleanup(CleanupFunction function, void* arg1, void* arg2);

 private:
  struct Cleanup {
    CleanupFunction function;
    void* arg1;
    void* arg2;
    Cleanup* next;
  };
  Cleanup cleanup_;

  //ticoin No copying allowed
  Iterator(const Iterator&);
  void operator=(const Iterator&);
};

//ticoin Return an empty iterator (yields nothing).
extern Iterator* NewEmptyIterator();

//ticoin Return an empty iterator with the specified status.
extern Iterator* NewErrorIterator(const Status& status);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_ITERATOR_H_
