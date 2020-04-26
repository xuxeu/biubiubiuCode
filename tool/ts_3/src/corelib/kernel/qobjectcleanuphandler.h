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

#ifndef QOBJECTCLEANUPHANDLER_H
#define QOBJECTCLEANUPHANDLER_H

#include "QtCore/qobject.h"

QT_MODULE(Core)

class Q_CORE_EXPORT QObjectCleanupHandler : public QObject
{
    Q_OBJECT

public:
    QObjectCleanupHandler();
    ~QObjectCleanupHandler();

    QObject* add(QObject* object);
    void remove(QObject *object);
    bool isEmpty() const;
    void clear();

private:
    // ### move into d pointer
    QObjectList cleanupObjects;

private slots:
    void objectDestroyed(QObject *);
};

#endif // QOBJECTCLEANUPHANDLER_H
