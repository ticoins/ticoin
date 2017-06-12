//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_HELPERS_MEMENV_MEMENV_H_
#define STORAGE_LEVELDB_HELPERS_MEMENV_MEMENV_H_

namespace leveldb {

class Env;

//ticoin Returns a new environment that stores its data in memory and delegates
//ticoin all non-file-storage tasks to base_env. The caller must delete the result
//ticoin when it is no longer needed.
//ticoin *base_env must remain live while the result is in use.
Env* NewMemEnv(Env* base_env);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_HELPERS_MEMENV_MEMENV_H_
