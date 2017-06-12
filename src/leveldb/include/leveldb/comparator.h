//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
#define STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_

#include <string>

namespace leveldb {

class Slice;

//ticoin A Comparator object provides a total order across slices that are
//ticoin used as keys in an sstable or a database.  A Comparator implementation
//ticoin must be thread-safe since leveldb may invoke its methods concurrently
//ticoin from multiple threads.
class Comparator {
 public:
  virtual ~Comparator();

  //ticoin Three-way comparison.  Returns value:
  //ticoin   < 0 iff "a" < "b",
  //ticoin   == 0 iff "a" == "b",
  //ticoin   > 0 iff "a" > "b"
  virtual int Compare(const Slice& a, const Slice& b) const = 0;

  //ticoin The name of the comparator.  Used to check for comparator
  //ticoin mismatches (i.e., a DB created with one comparator is
  //ticoin accessed using a different comparator.
  //
  //ticoin The client of this package should switch to a new name whenever
  //ticoin the comparator implementation changes in a way that will cause
  //ticoin the relative ordering of any two keys to change.
  //
  //ticoin Names starting with "leveldb." are reserved and should not be used
  //ticoin by any clients of this package.
  virtual const char* Name() const = 0;

  //ticoin Advanced functions: these are used to reduce the space requirements
  //ticoin for internal data structures like index blocks.

  //ticoin If *start < limit, changes *start to a short string in [start,limit).
  //ticoin Simple comparator implementations may return with *start unchanged,
  //ticoin i.e., an implementation of this method that does nothing is correct.
  virtual void FindShortestSeparator(
      std::string* start,
      const Slice& limit) const = 0;

  //ticoin Changes *key to a short string >= *key.
  //ticoin Simple comparator implementations may return with *key unchanged,
  //ticoin i.e., an implementation of this method that does nothing is correct.
  virtual void FindShortSuccessor(std::string* key) const = 0;
};

//ticoin Return a builtin comparator that uses lexicographic byte-wise
//ticoin ordering.  The result remains the property of this module and
//ticoin must not be deleted.
extern const Comparator* BytewiseComparator();

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
