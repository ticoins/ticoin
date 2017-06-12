//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin This file contains the specification, but not the implementations,
//ticoin of the types/operations/etc. that should be defined by a platform
//ticoin specific port_<platform>.h file.  Use this file as a reference for
//ticoin how to port this package to a new platform.

#ifndef STORAGE_LEVELDB_PORT_PORT_EXAMPLE_H_
#define STORAGE_LEVELDB_PORT_PORT_EXAMPLE_H_

namespace leveldb {
namespace port {

//ticoin TODO(jorlow): Many of these belong more in the environment class rather than
//ticoin               here. We should try moving them and see if it affects perf.

//ticoin The following boolean constant must be true on a little-endian machine
//ticoin and false otherwise.
static const bool kLittleEndian = true /* or some other expression */;

//ticoin ------------------ Threading -------------------

//ticoin A Mutex represents an exclusive lock.
class Mutex {
 public:
  Mutex();
  ~Mutex();

  //ticoin Lock the mutex.  Waits until other lockers have exited.
  //ticoin Will deadlock if the mutex is already locked by this thread.
  void Lock();

  //ticoin Unlock the mutex.
  //ticoin REQUIRES: This mutex was locked by this thread.
  void Unlock();

  //ticoin Optionally crash if this thread does not hold this mutex.
  //ticoin The implementation must be fast, especially if NDEBUG is
  //ticoin defined.  The implementation is allowed to skip all checks.
  void AssertHeld();
};

class CondVar {
 public:
  explicit CondVar(Mutex* mu);
  ~CondVar();

  //ticoin Atomically release *mu and block on this condition variable until
  //ticoin either a call to SignalAll(), or a call to Signal() that picks
  //ticoin this thread to wakeup.
  //ticoin REQUIRES: this thread holds *mu
  void Wait();

  //ticoin If there are some threads waiting, wake up at least one of them.
  void Signal();

  //ticoin Wake up all waiting threads.
  void SignallAll();
};

//ticoin Thread-safe initialization.
//ticoin Used as follows:
//ticoin      static port::OnceType init_control = LEVELDB_ONCE_INIT;
//ticoin      static void Initializer() { ... do something ...; }
//ticoin      ...
//ticoin      port::InitOnce(&init_control, &Initializer);
typedef intptr_t OnceType;
#define LEVELDB_ONCE_INIT 0
extern void InitOnce(port::OnceType*, void (*initializer)());

//ticoin A type that holds a pointer that can be read or written atomically
//ticoin (i.e., without word-tearing.)
class AtomicPointer {
 private:
  intptr_t rep_;
 public:
  //ticoin Initialize to arbitrary value
  AtomicPointer();

  //ticoin Initialize to hold v
  explicit AtomicPointer(void* v) : rep_(v) { }

  //ticoin Read and return the stored pointer with the guarantee that no
  //ticoin later memory access (read or write) by this thread can be
  //ticoin reordered ahead of this read.
  void* Acquire_Load() const;

  //ticoin Set v as the stored pointer with the guarantee that no earlier
  //ticoin memory access (read or write) by this thread can be reordered
  //ticoin after this store.
  void Release_Store(void* v);

  //ticoin Read the stored pointer with no ordering guarantees.
  void* NoBarrier_Load() const;

  //ticoin Set va as the stored pointer with no ordering guarantees.
  void NoBarrier_Store(void* v);
};

//ticoin ------------------ Compression -------------------

//ticoin Store the snappy compression of "input[0,input_length-1]" in *output.
//ticoin Returns false if snappy is not supported by this port.
extern bool Snappy_Compress(const char* input, size_t input_length,
                            std::string* output);

//ticoin If input[0,input_length-1] looks like a valid snappy compressed
//ticoin buffer, store the size of the uncompressed data in *result and
//ticoin return true.  Else return false.
extern bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                         size_t* result);

//ticoin Attempt to snappy uncompress input[0,input_length-1] into *output.
//ticoin Returns true if successful, false if the input is invalid lightweight
//ticoin compressed data.
//
//ticoin REQUIRES: at least the first "n" bytes of output[] must be writable
//ticoin where "n" is the result of a successful call to
//ticoin Snappy_GetUncompressedLength.
extern bool Snappy_Uncompress(const char* input_data, size_t input_length,
                              char* output);

//ticoin ------------------ Miscellaneous -------------------

//ticoin If heap profiling is not supported, returns false.
//ticoin Else repeatedly calls (*func)(arg, data, n) and then returns true.
//ticoin The concatenation of all "data[0,n-1]" fragments is the heap profile.
extern bool GetHeapProfile(void (*func)(void*, const char*, int), void* arg);

}  //ticoin namespace port
}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_PORT_PORT_EXAMPLE_H_
