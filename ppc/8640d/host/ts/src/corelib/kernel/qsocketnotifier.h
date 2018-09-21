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

#ifndef QSOCKETNOTIFIER_H
#define QSOCKETNOTIFIER_H

#include "QtCore/qobject.h"

QT_MODULE(Core)

class Q_CORE_EXPORT QSocketNotifier : public QObject
{
    Q_OBJECT
public:
    enum Type { Read, Write, Exception };

    QSocketNotifier(int socket, Type, QObject *parent = 0);
#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QSocketNotifier(int socket, Type, QObject *parent, const char *name);
#endif
    ~QSocketNotifier();

    inline int socket() const { return sockfd; }
    inline Type type() const { return sntype; }

    inline bool isEnabled() const { return snenabled; }

public slots:
    void setEnabled(bool);

signals:
    void activated(int socket);

protected:
    bool event(QEvent *);

private:
    Q_DISABLE_COPY(QSocketNotifier)

    int sockfd;
    Type sntype;
    bool snenabled;
};

#endif // QSOCKETNOTIFIER_H
