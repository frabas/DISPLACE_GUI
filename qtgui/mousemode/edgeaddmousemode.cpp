// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "edgeaddmousemode.h"

#include <displacemodel.h>

EdgeAddMouseMode::EdgeAddMouseMode(DisplaceModel *model)
    : MouseMode(),
      mModel(model),
      mStatus(0)
{
}

bool EdgeAddMouseMode::releaseEvent(const QPointF &point)
{
    QList<std::shared_ptr<NodeData> > nodes = mModel->getAllNodesWithin(point, 0.3);

    qDebug() << "Nodes IN " << point << nodes.size() << mStatus;

    if (nodes.size() != 1)
        return false;

    switch (mStatus) {
    case 0:     // first node
        mNodeFrom = nodes[0]->get_idx_node().toIndex();
        showMessage(QString(tr("Select second node")));
        ++mStatus;
        break;
    case 1:
        mNodeTo = nodes[0]->get_idx_node().toIndex();
        emit edgeAdded(mNodeFrom, mNodeTo);
        showMessage(QString(tr("Edge created.")));
        break;
    }

    return true;
}

bool EdgeAddMouseMode::beginMode()
{
    showMessage(QString(tr("Select first node")));
    return true;
}

bool EdgeAddMouseMode::endMode(bool success)
{
    Q_UNUSED(success);
    return true;
}

QString EdgeAddMouseMode::getModeDescription() const
{
    return QString(tr("Add Graph Edge mode"));
}

