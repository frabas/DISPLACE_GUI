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
#ifndef ADDNODEMOUSEMODE_H
#define ADDNODEMOUSEMODE_H

#include <mousemode.h>

#include <QObject>
#include <QPointF>
#include <QList>
#include <QDialog>

/**
 * @brief The SingleClickMouseMode class implements a mouse mode that completes once a point on the map is
 * clicked, by emitting the modeCompleted(QPointF) signal.
 * The class can be decorated using the MoveVilteringMouseModeDecorator.
 */
class SingleClickMouseMode : public MouseMode
{
    Q_OBJECT
public:
    explicit SingleClickMouseMode(QString description);

    virtual ~SingleClickMouseMode();

    /** \brief signals a mouse release event.
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool releaseEvent(const QPointF &point);

    /** \brief Completes the mouse mode
     * \return false if the mouse mode must be aborted, true otherwise
     * */
    virtual bool endMode(bool success);

    QString getModeDescription() const;
signals:
    void modeCompleted(QPointF);

protected:
    QString mDescription;
};

#endif // ADDNODEMOUSEMODE_H
