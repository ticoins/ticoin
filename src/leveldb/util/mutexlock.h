//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_MUTEXLOCK_H_
#define STORAGE_LEVELDB_UTIL_MUTEXLOCK_H_

#include "port/port.h"
#include "port/thread_annotations.h"

namespace leveldb {

//ticoin Helper class that locks a mutex on construction and unlocks the mutex when
//ticoin the destructor of the MutexLock object is invoked.
//
//ticoin Typical usage:
//
//ticoin   void MyClass::MyMethod() {
//ticoin     MutexLock l(&mu_);       //ticoin mu_ is an instance variable
//ticoin     ... some complex code, possibly with multiple return paths ...
//ticoin   }

class SCOPED_LOCKABLE MutexLock {
 public:
  explicit MutexLock(port::Mutex *mu) EXCLUSIVE_LOCK_FUNCTION(mu)
      : mu_(mu)  {
    this->mu_->Lock();
  }
  ~MutexLock() UNLOCK_FUNCTION() { this->mu_->Unlock(); }

 private:
  port::Mutex *const mu_;
  //ticoin No copying allowed
  MutexLock(const MutexLock&);
  void operator=(const MutexLock&);
};

}  //ticoin namespace leveldb


#endif  //ticoin STORAGE_LEVELDB_UTIL_MUTEXLOCK_H_
