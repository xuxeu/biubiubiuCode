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

#ifndef QMIMEDATA_H
#define QMIMEDATA_H

#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>

QT_MODULE(Core)

class QUrl;
class QMimeDataPrivate;

class Q_CORE_EXPORT QMimeData : public QObject
{
    Q_OBJECT
public:
    QMimeData();
    ~QMimeData();

    QList<QUrl> urls() const;
    void setUrls(const QList<QUrl> &urls);
    bool hasUrls() const;

    QString text() const;
    void setText(const QString &text);
    bool hasText() const;

    QString html() const;
    void setHtml(const QString &html);
    bool hasHtml() const;

    QVariant imageData() const;
    void setImageData(const QVariant &image);
    bool hasImage() const;

    QVariant colorData() const;
    void setColorData(const QVariant &color);
    bool hasColor() const;

    QByteArray data(const QString &mimetype) const;
    void setData(const QString &mimetype, const QByteArray &data);

    virtual bool hasFormat(const QString &mimetype) const;
    virtual QStringList formats() const;

    void clear();
protected:
    virtual QVariant retrieveData(const QString &mimetype,
                                      QVariant::Type preferredType) const;
private:
    Q_DISABLE_COPY(QMimeData)
    Q_DECLARE_PRIVATE(QMimeData)
};

#endif

