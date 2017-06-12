//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin A Status encapsulates the result of an operation.  It may indicate success,
//ticoin or it may indicate an error with an associated error message.
//
//ticoin Multiple threads can invoke const methods on a Status without
//ticoin external synchronization, but if any of the threads may call a
//ticoin non-const method, all threads accessing the same Status must use
//ticoin external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_STATUS_H_
#define STORAGE_LEVELDB_INCLUDE_STATUS_H_

#include <string>
#include "leveldb/slice.h"

namespace leveldb {

class Status {
 public:
  //ticoin Create a success status.
  Status() : state_(NULL) { }
  ~Status() { delete[] state_; }

  //ticoin Copy the specified status.
  Status(const Status& s);
  void operator=(const Status& s);

  //ticoin Return a success status.
  static Status OK() { return Status(); }

  //ticoin Return error status of an appropriate type.
  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotFound, msg, msg2);
  }
  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kCorruption, msg, msg2);
  }
  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotSupported, msg, msg2);
  }
  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kInvalidArgument, msg, msg2);
  }
  static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kIOError, msg, msg2);
  }

  //ticoin Returns true iff the status indicates success.
  bool ok() const { return (state_ == NULL); }

  //ticoin Returns true iff the status indicates a NotFound error.
  bool IsNotFound() const { return code() == kNotFound; }

  //ticoin Returns true iff the status indicates a Corruption error.
  bool IsCorruption() const { return code() == kCorruption; }

  //ticoin Returns true iff the status indicates an IOError.
  bool IsIOError() const { return code() == kIOError; }

  //ticoin Return a string representation of this status suitable for printing.
  //ticoin Returns the string "OK" for success.
  std::string ToString() const;

 private:
  //ticoin OK status has a NULL state_.  Otherwise, state_ is a new[] array
  //ticoin of the following form:
  //ticoin    state_[0..3] == length of message
  //ticoin    state_[4]    == code
  //ticoin    state_[5..]  == message
  const char* state_;

  enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5
  };

  Code code() const {
    return (state_ == NULL) ? kOk : static_cast<Code>(state_[4]);
  }

  Status(Code code, const Slice& msg, const Slice& msg2);
  static const char* CopyState(const char* s);
};

inline Status::Status(const Status& s) {
  state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
}
inline void Status::operator=(const Status& s) {
  //ticoin The following condition catches both aliasing (when this == &s),
  //ticoin and the common case where both s and *this are ok.
  if (state_ != s.state_) {
    delete[] state_;
    state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
  }
}

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_STATUS_H_
