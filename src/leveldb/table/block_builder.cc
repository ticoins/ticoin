//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin BlockBuilder generates blocks where keys are prefix-compressed:
//
//ticoin When we store a key, we drop the prefix shared with the previous
//ticoin string.  This helps reduce the space requirement significantly.
//ticoin Furthermore, once every K keys, we do not apply the prefix
//ticoin compression and store the entire key.  We call this a "restart
//ticoin point".  The tail end of the block stores the offsets of all of the
//ticoin restart points, and can be used to do a binary search when looking
//ticoin for a particular key.  Values are stored as-is (without compression)
//ticoin immediately following the corresponding key.
//
//ticoin An entry for a particular key-value pair has the form:
//ticoin     shared_bytes: varint32
//ticoin     unshared_bytes: varint32
//ticoin     value_length: varint32
//ticoin     key_delta: char[unshared_bytes]
//ticoin     value: char[value_length]
//ticoin shared_bytes == 0 for restart points.
//
//ticoin The trailer of the block has the form:
//ticoin     restarts: uint32[num_restarts]
//ticoin     num_restarts: uint32
//ticoin restarts[i] contains the offset within the block of the ith restart point.

#include "table/block_builder.h"

#include <algorithm>
#include <assert.h>
#include "leveldb/comparator.h"
#include "leveldb/table_builder.h"
#include "util/coding.h"

namespace leveldb {

BlockBuilder::BlockBuilder(const Options* options)
    : options_(options),
      restarts_(),
      counter_(0),
      finished_(false) {
  assert(options->block_restart_interval >= 1);
  restarts_.push_back(0);       //ticoin First restart point is at offset 0
}

void BlockBuilder::Reset() {
  buffer_.clear();
  restarts_.clear();
  restarts_.push_back(0);       //ticoin First restart point is at offset 0
  counter_ = 0;
  finished_ = false;
  last_key_.clear();
}

size_t BlockBuilder::CurrentSizeEstimate() const {
  return (buffer_.size() +                        //ticoin Raw data buffer
          restarts_.size() * sizeof(uint32_t) +   //ticoin Restart array
          sizeof(uint32_t));                      //ticoin Restart array length
}

Slice BlockBuilder::Finish() {
  //ticoin Append restart array
  for (size_t i = 0; i < restarts_.size(); i++) {
    PutFixed32(&buffer_, restarts_[i]);
  }
  PutFixed32(&buffer_, restarts_.size());
  finished_ = true;
  return Slice(buffer_);
}

void BlockBuilder::Add(const Slice& key, const Slice& value) {
  Slice last_key_piece(last_key_);
  assert(!finished_);
  assert(counter_ <= options_->block_restart_interval);
  assert(buffer_.empty() //ticoin No values yet?
         || options_->comparator->Compare(key, last_key_piece) > 0);
  size_t shared = 0;
  if (counter_ < options_->block_restart_interval) {
    //ticoin See how much sharing to do with previous string
    const size_t min_length = std::min(last_key_piece.size(), key.size());
    while ((shared < min_length) && (last_key_piece[shared] == key[shared])) {
      shared++;
    }
  } else {
    //ticoin Restart compression
    restarts_.push_back(buffer_.size());
    counter_ = 0;
  }
  const size_t non_shared = key.size() - shared;

  //ticoin Add "<shared><non_shared><value_size>" to buffer_
  PutVarint32(&buffer_, shared);
  PutVarint32(&buffer_, non_shared);
  PutVarint32(&buffer_, value.size());

  //ticoin Add string delta to buffer_ followed by value
  buffer_.append(key.data() + shared, non_shared);
  buffer_.append(value.data(), value.size());

  //ticoin Update state
  last_key_.resize(shared);
  last_key_.append(key.data() + shared, non_shared);
  assert(Slice(last_key_) == key);
  counter_++;
}

}  //ticoin namespace leveldb
