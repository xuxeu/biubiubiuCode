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

#ifndef SPARC_QATOMIC_H
#define SPARC_QATOMIC_H

#include <QtCore/qglobal.h>

#if defined(_LP64)

extern "C" {
    Q_CORE_EXPORT int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval);
    Q_CORE_EXPORT int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval);
    Q_CORE_EXPORT int q_atomic_increment(volatile int *ptr);
    Q_CORE_EXPORT int q_atomic_decrement(volatile int *ptr);
    Q_CORE_EXPORT int q_atomic_set_int(volatile int *ptr, int newval);
    Q_CORE_EXPORT void *q_atomic_set_ptr(volatile void *ptr, void *newval);
}

#else

extern "C" {
    Q_CORE_EXPORT int q_atomic_lock_int(volatile int *addr);
    Q_CORE_EXPORT int q_atomic_lock_ptr(volatile void *addr);
    Q_CORE_EXPORT void q_atomic_unlock(volatile void *addr, int value);
    Q_CORE_EXPORT int q_atomic_set_int(volatile int *ptr, int newval);
    Q_CORE_EXPORT void *q_atomic_set_ptr(volatile void *ptr, void *newval);
} // extern "C"

inline int q_atomic_test_and_set_int(volatile int *ptr, int expected, int newval)
{
    int val = q_atomic_lock_int(ptr);
    if (val == expected) {
        q_atomic_unlock(ptr, newval);
        return 1;
    }
    q_atomic_unlock(ptr, val);
    return 0;
}

inline int q_atomic_test_and_set_ptr(volatile void *ptr, void *expected, void *newval)
{
    void *val = reinterpret_cast<void *>(q_atomic_lock_ptr(ptr));
    if (val == expected) {
        q_atomic_unlock(ptr, reinterpret_cast<int>(newval));
        return 1;
    }
    q_atomic_unlock(ptr, reinterpret_cast<int>(val));
    return 0;
}

inline int q_atomic_increment(volatile int *ptr)
{
    const int val = q_atomic_lock_int(ptr);
    q_atomic_unlock(ptr, val + 1);
    return val != -1;
}

inline int q_atomic_decrement(volatile int *ptr)
{
    const int val = q_atomic_lock_int(ptr);
    q_atomic_unlock(ptr, val - 1);
    return val != 1;
}

#endif // !_LP64

#endif // SPARC_QATOMIC_H
