#include "movefilteringmousemodedecorator.h"

#include <QDebug>

MoveFilteringMouseModeDecorator::MoveFilteringMouseModeDecorator(MouseMode *mode)
    : MouseMode(),
      mMode(mode),
      mHasMoved(false)
{
}

bool MoveFilteringMouseModeDecorator::pressEvent(const QPointF &point)
{
    mHasMoved = false;
    mPressed = true;
    return mMode->pressEvent(point);
}

bool MoveFilteringMouseModeDecorator::releaseEvent(const QPointF &point)
{
    mPressed = false;
    if (!mHasMoved)
        return mMode->releaseEvent(point);
    return false;
}

bool MoveFilteringMouseModeDecorator::moveEvent(const QPointF &point)
{
    if (mPressed) {
        mHasMoved = true;
    }
    mMode->moveEvent(point);
    return true;
}

bool MoveFilteringMouseModeDecorator::beginMode()
{
    return mMode->beginMode();
}

bool MoveFilteringMouseModeDecorator::endMode(bool success)
{
    return mMode->endMode(success);
}

QString MoveFilteringMouseModeDecorator::getModeDescription() const
{
    return mMode->getModeDescription();
}
