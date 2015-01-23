#include "imageformathelpers.h"

#include <QImageWriter>
#include <QList>
#include <QByteArray>
#include <QDebug>

using namespace displace::helpers::images;

QStringList displace::helpers::images::supportedFormatsOnWriteAsFilter()
{
    QStringList formats;
    QList<QByteArray> list = QImageWriter::supportedImageFormats();
    foreach (QByteArray d, list) {
        QString des = QString(QObject::tr("%1 format (*.%1)")).arg(QString::fromLatin1(d.constData()));

        formats.append(des);
    }

    formats.append(QObject::tr("All files (*.*)"));

    return formats;
}

QStringList displace::helpers::images::supportedFormatsOnWrite()
{
    QStringList formats;
    QList<QByteArray> list = QImageWriter::supportedImageFormats();
    foreach (QByteArray d, list) {
        formats.append(QString::fromLatin1(d.constData()));
    }

    return formats;
}
