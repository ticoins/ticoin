//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_

#include <vector>

#include <stdint.h>
#include "leveldb/slice.h"

namespace leveldb {

struct Options;

class BlockBuilder {
 public:
  explicit BlockBuilder(const Options* options);

  //ticoin Reset the contents as if the BlockBuilder was just constructed.
  void Reset();

  //ticoin REQUIRES: Finish() has not been callled since the last call to Reset().
  //ticoin REQUIRES: key is larger than any previously added key
  void Add(const Slice& key, const Slice& value);

  //ticoin Finish building the block and return a slice that refers to the
  //ticoin block contents.  The returned slice will remain valid for the
  //ticoin lifetime of this builder or until Reset() is called.
  Slice Finish();

  //ticoin Returns an estimate of the current (uncompressed) size of the block
  //ticoin we are building.
  size_t CurrentSizeEstimate() const;

  //ticoin Return true iff no entries have been added since the last Reset()
  bool empty() const {
    return buffer_.empty();
  }

 private:
  const Options*        options_;
  std::string           buffer_;      //ticoin Destination buffer
  std::vector<uint32_t> restarts_;    //ticoin Restart points
  int                   counter_;     //ticoin Number of entries emitted since restart
  bool                  finished_;    //ticoin Has Finish() been called?
  std::string           last_key_;

  //ticoin No copying allowed
  BlockBuilder(const BlockBuilder&);
  void operator=(const BlockBuilder&);
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
