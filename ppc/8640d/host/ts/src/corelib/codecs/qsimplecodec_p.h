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

#ifndef QSIMPLECODEC_P_H
#define QSIMPLECODEC_P_H

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

#include "qtextcodec.h"

#ifndef QT_NO_TEXTCODEC

class QSimpleTextCodec: public QTextCodec
{
public:
    enum { numSimpleCodecs = 30 };
    explicit QSimpleTextCodec(int);
    ~QSimpleTextCodec();

    QString convertToUnicode(const char *, int, ConverterState *) const;
    QByteArray convertFromUnicode(const QChar *, int, ConverterState *) const;

    QByteArray name() const;
    QList<QByteArray> aliases() const;
    int mibEnum() const;

private:
    void buildReverseMap() const;

    int forwardIndex;
#ifndef Q_WS_QWS
    mutable QByteArray reverseMap;
#endif
};

#endif // QT_NO_TEXTCODEC
#endif // QSIMPLECODEC_P_H
