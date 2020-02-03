// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

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
