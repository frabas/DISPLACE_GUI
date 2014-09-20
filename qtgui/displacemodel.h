#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <QString>

class DisplaceModel
{
public:
    DisplaceModel();

    bool load (QString path, QString modelname);

    // Getter
    QString name() const { return mName; }
private:
    QString mName;
    QString mBasePath;
};

#endif // DISPLACEMODEL_H
