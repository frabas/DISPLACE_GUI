#ifndef NATIONALITYDATA_H
#define NATIONALITYDATA_H

#include <QString>

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
          mRevenues(0.0) {
    }

    double mTotCatches;
    double mRevenues;
};

#endif // NATIONALITYDATA_H
