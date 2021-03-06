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

#ifndef QFSFILEENGINE_H
#define QFSFILEENGINE_H

#include "qfileengine.h"
#include "private/qfileengine_p.h"

class QFSFileEnginePrivate;
class QFSFileEngine : public QFileEngine
{
    Q_DECLARE_PRIVATE(QFSFileEngine)
public:
    QFSFileEngine();
    explicit QFSFileEngine(const QString &file);
    ~QFSFileEngine();

    void setFileName(const QString &file);

    bool open(int flags);
    bool close();
    void flush();
    qint64 size() const;
    qint64 at() const;
    bool seek(qint64);
    qint64 read(char *data, qint64 maxlen);
    qint64 write(const char *data, qint64 len);
    QFile::FileError error() const;
    QString errorString() const;

    bool remove();
    bool copy(const QString &newName);
    bool rename(const QString &newName);
    bool link(const QString &newName);

    bool isSequential() const;

    uchar *map(qint64 off, qint64 len);
    void unmap(uchar *data);

    bool mkdir(const QString &dirName, bool createParentDirectories) const;
    bool rmdir(const QString &dirName, bool recurseParentDirectories) const;

    bool setSize(qint64 size);

    QStringList entryList(QDir::Filters filters, const QStringList &filterNames) const;

    bool caseSensitive() const;

    bool isRelativePath() const;

    FileFlags fileFlags(FileFlags type) const;

    bool chmod(uint perms);

    QString fileName(QFileEngine::FileName file) const;

    uint ownerId(QFileEngine::FileOwner) const;
    QString owner(FileOwner) const;

    QDateTime fileTime(FileTime time) const;

    Type type() const;

    //FS only!!
    bool open(int flags, int fd);
    int handle() const;
    static bool setCurrentPath(const QString &path);
    static QString currentPath(const QString &path = QString());
    static QString homePath();
    static QString rootPath();
    static QString tempPath();
    static QFileInfoList drives();

protected:
    QFSFileEngine(QFSFileEnginePrivate &dd);
};

class QFSFileEnginePrivate : public QFileEnginePrivate
{
    Q_DECLARE_PUBLIC(QFSFileEngine)

public:
#ifdef Q_WS_WIN
    static QString fixToQtSlashes(const QString &path);
    static QByteArray win95Name(const QString &path);
#else
    static inline QString fixToQtSlashes(const QString &path) { return path; }
#endif

    QString file;

    inline void resetErrors() const {
        error = QFile::UnspecifiedError;
        errorString.clear();
    }
    inline void setError(QFile::FileError err, int errorCode) const {
        error = err;
        errorString = qt_error_string(errorCode);
    }
    inline void setError(QFile::FileError err, QString errStr = QString()) const {
        error = err;
        errorString = errStr;
    }
    mutable QFile::FileError error;
    mutable QString errorString;

    int fd;
    mutable uint sequential : 1;
    mutable uint external_file : 1;
    QByteArray ungetchBuffer;

    mutable uint could_stat : 1;
    mutable uint tried_stat : 1;
#ifdef Q_OS_UNIX
    mutable uint isSymLink : 1;
#endif
    mutable QT_STATBUF st;
    bool doStat() const;
    int sysOpen(const QString &, int flags);

protected:
    QFSFileEnginePrivate();

    void init();

#if defined(Q_OS_WIN32)
    QFileEngine::FileFlags getPermissions() const;
    QString getLink() const;
#endif
};

#endif

