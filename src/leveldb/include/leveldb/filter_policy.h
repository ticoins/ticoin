//ticoin Copyright (c) 2012 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin A database can be configured with a custom FilterPolicy object.
//ticoin This object is responsible for creating a small filter from a set
//ticoin of keys.  These filters are stored in leveldb and are consulted
//ticoin automatically by leveldb to decide whether or not to read some
//ticoin information from disk. In many cases, a filter can cut down the
//ticoin number of disk seeks form a handful to a single disk seek per
//ticoin DB::Get() call.
//
//ticoin Most people will want to use the builtin bloom filter support (see
//ticoin NewBloomFilterPolicy() below).

#ifndef STORAGE_LEVELDB_INCLUDE_FILTER_POLICY_H_
#define STORAGE_LEVELDB_INCLUDE_FILTER_POLICY_H_

#include <string>

namespace leveldb {

class Slice;

class FilterPolicy {
 public:
  virtual ~FilterPolicy();

  //ticoin Return the name of this policy.  Note that if the filter encoding
  //ticoin changes in an incompatible way, the name returned by this method
  //ticoin must be changed.  Otherwise, old incompatible filters may be
  //ticoin passed to methods of this type.
  virtual const char* Name() const = 0;

  //ticoin keys[0,n-1] contains a list of keys (potentially with duplicates)
  //ticoin that are ordered according to the user supplied comparator.
  //ticoin Append a filter that summarizes keys[0,n-1] to *dst.
  //
  //ticoin Warning: do not change the initial contents of *dst.  Instead,
  //ticoin append the newly constructed filter to *dst.
  virtual void CreateFilter(const Slice* keys, int n, std::string* dst)
      const = 0;

  //ticoin "filter" contains the data appended by a preceding call to
  //ticoin CreateFilter() on this class.  This method must return true if
  //ticoin the key was in the list of keys passed to CreateFilter().
  //ticoin This method may return true or false if the key was not on the
  //ticoin list, but it should aim to return false with a high probability.
  virtual bool KeyMayMatch(const Slice& key, const Slice& filter) const = 0;
};

//ticoin Return a new filter policy that uses a bloom filter with approximately
//ticoin the specified number of bits per key.  A good value for bits_per_key
//ticoin is 10, which yields a filter with ~ 1% false positive rate.
//
//ticoin Callers must delete the result after any database that is using the
//ticoin result has been closed.
//
//ticoin Note: if you are using a custom comparator that ignores some parts
//ticoin of the keys being compared, you must not use NewBloomFilterPolicy()
//ticoin and must provide your own FilterPolicy that also ignores the
//ticoin corresponding parts of the keys.  For example, if the comparator
//ticoin ignores trailing spaces, it would be incorrect to use a
//ticoin FilterPolicy (like NewBloomFilterPolicy) that does not ignore
//ticoin trailing spaces in keys.
extern const FilterPolicy* NewBloomFilterPolicy(int bits_per_key);

}

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_FILTER_POLICY_H_
