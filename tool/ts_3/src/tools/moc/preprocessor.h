/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "symbols.h"
#include <qlist.h>
#include <qmap.h>
#include <qset.h>
#include <stdio.h>

typedef QMap<QByteArray,QByteArray> Macros;

class Preprocessor
{
public:
    static bool onlyPreprocess;
    static QByteArray protocol;
    static QList<QByteArray> includes;
    static QSet<QByteArray> preprocessedIncludes;
    static Macros macros;
    static QByteArray preprocessed(const QByteArray &filename, FILE *file);
};

#endif // PREPROCESSOR_H
