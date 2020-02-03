// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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
