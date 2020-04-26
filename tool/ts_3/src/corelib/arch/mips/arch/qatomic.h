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

#ifndef MIPS_QATOMIC_H
#define MIPS_QATOMIC_H

#include <QtCore/qglobal.h>

extern "C" {
    Q_CORE_EXPORT int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
    Q_CORE_EXPORT int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
} // extern "C"

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

#endif // MIPS_QATOMIC_H
