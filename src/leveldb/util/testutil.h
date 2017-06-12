//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_TESTUTIL_H_
#define STORAGE_LEVELDB_UTIL_TESTUTIL_H_

#include "leveldb/env.h"
#include "leveldb/slice.h"
#include "util/random.h"

namespace leveldb {
namespace test {

//ticoin Store in *dst a random string of length "len" and return a Slice that
//ticoin references the generated data.
extern Slice RandomString(Random* rnd, int len, std::string* dst);

//ticoin Return a random key with the specified length that may contain interesting
//ticoin characters (e.g. \x00, \xff, etc.).
extern std::string RandomKey(Random* rnd, int len);

//ticoin Store in *dst a string of length "len" that will compress to
//ticoin "N*compressed_fraction" bytes and return a Slice that references
//ticoin the generated data.
extern Slice CompressibleString(Random* rnd, double compressed_fraction,
                                size_t len, std::string* dst);

//ticoin A wrapper that allows injection of errors.
class ErrorEnv : public EnvWrapper {
 public:
  bool writable_file_error_;
  int num_writable_file_errors_;

  ErrorEnv() : EnvWrapper(Env::Default()),
               writable_file_error_(false),
               num_writable_file_errors_(0) { }

  virtual Status NewWritableFile(const std::string& fname,
                                 WritableFile** result) {
    if (writable_file_error_) {
      ++num_writable_file_errors_;
      *result = NULL;
      return Status::IOError(fname, "fake error");
    }
    return target()->NewWritableFile(fname, result);
  }
};

}  //ticoin namespace test
}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_UTIL_TESTUTIL_H_
