#include "mousemode.h"

#include <QDebug>

MouseMode::MouseMode()
    : mGui(0)
{
}

MouseMode::~MouseMode()
{

}

bool MouseMode::pressEvent(const QPointF &point)
{
    Q_UNUSED(point);
    return true;
}

bool MouseMode::releaseEvent(const QPointF &point)
{
    Q_UNUSED(point);
    return true;
}

bool MouseMode::moveEvent(const QPointF &point)
{
    Q_UNUSED(point);
    return true;
}

bool MouseMode::beginMode()
{
    return true;
}

void MouseMode::setMouseModeInterface(MouseModeInterface *ifc)
{
    mGui = ifc;
}

void MouseMode::showMessage(QString msg)
{
    if (mGui)
        mGui->showMessage(msg);
}
