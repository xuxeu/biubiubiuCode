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

#ifndef QBUFFEREDFSFILEENGINE_P_H
#define QBUFFEREDFSFILEENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qplatformdefs.h>
#include <qiodevice.h>

#include "qfileengine.h"
#include <private/qfsfileengine_p.h>

class QBufferedFSFileEnginePrivate;
class QBufferedFSFileEngine : public QFSFileEngine
{
    Q_DECLARE_PRIVATE(QBufferedFSFileEngine)
public:
    QBufferedFSFileEngine(const QString &fileName = QString());
    ~QBufferedFSFileEngine();

    Type type() const;
    bool open(int flags);
    bool open(int flags, FILE *fh);
    bool close();
    void flush();
    qint64 at() const;
    bool seek(qint64);
    qint64 read(char *data, qint64 maxlen);
    qint64 readLine(char *data, qint64 maxlen);
    qint64 write(const char *data, qint64 len);
};

class QBufferedFSFileEnginePrivate : public QFSFileEnginePrivate
{
    Q_DECLARE_PUBLIC(QBufferedFSFileEngine)

public:
    inline QBufferedFSFileEnginePrivate()
    {
        fh = 0;
        lastIOCommand = IOFlushCommand;
        closeFileHandle = false;
    }

    FILE *fh;
    bool closeFileHandle;
    
    enum LastIOCommand
    {
        IOFlushCommand,
        IOReadCommand,
        IOWriteCommand
    };
    LastIOCommand  lastIOCommand;

};

#endif
