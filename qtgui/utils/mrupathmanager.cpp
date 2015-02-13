#include "mrupathmanager.h"

#include <QDir>

const char *MruPathManager::codes[] = {
    "shapefiles",
    "graph"
};

MruPathManager::MruPathManager()
    : settings()
{
}

QString MruPathManager::getMru(MruPathManager::MRUType type) const
{
    return settings.value(QString("mru_%1").arg(codes[type]), QDir::homePath()).toString();
}

void MruPathManager::setMru(MruPathManager::MRUType type, const QString &path)
{
    QFileInfo info(path);
    settings.setValue(QString("mru_%1").arg(codes[type]), info.filePath());
}

