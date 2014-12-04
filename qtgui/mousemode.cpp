#include "mousemode.h"

MouseMode::MouseMode()
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
