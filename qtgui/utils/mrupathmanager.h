#ifndef MRUPATHMANAGER_H
#define MRUPATHMANAGER_H

#include <QSettings>

class MruPathManager
{
public:
    enum MRUType {
        ShapefileFolder,
        GraphFolder,
        StockNamesFile,
        N
    };

    enum SetOptions {
        FolderOnly,
        FilePath
    };

    MruPathManager();

    QString getMru(MRUType type) const;
    void setMru(MRUType type, const QString &path, SetOptions opt = FilePath);

private:
    QSettings settings;

    static const char *codes[];
};

#endif // MRUPATHMANAGER_H
