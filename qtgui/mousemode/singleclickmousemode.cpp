#include "singleclickmousemode.h"
#include <QMessageBox>

#include <QDebug>

SingleClickMouseMode::SingleClickMouseMode(QString description)
    : MouseMode(),
      mDescription(description)
{
}

SingleClickMouseMode::~SingleClickMouseMode()
{

}

bool SingleClickMouseMode::releaseEvent(const QPointF &point)
{
    emit modeCompleted(point);
    return true;
}

bool SingleClickMouseMode::endMode(bool success)
{
    return true;
}

QString SingleClickMouseMode::getModeDescription() const
{
    return mDescription;
}
