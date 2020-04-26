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

#ifndef QSEMAPHORE_H
#define QSEMAPHORE_H

#include "QtCore/qglobal.h"

#ifndef QT_NO_THREAD
QT_MODULE(Core)

class QSemaphorePrivate;

class Q_CORE_EXPORT QSemaphore
{
public:
    explicit QSemaphore(int n = 0);
    ~QSemaphore();

    void acquire(int n = 1);
    bool tryAcquire(int n = 1);

    void release(int n = 1);

    int available() const;

private:
    Q_DISABLE_COPY(QSemaphore)

    QSemaphorePrivate *d;
};

#endif // QT_NO_THREAD
#endif // QSEMAPHORE_H
