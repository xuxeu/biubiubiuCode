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

#ifndef QFILE_P_H
#define QFILE_P_H

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

#include <private/qiodevice_p.h>
#include <private/qinternal_p.h>
#include <qfileengine.h>

//#define QT_NO_FILE_BUFFER

class QFilePrivate : public QIODevicePrivate
{
    Q_DECLARE_PUBLIC(QFile)

protected:
    QFilePrivate();
    ~QFilePrivate();

    bool openExternalFile(int flags, int fd);
    bool openExternalFile(int flags, FILE *fh);

#ifndef QT_NO_FILE_BUFFER
    QCircularBuffer buffer;
#endif
    QString fileName;
    mutable QFileEngine *fileEngine;
    bool isOpen;

    QFile::FileError error;
    QString errorString;
    void setError(QFile::FileError err);
    void setError(QFile::FileError err, const QString &errorString);
    void setError(QFile::FileError err, int errNum);

private:
    static QFile::EncoderFn encoder;
    static QFile::DecoderFn decoder;
};

#endif // QFILE_P_H
