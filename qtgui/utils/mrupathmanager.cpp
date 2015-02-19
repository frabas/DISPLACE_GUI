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

