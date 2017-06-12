//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin An Env is an interface used by the leveldb implementation to access
//ticoin operating system functionality like the filesystem etc.  Callers
//ticoin may wish to provide a custom Env object when opening a database to
//ticoin get fine gain control; e.g., to rate limit file system operations.
//
//ticoin All Env implementations are safe for concurrent access from
//ticoin multiple threads without any external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_ENV_H_
#define STORAGE_LEVELDB_INCLUDE_ENV_H_

#include <string>
#include <vector>
#include <stdarg.h>
#include <stdint.h>
#include "leveldb/status.h"

namespace leveldb {

class FileLock;
class Logger;
class RandomAccessFile;
class SequentialFile;
class Slice;
class WritableFile;

class Env {
 public:
  Env() { }
  virtual ~Env();

  //ticoin Return a default environment suitable for the current operating
  //ticoin system.  Sophisticated users may wish to provide their own Env
  //ticoin implementation instead of relying on this default environment.
  //
  //ticoin The result of Default() belongs to leveldb and must never be deleted.
  static Env* Default();

  //ticoin Create a brand new sequentially-readable file with the specified name.
  //ticoin On success, stores a pointer to the new file in *result and returns OK.
  //ticoin On failure stores NULL in *result and returns non-OK.  If the file does
  //ticoin not exist, returns a non-OK status.
  //
  //ticoin The returned file will only be accessed by one thread at a time.
  virtual Status NewSequentialFile(const std::string& fname,
                                   SequentialFile** result) = 0;

  //ticoin Create a brand new random access read-only file with the
  //ticoin specified name.  On success, stores a pointer to the new file in
  //ticoin *result and returns OK.  On failure stores NULL in *result and
  //ticoin returns non-OK.  If the file does not exist, returns a non-OK
  //ticoin status.
  //
  //ticoin The returned file may be concurrently accessed by multiple threads.
  virtual Status NewRandomAccessFile(const std::string& fname,
                                     RandomAccessFile** result) = 0;

  //ticoin Create an object that writes to a new file with the specified
  //ticoin name.  Deletes any existing file with the same name and creates a
  //ticoin new file.  On success, stores a pointer to the new file in
  //ticoin *result and returns OK.  On failure stores NULL in *result and
  //ticoin returns non-OK.
  //
  //ticoin The returned file will only be accessed by one thread at a time.
  virtual Status NewWritableFile(const std::string& fname,
                                 WritableFile** result) = 0;

  //ticoin Returns true iff the named file exists.
  virtual bool FileExists(const std::string& fname) = 0;

  //ticoin Store in *result the names of the children of the specified directory.
  //ticoin The names are relative to "dir".
  //ticoin Original contents of *results are dropped.
  virtual Status GetChildren(const std::string& dir,
                             std::vector<std::string>* result) = 0;

  //ticoin Delete the named file.
  virtual Status DeleteFile(const std::string& fname) = 0;

  //ticoin Create the specified directory.
  virtual Status CreateDir(const std::string& dirname) = 0;

  //ticoin Delete the specified directory.
  virtual Status DeleteDir(const std::string& dirname) = 0;

  //ticoin Store the size of fname in *file_size.
  virtual Status GetFileSize(const std::string& fname, uint64_t* file_size) = 0;

  //ticoin Rename file src to target.
  virtual Status RenameFile(const std::string& src,
                            const std::string& target) = 0;

  //ticoin Lock the specified file.  Used to prevent concurrent access to
  //ticoin the same db by multiple processes.  On failure, stores NULL in
  //ticoin *lock and returns non-OK.
  //
  //ticoin On success, stores a pointer to the object that represents the
  //ticoin acquired lock in *lock and returns OK.  The caller should call
  //ticoin UnlockFile(*lock) to release the lock.  If the process exits,
  //ticoin the lock will be automatically released.
  //
  //ticoin If somebody else already holds the lock, finishes immediately
  //ticoin with a failure.  I.e., this call does not wait for existing locks
  //ticoin to go away.
  //
  //ticoin May create the named file if it does not already exist.
  virtual Status LockFile(const std::string& fname, FileLock** lock) = 0;

  //ticoin Release the lock acquired by a previous successful call to LockFile.
  //ticoin REQUIRES: lock was returned by a successful LockFile() call
  //ticoin REQUIRES: lock has not already been unlocked.
  virtual Status UnlockFile(FileLock* lock) = 0;

  //ticoin Arrange to run "(*function)(arg)" once in a background thread.
  //
  //ticoin "function" may run in an unspecified thread.  Multiple functions
  //ticoin added to the same Env may run concurrently in different threads.
  //ticoin I.e., the caller may not assume that background work items are
  //ticoin serialized.
  virtual void Schedule(
      void (*function)(void* arg),
      void* arg) = 0;

  //ticoin Start a new thread, invoking "function(arg)" within the new thread.
  //ticoin When "function(arg)" returns, the thread will be destroyed.
  virtual void StartThread(void (*function)(void* arg), void* arg) = 0;

  //ticoin *path is set to a temporary directory that can be used for testing. It may
  //ticoin or many not have just been created. The directory may or may not differ
  //ticoin between runs of the same process, but subsequent calls will return the
  //ticoin same directory.
  virtual Status GetTestDirectory(std::string* path) = 0;

  //ticoin Create and return a log file for storing informational messages.
  virtual Status NewLogger(const std::string& fname, Logger** result) = 0;

  //ticoin Returns the number of micro-seconds since some fixed point in time. Only
  //ticoin useful for computing deltas of time.
  virtual uint64_t NowMicros() = 0;

  //ticoin Sleep/delay the thread for the perscribed number of micro-seconds.
  virtual void SleepForMicroseconds(int micros) = 0;

 private:
  //ticoin No copying allowed
  Env(const Env&);
  void operator=(const Env&);
};

//ticoin A file abstraction for reading sequentially through a file
class SequentialFile {
 public:
  SequentialFile() { }
  virtual ~SequentialFile();

  //ticoin Read up to "n" bytes from the file.  "scratch[0..n-1]" may be
  //ticoin written by this routine.  Sets "*result" to the data that was
  //ticoin read (including if fewer than "n" bytes were successfully read).
  //ticoin May set "*result" to point at data in "scratch[0..n-1]", so
  //ticoin "scratch[0..n-1]" must be live when "*result" is used.
  //ticoin If an error was encountered, returns a non-OK status.
  //
  //ticoin REQUIRES: External synchronization
  virtual Status Read(size_t n, Slice* result, char* scratch) = 0;

  //ticoin Skip "n" bytes from the file. This is guaranteed to be no
  //ticoin slower that reading the same data, but may be faster.
  //
  //ticoin If end of file is reached, skipping will stop at the end of the
  //ticoin file, and Skip will return OK.
  //
  //ticoin REQUIRES: External synchronization
  virtual Status Skip(uint64_t n) = 0;

 private:
  //ticoin No copying allowed
  SequentialFile(const SequentialFile&);
  void operator=(const SequentialFile&);
};

//ticoin A file abstraction for randomly reading the contents of a file.
class RandomAccessFile {
 public:
  RandomAccessFile() { }
  virtual ~RandomAccessFile();

  //ticoin Read up to "n" bytes from the file starting at "offset".
  //ticoin "scratch[0..n-1]" may be written by this routine.  Sets "*result"
  //ticoin to the data that was read (including if fewer than "n" bytes were
  //ticoin successfully read).  May set "*result" to point at data in
  //ticoin "scratch[0..n-1]", so "scratch[0..n-1]" must be live when
  //ticoin "*result" is used.  If an error was encountered, returns a non-OK
  //ticoin status.
  //
  //ticoin Safe for concurrent use by multiple threads.
  virtual Status Read(uint64_t offset, size_t n, Slice* result,
                      char* scratch) const = 0;

 private:
  //ticoin No copying allowed
  RandomAccessFile(const RandomAccessFile&);
  void operator=(const RandomAccessFile&);
};

//ticoin A file abstraction for sequential writing.  The implementation
//ticoin must provide buffering since callers may append small fragments
//ticoin at a time to the file.
class WritableFile {
 public:
  WritableFile() { }
  virtual ~WritableFile();

  virtual Status Append(const Slice& data) = 0;
  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;

 private:
  //ticoin No copying allowed
  WritableFile(const WritableFile&);
  void operator=(const WritableFile&);
};

//ticoin An interface for writing log messages.
class Logger {
 public:
  Logger() { }
  virtual ~Logger();

  //ticoin Write an entry to the log file with the specified format.
  virtual void Logv(const char* format, va_list ap) = 0;

 private:
  //ticoin No copying allowed
  Logger(const Logger&);
  void operator=(const Logger&);
};


//ticoin Identifies a locked file.
class FileLock {
 public:
  FileLock() { }
  virtual ~FileLock();
 private:
  //ticoin No copying allowed
  FileLock(const FileLock&);
  void operator=(const FileLock&);
};

//ticoin Log the specified data to *info_log if info_log is non-NULL.
extern void Log(Logger* info_log, const char* format, ...)
#   if defined(__GNUC__) || defined(__clang__)
    __attribute__((__format__ (__printf__, 2, 3)))
#   endif
    ;

//ticoin A utility routine: write "data" to the named file.
extern Status WriteStringToFile(Env* env, const Slice& data,
                                const std::string& fname);

//ticoin A utility routine: read contents of named file into *data
extern Status ReadFileToString(Env* env, const std::string& fname,
                               std::string* data);

//ticoin An implementation of Env that forwards all calls to another Env.
//ticoin May be useful to clients who wish to override just part of the
//ticoin functionality of another Env.
class EnvWrapper : public Env {
 public:
  //ticoin Initialize an EnvWrapper that delegates all calls to *t
  explicit EnvWrapper(Env* t) : target_(t) { }
  virtual ~EnvWrapper();

  //ticoin Return the target to which this Env forwards all calls
  Env* target() const { return target_; }

  //ticoin The following text is boilerplate that forwards all methods to target()
  Status NewSequentialFile(const std::string& f, SequentialFile** r) {
    return target_->NewSequentialFile(f, r);
  }
  Status NewRandomAccessFile(const std::string& f, RandomAccessFile** r) {
    return target_->NewRandomAccessFile(f, r);
  }
  Status NewWritableFile(const std::string& f, WritableFile** r) {
    return target_->NewWritableFile(f, r);
  }
  bool FileExists(const std::string& f) { return target_->FileExists(f); }
  Status GetChildren(const std::string& dir, std::vector<std::string>* r) {
    return target_->GetChildren(dir, r);
  }
  Status DeleteFile(const std::string& f) { return target_->DeleteFile(f); }
  Status CreateDir(const std::string& d) { return target_->CreateDir(d); }
  Status DeleteDir(const std::string& d) { return target_->DeleteDir(d); }
  Status GetFileSize(const std::string& f, uint64_t* s) {
    return target_->GetFileSize(f, s);
  }
  Status RenameFile(const std::string& s, const std::string& t) {
    return target_->RenameFile(s, t);
  }
  Status LockFile(const std::string& f, FileLock** l) {
    return target_->LockFile(f, l);
  }
  Status UnlockFile(FileLock* l) { return target_->UnlockFile(l); }
  void Schedule(void (*f)(void*), void* a) {
    return target_->Schedule(f, a);
  }
  void StartThread(void (*f)(void*), void* a) {
    return target_->StartThread(f, a);
  }
  virtual Status GetTestDirectory(std::string* path) {
    return target_->GetTestDirectory(path);
  }
  virtual Status NewLogger(const std::string& fname, Logger** result) {
    return target_->NewLogger(fname, result);
  }
  uint64_t NowMicros() {
    return target_->NowMicros();
  }
  void SleepForMicroseconds(int micros) {
    target_->SleepForMicroseconds(micros);
  }
 private:
  Env* target_;
};

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_INCLUDE_ENV_H_
