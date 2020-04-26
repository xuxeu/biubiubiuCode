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

#ifndef QMUTEX_P_H
#define QMUTEX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qmutex.cpp, qmutex_unix.cpp, and qmutex_win.cpp.  This header
// file may change from version to version without notice, or even be
// removed.
//
// We mean it.
//

class QMutexPrivate {
public:
    QMutexPrivate(QMutex::RecursionMode mode);
    ~QMutexPrivate();

    ulong self();
    void wait();
    void wakeUp();

    QAtomic lock;
    ulong owner;
    uint count;

    bool recursive;

#if defined(Q_OS_UNIX)
    bool wakeup;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#elif defined(Q_OS_WIN32)
    HANDLE event;
#endif
};

#endif // QMUTEX_P_H
