#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <Harbour.h>

#include <QString>
#include <QList>

class DisplaceModel
{
public:
    DisplaceModel();

    bool load (QString path, QString modelname);

    // Getter
    QString name() const { return mName; }
    QString basepath() const { return mBasePath; }
    bool isModelLoaded() const { return !mName.isEmpty(); }

    int getHarboursCount() const;
    QString getHarbourId(int idx) const;

protected:


private:
    QString mName;
    QString mBasePath;

    QList<Harbour> mHarbours;
};

#endif // DISPLACEMODEL_H
