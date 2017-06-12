//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

//ticoin AtomicPointer provides storage for a lock-free pointer.
//ticoin Platform-dependent implementation of AtomicPointer:
//ticoin - If the platform provides a cheap barrier, we use it with raw pointers
//ticoin - If cstdatomic is present (on newer versions of gcc, it is), we use
//ticoin   a cstdatomic-based AtomicPointer.  However we prefer the memory
//ticoin   barrier based version, because at least on a gcc 4.4 32-bit build
//ticoin   on linux, we have encountered a buggy <cstdatomic>
//ticoin   implementation.  Also, some <cstdatomic> implementations are much
//ticoin   slower than a memory-barrier based implementation (~16ns for
//ticoin   <cstdatomic> based acquire-load vs. ~1ns for a barrier based
//ticoin   acquire-load).
//ticoin This code is based on atomicops-internals-* in Google's perftools:
//ticoin http://code.google.com/p/google-perftools/source/browse/#svn%2Ftrunk%2Fsrc%2Fbase

#ifndef PORT_ATOMIC_POINTER_H_
#define PORT_ATOMIC_POINTER_H_

#include <stdint.h>
#ifdef LEVELDB_CSTDATOMIC_PRESENT
#include <cstdatomic>
#endif
#ifdef OS_WIN
#include <windows.h>
#endif
#ifdef OS_MACOSX
#include <libkern/OSAtomic.h>
#endif

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#elif defined(_M_IX86) || defined(__i386__) || defined(__i386)
#define ARCH_CPU_X86_FAMILY 1
#elif defined(__ARMEL__)
#define ARCH_CPU_ARM_FAMILY 1
#elif defined(__ppc__) || defined(__powerpc__) || defined(__powerpc64__)
#define ARCH_CPU_PPC_FAMILY 1
#endif

namespace leveldb {
namespace port {

//ticoin Define MemoryBarrier() if available
//ticoin Windows on x86
#if defined(OS_WIN) && defined(COMPILER_MSVC) && defined(ARCH_CPU_X86_FAMILY)
//ticoin windows.h already provides a MemoryBarrier(void) macro
//ticoin http://msdn.microsoft.com/en-us/library/ms684208(v=vs.85).aspx
#define LEVELDB_HAVE_MEMORY_BARRIER

//ticoin Mac OS
#elif defined(OS_MACOSX)
inline void MemoryBarrier() {
  OSMemoryBarrier();
}
#define LEVELDB_HAVE_MEMORY_BARRIER

//ticoin Gcc on x86
#elif defined(ARCH_CPU_X86_FAMILY) && defined(__GNUC__)
inline void MemoryBarrier() {
  //ticoin See http://gcc.gnu.org/ml/gcc/2003-04/msg01180.html for a discussion on
  //ticoin this idiom. Also see http://en.wikipedia.org/wiki/Memory_ordering.
  __asm__ __volatile__("" : : : "memory");
}
#define LEVELDB_HAVE_MEMORY_BARRIER

//ticoin Sun Studio
#elif defined(ARCH_CPU_X86_FAMILY) && defined(__SUNPRO_CC)
inline void MemoryBarrier() {
  //ticoin See http://gcc.gnu.org/ml/gcc/2003-04/msg01180.html for a discussion on
  //ticoin this idiom. Also see http://en.wikipedia.org/wiki/Memory_ordering.
  asm volatile("" : : : "memory");
}
#define LEVELDB_HAVE_MEMORY_BARRIER

//ticoin ARM Linux
#elif defined(ARCH_CPU_ARM_FAMILY) && defined(__linux__)
typedef void (*LinuxKernelMemoryBarrierFunc)(void);
//ticoin The Linux ARM kernel provides a highly optimized device-specific memory
//ticoin barrier function at a fixed memory address that is mapped in every
//ticoin user-level process.
//
//ticoin This beats using CPU-specific instructions which are, on single-core
//ticoin devices, un-necessary and very costly (e.g. ARMv7-A "dmb" takes more
//ticoin than 180ns on a Cortex-A8 like the one on a Nexus One). Benchmarking
//ticoin shows that the extra function call cost is completely negligible on
//ticoin multi-core devices.
//
inline void MemoryBarrier() {
  (*(LinuxKernelMemoryBarrierFunc)0xffff0fa0)();
}
#define LEVELDB_HAVE_MEMORY_BARRIER

//ticoin PPC
#elif defined(ARCH_CPU_PPC_FAMILY) && defined(__GNUC__)
inline void MemoryBarrier() {
  //ticoin TODO for some powerpc expert: is there a cheaper suitable variant?
  //ticoin Perhaps by having separate barriers for acquire and release ops.
  asm volatile("sync" : : : "memory");
}
#define LEVELDB_HAVE_MEMORY_BARRIER

#endif

//ticoin AtomicPointer built using platform-specific MemoryBarrier()
#if defined(LEVELDB_HAVE_MEMORY_BARRIER)
class AtomicPointer {
 private:
  void* rep_;
 public:
  AtomicPointer() { }
  explicit AtomicPointer(void* p) : rep_(p) {}
  inline void* NoBarrier_Load() const { return rep_; }
  inline void NoBarrier_Store(void* v) { rep_ = v; }
  inline void* Acquire_Load() const {
    void* result = rep_;
    MemoryBarrier();
    return result;
  }
  inline void Release_Store(void* v) {
    MemoryBarrier();
    rep_ = v;
  }
};

//ticoin AtomicPointer based on <cstdatomic>
#elif defined(LEVELDB_CSTDATOMIC_PRESENT)
class AtomicPointer {
 private:
  std::atomic<void*> rep_;
 public:
  AtomicPointer() { }
  explicit AtomicPointer(void* v) : rep_(v) { }
  inline void* Acquire_Load() const {
    return rep_.load(std::memory_order_acquire);
  }
  inline void Release_Store(void* v) {
    rep_.store(v, std::memory_order_release);
  }
  inline void* NoBarrier_Load() const {
    return rep_.load(std::memory_order_relaxed);
  }
  inline void NoBarrier_Store(void* v) {
    rep_.store(v, std::memory_order_relaxed);
  }
};

//ticoin Atomic pointer based on sparc memory barriers
#elif defined(__sparcv9) && defined(__GNUC__)
class AtomicPointer {
 private:
  void* rep_;
 public:
  AtomicPointer() { }
  explicit AtomicPointer(void* v) : rep_(v) { }
  inline void* Acquire_Load() const {
    void* val;
    __asm__ __volatile__ (
        "ldx [%[rep_]], %[val] \n\t"
         "membar #LoadLoad|#LoadStore \n\t"
        : [val] "=r" (val)
        : [rep_] "r" (&rep_)
        : "memory");
    return val;
  }
  inline void Release_Store(void* v) {
    __asm__ __volatile__ (
        "membar #LoadStore|#StoreStore \n\t"
        "stx %[v], [%[rep_]] \n\t"
        :
        : [rep_] "r" (&rep_), [v] "r" (v)
        : "memory");
  }
  inline void* NoBarrier_Load() const { return rep_; }
  inline void NoBarrier_Store(void* v) { rep_ = v; }
};

//ticoin Atomic pointer based on ia64 acq/rel
#elif defined(__ia64) && defined(__GNUC__)
class AtomicPointer {
 private:
  void* rep_;
 public:
  AtomicPointer() { }
  explicit AtomicPointer(void* v) : rep_(v) { }
  inline void* Acquire_Load() const {
    void* val    ;
    __asm__ __volatile__ (
        "ld8.acq %[val] = [%[rep_]] \n\t"
        : [val] "=r" (val)
        : [rep_] "r" (&rep_)
        : "memory"
        );
    return val;
  }
  inline void Release_Store(void* v) {
    __asm__ __volatile__ (
        "st8.rel [%[rep_]] = %[v]  \n\t"
        :
        : [rep_] "r" (&rep_), [v] "r" (v)
        : "memory"
        );
  }
  inline void* NoBarrier_Load() const { return rep_; }
  inline void NoBarrier_Store(void* v) { rep_ = v; }
};

//ticoin We have neither MemoryBarrier(), nor <cstdatomic>
#else
#error Please implement AtomicPointer for this platform.

#endif

#undef LEVELDB_HAVE_MEMORY_BARRIER
#undef ARCH_CPU_X86_FAMILY
#undef ARCH_CPU_ARM_FAMILY
#undef ARCH_CPU_PPC_FAMILY

}  //ticoin namespace port
}  //ticoin namespace leveldb

#endif  //ticoin PORT_ATOMIC_POINTER_H_
