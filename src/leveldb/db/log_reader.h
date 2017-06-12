//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_LOG_READER_H_
#define STORAGE_LEVELDB_DB_LOG_READER_H_

#include <stdint.h>

#include "db/log_format.h"
#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace leveldb {

class SequentialFile;

namespace log {

class Reader {
 public:
  //ticoin Interface for reporting errors.
  class Reporter {
   public:
    virtual ~Reporter();

    //ticoin Some corruption was detected.  "size" is the approximate number
    //ticoin of bytes dropped due to the corruption.
    virtual void Corruption(size_t bytes, const Status& status) = 0;
  };

  //ticoin Create a reader that will return log records from "*file".
  //ticoin "*file" must remain live while this Reader is in use.
  //
  //ticoin If "reporter" is non-NULL, it is notified whenever some data is
  //ticoin dropped due to a detected corruption.  "*reporter" must remain
  //ticoin live while this Reader is in use.
  //
  //ticoin If "checksum" is true, verify checksums if available.
  //
  //ticoin The Reader will start reading at the first record located at physical
  //ticoin position >= initial_offset within the file.
  Reader(SequentialFile* file, Reporter* reporter, bool checksum,
         uint64_t initial_offset);

  ~Reader();

  //ticoin Read the next record into *record.  Returns true if read
  //ticoin successfully, false if we hit end of the input.  May use
  //ticoin "*scratch" as temporary storage.  The contents filled in *record
  //ticoin will only be valid until the next mutating operation on this
  //ticoin reader or the next mutation to *scratch.
  bool ReadRecord(Slice* record, std::string* scratch);

  //ticoin Returns the physical offset of the last record returned by ReadRecord.
  //
  //ticoin Undefined before the first call to ReadRecord.
  uint64_t LastRecordOffset();

 private:
  SequentialFile* const file_;
  Reporter* const reporter_;
  bool const checksum_;
  char* const backing_store_;
  Slice buffer_;
  bool eof_;   //ticoin Last Read() indicated EOF by returning < kBlockSize

  //ticoin Offset of the last record returned by ReadRecord.
  uint64_t last_record_offset_;
  //ticoin Offset of the first location past the end of buffer_.
  uint64_t end_of_buffer_offset_;

  //ticoin Offset at which to start looking for the first record to return
  uint64_t const initial_offset_;

  //ticoin Extend record types with the following special values
  enum {
    kEof = kMaxRecordType + 1,
    //ticoin Returned whenever we find an invalid physical record.
    //ticoin Currently there are three situations in which this happens:
    //ticoin * The record has an invalid CRC (ReadPhysicalRecord reports a drop)
    //ticoin * The record is a 0-length record (No drop is reported)
    //ticoin * The record is below constructor's initial_offset (No drop is reported)
    kBadRecord = kMaxRecordType + 2
  };

  //ticoin Skips all blocks that are completely before "initial_offset_".
  //
  //ticoin Returns true on success. Handles reporting.
  bool SkipToInitialBlock();

  //ticoin Return type, or one of the preceding special values
  unsigned int ReadPhysicalRecord(Slice* result);

  //ticoin Reports dropped bytes to the reporter.
  //ticoin buffer_ must be updated to remove the dropped bytes prior to invocation.
  void ReportCorruption(size_t bytes, const char* reason);
  void ReportDrop(size_t bytes, const Status& reason);

  //ticoin No copying allowed
  Reader(const Reader&);
  void operator=(const Reader&);
};

}  //ticoin namespace log
}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_LOG_READER_H_
