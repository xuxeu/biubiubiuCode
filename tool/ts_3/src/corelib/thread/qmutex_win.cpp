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

#include <windows.h>

#include "qmutex.h"
#include <qatomic.h>
#include "qmutex_p.h"

QMutexPrivate::QMutexPrivate(QMutex::RecursionMode mode)
    : lock(0), owner(0), count(0), recursive(mode == QMutex::Recursive),
      event(QT_WA_INLINE(CreateEventW(0, false, false, 0),
                         CreateEventA(0, false, false, 0)))
{
    if (!event)
        qWarning("QMutexPrivate::QMutexPrivate(): Creating event failed");
}

QMutexPrivate::~QMutexPrivate()
{ CloseHandle(event); }

ulong QMutexPrivate::self()
{ return GetCurrentThreadId(); }

void QMutexPrivate::wait()
{ 
    if (WaitForSingleObject(event, INFINITE) != WAIT_OBJECT_0)
        qWarning("QMutexPrivate::wait(): Waiting on event failed");
}

void QMutexPrivate::wakeUp()
{ SetEvent(event); }
