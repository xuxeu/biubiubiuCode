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

#ifndef X86_64_QATOMIC_H
#define X86_64_QATOMIC_H

#include <QtCore/qglobal.h>

#if defined(Q_CC_GNU)

inline int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval)
{
    unsigned char ret;
    asm volatile("lock cmpxchgl %2,%3\n"
                 "sete %1\n"
                 : "=a" (newval), "=qm" (ret)
                 : "r" (newval), "m" (*ptr), "0" (expected)
                 : "memory");
    return static_cast<int>(ret);
}

inline int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval)
{
    unsigned char ret;
    asm volatile("lock cmpxchgq %2,%3\n"
                 "sete %1\n"
                 : "=a" (newval), "=qm" (ret)
                 : "r" (newval), "m" (*reinterpret_cast<volatile long *>(ptr)), "0" (expected)
                 : "memory");
    return static_cast<int>(ret);
}

inline int q_atomic_increment(volatile int *ptr)
{
    unsigned char ret;
    asm volatile("lock incl %0\n"
                 "setne %1"
                 : "=m" (*ptr), "=qm" (ret)
                 : "m" (*ptr)
                 : "memory");
    return static_cast<int>(ret);
}

inline int q_atomic_decrement(volatile int *ptr)
{
    unsigned char ret;
    asm volatile("lock decl %0\n"
                 "setne %1"
                 : "=m" (*ptr), "=qm" (ret)
                 : "m" (*ptr)
                 : "memory");
    return static_cast<int>(ret);
}

inline int q_atomic_set_int(volatile int *ptr, int newval)
{
    asm volatile("xchgl %0,%1"
                 : "=r" (newval)
                 : "m" (*ptr), "0" (newval)
                 : "memory");
    return newval;
}

inline void *q_atomic_set_ptr(volatile void *ptr, void *newval)
{
    asm volatile("xchgq %0,%1"
                 : "=r" (newval)
                 : "m" (*reinterpret_cast<volatile long *>(ptr)), "0" (newval)
                 : "memory");
    return newval;
}

#endif

#endif // X86_64_QATOMIC_H
