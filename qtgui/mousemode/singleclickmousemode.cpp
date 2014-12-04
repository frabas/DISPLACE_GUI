#include "singleclickmousemode.h"
#include <QMessageBox>

#include <QDebug>

SingleClickMouseMode::SingleClickMouseMode()
    : MouseMode()
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
