/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef MOVEFILTERINGMOUSEMODEDECORATOR_H
#define MOVEFILTERINGMOUSEMODEDECORATOR_H

#include <mousemode.h>

class MoveFilteringMouseModeDecorator : public MouseMode
{
public:
    MoveFilteringMouseModeDecorator(MouseMode *mode);

    /** \brief signals a mouse press event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool pressEvent(const QPointF &point);

    /** \brief signals a mouse release event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool releaseEvent(const QPointF &point);

    /** \brief signals a mouse move event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool moveEvent(const QPointF &point);

    /** \brief Starts the mouse mode.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool beginMode();


    /** \brief Completes the mouse mode
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool endMode(bool success);

    QString getModeDescription() const;

    void setMouseModeInterface (MouseModeInterface *ifc) {
        mMode->setMouseModeInterface(ifc);
    }

private:
    MouseMode *mMode;
    bool mHasMoved;
    bool mPressed;
};

#endif // MOVEFILTERINGMOUSEMODEDECORATOR_H
