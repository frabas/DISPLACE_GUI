#ifndef NATIONALITYDATA_H
#define NATIONALITYDATA_H

#include <QString>
#include <QVector>

class NationData
{
    QString mName;
public:
    NationData();

    QString getName() const;
    void setName(const QString &value);
};

class NationStats {
public:
    NationStats()
        : mTotCatches(0.0),
          mRevenues(0.0),
          szGroups()
    {
    }

    double mTotCatches;
    double mRevenues;
    double mTimeAtSea;

    QVector<double> szGroups;
};

#endif // NATIONALITYDATA_H
