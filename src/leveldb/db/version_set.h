//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin The representation of a DBImpl consists of a set of Versions.  The
//ticoin newest version is called "current".  Older versions may be kept
//ticoin around to provide a consistent view to live iterators.
//
//ticoin Each Version keeps track of a set of Table files per level.  The
//ticoin entire set of versions is maintained in a VersionSet.
//
//ticoin Version,VersionSet are thread-compatible, but require external
//ticoin synchronization on all accesses.

#ifndef STORAGE_LEVELDB_DB_VERSION_SET_H_
#define STORAGE_LEVELDB_DB_VERSION_SET_H_

#include <map>
#include <set>
#include <vector>
#include "db/dbformat.h"
#include "db/version_edit.h"
#include "port/port.h"
#include "port/thread_annotations.h"

namespace leveldb {

namespace log { class Writer; }

class Compaction;
class Iterator;
class MemTable;
class TableBuilder;
class TableCache;
class Version;
class VersionSet;
class WritableFile;

//ticoin Return the smallest index i such that files[i]->largest >= key.
//ticoin Return files.size() if there is no such file.
//ticoin REQUIRES: "files" contains a sorted list of non-overlapping files.
extern int FindFile(const InternalKeyComparator& icmp,
                    const std::vector<FileMetaData*>& files,
                    const Slice& key);

//ticoin Returns true iff some file in "files" overlaps the user key range
//ticoin [*smallest,*largest].
//ticoin smallest==NULL represents a key smaller than all keys in the DB.
//ticoin largest==NULL represents a key largest than all keys in the DB.
//ticoin REQUIRES: If disjoint_sorted_files, files[] contains disjoint ranges
//ticoin           in sorted order.
extern bool SomeFileOverlapsRange(
    const InternalKeyComparator& icmp,
    bool disjoint_sorted_files,
    const std::vector<FileMetaData*>& files,
    const Slice* smallest_user_key,
    const Slice* largest_user_key);

class Version {
 public:
  //ticoin Append to *iters a sequence of iterators that will
  //ticoin yield the contents of this Version when merged together.
  //ticoin REQUIRES: This version has been saved (see VersionSet::SaveTo)
  void AddIterators(const ReadOptions&, std::vector<Iterator*>* iters);

  //ticoin Lookup the value for key.  If found, store it in *val and
  //ticoin return OK.  Else return a non-OK status.  Fills *stats.
  //ticoin REQUIRES: lock is not held
  struct GetStats {
    FileMetaData* seek_file;
    int seek_file_level;
  };
  Status Get(const ReadOptions&, const LookupKey& key, std::string* val,
             GetStats* stats);

  //ticoin Adds "stats" into the current state.  Returns true if a new
  //ticoin compaction may need to be triggered, false otherwise.
  //ticoin REQUIRES: lock is held
  bool UpdateStats(const GetStats& stats);

  //ticoin Record a sample of bytes read at the specified internal key.
  //ticoin Samples are taken approximately once every config::kReadBytesPeriod
  //ticoin bytes.  Returns true if a new compaction may need to be triggered.
  //ticoin REQUIRES: lock is held
  bool RecordReadSample(Slice key);

  //ticoin Reference count management (so Versions do not disappear out from
  //ticoin under live iterators)
  void Ref();
  void Unref();

  void GetOverlappingInputs(
      int level,
      const InternalKey* begin,         //ticoin NULL means before all keys
      const InternalKey* end,           //ticoin NULL means after all keys
      std::vector<FileMetaData*>* inputs);

  //ticoin Returns true iff some file in the specified level overlaps
  //ticoin some part of [*smallest_user_key,*largest_user_key].
  //ticoin smallest_user_key==NULL represents a key smaller than all keys in the DB.
  //ticoin largest_user_key==NULL represents a key largest than all keys in the DB.
  bool OverlapInLevel(int level,
                      const Slice* smallest_user_key,
                      const Slice* largest_user_key);

  //ticoin Return the level at which we should place a new memtable compaction
  //ticoin result that covers the range [smallest_user_key,largest_user_key].
  int PickLevelForMemTableOutput(const Slice& smallest_user_key,
                                 const Slice& largest_user_key);

  int NumFiles(int level) const { return files_[level].size(); }

  //ticoin Return a human readable string that describes this version's contents.
  std::string DebugString() const;

 private:
  friend class Compaction;
  friend class VersionSet;

  class LevelFileNumIterator;
  Iterator* NewConcatenatingIterator(const ReadOptions&, int level) const;

  //ticoin Call func(arg, level, f) for every file that overlaps user_key in
  //ticoin order from newest to oldest.  If an invocation of func returns
  //ticoin false, makes no more calls.
  //
  //ticoin REQUIRES: user portion of internal_key == user_key.
  void ForEachOverlapping(Slice user_key, Slice internal_key,
                          void* arg,
                          bool (*func)(void*, int, FileMetaData*));

  VersionSet* vset_;            //ticoin VersionSet to which this Version belongs
  Version* next_;               //ticoin Next version in linked list
  Version* prev_;               //ticoin Previous version in linked list
  int refs_;                    //ticoin Number of live refs to this version

  //ticoin List of files per level
  std::vector<FileMetaData*> files_[config::kNumLevels];

  //ticoin Next file to compact based on seek stats.
  FileMetaData* file_to_compact_;
  int file_to_compact_level_;

  //ticoin Level that should be compacted next and its compaction score.
  //ticoin Score < 1 means compaction is not strictly needed.  These fields
  //ticoin are initialized by Finalize().
  double compaction_score_;
  int compaction_level_;

  explicit Version(VersionSet* vset)
      : vset_(vset), next_(this), prev_(this), refs_(0),
        file_to_compact_(NULL),
        file_to_compact_level_(-1),
        compaction_score_(-1),
        compaction_level_(-1) {
  }

  ~Version();

  //ticoin No copying allowed
  Version(const Version&);
  void operator=(const Version&);
};

class VersionSet {
 public:
  VersionSet(const std::string& dbname,
             const Options* options,
             TableCache* table_cache,
             const InternalKeyComparator*);
  ~VersionSet();

  //ticoin Apply *edit to the current version to form a new descriptor that
  //ticoin is both saved to persistent state and installed as the new
  //ticoin current version.  Will release *mu while actually writing to the file.
  //ticoin REQUIRES: *mu is held on entry.
  //ticoin REQUIRES: no other thread concurrently calls LogAndApply()
  Status LogAndApply(VersionEdit* edit, port::Mutex* mu)
      EXCLUSIVE_LOCKS_REQUIRED(mu);

  //ticoin Recover the last saved descriptor from persistent storage.
  Status Recover();

  //ticoin Return the current version.
  Version* current() const { return current_; }

  //ticoin Return the current manifest file number
  uint64_t ManifestFileNumber() const { return manifest_file_number_; }

  //ticoin Allocate and return a new file number
  uint64_t NewFileNumber() { return next_file_number_++; }

  //ticoin Arrange to reuse "file_number" unless a newer file number has
  //ticoin already been allocated.
  //ticoin REQUIRES: "file_number" was returned by a call to NewFileNumber().
  void ReuseFileNumber(uint64_t file_number) {
    if (next_file_number_ == file_number + 1) {
      next_file_number_ = file_number;
    }
  }

  //ticoin Return the number of Table files at the specified level.
  int NumLevelFiles(int level) const;

  //ticoin Return the combined file size of all files at the specified level.
  int64_t NumLevelBytes(int level) const;

  //ticoin Return the last sequence number.
  uint64_t LastSequence() const { return last_sequence_; }

  //ticoin Set the last sequence number to s.
  void SetLastSequence(uint64_t s) {
    assert(s >= last_sequence_);
    last_sequence_ = s;
  }

  //ticoin Mark the specified file number as used.
  void MarkFileNumberUsed(uint64_t number);

  //ticoin Return the current log file number.
  uint64_t LogNumber() const { return log_number_; }

  //ticoin Return the log file number for the log file that is currently
  //ticoin being compacted, or zero if there is no such log file.
  uint64_t PrevLogNumber() const { return prev_log_number_; }

  //ticoin Pick level and inputs for a new compaction.
  //ticoin Returns NULL if there is no compaction to be done.
  //ticoin Otherwise returns a pointer to a heap-allocated object that
  //ticoin describes the compaction.  Caller should delete the result.
  Compaction* PickCompaction();

  //ticoin Return a compaction object for compacting the range [begin,end] in
  //ticoin the specified level.  Returns NULL if there is nothing in that
  //ticoin level that overlaps the specified range.  Caller should delete
  //ticoin the result.
  Compaction* CompactRange(
      int level,
      const InternalKey* begin,
      const InternalKey* end);

  //ticoin Return the maximum overlapping data (in bytes) at next level for any
  //ticoin file at a level >= 1.
  int64_t MaxNextLevelOverlappingBytes();

  //ticoin Create an iterator that reads over the compaction inputs for "*c".
  //ticoin The caller should delete the iterator when no longer needed.
  Iterator* MakeInputIterator(Compaction* c);

  //ticoin Returns true iff some level needs a compaction.
  bool NeedsCompaction() const {
    Version* v = current_;
    return (v->compaction_score_ >= 1) || (v->file_to_compact_ != NULL);
  }

  //ticoin Add all files listed in any live version to *live.
  //ticoin May also mutate some internal state.
  void AddLiveFiles(std::set<uint64_t>* live);

  //ticoin Return the approximate offset in the database of the data for
  //ticoin "key" as of version "v".
  uint64_t ApproximateOffsetOf(Version* v, const InternalKey& key);

  //ticoin Return a human-readable short (single-line) summary of the number
  //ticoin of files per level.  Uses *scratch as backing store.
  struct LevelSummaryStorage {
    char buffer[100];
  };
  const char* LevelSummary(LevelSummaryStorage* scratch) const;

 private:
  class Builder;

  friend class Compaction;
  friend class Version;

  void Finalize(Version* v);

  void GetRange(const std::vector<FileMetaData*>& inputs,
                InternalKey* smallest,
                InternalKey* largest);

  void GetRange2(const std::vector<FileMetaData*>& inputs1,
                 const std::vector<FileMetaData*>& inputs2,
                 InternalKey* smallest,
                 InternalKey* largest);

  void SetupOtherInputs(Compaction* c);

  //ticoin Save current contents to *log
  Status WriteSnapshot(log::Writer* log);

  void AppendVersion(Version* v);

  Env* const env_;
  const std::string dbname_;
  const Options* const options_;
  TableCache* const table_cache_;
  const InternalKeyComparator icmp_;
  uint64_t next_file_number_;
  uint64_t manifest_file_number_;
  uint64_t last_sequence_;
  uint64_t log_number_;
  uint64_t prev_log_number_;  //ticoin 0 or backing store for memtable being compacted

  //ticoin Opened lazily
  WritableFile* descriptor_file_;
  log::Writer* descriptor_log_;
  Version dummy_versions_;  //ticoin Head of circular doubly-linked list of versions.
  Version* current_;        //ticoin == dummy_versions_.prev_

  //ticoin Per-level key at which the next compaction at that level should start.
  //ticoin Either an empty string, or a valid InternalKey.
  std::string compact_pointer_[config::kNumLevels];

  //ticoin No copying allowed
  VersionSet(const VersionSet&);
  void operator=(const VersionSet&);
};

//ticoin A Compaction encapsulates information about a compaction.
class Compaction {
 public:
  ~Compaction();

  //ticoin Return the level that is being compacted.  Inputs from "level"
  //ticoin and "level+1" will be merged to produce a set of "level+1" files.
  int level() const { return level_; }

  //ticoin Return the object that holds the edits to the descriptor done
  //ticoin by this compaction.
  VersionEdit* edit() { return &edit_; }

  //ticoin "which" must be either 0 or 1
  int num_input_files(int which) const { return inputs_[which].size(); }

  //ticoin Return the ith input file at "level()+which" ("which" must be 0 or 1).
  FileMetaData* input(int which, int i) const { return inputs_[which][i]; }

  //ticoin Maximum size of files to build during this compaction.
  uint64_t MaxOutputFileSize() const { return max_output_file_size_; }

  //ticoin Is this a trivial compaction that can be implemented by just
  //ticoin moving a single input file to the next level (no merging or splitting)
  bool IsTrivialMove() const;

  //ticoin Add all inputs to this compaction as delete operations to *edit.
  void AddInputDeletions(VersionEdit* edit);

  //ticoin Returns true if the information we have available guarantees that
  //ticoin the compaction is producing data in "level+1" for which no data exists
  //ticoin in levels greater than "level+1".
  bool IsBaseLevelForKey(const Slice& user_key);

  //ticoin Returns true iff we should stop building the current output
  //ticoin before processing "internal_key".
  bool ShouldStopBefore(const Slice& internal_key);

  //ticoin Release the input version for the compaction, once the compaction
  //ticoin is successful.
  void ReleaseInputs();

 private:
  friend class Version;
  friend class VersionSet;

  explicit Compaction(int level);

  int level_;
  uint64_t max_output_file_size_;
  Version* input_version_;
  VersionEdit edit_;

  //ticoin Each compaction reads inputs from "level_" and "level_+1"
  std::vector<FileMetaData*> inputs_[2];      //ticoin The two sets of inputs

  //ticoin State used to check for number of of overlapping grandparent files
  //ticoin (parent == level_ + 1, grandparent == level_ + 2)
  std::vector<FileMetaData*> grandparents_;
  size_t grandparent_index_;  //ticoin Index in grandparent_starts_
  bool seen_key_;             //ticoin Some output key has been seen
  int64_t overlapped_bytes_;  //ticoin Bytes of overlap between current output
                              //ticoin and grandparent files

  //ticoin State for implementing IsBaseLevelForKey

  //ticoin level_ptrs_ holds indices into input_version_->levels_: our state
  //ticoin is that we are positioned at one of the file ranges for each
  //ticoin higher level than the ones involved in this compaction (i.e. for
  //ticoin all L >= level_ + 2).
  size_t level_ptrs_[config::kNumLevels];
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_DB_VERSION_SET_H_
