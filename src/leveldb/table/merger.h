//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_MERGER_H_
#define STORAGE_LEVELDB_TABLE_MERGER_H_

namespace leveldb {

class Comparator;
class Iterator;

//ticoin Return an iterator that provided the union of the data in
//ticoin children[0,n-1].  Takes ownership of the child iterators and
//ticoin will delete them when the result iterator is deleted.
//
//ticoin The result does no duplicate suppression.  I.e., if a particular
//ticoin key is present in K child iterators, it will be yielded K times.
//
//ticoin REQUIRES: n >= 0
extern Iterator* NewMergingIterator(
    const Comparator* comparator, Iterator** children, int n);

}  //ticoin namespace leveldb

#endif  //ticoin STORAGE_LEVELDB_TABLE_MERGER_H_
