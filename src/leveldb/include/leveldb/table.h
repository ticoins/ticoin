//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_TABLE_H_
#define STORAGE_LEVELDB_INCLUDE_TABLE_H_

#include <stdint.h>
#include "leveldb/iterator.h"

namespace leveldb {

class Block;
class BlockHandle;
class Footer;
struct Options;
class RandomAccessFile;
struct ReadOptions;
class TableCache;

//ticoin A Table is a sorted map from strings to strings.  Tables are
//ticoin immutable and persistent.  A Table may be safely accessed from
//ticoin multiple threads without external synchronization.
class Table {
 public:
  //ticoin Attempt to open the table that is stored in bytes [0..file_size)
  //ticoin of "file", and read the metadata entries necessary to allow
  //ticoin retrieving data from the table.
  //
  //ticoin If successful, returns ok and sets "*table" to the newly opened
  //ticoin table.  The client should delete "*table" when no longer needed.
  //ticoin If there was an error while initializing the table, sets "*table"
  //ticoin to NULL and returns a non-ok status.  Does not take ownership of
  //ticoin "*source", but the client must ensure that "source" remains live
  //ticoin for the duration of the returned table's lifetime.
  //
  //ticoin *file must remain live while this Table is in use.
  static Status Open(const Options& options,
                     RandomAccessFile* file,
                     uint64_t file_size,
                     Table** table);

  ~Table();

  //ticoin Returns a new iterator over the table contents.
  //ticoin The result of NewIterator() is initially invalid (caller must
  //ticoin call one of the Seek methods on the iterator before using it).
  Iterator* NewIterator(const ReadOptions&) const;

  //ticoin Given a key, return an approximate byte offset in the file where
  //ticoin the data for that key begins (or would begin if the key were
  //ticoin present in the file).  The returned value is in terms of file
  //ticoin bytes, and so includes effects like compression of the underlying data.
  //ticoin E.g., the approximate offset of the last key in the table will
  //ticoin be close to the file length.
  uint64_t ApproximateOffsetOf(const Slice& key) const;

 private:
  struct Rep;
  Rep* rep_;

  explicit Table(Rep* rep) { rep_ = rep; }
  static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);

  //ticoin Calls (*handle_result)(arg, ...) with the entry found after a call
  //ticoin to Seek(key).  May not make such a call if filter policy says
  //ticoin that key is not present.
  friend class TableCache;
  Status InternalGet(
      const ReadOptions&, const Slice& key,
      void* arg,
      void (*handle_result)(void* arg, const Slice& k, const Slice& v));


  void ReadMeta(const Footer& footer);
  void ReadFilter(const Slice& filter_handle_value);

  //ticoin No copying allowed
  Table(const Table&);
  void operator=(const Table&);
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_TABLE_H_
