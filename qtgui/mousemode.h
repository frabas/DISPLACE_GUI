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
#ifndef MOUSEMODE_H
#define MOUSEMODE_H

#include <QObject>
#include <QPointF>
#include <QString>

class MouseModeInterface {
public:
    virtual void showMessage(const QString &) = 0;
};

/**
 * @brief The MouseMode class
 *
 * @note this class can be decorated (and it is), so follow these simple rules:
 * - Data members are duplicated by decorators. So getter and setter must be overloaded
 * to allow pass the decorated version of the object
 * - for this purpose, getter and setter must be declared virtual.
 */
class MouseMode : public QObject
{
    Q_OBJECT
public:
    MouseMode();
    virtual ~MouseMode();

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
    virtual bool endMode(bool success) = 0;

    virtual QString getModeDescription() const = 0;

    virtual void setMouseModeInterface (MouseModeInterface *);

protected:
    void showMessage(QString msg);

private:
    MouseModeInterface *mGui;
};

#endif // MOUSEMODE_H
