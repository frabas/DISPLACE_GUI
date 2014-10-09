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

#endif // NATIONALITYDATA_H
