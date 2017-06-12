//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_BUILDER_H_
#define STORAGE_LEVELDB_DB_BUILDER_H_

#include "leveldb/status.h"

namespace leveldb {

struct Options;
struct FileMetaData;

class Env;
class Iterator;
class TableCache;
class VersionEdit;

//ticoin Build a Table file from the contents of *iter.  The generated file
//ticoin will be named according to meta->number.  On success, the rest of
//ticoin *meta will be filled with metadata about the generated table.
//ticoin If no data is present in *iter, meta->file_size will be set to
//ticoin zero, and no Table file will be produced.
extern Status BuildTable(const std::string& dbname,
                         Env* env,
                         const Options& options,
                         TableCache* table_cache,
                         Iterator* iter,
                         FileMetaData* meta);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_BUILDER_H_
