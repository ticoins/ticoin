//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.
//
//ticoin Thread safety
//ticoin -------------
//
//ticoin Writes require external synchronization, most likely a mutex.
//ticoin Reads require a guarantee that the SkipList will not be destroyed
//ticoin while the read is in progress.  Apart from that, reads progress
//ticoin without any internal locking or synchronization.
//
//ticoin Invariants:
//
//ticoin (1) Allocated nodes are never deleted until the SkipList is
//ticoin destroyed.  This is trivially guaranteed by the code since we
//ticoin never delete any skip list nodes.
//
//ticoin (2) The contents of a Node except for the next/prev pointers are
//ticoin immutable after the Node has been linked into the SkipList.
//ticoin Only Insert() modifies the list, and it is careful to initialize
//ticoin a node and use release-stores to publish the nodes in one or
//ticoin more lists.
//
//ticoin ... prev vs. next pointer ordering ...

#include <assert.h>
#include <stdlib.h>
#include "port/port.h"
#include "util/arena.h"
#include "util/random.h"

namespace leveldb {

class Arena;

template<typename Key, class Comparator>
class SkipList {
 private:
  struct Node;

 public:
  //ticoin Create a new SkipList object that will use "cmp" for comparing keys,
  //ticoin and will allocate memory using "*arena".  Objects allocated in the arena
  //ticoin must remain allocated for the lifetime of the skiplist object.
  explicit SkipList(Comparator cmp, Arena* arena);

  //ticoin Insert key into the list.
  //ticoin REQUIRES: nothing that compares equal to key is currently in the list.
  void Insert(const Key& key);

  //ticoin Returns true iff an entry that compares equal to key is in the list.
  bool Contains(const Key& key) const;

  //ticoin Iteration over the contents of a skip list
  class Iterator {
   public:
    //ticoin Initialize an iterator over the specified list.
    //ticoin The returned iterator is not valid.
    explicit Iterator(const SkipList* list);

    //ticoin Returns true iff the iterator is positioned at a valid node.
    bool Valid() const;

    //ticoin Returns the key at the current position.
    //ticoin REQUIRES: Valid()
    const Key& key() const;

    //ticoin Advances to the next position.
    //ticoin REQUIRES: Valid()
    void Next();

    //ticoin Advances to the previous position.
    //ticoin REQUIRES: Valid()
    void Prev();

    //ticoin Advance to the first entry with a key >= target
    void Seek(const Key& target);

    //ticoin Position at the first entry in list.
    //ticoin Final state of iterator is Valid() iff list is not empty.
    void SeekToFirst();

    //ticoin Position at the last entry in list.
    //ticoin Final state of iterator is Valid() iff list is not empty.
    void SeekToLast();

   private:
    const SkipList* list_;
    Node* node_;
    //ticoin Intentionally copyable
  };

 private:
  enum { kMaxHeight = 12 };

  //ticoin Immutable after construction
  Comparator const compare_;
  Arena* const arena_;    //ticoin Arena used for allocations of nodes

  Node* const head_;

  //ticoin Modified only by Insert().  Read racily by readers, but stale
  //ticoin values are ok.
  port::AtomicPointer max_height_;   //ticoin Height of the entire list

  inline int GetMaxHeight() const {
    return static_cast<int>(
        reinterpret_cast<intptr_t>(max_height_.NoBarrier_Load()));
  }

  //ticoin Read/written only by Insert().
  Random rnd_;

  Node* NewNode(const Key& key, int height);
  int RandomHeight();
  bool Equal(const Key& a, const Key& b) const { return (compare_(a, b) == 0); }

  //ticoin Return true if key is greater than the data stored in "n"
  bool KeyIsAfterNode(const Key& key, Node* n) const;

  //ticoin Return the earliest node that comes at or after key.
  //ticoin Return NULL if there is no such node.
  //
  //ticoin If prev is non-NULL, fills prev[level] with pointer to previous
  //ticoin node at "level" for every level in [0..max_height_-1].
  Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

  //ticoin Return the latest node with a key < key.
  //ticoin Return head_ if there is no such node.
  Node* FindLessThan(const Key& key) const;

  //ticoin Return the last node in the list.
  //ticoin Return head_ if list is empty.
  Node* FindLast() const;

  //ticoin No copying allowed
  SkipList(const SkipList&);
  void operator=(const SkipList&);
};

//ticoin Implementation details follow
template<typename Key, class Comparator>
struct SkipList<Key,Comparator>::Node {
  explicit Node(const Key& k) : key(k) { }

  Key const key;

  //ticoin Accessors/mutators for links.  Wrapped in methods so we can
  //ticoin add the appropriate barriers as necessary.
  Node* Next(int n) {
    assert(n >= 0);
    //ticoin Use an 'acquire load' so that we observe a fully initialized
    //ticoin version of the returned Node.
    return reinterpret_cast<Node*>(next_[n].Acquire_Load());
  }
  void SetNext(int n, Node* x) {
    assert(n >= 0);
    //ticoin Use a 'release store' so that anybody who reads through this
    //ticoin pointer observes a fully initialized version of the inserted node.
    next_[n].Release_Store(x);
  }

  //ticoin No-barrier variants that can be safely used in a few locations.
  Node* NoBarrier_Next(int n) {
    assert(n >= 0);
    return reinterpret_cast<Node*>(next_[n].NoBarrier_Load());
  }
  void NoBarrier_SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].NoBarrier_Store(x);
  }

 private:
  //ticoin Array of length equal to the node height.  next_[0] is lowest level link.
  port::AtomicPointer next_[1];
};

template<typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node*
SkipList<Key,Comparator>::NewNode(const Key& key, int height) {
  char* mem = arena_->AllocateAligned(
      sizeof(Node) + sizeof(port::AtomicPointer) * (height - 1));
  return new (mem) Node(key);
}

template<typename Key, class Comparator>
inline SkipList<Key,Comparator>::Iterator::Iterator(const SkipList* list) {
  list_ = list;
  node_ = NULL;
}

template<typename Key, class Comparator>
inline bool SkipList<Key,Comparator>::Iterator::Valid() const {
  return node_ != NULL;
}

template<typename Key, class Comparator>
inline const Key& SkipList<Key,Comparator>::Iterator::key() const {
  assert(Valid());
  return node_->key;
}

template<typename Key, class Comparator>
inline void SkipList<Key,Comparator>::Iterator::Next() {
  assert(Valid());
  node_ = node_->Next(0);
}

template<typename Key, class Comparator>
inline void SkipList<Key,Comparator>::Iterator::Prev() {
  //ticoin Instead of using explicit "prev" links, we just search for the
  //ticoin last node that falls before key.
  assert(Valid());
  node_ = list_->FindLessThan(node_->key);
  if (node_ == list_->head_) {
    node_ = NULL;
  }
}

template<typename Key, class Comparator>
inline void SkipList<Key,Comparator>::Iterator::Seek(const Key& target) {
  node_ = list_->FindGreaterOrEqual(target, NULL);
}

template<typename Key, class Comparator>
inline void SkipList<Key,Comparator>::Iterator::SeekToFirst() {
  node_ = list_->head_->Next(0);
}

template<typename Key, class Comparator>
inline void SkipList<Key,Comparator>::Iterator::SeekToLast() {
  node_ = list_->FindLast();
  if (node_ == list_->head_) {
    node_ = NULL;
  }
}

template<typename Key, class Comparator>
int SkipList<Key,Comparator>::RandomHeight() {
  //ticoin Increase height with probability 1 in kBranching
  static const unsigned int kBranching = 4;
  int height = 1;
  while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxHeight);
  return height;
}

template<typename Key, class Comparator>
bool SkipList<Key,Comparator>::KeyIsAfterNode(const Key& key, Node* n) const {
  //ticoin NULL n is considered infinite
  return (n != NULL) && (compare_(n->key, key) < 0);
}

template<typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node* SkipList<Key,Comparator>::FindGreaterOrEqual(const Key& key, Node** prev)
    const {
  Node* x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (KeyIsAfterNode(key, next)) {
      //ticoin Keep searching in this list
      x = next;
    } else {
      if (prev != NULL) prev[level] = x;
      if (level == 0) {
        return next;
      } else {
        //ticoin Switch to next list
        level--;
      }
    }
  }
}

template<typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node*
SkipList<Key,Comparator>::FindLessThan(const Key& key) const {
  Node* x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    assert(x == head_ || compare_(x->key, key) < 0);
    Node* next = x->Next(level);
    if (next == NULL || compare_(next->key, key) >= 0) {
      if (level == 0) {
        return x;
      } else {
        //ticoin Switch to next list
        level--;
      }
    } else {
      x = next;
    }
  }
}

template<typename Key, class Comparator>
typename SkipList<Key,Comparator>::Node* SkipList<Key,Comparator>::FindLast()
    const {
  Node* x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (next == NULL) {
      if (level == 0) {
        return x;
      } else {
        //ticoin Switch to next list
        level--;
      }
    } else {
      x = next;
    }
  }
}

template<typename Key, class Comparator>
SkipList<Key,Comparator>::SkipList(Comparator cmp, Arena* arena)
    : compare_(cmp),
      arena_(arena),
      head_(NewNode(0 /* any key will do */, kMaxHeight)),
      max_height_(reinterpret_cast<void*>(1)),
      rnd_(0xdeadbeef) {
  for (int i = 0; i < kMaxHeight; i++) {
    head_->SetNext(i, NULL);
  }
}

template<typename Key, class Comparator>
void SkipList<Key,Comparator>::Insert(const Key& key) {
  //ticoin TODO(opt): We can use a barrier-free variant of FindGreaterOrEqual()
  //ticoin here since Insert() is externally synchronized.
  Node* prev[kMaxHeight];
  Node* x = FindGreaterOrEqual(key, prev);

  //ticoin Our data structure does not allow duplicate insertion
  assert(x == NULL || !Equal(key, x->key));

  int height = RandomHeight();
  if (height > GetMaxHeight()) {
    for (int i = GetMaxHeight(); i < height; i++) {
      prev[i] = head_;
    }
    //fprintf(stderr, "Change height from %d to %d\n", max_height_, height);

    //ticoin It is ok to mutate max_height_ without any synchronization
    //ticoin with concurrent readers.  A concurrent reader that observes
    //ticoin the new value of max_height_ will see either the old value of
    //ticoin new level pointers from head_ (NULL), or a new value set in
    //ticoin the loop below.  In the former case the reader will
    //ticoin immediately drop to the next level since NULL sorts after all
    //ticoin keys.  In the latter case the reader will use the new node.
    max_height_.NoBarrier_Store(reinterpret_cast<void*>(height));
  }

  x = NewNode(key, height);
  for (int i = 0; i < height; i++) {
    //ticoin NoBarrier_SetNext() suffices since we will add a barrier when
    //ticoin we publish a pointer to "x" in prev[i].
    x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
    prev[i]->SetNext(i, x);
  }
}

template<typename Key, class Comparator>
bool SkipList<Key,Comparator>::Contains(const Key& key) const {
  Node* x = FindGreaterOrEqual(key, NULL);
  if (x != NULL && Equal(key, x->key)) {
    return true;
  } else {
    return false;
  }
}

}  //ticoin namespace leveldb
