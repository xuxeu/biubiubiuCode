/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef IA64_QATOMIC_H
#define IA64_QATOMIC_H

#include <QtCore/qglobal.h>

#if defined(Q_CC_INTEL)

// intrinsics provided by the Intel C++ Compiler
extern "C" {
    int _InterlockedExchange(volatile int *, int);
    int _InterlockedCompareExchange(volatile int *, int, int);
    int _InterlockedIncrement(volatile int *addend);
    int _InterlockedDecrement(volatile int *addend);
    void * _InterlockedCompareExchangePointer(void * volatile *, void *, void *);
    void * _InterlockedExchangePointer(void * volatile *, void *);
}

inline int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval)
{ return static_cast<int>(_InterlockedCompareExchange(ptr, newval, expected)) == expected; }

inline int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval)
{
    return _InterlockedCompareExchangePointer(reinterpret_cast<void * volatile *>(ptr),
                                              newval, expected) == expected;
}


inline int q_atomic_increment(volatile int *ptr)
{ return _InterlockedIncrement(ptr); }

inline int q_atomic_decrement(volatile int *ptr)
{ return _InterlockedDecrement(ptr); }

inline int q_atomic_set_int(volatile int *ptr, int newval)
{ return _InterlockedExchange(ptr, newval); }

inline void *q_atomic_set_ptr(volatile void *ptr, void *newval)
{
    return _InterlockedExchangePointer(reinterpret_cast<void * volatile *>(ptr), newval);
}

#else // !Q_CC_INTEL

#  if defined(Q_CC_GNU)

inline int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval)
{
    int ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg4.acq %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (*ptr)
                 : "r" (expected), "r" (newval)
                 : "memory");
    return ret == expected;
}

inline int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval) {
    void *ret;
    asm volatile("mov ar.ccv=%2\n"
                 ";;\n"
                 "cmpxchg8.acq %0=%1,%3,ar.ccv\n"
                 : "=r" (ret), "+m" (*reinterpret_cast<volatile unsigned long *>(ptr))
                 : "r" (expected), "r" (newval)
                 : "memory");
    return ret == expected;
}

#  else // !Q_CC_GNU

extern "C" {
    Q_CORE_EXPORT int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
    Q_CORE_EXPORT int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
} // extern "C"

#  endif // Q_CC_GNU

inline int q_atomic_increment(volatile int * const ptr)
{
    register int expected;
    for (;;) {
        expected = *ptr;
        if (q_atomic_test_and_set_int(ptr, expected, expected + 1)) break;
    }
    return expected != -1;
}

inline int q_atomic_decrement(volatile int * const ptr)
{
    register int expected;
    for (;;) {
        expected = *ptr;
        if (q_atomic_test_and_set_int(ptr, expected, expected - 1)) break;
    }
    return expected != 1;
}

inline int q_atomic_set_int(volatile int *ptr, int newval)
{
    register int expected;
    for (;;) {
        expected = *ptr;
        if (q_atomic_test_and_set_int(ptr, expected, newval)) break;
    }
    return expected;
}

inline void *q_atomic_set_ptr(volatile void *ptr, void *newval)
{
    register void *expected;
    for (;;) {
        expected = *reinterpret_cast<void * volatile *>(ptr);
        if (q_atomic_test_and_set_ptr(ptr, expected, newval)) break;
    }
    return expected;
}

#endif // Q_CC_INTEL

#endif // IA64_QATOMIC_H
