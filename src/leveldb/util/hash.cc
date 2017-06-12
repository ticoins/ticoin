//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <string.h>
#include "util/coding.h"
#include "util/hash.h"

//ticoin The FALLTHROUGH_INTENDED macro can be used to annotate implicit fall-through
//ticoin between switch labels. The real definition should be provided externally.
//ticoin This one is a fallback version for unsupported compilers.
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif

namespace leveldb {

uint32_t Hash(const char* data, size_t n, uint32_t seed) {
  //ticoin Similar to murmur hash
  const uint32_t m = 0xc6a4a793;
  const uint32_t r = 24;
  const char* limit = data + n;
  uint32_t h = seed ^ (n * m);

  //ticoin Pick up four bytes at a time
  while (data + 4 <= limit) {
    uint32_t w = DecodeFixed32(data);
    data += 4;
    h += w;
    h *= m;
    h ^= (h >> 16);
  }

  //ticoin Pick up remaining bytes
  switch (limit - data) {
    case 3:
      h += data[2] << 16;
      FALLTHROUGH_INTENDED;
    case 2:
      h += data[1] << 8;
      FALLTHROUGH_INTENDED;
    case 1:
      h += data[0];
      h *= m;
      h ^= (h >> r);
      break;
  }
  return h;
}


}  //ticoin namespace leveldb
