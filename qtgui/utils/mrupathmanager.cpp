// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2024 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "mrupathmanager.h"

#include <QDir>

const char *MruPathManager::codes[] = {
    "shapefiles",
    "graph",
    "stocknames"
};

MruPathManager::MruPathManager()
    : settings()
{
}

QString MruPathManager::getMru(MruPathManager::MRUType type) const
{
    return settings.value(QString("mru_%1").arg(codes[type]), QDir::homePath()).toString();
}

void MruPathManager::setMru(MruPathManager::MRUType type, const QString &path, SetOptions opt)
{
    QString r = path;

    if (opt == FolderOnly) {
        QFileInfo info(path);
        if (info.isFile())
            r = info.filePath();
    }

    settings.setValue(QString("mru_%1").arg(codes[type]), r);
}

