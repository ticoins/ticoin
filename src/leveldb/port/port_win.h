//ticoin LevelDB Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin See port_example.h for documentation for the following types/functions.

//ticoin Redistribution and use in source and binary forms, with or without
//ticoin modification, are permitted provided that the following conditions are met:
//ticoin 
//ticoin  * Redistributions of source code must retain the above copyright
//ticoin    notice, this list of conditions and the following disclaimer.
//ticoin  * Redistributions in binary form must reproduce the above copyright
//ticoin    notice, this list of conditions and the following disclaimer in the
//ticoin    documentation and/or other materials provided with the distribution.
//ticoin  * Neither the name of the University of California, Berkeley nor the
//ticoin    names of its contributors may be used to endorse or promote products
//ticoin    derived from this software without specific prior written permission.
//ticoin 
//ticoin THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//ticoin EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//ticoin WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//ticoin DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//ticoin DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//ticoin (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//ticoin LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ticoin ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//ticoin (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//ticoin SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef STORAGE_LEVELDB_PORT_PORT_WIN_H_
#define STORAGE_LEVELDB_PORT_PORT_WIN_H_

#ifdef _MSC_VER
#define snprintf _snprintf
#define close _close
#define fread_unlocked _fread_nolock
#endif

#include <string>
#include <stdint.h>
#ifdef SNAPPY
#include <snappy.h>
#endif

namespace leveldb {
namespace port {

//ticoin Windows is little endian (for now :p)
static const bool kLittleEndian = true;

class CondVar;

class Mutex {
 public:
  Mutex();
  ~Mutex();

  void Lock();
  void Unlock();
  void AssertHeld();

 private:
  friend class CondVar;
  //ticoin critical sections are more efficient than mutexes
  //ticoin but they are not recursive and can only be used to synchronize threads within the same process
  //ticoin we use opaque void * to avoid including windows.h in port_win.h
  void * cs_;

  //ticoin No copying
  Mutex(const Mutex&);
  void operator=(const Mutex&);
};

//ticoin the Win32 API offers a dependable condition variable mechanism, but only starting with
//ticoin Windows 2008 and Vista
//ticoin no matter what we will implement our own condition variable with a semaphore
//ticoin implementation as described in a paper written by Andrew D. Birrell in 2003
class CondVar {
 public:
  explicit CondVar(Mutex* mu);
  ~CondVar();
  void Wait();
  void Signal();
  void SignalAll();
 private:
  Mutex* mu_;
  
  Mutex wait_mtx_;
  long waiting_;
  
  void * sem1_;
  void * sem2_;
  
  
};

class OnceType {
public:
//ticoin    OnceType() : init_(false) {}
    OnceType(const OnceType &once) : init_(once.init_) {}
    OnceType(bool f) : init_(f) {}
    void InitOnce(void (*initializer)()) {
        mutex_.Lock();
        if (!init_) {
            init_ = true;
            initializer();
        }
        mutex_.Unlock();
    }

private:
    bool init_;
    Mutex mutex_;
};

#define LEVELDB_ONCE_INIT false
extern void InitOnce(port::OnceType*, void (*initializer)());

//ticoin Storage for a lock-free pointer
class AtomicPointer {
 private:
  void * rep_;
 public:
  AtomicPointer() : rep_(NULL) { }
  explicit AtomicPointer(void* v); 
  void* Acquire_Load() const;

  void Release_Store(void* v);

  void* NoBarrier_Load() const;

  void NoBarrier_Store(void* v);
};

inline bool Snappy_Compress(const char* input, size_t length,
                            ::std::string* output) {
#ifdef SNAPPY
  output->resize(snappy::MaxCompressedLength(length));
  size_t outlen;
  snappy::RawCompress(input, length, &(*output)[0], &outlen);
  output->resize(outlen);
  return true;
#endif

  return false;
}

inline bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                         size_t* result) {
#ifdef SNAPPY
  return snappy::GetUncompressedLength(input, length, result);
#else
  return false;
#endif
}

inline bool Snappy_Uncompress(const char* input, size_t length,
                              char* output) {
#ifdef SNAPPY
  return snappy::RawUncompress(input, length, output);
#else
  return false;
#endif
}

inline bool GetHeapProfile(void (*func)(void*, const char*, int), void* arg) {
  return false;
}

}
}

#endif  //ticoin STORAGE_LEVELDB_PORT_PORT_WIN_H_
