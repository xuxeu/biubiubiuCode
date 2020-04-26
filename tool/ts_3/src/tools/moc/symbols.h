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

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <qstring.h>
#include <qvector.h>
#include "token.h"

struct Symbol
{
    Symbol() : lineNum(-1),token(NOTOKEN), from(0),len(-1) {}
    Symbol(int lineNum, Token token, const QByteArray &lexem, int from = 0, int len=-1):
        lineNum(lineNum), token(token),lexem_data(lexem),from(from), len(len){}
    Symbol(int lineNum, PP_Token token, const QByteArray &lexem, int from = 0, int len=-1):
        lineNum(lineNum), pp_token(token),lexem_data(lexem),from(from), len(len){}
    int lineNum;
    union {Token token; PP_Token pp_token;};
    inline QByteArray lexem() const { return lexem_data.mid(from, len); }
    inline QByteArray unquotedLexem() const { return lexem_data.mid(from+1, len-2); }
    QByteArray lexem_data;
    int from, len;
};
Q_DECLARE_TYPEINFO(Symbol, Q_MOVABLE_TYPE);

typedef QVector<Symbol> Symbols;

#endif // SYMBOLS_H
