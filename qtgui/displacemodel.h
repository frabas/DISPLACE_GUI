#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <QString>
#include <QList>

class Ship;

class DisplaceModel
{
public:
    DisplaceModel();

    bool load (QString path, QString modelname);

    // Getter
    QString name() const { return mName; }
    QString basepath() const { return mBasePath; }
    bool isModelLoaded() const { return !mName.isEmpty(); }

    int getShipCount() const;
    QString getShipId(int idx) const;

private:
    QString mName;
    QString mBasePath;

    QList<Ship> mShips;
};

#endif // DISPLACEMODEL_H
