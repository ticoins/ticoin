//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_DB_IMPL_H_
#define STORAGE_LEVELDB_DB_DB_IMPL_H_

#include <deque>
#include <set>
#include "db/dbformat.h"
#include "db/log_writer.h"
#include "db/snapshot.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "port/port.h"
#include "port/thread_annotations.h"

namespace leveldb {

class MemTable;
class TableCache;
class Version;
class VersionEdit;
class VersionSet;

class DBImpl : public DB {
 public:
  DBImpl(const Options& options, const std::string& dbname);
  virtual ~DBImpl();

  //ticoin Implementations of the DB interface
  virtual Status Put(const WriteOptions&, const Slice& key, const Slice& value);
  virtual Status Delete(const WriteOptions&, const Slice& key);
  virtual Status Write(const WriteOptions& options, WriteBatch* updates);
  virtual Status Get(const ReadOptions& options,
                     const Slice& key,
                     std::string* value);
  virtual Iterator* NewIterator(const ReadOptions&);
  virtual const Snapshot* GetSnapshot();
  virtual void ReleaseSnapshot(const Snapshot* snapshot);
  virtual bool GetProperty(const Slice& property, std::string* value);
  virtual void GetApproximateSizes(const Range* range, int n, uint64_t* sizes);
  virtual void CompactRange(const Slice* begin, const Slice* end);

  //ticoin Extra methods (for testing) that are not in the public DB interface

  //ticoin Compact any files in the named level that overlap [*begin,*end]
  void TEST_CompactRange(int level, const Slice* begin, const Slice* end);

  //ticoin Force current memtable contents to be compacted.
  Status TEST_CompactMemTable();

  //ticoin Return an internal iterator over the current state of the database.
  //ticoin The keys of this iterator are internal keys (see format.h).
  //ticoin The returned iterator should be deleted when no longer needed.
  Iterator* TEST_NewInternalIterator();

  //ticoin Return the maximum overlapping data (in bytes) at next level for any
  //ticoin file at a level >= 1.
  int64_t TEST_MaxNextLevelOverlappingBytes();

  //ticoin Record a sample of bytes read at the specified internal key.
  //ticoin Samples are taken approximately once every config::kReadBytesPeriod
  //ticoin bytes.
  void RecordReadSample(Slice key);

 private:
  friend class DB;
  struct CompactionState;
  struct Writer;

  Iterator* NewInternalIterator(const ReadOptions&,
                                SequenceNumber* latest_snapshot,
                                uint32_t* seed);

  Status NewDB();

  //ticoin Recover the descriptor from persistent storage.  May do a significant
  //ticoin amount of work to recover recently logged updates.  Any changes to
  //ticoin be made to the descriptor are added to *edit.
  Status Recover(VersionEdit* edit) EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void MaybeIgnoreError(Status* s) const;

  //ticoin Delete any unneeded files and stale in-memory entries.
  void DeleteObsoleteFiles();

  //ticoin Compact the in-memory write buffer to disk.  Switches to a new
  //ticoin log-file/memtable and writes a new descriptor iff successful.
  //ticoin Errors are recorded in bg_error_.
  void CompactMemTable() EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  Status RecoverLogFile(uint64_t log_number,
                        VersionEdit* edit,
                        SequenceNumber* max_sequence)
      EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  Status WriteLevel0Table(MemTable* mem, VersionEdit* edit, Version* base)
      EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  Status MakeRoomForWrite(bool force /* compact even if there is room? */)
      EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  WriteBatch* BuildBatchGroup(Writer** last_writer);

  void RecordBackgroundError(const Status& s);

  void MaybeScheduleCompaction() EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  static void BGWork(void* db);
  void BackgroundCall();
  void  BackgroundCompaction() EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  void CleanupCompaction(CompactionState* compact)
      EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  Status DoCompactionWork(CompactionState* compact)
      EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  Status OpenCompactionOutputFile(CompactionState* compact);
  Status FinishCompactionOutputFile(CompactionState* compact, Iterator* input);
  Status InstallCompactionResults(CompactionState* compact)
      EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  //ticoin Constant after construction
  Env* const env_;
  const InternalKeyComparator internal_comparator_;
  const InternalFilterPolicy internal_filter_policy_;
  const Options options_;  //ticoin options_.comparator == &internal_comparator_
  bool owns_info_log_;
  bool owns_cache_;
  const std::string dbname_;

  //ticoin table_cache_ provides its own synchronization
  TableCache* table_cache_;

  //ticoin Lock over the persistent DB state.  Non-NULL iff successfully acquired.
  FileLock* db_lock_;

  //ticoin State below is protected by mutex_
  port::Mutex mutex_;
  port::AtomicPointer shutting_down_;
  port::CondVar bg_cv_;          //ticoin Signalled when background work finishes
  MemTable* mem_;
  MemTable* imm_;                //ticoin Memtable being compacted
  port::AtomicPointer has_imm_;  //ticoin So bg thread can detect non-NULL imm_
  WritableFile* logfile_;
  uint64_t logfile_number_;
  log::Writer* log_;
  uint32_t seed_;                //ticoin For sampling.

  //ticoin Queue of writers.
  std::deque<Writer*> writers_;
  WriteBatch* tmp_batch_;

  SnapshotList snapshots_;

  //ticoin Set of table files to protect from deletion because they are
  //ticoin part of ongoing compactions.
  std::set<uint64_t> pending_outputs_;

  //ticoin Has a background compaction been scheduled or is running?
  bool bg_compaction_scheduled_;

  //ticoin Information for a manual compaction
  struct ManualCompaction {
    int level;
    bool done;
    const InternalKey* begin;   //ticoin NULL means beginning of key range
    const InternalKey* end;     //ticoin NULL means end of key range
    InternalKey tmp_storage;    //ticoin Used to keep track of compaction progress
  };
  ManualCompaction* manual_compaction_;

  VersionSet* versions_;

  //ticoin Have we encountered a background error in paranoid mode?
  Status bg_error_;

  //ticoin Per level compaction stats.  stats_[level] stores the stats for
  //ticoin compactions that produced data for the specified "level".
  struct CompactionStats {
    int64_t micros;
    int64_t bytes_read;
    int64_t bytes_written;

    CompactionStats() : micros(0), bytes_read(0), bytes_written(0) { }

    void Add(const CompactionStats& c) {
      this->micros += c.micros;
      this->bytes_read += c.bytes_read;
      this->bytes_written += c.bytes_written;
    }
  };
  CompactionStats stats_[config::kNumLevels];

  //ticoin No copying allowed
  DBImpl(const DBImpl&);
  void operator=(const DBImpl&);

  const Comparator* user_comparator() const {
    return internal_comparator_.user_comparator();
  }
};

//ticoin Sanitize db options.  The caller should delete result.info_log if
//ticoin it is not equal to src.info_log.
extern Options SanitizeOptions(const std::string& db,
                               const InternalKeyComparator* icmp,
                               const InternalFilterPolicy* ipolicy,
                               const Options& src);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_DB_IMPL_H_
