//ticoin Copyright (c) 2012 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin A filter block is stored near the end of a Table file.  It contains
//ticoin filters (e.g., bloom filters) for all data blocks in the table combined
//ticoin into a single filter block.

#ifndef STORAGE_LEVELDB_TABLE_FILTER_BLOCK_H_
#define STORAGE_LEVELDB_TABLE_FILTER_BLOCK_H_

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "leveldb/slice.h"
#include "util/hash.h"

namespace leveldb {

class FilterPolicy;

//ticoin A FilterBlockBuilder is used to construct all of the filters for a
//ticoin particular Table.  It generates a single string which is stored as
//ticoin a special block in the Table.
//
//ticoin The sequence of calls to FilterBlockBuilder must match the regexp:
//ticoin      (StartBlock AddKey*)* Finish
class FilterBlockBuilder {
 public:
  explicit FilterBlockBuilder(const FilterPolicy*);

  void StartBlock(uint64_t block_offset);
  void AddKey(const Slice& key);
  Slice Finish();

 private:
  void GenerateFilter();

  const FilterPolicy* policy_;
  std::string keys_;              //ticoin Flattened key contents
  std::vector<size_t> start_;     //ticoin Starting index in keys_ of each key
  std::string result_;            //ticoin Filter data computed so far
  std::vector<Slice> tmp_keys_;   //ticoin policy_->CreateFilter() argument
  std::vector<uint32_t> filter_offsets_;

  //ticoin No copying allowed
  FilterBlockBuilder(const FilterBlockBuilder&);
  void operator=(const FilterBlockBuilder&);
};

class FilterBlockReader {
 public:
 //ticoin REQUIRES: "contents" and *policy must stay live while *this is live.
  FilterBlockReader(const FilterPolicy* policy, const Slice& contents);
  bool KeyMayMatch(uint64_t block_offset, const Slice& key);

 private:
  const FilterPolicy* policy_;
  const char* data_;    //ticoin Pointer to filter data (at block-start)
  const char* offset_;  //ticoin Pointer to beginning of offset array (at block-end)
  size_t num_;          //ticoin Number of entries in offset array
  size_t base_lg_;      //ticoin Encoding parameter (see kFilterBaseLg in .cc file)
};

}

#endif  //ticoin STORAGE_LEVELDB_TABLE_FILTER_BLOCK_H_
