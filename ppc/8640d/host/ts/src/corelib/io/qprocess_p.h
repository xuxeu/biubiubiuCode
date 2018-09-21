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

#ifndef QPROCESS_P_H
#define QPROCESS_P_H

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

#include "qprocess.h"

#include <private/qinternal_p.h>
#include <private/qiodevice_p.h>
#include <qstringlist.h>

#ifdef Q_OS_WIN
#include "qt_windows.h"
typedef HANDLE Q_PIPE;
#define INVALID_Q_PIPE INVALID_HANDLE_VALUE
#else
typedef int Q_PIPE;
#define INVALID_Q_PIPE -1
#endif

#ifndef QT_NO_PROCESS

class QSocketNotifier;
class QWindowsPipeWriter;
class QWinEventNotifier;
class QTimer;

class QProcessPrivate : public QIODevicePrivate
{
public:
    Q_DECLARE_PUBLIC(QProcess)

    QProcessPrivate();
    virtual ~QProcessPrivate();

    // private slots
    bool canReadStandardOutput();
    bool canReadStandardError();
    bool canWrite();
    bool startupNotification();
    bool processDied();
    void notified();

    QProcess::ProcessChannel processChannel;
    QProcess::ProcessChannelMode processChannelMode;
    QProcess::ProcessError processError;
    QProcess::ProcessState processState;
    QString workingDirectory;
    Q_PID pid;
    int sequenceNumber;

    bool standardOutputClosed;
    bool standardErrorClosed;

    bool emittedReadyRead;
    bool emittedBytesWritten;

    bool writeChannelClosing;
    void closeWriteChannel();

    QString program;
    QStringList arguments;
    QStringList environment;

    QRingBuffer outputReadBuffer;
    QRingBuffer errorReadBuffer;
    QRingBuffer writeBuffer;

    Q_PIPE standardReadPipe[2];
    Q_PIPE errorReadPipe[2];
    Q_PIPE writePipe[2];
    Q_PIPE childStartedPipe[2];
    Q_PIPE deathPipe[2];
    void destroyPipe(Q_PIPE pipe[2]);

    QSocketNotifier *standardReadSocketNotifier;
    QSocketNotifier *errorReadSocketNotifier;
    QSocketNotifier *writeSocketNotifier;
    QSocketNotifier *startupSocketNotifier;
    QSocketNotifier *deathNotifier;

    // the wonderful windows notifier
    QTimer *notifier;
    QWindowsPipeWriter *pipeWriter;
    QWinEventNotifier *processFinishedNotifier;

    void startProcess();
    void execChild(const QByteArray &encodedProgramName);
    bool processStarted();
    void terminateProcess();
    void killProcess();
    void findExitCode();
#ifdef Q_OS_UNIX
    bool waitForDeadChild();
#endif

    static bool startDetached(const QString &program, const QStringList &arguments);

    int exitCode;
    bool crashed;
#ifdef Q_OS_UNIX
    int serial;
#endif

    bool waitForStarted(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000);
    bool waitForBytesWritten(int msecs = 30000);
    bool waitForFinished(int msecs = 30000);
    bool waitForWrite(int msecs = 30000);

    qint64 bytesAvailableFromStdout() const;
    qint64 bytesAvailableFromStderr() const;
    qint64 readFromStdout(char *data, qint64 maxlen);
    qint64 readFromStderr(char *data, qint64 maxlen);
    qint64 writeToStdin(const char *data, qint64 maxlen);

    void cleanup();
#ifdef Q_OS_UNIX
    static void initializeProcessManager();
#endif
};

#endif // QT_NO_PROCESS
#endif // QPROCESS_P_H
