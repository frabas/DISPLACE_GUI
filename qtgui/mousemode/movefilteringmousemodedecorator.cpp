// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

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
