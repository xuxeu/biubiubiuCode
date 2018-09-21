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

#ifndef QSIGNALMAPPER_H
#define QSIGNALMAPPER_H

#include "QtCore/qobject.h"

QT_MODULE(Core)

#ifndef QT_NO_SIGNALMAPPER
class QSignalMapperPrivate;

class Q_CORE_EXPORT QSignalMapper : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSignalMapper)
public:
    explicit QSignalMapper(QObject *parent = 0);
    ~QSignalMapper();

    void setMapping(QObject *sender, int id);
    void setMapping(QObject *sender, const QString &text);
    void setMapping(QObject *sender, QWidget *widget);
    void removeMappings(QObject *sender);

    QObject *mapping(int id) const;
    QObject *mapping(const QString &text) const;
    QObject *mapping(QWidget *widget) const;

signals:
    void mapped(int);
    void mapped(const QString &);
    void mapped(QWidget *);

public slots:
    void map();
    void map(QObject *sender);

private:
    Q_DISABLE_COPY(QSignalMapper)
    Q_PRIVATE_SLOT(d_func(), void senderDestroyed())

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QSignalMapper(QObject *parent, const char *name);
#endif
};
#endif // QT_NO_SIGNALMAPPER

#endif // QSIGNALMAPPER_H
