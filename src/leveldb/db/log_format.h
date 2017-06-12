//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin Log format information shared by reader and writer.
//ticoin See ../doc/log_format.txt for more detail.

#ifndef STORAGE_LEVELDB_DB_LOG_FORMAT_H_
#define STORAGE_LEVELDB_DB_LOG_FORMAT_H_

namespace leveldb {
namespace log {

enum RecordType {
  //ticoin Zero is reserved for preallocated files
  kZeroType = 0,

  kFullType = 1,

  //ticoin For fragments
  kFirstType = 2,
  kMiddleType = 3,
  kLastType = 4
};
static const int kMaxRecordType = kLastType;

static const int kBlockSize = 32768;

//ticoin Header is checksum (4 bytes), type (1 byte), length (2 bytes).
static const int kHeaderSize = 4 + 1 + 2;

}  //ticoin namespace log
}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_LOG_FORMAT_H_
