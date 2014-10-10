#include <modelobjects/nationdata.h>

NationData::NationData()
{
}

QString NationData::getName() const
{
    return mName;
}

void NationData::setName(const QString &value)
{
    mName = value;
}
