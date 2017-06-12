//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin TableBuilder provides the interface used to build a Table
//ticoin (an immutable and sorted map from keys to values).
//
//ticoin Multiple threads can invoke const methods on a TableBuilder without
//ticoin external synchronization, but if any of the threads may call a
//ticoin non-const method, all threads accessing the same TableBuilder must use
//ticoin external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_TABLE_BUILDER_H_
#define STORAGE_LEVELDB_INCLUDE_TABLE_BUILDER_H_

#include <stdint.h>
#include "leveldb/options.h"
#include "leveldb/status.h"

namespace leveldb {

class BlockBuilder;
class BlockHandle;
class WritableFile;

class TableBuilder {
 public:
  //ticoin Create a builder that will store the contents of the table it is
  //ticoin building in *file.  Does not close the file.  It is up to the
  //ticoin caller to close the file after calling Finish().
  TableBuilder(const Options& options, WritableFile* file);

  //ticoin REQUIRES: Either Finish() or Abandon() has been called.
  ~TableBuilder();

  //ticoin Change the options used by this builder.  Note: only some of the
  //ticoin option fields can be changed after construction.  If a field is
  //ticoin not allowed to change dynamically and its value in the structure
  //ticoin passed to the constructor is different from its value in the
  //ticoin structure passed to this method, this method will return an error
  //ticoin without changing any fields.
  Status ChangeOptions(const Options& options);

  //ticoin Add key,value to the table being constructed.
  //ticoin REQUIRES: key is after any previously added key according to comparator.
  //ticoin REQUIRES: Finish(), Abandon() have not been called
  void Add(const Slice& key, const Slice& value);

  //ticoin Advanced operation: flush any buffered key/value pairs to file.
  //ticoin Can be used to ensure that two adjacent entries never live in
  //ticoin the same data block.  Most clients should not need to use this method.
  //ticoin REQUIRES: Finish(), Abandon() have not been called
  void Flush();

  //ticoin Return non-ok iff some error has been detected.
  Status status() const;

  //ticoin Finish building the table.  Stops using the file passed to the
  //ticoin constructor after this function returns.
  //ticoin REQUIRES: Finish(), Abandon() have not been called
  Status Finish();

  //ticoin Indicate that the contents of this builder should be abandoned.  Stops
  //ticoin using the file passed to the constructor after this function returns.
  //ticoin If the caller is not going to call Finish(), it must call Abandon()
  //ticoin before destroying this builder.
  //ticoin REQUIRES: Finish(), Abandon() have not been called
  void Abandon();

  //ticoin Number of calls to Add() so far.
  uint64_t NumEntries() const;

  //ticoin Size of the file generated so far.  If invoked after a successful
  //ticoin Finish() call, returns the size of the final generated file.
  uint64_t FileSize() const;

 private:
  bool ok() const { return status().ok(); }
  void WriteBlock(BlockBuilder* block, BlockHandle* handle);
  void WriteRawBlock(const Slice& data, CompressionType, BlockHandle* handle);

  struct Rep;
  Rep* rep_;

  //ticoin No copying allowed
  TableBuilder(const TableBuilder&);
  void operator=(const TableBuilder&);
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_TABLE_BUILDER_H_
