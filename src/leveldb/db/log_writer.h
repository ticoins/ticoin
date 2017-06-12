//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_LOG_WRITER_H_
#define STORAGE_LEVELDB_DB_LOG_WRITER_H_

#include <stdint.h>
#include "db/log_format.h"
#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace leveldb {

class WritableFile;

namespace log {

class Writer {
 public:
  //ticoin Create a writer that will append data to "*dest".
  //ticoin "*dest" must be initially empty.
  //ticoin "*dest" must remain live while this Writer is in use.
  explicit Writer(WritableFile* dest);
  ~Writer();

  Status AddRecord(const Slice& slice);

 private:
  WritableFile* dest_;
  int block_offset_;       //ticoin Current offset in block

  //ticoin crc32c values for all supported record types.  These are
  //ticoin pre-computed to reduce the overhead of computing the crc of the
  //ticoin record type stored in the header.
  uint32_t type_crc_[kMaxRecordType + 1];

  Status EmitPhysicalRecord(RecordType type, const char* ptr, size_t length);

  //ticoin No copying allowed
  Writer(const Writer&);
  void operator=(const Writer&);
};

}  //ticoin namespace log
}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_LOG_WRITER_H_
