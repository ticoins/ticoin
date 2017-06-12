//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin Must not be included from any .h files to avoid polluting the namespace
//ticoin with macros.

#ifndef STORAGE_LEVELDB_UTIL_LOGGING_H_
#define STORAGE_LEVELDB_UTIL_LOGGING_H_

#include <stdio.h>
#include <stdint.h>
#include <string>
#include "port/port.h"

namespace leveldb {

class Slice;
class WritableFile;

//ticoin Append a human-readable printout of "num" to *str
extern void AppendNumberTo(std::string* str, uint64_t num);

//ticoin Append a human-readable printout of "value" to *str.
//ticoin Escapes any non-printable characters found in "value".
extern void AppendEscapedStringTo(std::string* str, const Slice& value);

//ticoin Return a human-readable printout of "num"
extern std::string NumberToString(uint64_t num);

//ticoin Return a human-readable version of "value".
//ticoin Escapes any non-printable characters found in "value".
extern std::string EscapeString(const Slice& value);

//ticoin If *in starts with "c", advances *in past the first character and
//ticoin returns true.  Otherwise, returns false.
extern bool ConsumeChar(Slice* in, char c);

//ticoin Parse a human-readable number from "*in" into *value.  On success,
//ticoin advances "*in" past the consumed number and sets "*val" to the
//ticoin numeric value.  Otherwise, returns false and leaves *in in an
//ticoin unspecified state.
extern bool ConsumeDecimalNumber(Slice* in, uint64_t* val);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_UTIL_LOGGING_H_
