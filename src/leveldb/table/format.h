//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_FORMAT_H_
#define STORAGE_LEVELDB_TABLE_FORMAT_H_

#include <string>
#include <stdint.h>
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "leveldb/table_builder.h"

namespace leveldb {

class Block;
class RandomAccessFile;
struct ReadOptions;

//ticoin BlockHandle is a pointer to the extent of a file that stores a data
//ticoin block or a meta block.
class BlockHandle {
 public:
  BlockHandle();

  //ticoin The offset of the block in the file.
  uint64_t offset() const { return offset_; }
  void set_offset(uint64_t offset) { offset_ = offset; }

  //ticoin The size of the stored block
  uint64_t size() const { return size_; }
  void set_size(uint64_t size) { size_ = size; }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(Slice* input);

  //ticoin Maximum encoding length of a BlockHandle
  enum { kMaxEncodedLength = 10 + 10 };

 private:
  uint64_t offset_;
  uint64_t size_;
};

//ticoin Footer encapsulates the fixed information stored at the tail
//ticoin end of every table file.
class Footer {
 public:
  Footer() { }

  //ticoin The block handle for the metaindex block of the table
  const BlockHandle& metaindex_handle() const { return metaindex_handle_; }
  void set_metaindex_handle(const BlockHandle& h) { metaindex_handle_ = h; }

  //ticoin The block handle for the index block of the table
  const BlockHandle& index_handle() const {
    return index_handle_;
  }
  void set_index_handle(const BlockHandle& h) {
    index_handle_ = h;
  }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(Slice* input);

  //ticoin Encoded length of a Footer.  Note that the serialization of a
  //ticoin Footer will always occupy exactly this many bytes.  It consists
  //ticoin of two block handles and a magic number.
  enum {
    kEncodedLength = 2*BlockHandle::kMaxEncodedLength + 8
  };

 private:
  BlockHandle metaindex_handle_;
  BlockHandle index_handle_;
};

//ticoin kTableMagicNumber was picked by running
//ticoin    echo http://code.google.com/p/leveldb/ | sha1sum
//ticoin and taking the leading 64 bits.
static const uint64_t kTableMagicNumber = 0xdb4775248b80fb57ull;

//ticoin 1-byte type + 32-bit crc
static const size_t kBlockTrailerSize = 5;

struct BlockContents {
  Slice data;           //ticoin Actual contents of data
  bool cachable;        //ticoin True iff data can be cached
  bool heap_allocated;  //ticoin True iff caller should delete[] data.data()
};

//ticoin Read the block identified by "handle" from "file".  On failure
//ticoin return non-OK.  On success fill *result and return OK.
extern Status ReadBlock(RandomAccessFile* file,
                        const ReadOptions& options,
                        const BlockHandle& handle,
                        BlockContents* result);

//ticoin Implementation details follow.  Clients should ignore,

inline BlockHandle::BlockHandle()
    : offset_(~static_cast<uint64_t>(0)),
      size_(~static_cast<uint64_t>(0)) {
}

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_TABLE_FORMAT_H_
