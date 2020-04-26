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

#ifndef QQUEUE_H
#define QQUEUE_H

#include "QtCore/qlist.h"

QT_MODULE(Core)

template <class T>
class QQueue : public QList<T>
{
public:
    inline QQueue() {}
    inline ~QQueue() {}
    inline void enqueue(const T &t) { QList<T>::append(t); }
    inline T dequeue() { return QList<T>::takeFirst(); }
    inline T &head() { return QList<T>::first(); }
    inline const T &head() const { return QList<T>::first(); }
};

#endif // QQUEUE_H
