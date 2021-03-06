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

#ifndef QMETATYPE_H
#define QMETATYPE_H

#include "QtCore/qglobal.h"
#include "QtCore/qstring.h"

#ifndef QT_NO_DATASTREAM
#include "QtCore/qdatastream.h"
#endif

#ifdef Bool
#error qmetatype.h must be included before any header file that define Bool
#endif

QT_MODULE(Core)

class Q_CORE_EXPORT QMetaType {
public:
    enum Type {
        // these are merged with QVariant
        Void = 0, Bool = 1, Int = 2, UInt = 3, Double = 6, QChar = 7,
        QString = 10, QByteArray = 12,

        VoidStar = 128, Long, Short, Char, ULong,
        UShort, UChar, Float, QObjectStar, QWidgetStar,
        User = 256
    };

    typedef void (*Destructor)(void *);
    typedef void *(*Constructor)(const void *);

#ifndef QT_NO_DATASTREAM
    typedef void (*SaveOperator)(QDataStream &, const void *);
    typedef void (*LoadOperator)(QDataStream &, void *);
    static void registerStreamOperators(const char *typeName, SaveOperator saveOp,
                                        LoadOperator loadOp);
#endif
    static int registerType(const char *typeName, Destructor destructor,
                            Constructor constructor);
    static int type(const char *typeName);
    static const char *typeName(int type);
    static bool isRegistered(int type);
    static void *construct(int type, const void *copy);
    static void destroy(int type, void *data);

#ifndef QT_NO_DATASTREAM
    static bool save(QDataStream &stream, int type, const void *data);
    static bool load(QDataStream &stream, int type, void *data);
#endif
};

template <typename T>
void qMetaTypeDeleteHelper(T *t)
{
    delete t;
}

template <typename T>
void *qMetaTypeConstructHelper(const T *t)
{
    if (!t)
        return new T;
    return new T(*static_cast<const T*>(t));
}

#ifndef QT_NO_DATASTREAM
template <typename T>
void qMetaTypeSaveHelper(QDataStream &stream, const T *t)
{
    stream << *t;
}

template <typename T>
void qMetaTypeLoadHelper(QDataStream &stream, T *t)
{
    stream >> *t;
}

template <typename T>
void qRegisterMetaTypeStreamOperators(const char *typeName, T * = 0)
{
    typedef void(*SavePtr)(QDataStream &, const T *);
    typedef void(*LoadPtr)(QDataStream &, T *);
    SavePtr sptr = qMetaTypeSaveHelper<T>;
    LoadPtr lptr = qMetaTypeLoadHelper<T>;

    QMetaType::registerStreamOperators(typeName, reinterpret_cast<QMetaType::SaveOperator>(sptr),
                                       reinterpret_cast<QMetaType::LoadOperator>(lptr));
}

#endif
template <typename T>
int qRegisterMetaType(const char *typeName, T * = 0)
{
    typedef void*(*ConstructPtr)(const T*);
    ConstructPtr cptr = qMetaTypeConstructHelper<T>;
    typedef void(*DeletePtr)(T*);
    DeletePtr dptr = qMetaTypeDeleteHelper<T>;

    return QMetaType::registerType(typeName, reinterpret_cast<QMetaType::Destructor>(dptr),
                                   reinterpret_cast<QMetaType::Constructor>(cptr));
}

template <typename T>
struct QMetaTypeId
{
};

#define Q_DECLARE_METATYPE(TYPE) \
template <> \
struct QMetaTypeId< TYPE > \
{ \
    static int qt_metatype_id() \
    { \
       static int id = qRegisterMetaType< TYPE >(#TYPE); \
       return id; \
    } \
};

template<> struct QMetaTypeId<QString>
{ static inline int qt_metatype_id() { return QMetaType::QString; } };
template<> struct QMetaTypeId<int>
{ static inline int qt_metatype_id() { return QMetaType::Int; } };
template<> struct QMetaTypeId<uint>
{ static inline int qt_metatype_id() { return QMetaType::UInt; } };
template<> struct QMetaTypeId<bool>
{ static inline int qt_metatype_id() { return QMetaType::Bool; } };
template<> struct QMetaTypeId<double>
{ static inline int qt_metatype_id() { return QMetaType::Double; } };
template<> struct QMetaTypeId<QByteArray>
{ static inline int qt_metatype_id() { return QMetaType::QByteArray; } };
template<> struct QMetaTypeId<QChar>
{ static inline int qt_metatype_id() { return QMetaType::QChar; } };
template<> struct QMetaTypeId<void>
{ static inline int qt_metatype_id() { return QMetaType::Void; } };
template<> struct QMetaTypeId<long>
{ static inline int qt_metatype_id() { return QMetaType::Long; } };
template<> struct QMetaTypeId<short>
{ static inline int qt_metatype_id() { return QMetaType::Short; } };
template<> struct QMetaTypeId<char>
{ static inline int qt_metatype_id() { return QMetaType::Char; } };
template<> struct QMetaTypeId<ulong>
{ static inline int qt_metatype_id() { return QMetaType::ULong; } };
template<> struct QMetaTypeId<ushort>
{ static inline int qt_metatype_id() { return QMetaType::UShort; } };
template<> struct QMetaTypeId<uchar>
{ static inline int qt_metatype_id() { return QMetaType::UChar; } };
template<> struct QMetaTypeId<float>
{ static inline int qt_metatype_id() { return QMetaType::Float; } };
class QObject;
template<> struct QMetaTypeId<QObject *>
{ static inline int qt_metatype_id() { return QMetaType::QObjectStar; } };
class QWidget;
template<> struct QMetaTypeId<QWidget *>
{ static inline int qt_metatype_id() { return QMetaType::QWidgetStar; } };
template<> struct QMetaTypeId<void *>
{ static inline int qt_metatype_id() { return QMetaType::VoidStar; } };

#endif // QMETATYPE_H
