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
#ifndef EDGEADDMOUSEMODE_H
#define EDGEADDMOUSEMODE_H

#include "mousemode.h"
#include <QObject>

class DisplaceModel;

class EdgeAddMouseMode : public MouseMode
{
    Q_OBJECT
public:
    explicit EdgeAddMouseMode(DisplaceModel *model);

    bool releaseEvent(const QPointF &point);

    bool beginMode();
    bool endMode(bool success);

    QString getModeDescription() const;
signals:
    void edgeAdded (int from_idx, int to_idx);

private:
    DisplaceModel *mModel;
    int mStatus;        ///< 0: started, 1: first point selected
    int mNodeFrom, mNodeTo;
};

#endif // EDGEADDMOUSEMODE_H
