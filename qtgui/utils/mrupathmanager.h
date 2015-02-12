#ifndef MRUPATHMANAGER_H
#define MRUPATHMANAGER_H

#include <QSettings>

class MruPathManager
{
public:
    enum MRUType {
        ShapefileFolder,
        N
    };

    MruPathManager();

    QString getMru(MRUType type) const;
    void setMru(MRUType type, const QString &path);

private:
    QSettings settings;

    static const char *codes[];
};

#endif // MRUPATHMANAGER_H
