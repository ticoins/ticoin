//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin Slice is a simple structure containing a pointer into some external
//ticoin storage and a size.  The user of a Slice must ensure that the slice
//ticoin is not used after the corresponding external storage has been
//ticoin deallocated.
//
//ticoin Multiple threads can invoke const methods on a Slice without
//ticoin external synchronization, but if any of the threads may call a
//ticoin non-const method, all threads accessing the same Slice must use
//ticoin external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace leveldb {

class Slice {
 public:
  //ticoin Create an empty slice.
  Slice() : data_(""), size_(0) { }

  //ticoin Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) { }

  //ticoin Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) { }

  //ticoin Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) { }

  //ticoin Return a pointer to the beginning of the referenced data
  const char* data() const { return data_; }

  //ticoin Return the length (in bytes) of the referenced data
  size_t size() const { return size_; }

  //ticoin Return true iff the length of the referenced data is zero
  bool empty() const { return size_ == 0; }

  //ticoin Return the ith byte in the referenced data.
  //ticoin REQUIRES: n < size()
  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  //ticoin Change this slice to refer to an empty array
  void clear() { data_ = ""; size_ = 0; }

  //ticoin Drop the first "n" bytes from this slice.
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  //ticoin Return a string that contains the copy of the referenced data.
  std::string ToString() const { return std::string(data_, size_); }

  //ticoin Three-way comparison.  Returns value:
  //ticoin   <  0 iff "*this" <  "b",
  //ticoin   == 0 iff "*this" == "b",
  //ticoin   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;

  //ticoin Return true iff "x" is a prefix of "*this"
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) &&
            (memcmp(data_, x.data_, x.size_) == 0));
  }

 private:
  const char* data_;
  size_t size_;

  //ticoin Intentionally copyable
};

inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) {
  return !(x == y);
}

inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_) r = -1;
    else if (size_ > b.size_) r = +1;
  }
  return r;
}

}  //ticoin namespace leveldb


#endif  //ticoin STORAGE_LEVELDB_INCLUDE_SLICE_H_
