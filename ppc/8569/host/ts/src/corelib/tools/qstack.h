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

#ifndef QSTACK_H
#define QSTACK_H

#include "QtCore/qvector.h"

QT_MODULE(Core)

template<class T>
class QStack : public QVector<T>
{
public:
    inline QStack() {}
    inline ~QStack() {}
    inline void push(const T &t) { append(t); }
    T pop();
    T &top();
    const T &top() const;
};

template<class T>
inline T QStack<T>::pop()
{ Q_ASSERT(!this->isEmpty()); T t = this->data()[this->size() -1];
  this->resize(this->size()-1); return t; }

template<class T>
inline T &QStack<T>::top()
{ Q_ASSERT(!this->isEmpty()); this->detach(); return this->data()[this->size()-1]; }

template<class T>
inline const T &QStack<T>::top() const
{ Q_ASSERT(!this->isEmpty()); return this->data()[this->size()-1]; }

#endif // QSTACK_H
