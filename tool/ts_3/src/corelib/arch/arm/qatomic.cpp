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

#include "arch/qatomic.h"

static char locks[256];

char *getLock(volatile void *addr)
{ return &locks[(ulong(addr) >> 2) & 0xff]; }

#ifdef Q_CC_GNU

inline char q_atomic_swp(volatile char *ptr, char newval)
{
    register int ret;
    asm volatile("swpb %0,%1,[%2]"
                 : "=r"(ret)
                 : "r"(newval), "r"(ptr)
                 : "cc", "memory");
    return ret;
}

#endif // Q_CC_GNU

extern "C" {

    int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval)
    {
        char *lock = getLock(ptr);
        int returnValue = 0;
        while (q_atomic_swp(lock, ~0) != 0)
            ;
        if (*ptr == expected) {
            *ptr = newval;
            returnValue = 1;
        }
        (void) q_atomic_swp(lock, 0);
        return returnValue;
    }

    int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval)
    {
        char *lock = getLock(ptr);
        int returnValue = 0;
        while (q_atomic_swp(lock, ~0) != 0)
            ;
        if (*reinterpret_cast<void * volatile *>(ptr) == expected) {
            *reinterpret_cast<void * volatile *>(ptr) = newval;
            returnValue = 1;
        }
        (void) q_atomic_swp(lock, 0);
        return returnValue;
    }

    int q_atomic_increment(volatile int *ptr)
    {
        char *lock = getLock(ptr);
        while (q_atomic_swp(lock, ~0) != 0)
            ;
        int originalValue = *ptr;
        *ptr = originalValue + 1;
        (void) q_atomic_swp(lock, 0);
        return originalValue != -1;
    }

    int q_atomic_decrement(volatile int *ptr)
    {
        char *lock = getLock(ptr);
        while (q_atomic_swp(lock, ~0) != 0)
            ;
        int originalValue = *ptr;
        *ptr = originalValue - 1;
        (void) q_atomic_swp(lock, 0);
        return originalValue != 1;
    }

    int q_atomic_set_int(volatile int *ptr, int newval)
    {
        char *lock = getLock(ptr);
        while (q_atomic_swp(lock, ~0) != 0)
            ;
        int originalValue = *ptr;
        *ptr = newval;
        (void) q_atomic_swp(lock, 0);
        return originalValue;
    }

    void *q_atomic_set_ptr(volatile void *ptr, void *newval)
    {
        char *lock = getLock(ptr);
        while (q_atomic_swp(lock, ~0) != 0)
            ;
	void *originalValue = *reinterpret_cast<void * volatile *>(ptr);
        *reinterpret_cast<void * volatile *>(ptr) = newval;
        (void) q_atomic_swp(lock, 0);
        return originalValue;
    }
}
