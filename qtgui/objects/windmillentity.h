/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014, 2015, 2106 Francois Bastardie <fba@aqua.dtu.dk>
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
#ifndef WINDMILLFARMENTITY_H
#define WINDMILLFARMENTITY_H

#include <objects/objecttreeentity.h>

namespace objecttree {

class WindmillEntity : public ObjectTreeEntity
{
    int mWindmillId;

public:
    WindmillEntity(ObjectTreeModel *_model, int id = -1);

    virtual ObjectTreeModel::Category getCategory() const override{
        return ObjectTreeModel::Windmills;
    }

    virtual QModelIndex parent (const QModelIndex &parent) const override;
    virtual QModelIndex index (int row, int column, const QModelIndex &parent) const override;
    virtual int rowCount() const override;
    virtual int columnCount() const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

    // Specific access functions
    int getWindmillId() const { return mWindmillId; }

    Qt::ItemFlags flags(Qt::ItemFlags defFlags, const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

};

}

#endif // WINDMILLENTITY_H
