//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_TWO_LEVEL_ITERATOR_H_
#define STORAGE_LEVELDB_TABLE_TWO_LEVEL_ITERATOR_H_

#include "leveldb/iterator.h"

namespace leveldb {

struct ReadOptions;

//ticoin Return a new two level iterator.  A two-level iterator contains an
//ticoin index iterator whose values point to a sequence of blocks where
//ticoin each block is itself a sequence of key,value pairs.  The returned
//ticoin two-level iterator yields the concatenation of all key/value pairs
//ticoin in the sequence of blocks.  Takes ownership of "index_iter" and
//ticoin will delete it when no longer needed.
//
//ticoin Uses a supplied function to convert an index_iter value into
//ticoin an iterator over the contents of the corresponding block.
extern Iterator* NewTwoLevelIterator(
    Iterator* index_iter,
    Iterator* (*block_function)(
        void* arg,
        const ReadOptions& options,
        const Slice& index_value),
    void* arg,
    const ReadOptions& options);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_TABLE_TWO_LEVEL_ITERATOR_H_
