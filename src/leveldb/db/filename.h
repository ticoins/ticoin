//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin File names used by DB code

#ifndef STORAGE_LEVELDB_DB_FILENAME_H_
#define STORAGE_LEVELDB_DB_FILENAME_H_

#include <stdint.h>
#include <string>
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "port/port.h"

namespace leveldb {

class Env;

enum FileType {
  kLogFile,
  kDBLockFile,
  kTableFile,
  kDescriptorFile,
  kCurrentFile,
  kTempFile,
  kInfoLogFile  //ticoin Either the current one, or an old one
};

//ticoin Return the name of the log file with the specified number
//ticoin in the db named by "dbname".  The result will be prefixed with
//ticoin "dbname".
extern std::string LogFileName(const std::string& dbname, uint64_t number);

//ticoin Return the name of the sstable with the specified number
//ticoin in the db named by "dbname".  The result will be prefixed with
//ticoin "dbname".
extern std::string TableFileName(const std::string& dbname, uint64_t number);

//ticoin Return the legacy file name for an sstable with the specified number
//ticoin in the db named by "dbname". The result will be prefixed with
//ticoin "dbname".
extern std::string SSTTableFileName(const std::string& dbname, uint64_t number);

//ticoin Return the name of the descriptor file for the db named by
//ticoin "dbname" and the specified incarnation number.  The result will be
//ticoin prefixed with "dbname".
extern std::string DescriptorFileName(const std::string& dbname,
                                      uint64_t number);

//ticoin Return the name of the current file.  This file contains the name
//ticoin of the current manifest file.  The result will be prefixed with
//ticoin "dbname".
extern std::string CurrentFileName(const std::string& dbname);

//ticoin Return the name of the lock file for the db named by
//ticoin "dbname".  The result will be prefixed with "dbname".
extern std::string LockFileName(const std::string& dbname);

//ticoin Return the name of a temporary file owned by the db named "dbname".
//ticoin The result will be prefixed with "dbname".
extern std::string TempFileName(const std::string& dbname, uint64_t number);

//ticoin Return the name of the info log file for "dbname".
extern std::string InfoLogFileName(const std::string& dbname);

//ticoin Return the name of the old info log file for "dbname".
extern std::string OldInfoLogFileName(const std::string& dbname);

//ticoin If filename is a leveldb file, store the type of the file in *type.
//ticoin The number encoded in the filename is stored in *number.  If the
//ticoin filename was successfully parsed, returns true.  Else return false.
extern bool ParseFileName(const std::string& filename,
                          uint64_t* number,
                          FileType* type);

//ticoin Make the CURRENT file point to the descriptor file with the
//ticoin specified number.
extern Status SetCurrentFile(Env* env, const std::string& dbname,
                             uint64_t descriptor_number);


}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_FILENAME_H_
